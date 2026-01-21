"""
CT-HYB solver interface with optional post-processing strategies.

This module provides a functional API to the triqs_cthyb solver with:
- Dynamic dispatch based on mesh type (MeshImFreq, MeshDLRImFreq)
- Optional post-processing strategies for self-energy extraction
- Always-on density matrix measurement for moment access
"""

from __future__ import annotations

import numpy as np
from triqs.gf import (
    MeshImFreq, MeshDLRImFreq, BlockGf,
    make_gf_from_fourier, fit_hermitian_tail, make_hermitian,
    make_gf_imfreq, make_gf_dlr_imfreq, make_gf_imtime, 
    fit_gf_dlr, inverse, iOmega_n,
)
from triqs.gf.tools import make_zero_tail
from triqs.operators import Operator
from triqs_cthyb.solver import Solver
import triqs.utility.mpi as mpi

from .utils import SolverResults, matrix_to_many_body_operator
from .postprocessing import (
    TailFitParams, LegendreParams, CRMParams, PostProcessParams,
    apply_postprocessing,
)


# =============================================================================
# Internal Helpers
# =============================================================================

def _extract_interface_params(solver_params: dict) -> tuple[int, int, dict]:
    """Extract and remove interface-specific params from solver_params.
    
    Returns (n_iw, n_tau, n_l, remaining_params).
    """
    params = solver_params.copy()
    n_tau = params.pop('n_tau', 10001)
    n_l = params.pop('n_l', 40)
    return n_tau, n_l, params


def _prepare_solver(
    Delta_iw: BlockGf,
    hloc0_bl_mat: list[np.ndarray],
    n_iw: int,
    n_tau: int,
    n_l: int,
    solver_params: dict,
    postprocess: str | PostProcessParams | None,
) -> tuple[Solver, Operator, dict]:
    """Prepare the CT-HYB solver instance and parameters.
    
    Parameters
    ----------
    Delta_iw : BlockGf
        Hybridization function Δ(iω).
    hloc0_bl_mat : list[np.ndarray]
        Local non-interacting Hamiltonian as list of matrices per block.
    n_iw : int
        Number of Matsubara frequencies.
    n_tau : int
        Number of imaginary time points.
    n_l : int
        Number of Legendre polynomials.
    solver_params : dict
        Parameters to pass to solver.solve().
    postprocess : str | PostProcessParams | None
        Post-processing strategy (used to configure solver appropriately).
        
    Returns
    -------
    S : Solver
        The CT-HYB solver instance.
    h_loc0 : Operator
        Local Hamiltonian as many-body operator.
    solver_params : dict
        Updated solver parameters.
    """
    gf_struct = [(bl, gf.target_shape[0]) for (bl, gf) in Delta_iw]
    h_loc0 = matrix_to_many_body_operator(hloc0_bl_mat, gf_struct)
    beta = Delta_iw.mesh.beta
    
    # Always measure density matrix for Sigma_moments access
    solver_params = solver_params.copy()
    solver_params['measure_density_matrix'] = True
    solver_params['use_norm_as_weight'] = True
    
    # Configure Legendre measurement if using measured coefficients
    if isinstance(postprocess, LegendreParams) and postprocess.use_measured:
        solver_params['measure_G_l'] = True
    
    S = Solver(
        gf_struct=gf_struct,
        beta=beta,
        n_iw=n_iw,
        n_tau=n_tau,
        n_l=n_l,
        delta_interface=True,
    )
    
    return S, h_loc0, solver_params


def _prepare_delta_tau_imfreq(Delta_iw: BlockGf, S: Solver) -> None:
    """Fourier transform Δ(iω) to Δ(τ) with hermitian tail fitting (in-place)."""
    def _fourier_with_tail(G):
        tail = fit_hermitian_tail(G, make_zero_tail(G, 1))[0]
        return make_gf_from_fourier(G, S.Delta_tau[0].mesh, tail)
    
    for block, _ in S.Delta_tau:
        S.Delta_tau[block] << _fourier_with_tail(Delta_iw[block])


def _prepare_delta_tau_dlr(Delta_iw: BlockGf, S: Solver) -> None:
    """Convert Δ from DLR mesh to τ mesh (in-place)."""
    S.Delta_tau << make_gf_imtime(Delta_iw, S.n_tau)


def _build_G0_iw(Delta_iw: BlockGf, hloc0_bl_mat: list[np.ndarray]) -> BlockGf:
    """Construct G₀(iω) = [iω - h_loc0 - Δ(iω)]⁻¹."""
    G0_iw = Delta_iw.copy()
    for idx, (bl, g) in enumerate(G0_iw):
        G0_iw[bl] << inverse(iOmega_n - hloc0_bl_mat[idx] - Delta_iw[bl])
    return G0_iw



# =============================================================================
# Main Solve Function
# =============================================================================

def solve(
    Delta_iw: BlockGf,
    h_loc0: list[np.ndarray],
    h_int: Operator,
    postprocess: str | PostProcessParams | None = None,
    **solver_params,
) -> SolverResults:
    """Solve the quantum impurity problem using CT-HYB.
    
    Parameters
    ----------
    Delta_iw : BlockGf
        Hybridization function Δ(iω). Mesh type determines preprocessing:
        - MeshImFreq: Fourier transform with tail fitting
        - MeshDLRImFreq: Direct DLR to τ conversion
    h_loc0 : list[np.ndarray]
        Local non-interacting Hamiltonian as list of matrices per block.
    h_int : Operator
        Interaction Hamiltonian (e.g., Hubbard U term).
    postprocess : str | PostProcessParams | None, optional
        Post-processing strategy for self-energy extraction:
        - None: Return raw solver output (G_iw, G_tau, moments)
        - 'dyson': Plain Dyson equation Σ = G₀⁻¹ - G⁻¹
        - TailFitParams(...): Use solver's internal tail fitting
        - LegendreParams(...): Legendre filtering then Dyson
        - CRMParams(...): Constrained Residual Minimization
    **solver_params
        Parameters passed to cthyb solver. Key parameters:
        - n_iw : int - Number of Matsubara frequencies (required for MeshDLRImFreq)
        - n_tau : int - Number of τ points (default: 10001)
        - n_l : int - Number of Legendre polynomials (default: 40)
        - length_cycle : int - MC cycle length
        - n_cycles : int - Number of MC cycles per MPI rank
        - n_warmup_cycles : int - Warmup cycles
        
    Returns
    -------
    SolverResults
        Container with solver output. Contents depend on postprocess:
        - postprocess=None: G_iw, G_tau, Sigma_HartreeFock, Sigma_moments, density_matrix, Solver
        - postprocess specified: Above plus Sigma_iw, Sigma_dynamic, and strategy-specific fields
        
    Examples
    --------
    Raw output for custom post-processing:
    
    >>> result = solve(Delta_iw, h_loc0, h_int, n_iw=1000, length_cycle=100, n_cycles=1000000)
    >>> result.G_tau  # Access raw G(τ) for custom analysis
    
    With plain Dyson equation:
    
    >>> result = solve(Delta_iw, h_loc0, h_int, postprocess='dyson', ...)
    >>> result.Sigma_iw  # Full self-energy
    
    With CRM post-processing:
    
    >>> from triqs_modest.solver_interfaces import CRMParams
    >>> result = solve(Delta_iw, h_loc0, h_int, postprocess=CRMParams(w_max=15.0, eps=1e-12), ...)
    >>> result.Sigma_dlr  # Self-energy in DLR representation
    """
    # Extract interface params
    n_tau, n_l, solver_params = _extract_interface_params(solver_params)
    
    mesh = Delta_iw.mesh

    # Determine n_iw 
    n_iw = len(mesh) // 2 if isinstance(mesh, MeshImFreq) else int(2*mesh.values()[-1].n) # conservative estimate for n_iw
    
    # Prepare solver
    S, h_loc0_op, solver_params = _prepare_solver(
        Delta_iw, h_loc0, n_iw, n_tau, n_l, solver_params, postprocess
    )
    
    # Prepare Δ(τ) based on mesh type
    if isinstance(mesh, MeshImFreq):
        _prepare_delta_tau_imfreq(Delta_iw, S)
    elif isinstance(mesh, MeshDLRImFreq):
        _prepare_delta_tau_dlr(Delta_iw, S)
    else:
        raise NotImplementedError(f"Unsupported mesh type: {type(mesh)}")
    
    # Solve
    mpi.report(f"Solving impurity problem with CT-HYB (postprocess={postprocess})")
    S.solve(h_loc0=h_loc0_op, h_int=h_int, **solver_params)
    
    # Post-process or return raw results
    if postprocess is None:
        return SolverResults(Solver=S)
    
    # Build G0 for post-processing
    G0_iw = _build_G0_iw(Delta_iw, h_loc0)
    
    # For MeshDLRImFreq, we need G0 on ImFreq mesh for some strategies
    if isinstance(mesh, MeshDLRImFreq) and not isinstance(postprocess, CRMParams):
        G0_iw = make_gf_imfreq(G0_iw, n_iw=n_iw)
    
    # Apply post-processing
    pp_result = apply_postprocessing(S, G0_iw, postprocess)
    
    # Build final results, filtering out None values
    # Always include the raw Solver for access to pre-processed data
    result_kwargs = {k: v for k, v in pp_result.items() if v is not None}
    result_kwargs['Solver'] = S
    
    return SolverResults(**result_kwargs)