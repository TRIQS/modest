"""
TRIQS Solver Interfaces for modest.

Provides functional APIs to quantum impurity solvers with optional post-processing
strategies for self-energy extraction.

Available Solvers
-----------------
- cthyb: CT-HYB continuous-time hybridization expansion solver

Post-Processing Strategies
--------------------------
- None: Raw solver output for custom post-processing
- 'dyson': Plain Dyson equation Σ = G₀⁻¹ - G⁻¹
- TailFitParams: High-frequency tail fitting (performed inside solver)
- LegendreParams: Legendre polynomial filtering
- CRMParams: Constrained Residual Minimization via DLR

Example
-------
>>> from triqs_modest.solver_interfaces import solve, CRMParams
>>> result = solve(Delta_iw, h_loc0, h_int, postprocess=CRMParams(w_max=15.0))
>>> result.Sigma_iw  # Extracted self-energy
"""

from .utils import SolverResults
from .postprocessing import (
    TailFitParams,
    LegendreParams,
    CRMParams,
    PostProcessParams,
)
from .cthyb import solve

__all__ = [
    'solve',
    'SolverResults',
    'TailFitParams',
    'LegendreParams',
    'CRMParams',
    'PostProcessParams',
]
