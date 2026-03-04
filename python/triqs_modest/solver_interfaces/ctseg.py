"""
CT-SEG solver interface with optional post-processing strategies.

This module provides a functional API to the triqs_ctseg solver with:
- Dynamic dispatch based on mesh type (MeshImFreq, MeshDLRImFreq)
- Dynamic or static Coulomb interactions
- Optional post-processing strategies for self-energy and polarization extraction

"""

import triqs.utility.mpi as mpi
import numpy as np

from triqs.gf import (
    Gf, BlockGf, Block2Gf, 
    MeshDLRImFreq, MeshImFreq, make_gf_from_fourier, 
    make_gf_imtime, fit_hermitian_tail
)
from triqs.gf.tools import make_zero_tail
from triqs.operators.util.extractors import block_matrix_from_op

from triqs_ctseg import Solver
from .utils import SolverResults, fill_dlr_imfreq_gf
from .ctseg_utils import postprocess


def _prepare_solver(
    Delta_iw : BlockGf, 
    h_loc0 : Operator, 
    solve_density_only : bool = False, 
    **solver_interface_params : dict
) -> tuple[Solver, dict, dict] | tuple[Solver, dict]:
    """Prepare the CT-SEG solver instance and parameters.

    Parameters
    ----------
    Delta_iw : BlockGf
        Hybridization function Δ(iω).
    h_loc0 : Operator
        Local one-body Hamiltonian.
    solve_density_only : bool, optional
        If True, measurements of non-density observables are turned off explicitly. Default is False
    solver_interface_params : dict
        Additional parameters for solver and post-processing. 

    Returns
    -------
    S : Solver
        The CT-SEG solver instance.
    solver_interface_params : dict
        Updated solver interface parameters
    post_proc_params : dict (optional if solve_density_only=False)
        Parameters relevant for post-processing steps.
    """
    gf_struct = [(bl, gf.target_shape[0]) for (bl, gf) in Delta_iw]
    mesh = Delta_iw.mesh
    beta = mesh.beta

    solver_params = solver_interface_params.copy()

    n_iw = solver_params.pop('n_iw', 1025)
    n_tau = solver_params.pop('n_tau', 10001)
    n_tau_bosonic = solver_params.pop('n_tau_bosonic', n_tau)

    # Always measure dnnsities, F(tau) and charge susceptibilities
    solver_params['measure_densities'] = True
    solver_params['measure_G_tau']     = False if solve_density_only else True
    solver_params['measure_F_tau']     = False if solve_density_only else True
    if isinstance(mesh, MeshDLRImFreq):
        solver_params['measure_nn_nu']     = False if solve_density_only else True
    else:
        solver_params['measure_nn_tau']    = False if solve_density_only else True

    if isinstance(mesh, MeshDLRImFreq):
        # 
        solver_params.setdefault('dlr_omega_max', mesh.w_max)
        solver_params.setdefault('dlr_epsilon', mesh.eps)
        # check if n_iw is compatible with dlr mesh
        mesh_dlr_idx = np.array([iw.index for iw in mesh])
        max_dlr_idx = max(abs(mesh_dlr_idx[0]), abs(mesh_dlr_idx[-1]))
        if max_dlr_idx > n_iw:
            mpi.report(f"WARNING: n_iw = {n_iw} is smaller than the maximum DLR frequency index"
                       f" ({max_dlr_idx}). Setting n_iw to {max_dlr_idx+1}.")
            n_iw = max_dlr_idx+1

    # Prepare solver instance
    S = Solver(gf_struct=gf_struct, beta=beta, n_tau=n_tau, n_tau_bosonic=n_tau_bosonic)
    # for post-processing
    S.n_iw, S.beta, S.gf_struct = n_iw, beta, gf_struct
    S.h_int = h_int
    S.h_loc0_mat = block_matrix_from_op(h_loc0, gf_struct)

    # Pop post-processing parameters out of solver_params
    post_proc_params = {}
    post_proc_params['perform_tail_fit'] = solver_params.pop('perform_tail_fit', False)
    post_proc_params['fit_max_moment']   = solver_params.pop('fit_max_moment', 3)
    post_proc_params['fit_min_w']        = solver_params.pop('fit_min_w', None)
    post_proc_params['fit_max_w']        = solver_params.pop('fit_max_w', None)
    post_proc_params['fit_min_n']        = solver_params.pop('fit_min_n', None)
    post_proc_params['fit_max_n']        = solver_params.pop('fit_max_n', None)
    post_proc_params['analytic_hf']      = solver_params.pop('analytic_hf', False)
    post_proc_params['degenerate_blk']   = solver_params.pop('degenerate_blk', None)
    post_proc_params['truncate_uchi']    = solver_params.pop('truncate_uchi', False)
    post_proc_params['dlr_w_max']        = solver_params.pop('dlr_omega_max', None)
    post_proc_params['dlr_eps']          = solver_params.pop('dlr_eps', None)

    # no post-processing if density is the only quantitiy of interest
    if solve_density_only:
        return S, solver_params
    else:
        return S, solver_params, post_proc_params
    

# =============================================================================
# Main Solve Function
# =============================================================================

def solve(
    Delta_iw: BlockGf, 
    h_loc0: Operator, 
    h_int: Operator, 
    D0_iw: Block2Gf | None = None, 
    symmetrize_func: callable | None = None,
    **solver_interface_params
) -> SolverResults:
    """Solve the impurity problem with dynamic interactions using CT-SEG

    Parameters
    ----------
    Delta_iw : BlockGf
        Hybridization function Δ(iω) in Matsubara frequencies (DLR mesh).
    h_loc0 : Operator
        Local one-body Hamiltonian.
    D0_iw : Block2Gf
        Screening function D0(iω) in Matsubara frequencies (DLR mesh).
    h_int : Operator
        Interaction Hamiltonian
    symmetrize_func : callable, optional
        Function to symmetrize the self-energy and polarization results. 
    **solver_interface_params : dict
        Additional parameters for solver and post-processing. Key parameters include:
        - n_iw : int, optional
            Number of Matsubara frequencies.
        - n_tau : int, optional
            Number of imaginary time τ points.
        - n_tau_bosonic : int, optional
            Number of imaginary time points for bosonic quantities.
    
    Returns
    -------
    SolverResults
        Container with solver output. 

    """
    # Prepare solver
    S, solver_params, post_proc_params = _prepare_solver(
        Delta_iw, h_loc0, solver_interface_params
    )

    # Prepare Δ(τ) and D0(τ) based on mesh type
    mesh = Delta_iw.mesh
    if isinstance(mesh, MeshDLRImFreq):
        # Δ(τ)
        S.Delta_tau << make_gf_imtime(Delta_iw, n_tau)
        if D0_iw is not None:
            # D0(τ)
            for name1, name2 in D0_iw.indices:
                S.D0_tau[name1, name2] << make_gf_imtime(D0_iw[name1, name2], n_tau)
    elif isinstance(mesh, MeshImFreq):
        # Δ(τ)
        for block, _ in S.Delta_tau:
            S.Delta_tau[block] << make_gf_from_fourier(
                Delta_iw[block],                                          # Δ(iω)
                S.Delta_tau[block].mesh,                                  # time mesh
                fit_hermitian_tail(Delta_iw[block], make_zero_tail(Delta_iw[block], 1))[0] # tail
            )
        if D0_iw is not None:
            # D0(τ)
            for name1, name2 in D0_iw.indices:
                S.D0_tau[name1, name2] << make_gf_from_fourier(
                    D0_iw[name1, name2],                                          # D0(iω)
                    S.D0_tau[name1, name2].mesh,                                  # time mesh
                    fit_hermitian_tail(D0_iw[name1, name2], make_zero_tail(D0_iw[name1, name2], n_moments=2))[0] # tail
                )
    else:
        raise NotImplementedError(f"Unsupported mesh type: {type(mesh)}")

    # Call solver
    mpi.report(f"Solving the impurity problem with CT-SEG")
    S.solve(h_loc0=h_loc0, h_int=h_int, **solver_params)

    # Post-processing
    pp_results = postprocess(S, symmetrize_func=symmetrize_func, **post_proc_params)

    # Build final results, filtering out None values
    # Always include the raw Solver for access to pre-processed data
    result_kwargs = {k: v for k, v in pp_result.items() if v is not None}
    result_kwargs['Solver'] = S
    
    return SolverResults(**result_kwargs)
    

def solve_density(
    Delta_iw : BlockGf, 
    h_loc0 : Operator, 
    h_int : Operator, 
    D0_iw : Block2Gf | None = None,
    **solver_interface_params):
    """
    Solve the impurity with dynamic interactions

    Delta_iw : triqs.BlockGf
    h_loc0: triqs.operator
    D0_iw : triqs.Block2Gf
    h_int: triqs.operator
    """
    mesh = Delta_iw.mesh
    if not isinstance(mesh, MeshImFreq) and not isinstance(mesh, MeshDLRImFreq):
        raise NotImplementedError(f"Unsupported mesh type: {type(mesh)}")

    # Prepare solver
    S, solver_params = _prepare_solver(
        Delta_iw, h_loc0, solve_density_only=True, **solver_interface_params 
    )

    # Prepare Δ(τ) and D0(τ) based on mesh type
    mesh = Delta_iw.mesh
    if isinstance(mesh, MeshDLRImFreq):
        # Δ(τ)
        S.Delta_tau << make_gf_imtime(Delta_iw, n_tau)
        if D0_iw is not None:
            # D0(τ)
            for name1, name2 in D0_iw.indices:
                S.D0_tau[name1, name2] << make_gf_imtime(D0_iw[name1, name2], n_tau)
    elif isinstance(mesh, MeshImFreq):
        # Δ(τ)
        for block, _ in S.Delta_tau:
            S.Delta_tau[block] << make_gf_from_fourier(
                Delta_iw[block],                                          # Δ(iω)
                S.Delta_tau[block].mesh,                                  # time mesh
                fit_hermitian_tail(Delta_iw[block], make_zero_tail(Delta_iw[block], 1))[0] # tail
            )
        if D0_iw is not None:
            # D0(τ)
            for name1, name2 in D0_iw.indices:
                S.D0_tau[name1, name2] << make_gf_from_fourier(
                    D0_iw[name1, name2],                                          # D0(iω)
                    S.D0_tau[name1, name2].mesh,                                  # time mesh
                    fit_hermitian_tail(D0_iw[name1, name2], make_zero_tail(D0_iw[name1, name2], n_moments=2))[0] # tail
                )
    else:
        raise NotImplementedError(f"Unsupported mesh type: {type(mesh)}")

    # Call solver
    mpi.report("Solving the impurity problem with CT-SEG for density")
    S.solve(h_loc0=h_loc0, h_int=h_int, **solver_params)

    return SolverResults(# required
        orbital_occupations = S.results.densities,
        average_order = S.results.average_order_Delta,
        average_sign = S.results.average_sign
    )
