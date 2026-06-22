# ============================================================================
# Charge self-consistent (CSC) DFT+DMFT for SrVO3 with VASP + TRIQS/modest
# ============================================================================
#
# This script drives a full CSC DFT+DMFT calculation. The structure is two
# nested loops:
#
#   * outer loop (n_total_loops): one DFT charge update per iteration. VASP is
#     kept alive as a persistent background process by the dftkit VASP driver;
#     each outer iteration feeds the DMFT charge-density correction back into
#     VASP and reads the updated Kohn-Sham Hamiltonian / projectors.
#   * inner loop (n_dmft_loops): the DMFT self-consistency cycle (build the
#     hybridization, solve the impurity with CT-SEG, update the self-energy)
#     for the current DFT Hamiltonian.
#
# The run always ends on a DMFT step: after the last outer iteration the DFT
# code is not invoked again (see the guard around the charge update below).
#
# Required inputs in this directory: INCAR, POSCAR, POTCAR, KPOINTS and the
# PLOVasp projector definition plo.cfg.

import numpy as np

import triqs.utility.mpi as mpi

from triqs.gfs import BlockGf, MeshImFreq

from triqs_ctseg.solve_generic import solve_generic

import triqs_modest as M

from triqs_dftkit.vasp import Driver as VaspDriver, MPIHandler
from triqs_modest.dft_driver import DftDriver

from h5 import HDFArchive


# --- Physical and run parameters ---------------------------------------------
seedname = "vasp"
beta = 20.0          # inverse temperature (1/eV)
U    = 4.50          # Kanamori intra-orbital interaction (eV)
J    = 0.65          # Hund's coupling (eV)
Up   = U -2*J        # inter-orbital interaction (rotationally invariant choice)
n_iw = 1000          # number of Matsubara frequencies
n_total_loops = 10    # outer CSC (DFT charge) iterations
n_dmft_loops  =  1   # inner DMFT iterations per outer loop
n_iter_dft    =  2   # VASP SCF steps per outer loop (>1 for ALGO that needs
                     # several charge updates before DMFT; Sigma is held fixed
                     # and the charge correction is recomputed between steps)

# --- DFT driver --------------------------------------------------------------
# Wrap the dftkit VASP driver in the modest DftDriver. The VASP driver launches
# vasp_command under MPI and converts the output via PLOVasp (plo.cfg).
driver = DftDriver(VaspDriver(seedname=seedname, plo_cfg="plo.cfg",
                               mpi_handler=MPIHandler(mpi_exec="mpirun -np 16"),
                               vasp_command="vasp_std"))

# Run the initial DFT, convert the output, and return the target electron count
# together with the one-body elements (Kohn-Sham Hamiltonian + projectors).
target_density, obe = driver.one_body_elements_from_dft()
mpi.report(obe)

# Build the embedding (correlated subspace) from the projector space.
E = M.make_embedding(obe.C_space); mpi.report(E.description(True))

# Local Kanamori interaction Hamiltonian on the impurity.
h_int = M.make_kanamori(E.sigma_names, E.imp_decomposition(0), U, Up, J, False, False)

# Double-counting correction (Held's fully-localized-limit flavour).
DcTerm = M.DcSolver("NonPolarized", "cHeld", U, J)

mesh = MeshImFreq(beta=beta, S = "Fermion", n_iw=n_iw)

# DFT-only chemical potential and local Green's function, used to detect the
# degenerate block structure that the impurity quantities are symmetrized over.
mu_dft = M.find_chemical_potential(target_density, obe, beta, verbosity=False)
Gdft = E.extract(M.local_gf.gloc(mesh, obe, mu_dft))[0]
mpi.report(f"Gdft density= {Gdft.total_density().real}")

deg_blocks = M.analyze_degenerate_blocks(Gdft)
mpi.report(f"deg_blocks= {deg_blocks}")

# Initialise the self-energy: zero dynamic part plus the static double counting.
Sigma_imp_dc = DcTerm.dc_self_energy(Gdft)

Sigma_imp_dynamic, Sigma_imp_static = E.make_zero_imp_self_energies(mesh)[0]
for i in range(len(Sigma_imp_static)): Sigma_imp_static[i] += Sigma_imp_dc[i]

# DFT + DMFT loop
try:
    for n_iter in range(n_total_loops):
        mpi.report(f"\n{'='*60}\n=== Global DFT+DMFT iteration {n_iter+1}/{n_total_loops} ===\n{'='*60}")

        Hloc0 = E.extract(M.atomic_levels_and_delta.impurity_levels(obe))[0]
        mpi.report(f"Hloc0= {[h[0,0].real for h in Hloc0]}")

        # for first iteration converge Sigma imp first
        if n_iter == 0:
            n_dmft_loops_loc = 4
        else:
            n_dmft_loops_loc = n_dmft_loops
        # Begin DMFT loop
        for n_dmft_iter in range(n_dmft_loops_loc):
            mpi.report(f"\n--- DMFT iteration {n_dmft_iter+1}/{n_dmft_loops_loc} "
                       f"(global iter {n_iter+1}/{n_total_loops}) ---")

            Sigma_imp_static_minus_dc = [block-dc for (block, dc) in zip(Sigma_imp_static, Sigma_imp_dc)]

            Sigma_C_dynamic, Sigma_C_static = E.embed([Sigma_imp_dynamic], [Sigma_imp_static_minus_dc])

            mu = M.find_chemical_potential(target_density, obe, Sigma_C_dynamic, Sigma_C_static)

            Gloc = E.extract(M.local_gf.gloc(obe, mu, Sigma_C_dynamic, Sigma_C_static))[0]

            Eimp = [h-mu*np.eye(h.shape[0])-dc for (h,dc) in zip(Hloc0, Sigma_imp_dc)]

            Delta = M.symmetrize(M.hybridization(Eimp, Gloc, Sigma_imp_dynamic, Sigma_imp_static), deg_blocks)

            solver_params = dict(n_iw=n_iw, n_tau=10*n_iw, length_cycle=50,
                                 n_cycles = int(4e+6/mpi.size),
                                 n_warmup_cycles = int(1e+4),
                                 perform_tail_fit=True,
                                 fit_min_w=10, fit_max_w=14,
                                 )
            solver_results = solve_generic(Delta, Eimp, h_int, **solver_params)

            Sigma_imp_dynamic, Sigma_imp_static = solver_results.Sigma_dynamic, solver_results.Sigma_HartreeFock

            solver_results.G_iw         <<  M.symmetrize(solver_results.G_iw,          deg_blocks)
            solver_results.Sigma_iw     <<  M.symmetrize(solver_results.Sigma_iw,      deg_blocks)
            solver_results.Sigma_dynamic << M.symmetrize(solver_results.Sigma_dynamic, deg_blocks)
        # End of DMFT loop
        # Update Double-counting Term
        Sigma_imp_dc = DcTerm.dc_self_energy(solver_results.G_iw)
        Eimp_dc      = DcTerm.dc_energy(solver_results.G_iw)

        Sigma_imp_static_minus_dc = [block-dc for (block, dc) in zip(Sigma_imp_static, Sigma_imp_dc)]

        # Re-embed the self-energy
        Sigma_C_dynamic, Sigma_C_static = E.embed([Sigma_imp_dynamic], [Sigma_imp_static_minus_dc])

        # Re-compute the chemical potential
        mu = M.find_chemical_potential(target_density, obe, Sigma_C_dynamic, Sigma_C_static)

        # Compute the charge density correction
        N_k = M.charge_density_correction(obe, mu, Sigma_C_dynamic, Sigma_C_static)

        # Impurity interaction energy
        Eint = 0.5 * np.real((solver_results.Sigma_iw*solver_results.G_iw).total_density())
        mpi.report(f"Eint= {Eint}")
        Eint_m_dc = (Eint - Eimp_dc)
        mpi.report(f"Eint-Edc= {Eint_m_dc}")

        mpi.report("Saving DFT + DMFT iteration...")
        if mpi.is_master_node():
            with HDFArchive("checkpoint.h5", "a") as ar:
                path = f"it={len(ar) + 1}"
                ar.create_group(path)
                ar[path]["mu"]              = mu
                ar[path]["nloc"]            = Gloc.total_density().real
                ar[path]["nimp"]            = solver_results.G_iw.total_density().real
                ar[path]["Delta_iw"]        = Delta
                ar[path]["Eimp"]            = Eimp
                ar[path]["Gloc_iw"]         = Gloc
                ar[path]["Gimp_iw"]         = solver_results.G_iw
                ar[path]["Sigma_iw"]        = solver_results.Sigma_iw
                ar[path]["Sigma_iw_static"] = solver_results.Sigma_HartreeFock
                ar[path]["Sigma_dc"]        = Sigma_imp_dc

        # Update the one-body Hamiltonian with the charge density correction.
        # Skip on the last outer iteration: the calculation must end on a DMFT
        # step, so there is no point triggering another VASP charge update whose
        # result would never be used.
        if n_iter < n_total_loops - 1:
            mpi.report(f"Calling VASP charge update / DFT driver "
                       f"(global iter {n_iter+1}/{n_total_loops})...")
            # Run n_iter_dft VASP charge-update steps with the self-energy held
            # fixed. Between steps the projectors change, so recompute mu and the
            # charge density correction from the freshly converted one-body
            # elements. The last step is followed directly by DMFT, which
            # recomputes everything, so no recompute is needed there.
            for i_dft in range(n_iter_dft):
                obe = driver.update_one_body_elements_with_charge_correction(N_k, Eint_m_dc)[1]
                if i_dft == n_iter_dft - 1:
                    break
                Sigma_C_dynamic, Sigma_C_static = E.embed([Sigma_imp_dynamic], [Sigma_imp_static_minus_dc])
                mu = M.find_chemical_potential(target_density, obe, Sigma_C_dynamic, Sigma_C_static)
                N_k = M.charge_density_correction(obe, mu, Sigma_C_dynamic, Sigma_C_static)

finally:
    # Ensure VASP is killed even if the script crashes
    driver.driver.kill()
