import numpy as np
from triqs.gfs import MeshImFreq, MeshDLRImFreq, make_gf_from_fourier, fit_hermitian_tail, make_hermitian
from triqs.gfs import make_gf_dlr, make_gf_imfreq, make_gf_dlr_imfreq, make_gf_imtime, fit_gf_dlr, inverse, iOmega_n
from triqs.gfs.dlr_crm_dyson_solver import minimize_dyson
from triqs.gfs.tools import make_zero_tail
from triqs.operators import c_dag, c
from triqs_cthyb.solver import Solver
import triqs.utility.mpi as mpi

class SolverResults(dict):
    def __init__(self, **kwargs): super().__init__(kwargs)

    def __getattr__(self, key):
        try: return self[key]
        except KeyError: raise AttributeError(f"SolverResults has no property {key}")

    def __repr__(self):
        return "\n".join([f"{key:<12}" for key in self.keys() ])

    __str__ = __repr__


def matrix_to_many_body_operator(h_loc_matrix, gf_struct):
    h_loc0_mat = {block : h_loc_matrix[ibl].real for ibl, (block, _) in enumerate(gf_struct) }
    c_dag_vec  = {block : np.matrix([[c_dag(block, o) for o in range(bl_size)]]) for block, bl_size in gf_struct }
    c_vec      = {block : np.matrix([[c(block, o) for o in range(bl_size)]]) for block, bl_size in gf_struct }
    return sum(c_dag_vec[bl]*h_loc0_mat[bl]*c_vec[bl].T for bl, bl_size in gf_struct)[0,0]

def prepare_solver(Delta_iw, hloc0_bl_mat, solver_interface_params):

    gf_struct = [(bl, gf.target_shape[0]) for (bl, gf) in Delta_iw]
    h_loc0 = matrix_to_many_body_operator(hloc0_bl_mat, gf_struct)

    beta = Delta_iw.mesh.beta
    n_iw = solver_interface_params.pop('n_iw', None)
    assert n_iw is not None, "The length of the Mastsubara mesh is not defined!"

    n_tau = solver_interface_params.pop('n_tau', 10001)
    n_l  = solver_interface_params.pop('n_l', 40)

    # measure the density matrix as default for Sigma_Hartree
    mpi.report("measuring the density matrix as default for self-energy moments.")
    solver_interface_params['measure_density_matrix'] = True
    solver_interface_params['use_norm_as_weight']     = True

    S = Solver(gf_struct=gf_struct, beta=beta, n_iw=n_iw, n_tau=n_tau, n_l=n_l, delta_interface=True)

    return S, h_loc0, solver_interface_params

def build_solver_results(S):
    return SolverResults(G_iw=S.G_iw, G_tau=S.G_tau, Sigma_Hartree=list(S.Sigma_Hartree.values()), Sigma_iw=S.Sigma_iw, Sigma_dynamic=S.Sigma_dynamic, Solver=S)

def SolveMeshImFreq(Delta_iw, hloc0_bl_mat, h_int, **solver_interface_params):

    solver_interface_params.pop('n_iw', None)  # remove n_iw from solver interface params if present
    solver_interface_params['n_iw'] = len(Delta_iw.mesh) // 2

    S, h_loc0, solver_interface_params = prepare_solver(Delta_iw, hloc0_bl_mat, solver_interface_params)

    # Fourier transform Δ(iω) to Δ(τ)
    make_gf_from_fourier_with_hermitian_tail = lambda G : make_gf_from_fourier(G, S.Delta_tau[0].mesh, fit_hermitian_tail(G, make_zero_tail(G,1))[0])
    for block, Delta in S.Delta_tau: S.Delta_tau[block] << make_gf_from_fourier_with_hermitian_tail(Delta_iw[block])

    # call the solver with this Δ
    S.solve(h_loc0=h_loc0, h_int=h_int, **solver_interface_params)

    S.Sigma_dynamic = S.Sigma_iw.copy()
    for bl, g in S.Sigma_dynamic: S.Sigma_dynamic[bl] << g - S.Sigma_Hartree[bl]

    return build_solver_results(S)


def SolveMeshDLRImFreq(Delta_iw, hloc0_bl_mat, h_int, **solver_interface_params):

    S, h_loc0, solver_interface_params = prepare_solver(Delta_iw, hloc0_bl_mat, solver_interface_params)

    S.Delta_tau << make_gf_imtime(Delta_iw, S.n_tau)
        
    S.solve(h_loc0=h_loc0, h_int=h_int, **solver_interface_params)

    G_iw_dlr  = make_gf_dlr_imfreq(fit_gf_dlr(S.G_tau, Delta_iw.mesh.w_max, Delta_iw.mesh.eps))
    G0_iw_dlr = Delta_iw.copy()
    for idx, (bl, g) in enumerate(G0_iw_dlr): G0_iw_dlr[bl] << inverse(iOmega_n - hloc0_bl_mat[idx] - Delta_iw[bl])

    Sigma_dynamic, Sigma_hartree, err = minimize_dyson(G0_iw_dlr, G_iw_dlr, S.Sigma_moments)

    Sigma_iw = make_gf_imfreq(Sigma_dynamic, S.n_iw)
    for block, g in Sigma_iw: g += S.Sigma_Hartree[block]

    S.Sigma_dynamic = Sigma_dynamic
    S.Sigma_iw << Sigma_iw

    return build_solver_results(S)


def solve(Delta_iw, h_loc0, h_int, **solver_params):
    if isinstance(Delta_iw.mesh, MeshImFreq): 
        return SolveMeshImFreq(Delta_iw, h_loc0, h_int, **solver_params)
    elif isinstance(Delta_iw.mesh, MeshDLRImFreq): 
        return SolveMeshDLRImFreq(Delta_iw, h_loc0, h_int, **solver_params)
    else: raise NotImplementedError