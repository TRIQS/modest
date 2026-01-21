from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any

import numpy as np

from triqs.gf import BlockGf
from triqs.operators import c_dag, c


@dataclass
class SolverResults:
    """Container for quantum impurity solver results.
    
    All fields are optional to support both raw output (postprocess=None)
    and post-processed results with computed self-energies.
    
    Attributes
    ----------
    G_iw : BlockGf, optional
        Green's function in Matsubara frequencies.
    G_tau : BlockGf, optional
        Green's function in imaginary time.
    G_l : BlockGf, optional
        Green's function in Legendre basis (if Legendre post-processing used).
    G_tau_dlr : BlockGf, optional
        Green's function in DLR imaginary time (if CRM post-processing used).
    Sigma_iw : BlockGf, optional
        Self-energy in Matsubara frequencies (full: dynamic + Hartree-Fock).
    Sigma_dynamic : BlockGf, optional
        Dynamic part of self-energy (Σ - Σ_HF).
    Sigma_dlr : BlockGf, optional
        Self-energy in DLR representation (if CRM post-processing used).
    Sigma_HartreeFock : dict, optional
        Hartree-Fock (static) contribution to self-energy per block.
    Sigma_moments : dict, optional
        High-frequency moments of self-energy per block.
    density_matrix : Any, optional
        Density matrix from solver (if measured).
    Solver : Any, optional
        Reference to the underlying triqs solver object for advanced access.
    """
    G_iw: BlockGf | None = None
    G_tau: BlockGf | None = None
    G_l: BlockGf | None = None
    G_tau_dlr: BlockGf | None = None
    Sigma_iw: BlockGf | None = None
    Sigma_dynamic: BlockGf | None = None
    Sigma_dlr: BlockGf | None = None
    Sigma_HartreeFock: list[np.ndarray] | None = None
    Sigma_moments: dict | None = None
    density_matrix: Any | None = None
    Solver: Any | None = None
    
    def __getitem__(self, key: str) -> Any:
        """Allow dict-style access: result['G_iw']."""
        if hasattr(self, key):
            return getattr(self, key)
        raise KeyError(f"SolverResults has no field '{key}'")
    
    def __contains__(self, key: str) -> bool:
        """Allow 'G_iw' in result checks."""
        return hasattr(self, key) and getattr(self, key) is not None
    
    def keys(self):
        """Return names of non-None fields."""
        return [f for f in self.__dataclass_fields__ if getattr(self, f) is not None]
    
    def __repr__(self) -> str:
        fields = [f for f in self.__dataclass_fields__ if getattr(self, f) is not None]
        return f"SolverResults({', '.join(fields)})"


def matrix_to_many_body_operator(h_loc_matrix, gf_struct):
    h_loc0_mat = {block : h_loc_matrix[ibl].real for ibl, (block, _) in enumerate(gf_struct) }
    c_dag_vec  = {block : np.matrix([[c_dag(block, o) for o in range(bl_size)]]) for block, bl_size in gf_struct }
    c_vec      = {block : np.matrix([[c(block, o) for o in range(bl_size)]]) for block, bl_size in gf_struct }
    return sum(c_dag_vec[bl]*h_loc0_mat[bl]*c_vec[bl].T for bl, bl_size in gf_struct)[0,0]

def many_body_operator_to_matrix(h_loc_operator, gf_struct):
    n_blocks = len(gf_struct)
    h_loc_matrix = np.zeros((n_blocks, n_blocks), dtype=np.complex_)
    for ibl, (block, bl_size) in enumerate(gf_struct):
        for i in range(bl_size):
            for j in range(bl_size):
                op = c_dag(block, i)*c(block, j)
                coeff = h_loc_operator.get_coeff(op)
                h_loc_matrix[ibl][i, j] = coeff
    return h_loc_matrix