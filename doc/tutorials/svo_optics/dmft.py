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
U, J = 4.5, 0.68 # eV
Up   = U - 2*J
beta = 40        # 1/eV
n_iw = int(beta*10)

mesh = MeshImFreq(beta=beta, statistic='Fermion', n_iw=n_iw)

n_dmft_loops = 25
conv_crit    = 1e-3

# ---- solver setup ---
solver_params = dict(
        n_tau = 10*n_iw,
        length_cycle = 800,
        n_cycles =int(5e7/mpi.size),
        n_warmup_cycles=int(4e4),
        perform_tail_fit=True 
        )
tail_fit_params = TailFitParams(fit_min_w=10, fit_max_w=14)

# --- dmft checkpoitng ---
ckpt = Checkpointer(f"svo_beta{beta}_U{U}_J{J}.ckpt")
# --- one-body elements ---
target_density, obe = tm.one_body_elements_from_dft_converter("svo_dft_data.h5")
mpi.report(f"target_density= {target_density}")
mpi.report(obe)

# --- embedding ---
E = tm.make_embedding(obe.C_space)
mpi.report(E.description(True))

# --- find symmetries --- 
mu = tm.find_chemical_potential(target_density, obe, beta, verbosity=False);
Gloc = E.extract(tm.gloc(mesh, obe, mu))[0]
deg_blocks = tm.analyze_degenerate_blocks(Gloc)
mpi.report(f"degenerate blocks= {deg_blocks}")

# --- effecitve levels
epsilon_d = E.extract(tm.impurity_levels(obe))[0]

# --- interaction hamiltonian --- 
h_int = tm.make_kanamori(E.sigma_names, E.imp_decomposition(0), U, Up, J)

# --- nominal double counting ---
nominal_d = 1.0
sig_dc = (U-2*J)*(nominal_d-0.5)

# --- initial self-energies
if (prev := ckpt.restart()):
    Sigma_imp_dyn, Sigma_imp_hf = prev.Sigma_imp_list[0], prev.Sigma_hartree_list[0]
else:
    Sigma_imp_dyn, Sigma_imp_hf = E.make_zero_imp_self_energies(mesh)[0]
    for ibl, bl in enumerate(Sigma_imp_hf):
        Sigma_imp_hf[ibl] += sig_dc

# --- dmft loop --- 
for it in range(n_dmft_loops):
    
    Sigma_imp_hf_m_dc = [sig-sig_dc for sig in Sigma_imp_hf]
    Sigma_C = E.embed([Sigma_imp_dyn], [Sigma_imp_hf_m_dc])                                                  # Embed self-energy

    mu = tm.find_chemical_potential(target_density, obe, *Sigma_C, verbosity=False)                          # Find chemical potential

    Gloc = E.extract(tm.gloc(obe, mu, *Sigma_C))[0]                                                          # Compute Gloc

    ed = [block.real - mu - sig_dc for block in epsilon_d]                                                   # Update impurity levels

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
    if diff < conv_crit:
        mpi.report(f"Converged after {it + 1} iterations.")
        break
