import numpy as np; np.set_printoptions(3)

# TRIQS
import triqs.utility.mpi as mpi
from triqs.operators import n, c, c_dag
from triqs.gfs import MeshImFreq

# TRIQS/cthyb
from triqs_cthyb import solve_generic, TailFitParams

# TRIQS/modest
import triqs_modest as tm
from triqs_modest.utils import Checkpointer, IterationData

# ---- parameters ---
U, J = 2.0, 0.20 # eV
beta = 40        # 1/eV
n_iw = int(beta*10)

mesh = MeshImFreq(beta=beta, statistic='Fermion', n_iw=n_iw)

n_dmft_loops = 1

# ---- solver setup ---
solver_params = dict(
        n_tau = 10*n_iw,
        length_cycle = 800,
        n_cycles =int(1e7/mpi.size),
        n_warmup_cycles=int(1e4),
        perform_tail_fit=True 
        )
tail_fit_params = TailFitParams(fit_min_w=4, fit_max_w=8)

# --- dmft checkpoitng ---
ckpt = Checkpointer(f"beta{beta}_U{U}_J{J}.ckpt")

# --- one-body elements ---
target_density, obe = tm.one_body_elements_from_dft_converter("Sr2MgOsO6.h5", diagonalize_hloc=True, threshold=-1)
mpi.report(f"target_density= {target_density}")
mpi.report(obe)

# --- embedding ---
E = tm.make_embedding(obe.C_space).split_imp_block(0,0,[1,1,1,2]).split_imp(0, [0,1,2]).drop_imp(1)
mpi.report(E.description(True))

# --- find symmetries --- 
mu = tm.find_chemical_potential(target_density, obe, beta, verbosity=False);
Gloc = E.extract(tm.gloc(mesh, obe, mu))[0]
deg_blocks = tm.analyze_degenerate_blocks(Gloc)
mpi.report(f"degenerate blocks= {deg_blocks}")

# --- effecitve levels
epsilon_d = E.extract(tm.impurity_levels(obe))[0]

# --- interaction hamiltonian --- 
from triqs.operators.util.U_matrix import U_matrix_slater, transform_U_matrix, spherical_to_cubic
from triqs.operators.util.hamiltonians import h_int_slater

R_mat  = list(obe.C_space.rotation_from_dft_to_local_basis)[0]
U_mat = U_matrix_slater(l=2, U_int=U, J_hund=J, basis='spherical')
U_mat = transform_U_matrix(U_mat, spherical_to_cubic(l=2, convention='wien2k'))
U_mat = transform_U_matrix(U_mat, R_mat.conjugate().T)
Uimp  =  (E.merge_embed_block_by_imp.slice_sigma).extract([U_mat])[0][0].real
spin_names = E.sigma_names; n_orb = len(E.imp_decomposition(0));
map_to_solver = { (sp, o) : (f'{sp}_{o}',0) for sp in spin_names for o in range(n_orb) }
h_int = h_int_slater(spin_names, n_orb, Uimp, map_operator_structure=map_to_solver)

# --- double counting ---
dc = tm.DcSolver("NonPolarized", "cFLL", U, J)
Sigma_dc = dc.dc_self_energy(Gloc)

# --- initial self-energies
if (prev := ckpt.restart()):
    Sigma_imp_dyn, Sigma_imp_hf = prev.Sigma_imp_list[0], prev.Sigma_hartree_list[0]
else:
    Sigma_imp_dyn, Sigma_imp_hf = E.make_zero_imp_self_energies(mesh)[0]
    for ibl, bl in enumerate(Sigma_imp_hf):
        Sigma_imp_hf[ibl] += Sigma_dc[ibl]

# --- dmft loop --- 
for it in range(n_dmft_loops):
    
    Sigma_imp_hf_m_dc = [sig-sig_dc for (sig,sig_dc) in zip(Sigma_imp_hf,Sigma_dc)]

    Sigma_C = E.embed([Sigma_imp_dyn], [Sigma_imp_hf_m_dc])                                                  # Embed self-energy

    mu = tm.find_chemical_potential(target_density, obe, *Sigma_C, verbosity=False)                          # Find chemical potential

    Gloc = E.extract(tm.gloc(obe, mu, *Sigma_C))[0]                                                          # Compute Gloc

    ed = [(block - mu - sig_dc).real for (block,sig_dc) in zip(epsilon_d,Sigma_dc)]                          # Update impurity levels

    Delta = tm.symmetrize(tm.hybridization(ed, Gloc, Sigma_imp_dyn, Sigma_imp_hf), deg_blocks)               # Compute Δ

    res = solve_generic(Delta, ed, h_int, postprocess=tail_fit_params, **solver_params)                      # Solve F(Δ, ϵd, Hint) -> (Σimp, Gimp)

    Sigma_imp_dyn = tm.symmetrize(res.Sigma_dynamic, deg_blocks)                                             # Update Σimp
    Sigma_imp_hf  = tm.symmetrize(res.Sigma_HartreeFock, deg_blocks)
    Gimp = tm.symmetrize(res.G_iw, deg_blocks)

    ckpt.append(IterationData(mu=mu,                                                                         # Save this iteration
                           Sigma_imp_list=[Sigma_imp_dyn],
                           Sigma_hartree_list=[Sigma_imp_hf],
                           Gimp_list=[Gimp], Gloc_list=[Gloc], Delta_list=[Delta]
                           )
                )
    diff = abs(Gloc.total_density().real-res.G_iw.total_density().real)
    mpi.report(f"[iter {it}] mu = {mu:.6f}  |Δn residual| = {diff:.3e}")                              # log difference
