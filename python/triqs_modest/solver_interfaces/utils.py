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

def extract_u_tensor_from_h_int(h_int, gf_struct, return_4idx=False):
    """
    Return Uijkl tensor in TRIQS's notation from a Coulomb many-body operator h_int
    """
    from triqs.operators.util.extractors import extract_U_dict2, dict_to_matrix

    U_dd = dict_to_matrix(extract_U_dict2(h_int), gf_struct=gf_struct)
    if not return_4idx:
        return U_dd

    n_orb = U_dd.shape[0]//2

    # extract Uijij (inter- and intra-orbital Coulomb) and Uijji (Hund's coupling) terms
    # a) For static impurity problem, Us are the static screened interactions
    # b) For dynamic impurity problem, Us are the bare interactions
    Uijij = U_dd[:n_orb, n_orb:2*n_orb]
    Uijji = Uijij - U_dd[:n_orb, :n_orb]
    
    # construct full Uijkl tensor for static interaction
    Uijkl = np.zeros((n_orb, n_orb, n_orb, n_orb), dtype=complex)

    # assuming Uijji = Uiijj
    for i, j, k, l in product(range(n_orb), repeat=4):
        if i == j == k == l:  # Uiiii
            Uijkl[i, i, i, i] = Uijij[i, j]
        elif i == k and j == l:  # Uijij
            Uijkl[i, j, i, j] = Uijij[i, j]
        elif i == l and j == k:  # Uijji
            Uijkl[i, j, j, i] = Uijji[i, j]
        elif i == j and k == l:  # Uiijj
            Uijkl[i, i, k, k] = Uijji[i, k]
    
    return Uijkl

def fill_dlr_imfreq_gf(
    g_iw : Gf | BlockGf, 
    wmax : float, 
    eps : float
) -> Gf | BlockGf:
    """Fill a Green's function defined on MeshDLRImFreq with values from a Green's function defined on MeshImFreq.

    Parameters
    ----------
    g_iw : Gf or BlockGf
        Input Green's function defined on MeshImFreq. The data on this Green's function will be used to fill the output Green's function defined on MeshDLRImFreq.
    wmax : float
        Maximum frequency for the DLR mesh. 
    eps : float
        Accuracy for the DLR mesh. 

    Returns
    -------
    g_dlr_iw : Gf or BlockGf
        A Green's function defined on MeshDLRImFreq, filled with values from g_iw.
    """

    assert isinstance(g_iw.mesh, MeshImFreq), (
        "fill_dlr_imfreq_gf: input Green's function should live on MeshImFreq."
    )

    mesh_dlr_iw = MeshDLRImFreq(
        beta=g_iw.mesh.beta, statistic=g_iw.mesh.statistic,
        w_max=wmax, eps=eps, symmetrize=True
    )

    mesh_dlr_idx = np.array([iw.index for iw in mesh_dlr_iw])
    max_dlr_idx = max(abs(mesh_dlr_idx[0]), abs(mesh_dlr_idx[-1]))
    assert max_dlr_idx <= g_iw.mesh.n_iw, (
        f"fill_dlr_imfreq_gf: g_iw.mesh.n_iw = {g_iw.mesh.n_iw} < maximum DLRImFreq index ({max_dlr_idx})."
    )

    if isinstance(g_iw, BlockGf):
        gf_struct = [(bl, gf.target_shape[0]) for (bl, gf) in g_iw]
        g_dlr_iw = BlockGf(mesh=mesh_dlr_iw, gf_struct=gf_struct)
        for w in g_dlr_iw.mesh:
            for block, gf in g_dlr_iw:
                gf[w] = g_iw[block](w)
        return g_dlr_iw

    elif isinstance(g_iw, Gf):
        g_dlr_iw = Gf(mesh=mesh_dlr_iw, target_shape=g_iw.target_shape)
        for w in g_dlr_iw.mesh:
            g_dlr_iw[w] = g_iw(w)
        return g_dlr_iw

    else:
        raise NotImplementedError("fill_dlr_imfreq_gf: input Green's function should be either Gf or BlockGf.")