"""
Post-processing strategies for extracting self-energy from QMC solver output.

Strategies:
- dyson: Plain Dyson equation Σ = G₀⁻¹ - G⁻¹
- tail_fit: Dyson equation followed by high-frequency tail fitting
- legendre: Fit G(τ) to Legendre polynomials, then Dyson
- crm: Constrained Residual Minimization via DLR representation
"""

from __future__ import annotations
from dataclasses import dataclass

import numpy as np
import triqs.utility.mpi as mpi
from triqs.gf import (
    BlockGf, Gf, MeshDLRImFreq,
    make_gf_imfreq, make_gf_dlr_imfreq, make_gf_dlr_imtime,
    fit_gf_dlr, make_hermitian, inverse,
)
from triqs.gf.gf_fnt import fit_hermitian_tail_on_window, replace_by_tail
from triqs_cthyb.solver import Solver

from .utils import SolverResults


# =============================================================================
# Strategy Parameter Dataclasses
# =============================================================================

@dataclass(frozen=True)
class TailFitParams:
    """Parameters for high-frequency tail fitting.
    
    Fits a 1/(iω)^n expansion to the high-frequency part of Σ(iω) and replaces
    the noisy high-frequency data with the fitted tail.
    
    Either specify the fitting window via Matsubara indices (fit_min_n, fit_max_n)
    or via frequency values (fit_min_w, fit_max_w). Frequency values take precedence.
    """
    fit_min_n: int | None = None
    """Minimum Matsubara index for tail fitting. Default: 80% of mesh."""
    fit_max_n: int | None = None
    """Maximum Matsubara index for tail fitting. Default: end of mesh."""
    fit_min_w: float | None = None
    """Minimum frequency for tail fitting (alternative to fit_min_n)."""
    fit_max_w: float | None = None
    """Maximum frequency for tail fitting (alternative to fit_max_n)."""
    fit_max_moment: int = 3
    """Maximum moment to fit in the tail expansion."""


@dataclass(frozen=True)
class LegendreParams:
    """Parameters for Legendre polynomial filtering/fitting.
    
    Can use either measured Legendre coefficients from solver (use_measured=True)
    or fit G(τ) to Legendre polynomials after the solve (use_measured=False).
    """
    n_l: int = 40
    """Number of Legendre polynomials."""
    use_measured: bool = False
    """If True, use solver's measured G_l; if False, fit G_tau to Legendre."""


@dataclass(frozen=True)
class CRMParams:
    """Parameters for Constrained Residual Minimization (CRM) Dyson solver.
    
    Uses DLR (Discrete Lehmann Representation) to fit G(τ), then minimizes
    the Dyson equation residual to extract Σ.
    """
    w_max: float = 10.0
    """DLR frequency cutoff (should match or exceed bandwidth)."""
    eps: float = 1e-10
    """DLR precision parameter."""


# Type alias for any post-processing params
PostProcessParams = TailFitParams | LegendreParams | CRMParams


# =============================================================================
# Legendre Filter Implementation
# =============================================================================

def legendre_filter(G_tau: BlockGf, n_l: int) -> BlockGf:
    """Fit G(τ) to Legendre polynomials to filter high-frequency noise.
    
    Parameters
    ----------
    G_tau : BlockGf
        Green's function in imaginary time.
    n_l : int
        Number of Legendre polynomials to use.
        
    Returns
    -------
    G_l : BlockGf
        Green's function in Legendre basis.
    """
    from triqs.gf import MeshLegendre, GfLegendre
    
    beta = G_tau.mesh.beta
    G_l = G_tau.copy()
    
    for block, g_tau in G_tau:
        # Create Legendre mesh and Gf
        mesh_l = MeshLegendre(beta=beta, S='Fermion', n_max=n_l)
        g_l = GfLegendre(mesh=mesh_l, target_shape=g_tau.target_shape)
        
        # Fit tau data to Legendre
        g_l.set_from_imtime(g_tau)
        
        # Store in output (we reuse the BlockGf structure but replace data)
        G_l[block] = g_l
        
    return G_l


def _set_gf_from_legendre(G_l: Gf, G_iw: Gf, G_tau: Gf) -> None:
    """Set G(iω) and G(τ) from Legendre representation G_l (in-place).
    
    Parameters
    ----------
    G_l : Gf
        Green's function in Legendre basis.
    G_iw : Gf
        Output: Green's function in Matsubara frequencies (modified in-place).
    G_tau : Gf
        Output: Green's function in imaginary time (modified in-place).
    """
    # Enforce correct discontinuity at τ=0 and τ=β
    G_l.enforce_discontinuity(np.identity(G_l.target_shape[0]))
    G_iw.set_from_legendre(G_l)
    G_tau.set_from_legendre(G_l)


# =============================================================================
# Tail Fitting Implementation
# =============================================================================

def tail_fit(Sigma_iw: BlockGf, params: TailFitParams, known_moments: dict | None = None) -> BlockGf:
    """Fit high-frequency tail of Σ(iω) and replace noisy data with fitted expansion.
    
    Fits a 1/(iω)^n expansion to the specified frequency window and replaces
    the high-frequency part of Σ(iω) with the fitted tail.
    
    Parameters
    ----------
    Sigma_iw : BlockGf
        Self-energy in Matsubara frequencies.
    params : TailFitParams
        Tail fitting parameters (frequency window, max moment).
    known_moments : dict, optional
        Known high-frequency moments per block. If None, no moments are fixed.
        
    Returns
    -------
    Sigma_iw : BlockGf
        Self-energy with high-frequency part replaced by fitted tail.
    """
    Sigma_iw = Sigma_iw.copy()
    
    # Determine fitting window
    fit_min_n = params.fit_min_n
    fit_max_n = params.fit_max_n
    
    # Convert frequency to index if specified
    for bl, sig in Sigma_iw:
        beta = sig.mesh.beta
        n_iw = len(sig.mesh) // 2
        
        if params.fit_min_w is not None:
            fit_min_n = int(0.5 * (params.fit_min_w * beta / np.pi - 1.0))
        if params.fit_max_w is not None:
            fit_max_n = int(0.5 * (params.fit_max_w * beta / np.pi - 1.0))
        
        # Apply defaults if not specified
        if fit_min_n is None:
            fit_min_n = int(0.8 * n_iw)
        if fit_max_n is None:
            fit_max_n = n_iw
        
        # Get known moments for this block
        if known_moments is not None and bl in known_moments:
            block_moments = known_moments[bl]
        else:
            block_moments = np.zeros([0] + list(sig.target_shape), dtype=complex)
        
        # Fit tail and replace high-frequency data
        tail, err = fit_hermitian_tail_on_window(
            sig,
            n_min=fit_min_n,
            n_max=fit_max_n,
            known_moments=block_moments,
            n_tail_max=10 * len(sig.mesh),  # Use all data in fit
            expansion_order=params.fit_max_moment,
        )
        replace_by_tail(sig, tail, n_min=fit_min_n)
    
    return Sigma_iw


# =============================================================================
# Post-Processing Strategy Functions
# =============================================================================

def postprocess_dyson(solver: Solver, G0_iw: BlockGf) -> dict:
    """Extract self-energy via plain Dyson equation: Σ = G₀⁻¹ - G⁻¹.
    
    Parameters
    ----------
    solver : Solver
        The cthyb solver after solve() has been called.
    G0_iw : BlockGf
        Non-interacting Green's function G₀(iω).
        
    Returns
    -------
    dict
        Dictionary with keys: G_iw, G_tau, Sigma_iw, Sigma_dynamic, 
        Sigma_HartreeFock, Sigma_moments.
    """
    G_iw = make_hermitian(solver.G_iw.copy())
    G_tau = solver.G_tau.copy()
    
    # Dyson equation
    Sigma_iw = G0_iw.copy()
    Sigma_iw << inverse(G0_iw) - inverse(G_iw)
    
    # Extract dynamic part (Σ - Σ_Hartree)
    Sigma_dynamic = Sigma_iw.copy()
    for bl, g in Sigma_dynamic:
        g << g - solver.Sigma_Hartree[bl]
    
    return {
        'G_iw': G_iw,
        'G_tau': G_tau,
        'Sigma_iw': Sigma_iw,
        'Sigma_dynamic': Sigma_dynamic,
        'Sigma_HartreeFock': [solver.Sigma_Hartree[bl] for bl, _ in solver.G_iw],
    }


def postprocess_tail_fit(solver: Solver, G0_iw: BlockGf, params: TailFitParams) -> dict:
    """Extract self-energy via Dyson equation followed by tail fitting.
    
    Computes Σ = G₀⁻¹ - G⁻¹, then fits the high-frequency tail and replaces
    noisy high-frequency data with the fitted expansion.
    
    Parameters
    ----------
    solver : Solver
        The cthyb solver after solve() has been called.
    G0_iw : BlockGf
        Non-interacting Green's function G₀(iω).
    params : TailFitParams
        Tail fitting parameters.
        
    Returns
    -------
    dict
        Dictionary with keys: G_iw, G_tau, Sigma_iw, Sigma_dynamic,
        Sigma_HartreeFock, Sigma_moments.
    """
    G_iw = make_hermitian(solver.G_iw.copy())
    G_tau = solver.G_tau.copy()
    
    # Dyson equation
    Sigma_iw = G0_iw.copy()
    Sigma_iw << inverse(G0_iw) - inverse(G_iw)
    
    # Apply tail fitting with known moments if available
    known_moments = dict(solver.Sigma_moments) if solver.Sigma_moments else None
    Sigma_iw = tail_fit(Sigma_iw, params, known_moments)
    
    # Extract dynamic part (Σ - Σ_Hartree)
    Sigma_dynamic = Sigma_iw.copy()
    for bl, g in Sigma_dynamic:
        g << g - solver.Sigma_Hartree[bl]
    
    return {
        'G_iw': G_iw,
        'G_tau': G_tau,
        'Sigma_iw': Sigma_iw,
        'Sigma_dynamic': Sigma_dynamic,
        'Sigma_HartreeFock': [solver.Sigma_Hartree[bl] for bl, _ in solver.G_iw],
    }


def postprocess_legendre(solver: Solver, G0_iw: BlockGf, params: LegendreParams) -> dict:
    """Extract self-energy via Legendre filtering then Dyson equation.
    
    Either uses measured Legendre coefficients (if use_measured=True and solver
    was configured to measure G_l) or fits G(τ) to Legendre polynomials.
    
    Parameters
    ----------
    solver : Solver
        The cthyb solver after solve() has been called.
    G0_iw : BlockGf
        Non-interacting Green's function G₀(iω).
    params : LegendreParams
        Legendre filtering parameters.
        
    Returns
    -------
    dict
        Dictionary with keys: G_iw, G_tau, G_l, Sigma_iw, Sigma_dynamic,
        Sigma_HartreeFock, Sigma_moments.
    """
    if params.use_measured:
        if not hasattr(solver, 'G_l') or solver.G_l is None:
            raise ValueError("use_measured=True but solver has no G_l. "
                           "Set measure_G_l=True in solver params.")
        G_l = solver.G_l.copy()
    else:
        G_l = legendre_filter(solver.G_tau, params.n_l)
    
    # Create output Green's functions from Legendre
    G_iw = G0_iw.copy()
    G_tau = solver.G_tau.copy()
    
    for bl, g_l in G_l:
        _set_gf_from_legendre(g_l, G_iw[bl], G_tau[bl])
    
    G_iw = make_hermitian(G_iw)
    
    # Dyson equation
    Sigma_iw = G0_iw.copy()
    Sigma_iw << inverse(G0_iw) - inverse(G_iw)
    
    # Extract dynamic part
    Sigma_dynamic = Sigma_iw.copy()
    for bl, g in Sigma_dynamic:
        g << g - solver.Sigma_Hartree[bl]
    
    return {
        'G_iw': G_iw,
        'G_tau': G_tau,
        'G_l': G_l,
        'Sigma_iw': Sigma_iw,
        'Sigma_dynamic': Sigma_dynamic,
        'Sigma_HartreeFock': [solver.Sigma_Hartree[bl] for bl, _ in solver.G_iw],
    }


def postprocess_crm(solver: Solver, G0_iw: BlockGf, params: CRMParams) -> dict:
    """Extract self-energy via Constrained Residual Minimization (CRM).
    
    Fits G(τ) to DLR representation, then minimizes the Dyson equation residual
    to extract Σ. Runs on master node and broadcasts results.
    
    Parameters
    ----------
    solver : Solver
        The cthyb solver after solve() has been called.
    G0_iw : BlockGf
        Non-interacting Green's function G₀(iω).
    params : CRMParams
        CRM parameters (w_max, eps for DLR).
        
    Returns
    -------
    dict
        Dictionary with keys: G_iw, G_tau, Sigma_iw, Sigma_dynamic, Sigma_dlr,
        G_tau_dlr, Sigma_HartreeFock, Sigma_moments.
    """
    from triqs.gf.dlr_crm_dyson_solver import minimize_dyson
    
    result = None
    
    if mpi.is_master_node():
        mpi.report(f'\nCRM Dyson solver with (w_max, eps) = ({params.w_max}, {params.eps})\n')
        
        # Fit G(τ) to DLR
        G_dlr = fit_gf_dlr(solver.G_tau, w_max=params.w_max, eps=params.eps)
        G_tau_dlr = make_gf_dlr_imtime(G_dlr)
        G_iw_dlr = make_gf_dlr_imfreq(G_dlr)
        
        # Construct G0 on DLR mesh by sampling from G0_iw
        mesh_dlr_iw = MeshDLRImFreq(G_dlr.mesh)
        G0_dlr_iw = G0_iw.copy()
        for bl, gf in G0_dlr_iw:
            G0_dlr_iw[bl] = Gf(mesh=mesh_dlr_iw, target_shape=gf.target_shape)
            for iwn in mesh_dlr_iw:
                G0_dlr_iw[bl][iwn] = G0_iw[bl](iwn)
        
        # Minimize Dyson equation for each block
        Sigma_dlr = G0_dlr_iw.copy()
        np.random.seed(85281)  # For reproducibility
        
        for bl, gf in Sigma_dlr:
            mpi.report(f'Minimizing Dyson via CRM for Σ[{bl}]')
            Sigma_dlr[bl], _, _ = minimize_dyson(
                G0_dlr=G0_dlr_iw[bl],
                G_dlr=G_iw_dlr[bl],
                Sigma_moments=solver.Sigma_moments[bl]
            )
        
        # Convert to ImFreq mesh
        n_iw = len(G0_iw.mesh) // 2 if hasattr(G0_iw.mesh, '__len__') else solver.n_iw
        Sigma_iw = make_gf_imfreq(Sigma_dlr, n_iw=n_iw)
        
        # Add Hartree-Fock contribution
        for bl, gf in Sigma_iw:
            gf += solver.Sigma_moments[bl][0]
        
        # Extract dynamic part
        Sigma_dynamic = Sigma_dlr.copy()
        
        G_iw = make_hermitian(solver.G_iw.copy())
        G_tau = solver.G_tau.copy()
        
        result = {
            'G_iw': G_iw,
            'G_tau': G_tau,
            'G_tau_dlr': G_tau_dlr,
            'Sigma_iw': Sigma_iw,
            'Sigma_dynamic': Sigma_dynamic,
            'Sigma_dlr': Sigma_dlr,
            'Sigma_HartreeFock': [solver.Sigma_Hartree[bl] for bl, _ in solver.G_iw],
        }
    
    mpi.barrier()
    result = mpi.bcast(result)
    
    return result


# =============================================================================
# Dispatcher
# =============================================================================

def apply_postprocessing(
    solver: Solver,
    G0_iw: BlockGf,
    postprocess: str | PostProcessParams,
) -> dict:
    """Apply post-processing strategy to extract self-energy from solver output.
    
    Parameters
    ----------
    solver : Solver
        The cthyb solver after solve() has been called.
    G0_iw : BlockGf
        Non-interacting Green's function G₀(iω).
    postprocess : str | PostProcessParams
        Post-processing strategy. Can be:
        - 'dyson': Plain Dyson equation
        - TailFitParams: Use solver's tail fitting
        - LegendreParams: Legendre filtering
        - CRMParams: Constrained Residual Minimization
        
    Returns
    -------
    dict
        Dictionary of results.
        
    Raises
    ------
    TypeError
        If postprocess type is not recognized.
    """
    if postprocess == 'dyson':
        return postprocess_dyson(solver, G0_iw)
    
    if isinstance(postprocess, TailFitParams):
        return postprocess_tail_fit(solver, G0_iw, postprocess)
    
    if isinstance(postprocess, LegendreParams):
        return postprocess_legendre(solver, G0_iw, postprocess)
    
    if isinstance(postprocess, CRMParams):
        return postprocess_crm(solver, G0_iw, postprocess)
    
    raise TypeError(f"Unknown postprocess type: {type(postprocess)}. "
                   f"Expected 'dyson' or one of: TailFitParams, LegendreParams, CRMParams")
