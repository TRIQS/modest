import triqs.utility.mpi as mpi
import numpy as np
from itertools import product

from triqs.gf import (
    iOmega_n, MeshImFreq, MeshDLRImFreq, Gf, BlockGf,
    make_gf_dlr, make_gf_imfreq, make_hermitian
)
from triqs.gf.tools import inverse, make_zero_tail

from triqs_ctseg import Solver as CTSEG_Solver
from .utils import fill_dlr_imfreq_gf, extract_u_tensor_from_h_int
from .postprocessing import (
    TailFitParams, tail_fit
)


def postprocess(
    solver : CTSEG_Solver, 
    symmetrize_func : callable | None = None, 
    **post_proc_params : dict
) -> dict:
    pp_results = postprocess_sigma(solver, symmetrize_func, **post_proc_params)

    if solver.D0_tau is not None and solver.results.nn_nu is not None:
        # Post-process the impurity polarizability
        if post_proc_params['degenerate_blk']:
            deg_blk_2e = []
            for blks in post_proc_params['degenerate_blk']:
                n_half = len(blks) // 2
                deg_blk_2e.append(np.array(blks[:n_half]))
        else:
            deg_blk_2e = None
        pp_results.update(
            postprocess_pi(
                solver, deg_blk_2e, symmetrize_func, 
                truncate_uchi=post_proc_params['truncate_uchi']
            )
        )

    if "dlr_w_max" in post_proc_params and "dlr_eps" in post_proc_params:
        wmax = post_proc_params['dlr_w_max']
        eps = post_proc_params['dlr_eps']
        pp_results['Sigma_iw'] = fill_dlr_imfreq_gf(pp_results['Sigma_iw'], wmax, eps)
        pp_results['G_iw'] = fill_dlr_imfreq_gf(pp_results['G_iw'], wmax, eps)
        if solver.D0_tau is not None and solver.results.nn_nu is not None: 
            pp_results['Pi_iw'] = fill_dlr_imfreq_gf(pp_results['Pi_iw'], wmax, eps)
            pp_results['W_iw'] = fill_dlr_imfreq_gf(pp_results['W_iw'], wmax, eps)
            pp_results['Chi_iw'] = fill_dlr_imfreq_gf(pp_results['Chi_iw'], wmax, eps)

    return pp_results


def postprocess_pi(
    solver : CTSEG_Solver, 
    degenerate_blk : list | None = None, 
    symmetrize_func : callable | None = None, 
    output_in_4idx : bool = False,
    truncate_uchi : bool = False
) -> dict:
    """Post-process the charge susceptibility from the CT-SEG solver to obtain the impurity polarizability and screened interaction.

    This function assumes 'measure_nn_nu' is True in the solver parameters. 
    `measure_nn_tau` is not compatible with the current post-processing implementation. 

    """
    mpi.report("Charge susceptibility is measured for a impurity with dynamic interactions")
    mpi.report('--> Post-processing the density-density susceptibility to obtain the impurity polarizability.\n')

    # 0. initialization
    n_color = 0
    for _, blk_dim in solver.gf_struct:
        n_color += blk_dim
    assert n_color % 2 == 0, "Oh oh... n_color is an odd number." 
    n_orb = n_color// 2 

    block_name       = []
    index_in_block   = []
    color_to_orbital = []  # mapping from color to orbital
    for color in range(n_color):
        block_name.append(find_block_name(color, solver.gf_struct))
        index_in_block.append(find_index_in_block(color, solver.gf_struct))
        color_to_orbital.append(find_orbital_index(color, solver.gf_struct))
    
    tau_mesh = solver.D0_tau[block_name[0], block_name[0]].mesh
    iw_mesh_dlr = solver.results.nn_nu[block_name[0], block_name[0]].mesh

    D0_tau = Gf(mesh=tau_mesh, target_shape=(n_color, n_color))
    nn_iw_dlr = Gf(mesh=iw_mesh_dlr, target_shape=(n_color, n_color))
    for c1, c2 in product(range(n_color), repeat=2):
        nn_iw_dlr.data[:, c1, c2] = (
            solver.results.nn_nu[block_name[c1], block_name[c2]].data[:, index_in_block[c1], index_in_block[c2]]
        )
        D0_tau.data[:, c1, c2] = (
            solver.D0_tau[block_name[c1], block_name[c2]].data[:, index_in_block[c1], index_in_block[c2]]
        )

    # 1. Subtract the the constant part of the charge susceptibility (i.e. density^2) in the color basis. 
    densities = np.zeros(n_color, dtype=float)
    for c1 in range(n_color):
        densities[c1] = solver.results.densities[block_name[c1]][index_in_block[c1]]
    mpi.report(f"Average of time-dependent occupations: {densities}")

    mpi.report("Subtracting the constant component, and then symmetrizing the density-density susceptibility: \n"
               "  1. nn(t).imag = 0.0\n"
               "  2. nn(i, j) = nn(j, i)\n")

    for c1, c2 in product(range(n_color), repeat=2):
        iw_idx = np.array([ iw.index for iw in nn_iw_dlr[c1,c2].mesh ])
        w0_idx = np.where(iw_idx==0)[0][0]
        if c1 >= c2:
            # remove the constant part
            nn_iw_dlr[c1,c2].data[w0_idx] -= iw_mesh_dlr.beta * (densities[c1] * densities[c2])
            # symmetrization
            nn_iw_dlr[c1,c2].data.imag = 0.0
            if c1 != c2:
                nn_iw_dlr[c2,c1].data[w0_idx] -= iw_mesh_dlr.beta * (densities[c2] * densities[c1])
                nn_iw_dlr[c1,c2].data[:] += nn_iw_dlr[c2,c1].data[:].real
                nn_iw_dlr[c1,c2].data[:] /= 2.0
                nn_iw_dlr[c2,c1] << nn_iw_dlr[c1,c2]

    nn_iw = make_gf_imfreq(make_gf_dlr(nn_iw_dlr), n_iw = solver.n_iw)

    # Convert to density-density basis
    nn_iw_dd = Gf(mesh=nn_iw.mesh, target_shape=[n_orb, n_orb])
    for c1, c2 in product(range(n_color), repeat=2):
        nn_iw_dd[color_to_orbital[c1], color_to_orbital[c2]].data[:] += nn_iw[c1, c2].data[:]
    
    if symmetrize_func is not None and degenerate_blk is not None:
        nn_iw_dd << symmetrize_func(nn_iw_dd, degenerate_blk)
        
    # Convert to product basis 
    nn_iw_pb = Gf(mesh=nn_iw.mesh, target_shape=[n_orb*n_orb, n_orb*n_orb])
    for i, j in product(range(n_orb), repeat=2):
        if i >= j:
            # only the real part
            nn_iw_pb[i*n_orb+i, j*n_orb+j] << nn_iw_dd[i,j].real
            if i != j:
                nn_iw_pb[j*n_orb+j, i*n_orb+i] << nn_iw_pb[i*n_orb+i, j*n_orb+j]


    # 2. Construct bosonic Weiss field in product basis
    D0_iw = nn_iw.copy()
    D0_iw << 0.0
    u_known_moments = make_zero_tail(D0_iw, n_moments=2)
    D0_iw.set_from_fourier(D0_tau, u_known_moments)

    # inter-/intra-orbital U(iw) 
    D0_iijj = D0_iw[:n_orb, n_orb:2*n_orb]
    # screening J if non-zero
    D0_ijij = D0_iijj - D0_iw[:n_orb, 0:n_orb]

    # Vijkl is the full 4-index tensor without the block structure in TRIQS's notation
    Vijkl = extract_u_tensor_from_h_int(h_int=solver.h_int, gf_struct=solver.gf_struct, return_4idx=True)

    U_iw_pb = Gf(mesh=nn_iw_pb.mesh, target_shape=nn_iw_pb.target_shape)
    for i, j in product(range(n_orb), repeat=2):
        if i == j:
            # intra-orbital density-density term
            U_iw_pb[i*n_orb+i, i*n_orb+i] << D0_iijj[i, i].real
            U_iw_pb[i*n_orb+i, i*n_orb+i].data[:] += Vijkl[i, i, i, i]
        if i > j:
            # inter-orbital density-density term
            U_iw_pb[i*n_orb+i, j*n_orb+j] << D0_iijj[i, j].real
            U_iw_pb[i*n_orb+i, j*n_orb+j].data[:] += Vijkl[i, j, i, j]
            U_iw_pb[j*n_orb+j, i*n_orb+i] << U_iw_pb[i*n_orb+i, j*n_orb+j]
            # Hund's J: Spin-flip (i, j, j, i)
            U_iw_pb[i*n_orb+j, i*n_orb+j] << D0_ijij[i, j].real
            U_iw_pb[i*n_orb+j, i*n_orb+j].data[:] += Vijkl[i, j, j, i]
            U_iw_pb[j*n_orb+i, j*n_orb+i] << U_iw_pb[i*n_orb+j, i*n_orb+j]
            # Hund's J: Pair hopping (i, j, i, j)
            U_iw_pb[i*n_orb+j, j*n_orb+i] << D0_ijij[i, j].real
            U_iw_pb[i*n_orb+j, j*n_orb+i].data[:] += Vijkl[i, i, j, j]
            U_iw_pb[j*n_orb+i, i*n_orb+j] << U_iw_pb[i*n_orb+j, j*n_orb+i]
    

    # 3. Dyson equation: Pi(w) = Chi(w) * [U(w)*Chi(w) - I]^-1
    Pi_iw_pb = Gf(mesh=nn_iw_pb.mesh, target_shape=nn_iw_pb.target_shape)
    ones = np.eye(n_orb*n_orb, dtype=complex)
    for iwn in nn_iw_pb.mesh:
        UX = U_iw_pb[iwn] @ nn_iw_pb[iwn]
        UX = check_spectrum(UX, truncation=truncate_uchi)
        denom = UX - ones
        cond = np.linalg.cond(denom)
        if cond > 20: 
            mpi.report(f"WARNING: Large condition number for [U(w) * Chi(w) - I] = {cond} at n = {iwn.index}.")        
        Pi_iw_pb[iwn] = nn_iw_pb[iwn] @ np.linalg.pinv(denom)
        # explicit set Pi(iw).imag = 0.0
        Pi_iw_pb[iwn].imag = 0.0


    # 4. Screened interaction W(w) = U(w) - U(w) * Chi(w) * U(w)
    W_iw_pb = Gf(mesh=nn_iw_pb.mesh, target_shape=nn_iw_pb.target_shape)
    for iwn in nn_iw_pb.mesh:
        W_iw_pb[iwn] = U_iw_pb[iwn] - U_iw_pb[iwn] @ nn_iw_pb[iwn] @ U_iw_pb[iwn]
    
    # 5. Remove the static part 
    for i, j in product(range(n_orb), repeat=2):
        if i == j:
            # intra-orbital density-density term
            W_iw_pb[i*n_orb+i, i*n_orb+i].data[:] -= Vijkl[i, i, i, i]
        if i > j:
            # inter-orbital density-density term
            W_iw_pb[i*n_orb+i, j*n_orb+j].data[:] -= Vijkl[i, j, i, j]
            W_iw_pb[j*n_orb+j, i*n_orb+i] << W_iw_pb[i*n_orb+i, j*n_orb+j]
            # Hund's J: Spin-flip (i, j, j, i)
            W_iw_pb[j*n_orb+i, j*n_orb+i].data[:] -= Vijkl[i, j, j, i]
            W_iw_pb[i*n_orb+j, i*n_orb+j] << W_iw_pb[j*n_orb+i, j*n_orb+i]
            # Hund's J: Pair hopping (i, j, i, j)
            W_iw_pb[j*n_orb+i, i*n_orb+j].data[:] -= Vijkl[i, i, j, j]
            W_iw_pb[i*n_orb+j, j*n_orb+i] << W_iw_pb[j*n_orb+i, i*n_orb+j]

    # transform back to density-density basis
    # TODO is W(iw) density-density only for density-density interaction?
    if not output_in_4idx:
        # transform back to the density-density basis
        Chi_iw = nn_iw_dd
        Pi_iw  = Gf(mesh=nn_iw_pb.mesh, target_shape=(n_orb, n_orb))
        W_iw   = Pi_iw.copy()
        for i, j in product(range(n_orb), repeat=2):
            Pi_iw[i,j] << Pi_iw_pb[i*n_orb+i, j*n_orb+j]
            W_iw[i,j] << W_iw_pb[i*n_orb+i, j*n_orb+j]
        if symmetrize_func is not None and degenerate_blk is not None:
            Pi_iw << symmetrize_func(Pi_iw, degenerate_blk)
            W_iw  << symmetrize_func(W_iw, degenerate_blk)
        
    else:
        # transform back to 4-index tensor
        Chi_iw = nn_iw_pb
        Pi_iw  = Gf(mesh=nn_iw_pb.mesh, target_shape=(n_orb, n_orb, n_orb, n_orb))
        W_iw  = Pi_iw.copy()
        for i, j, k, l in product(range(n_orb), repeat=4):
            Pi_iw[i,j,k,l] << Pi_iw_pb[i*n_orb+j, k*n_orb+l]
            W_iw[i,j,k,l] << W_iw_pb[i*n_orb+j, k*n_orb+l]
    
    return {
        'Pi_iw': Pi_iw,
        'W_iw': W_iw,
        'Chi_iw': Chi_iw
    }


def postprocess_sigma(
    solver : CTSEG_Solver, 
    symmetrize_func = None, 
    **post_proc_params : dict
) -> dict:
    """
    Post-process the self-energy from the CT-SEG solver.
    The post-processing outputs are returned as a dictionary, which include:
    - Sigma_iw: the self-energy on the imaginary frequency axis, after tail fitting and removing the static part.
    - Sigma_HartreeFock: the static part of the self-energy (Σ_HF) extracted via tail fitting.
    The post-processing steps include:
    1. Fourier transform G(τ) to G(iω) if G(iω) is not directly measured.
    2. Compute the fermionic Weiss field g(iω) from Δ(τ).
    3. Compute the static self-energy Σ_HF either via tail fitting or analytically.
    4. Compute the dynamic self-energy Σ(iω) either via the Dyson equation or using the improved estimator if F(τ) is measured.
    5. Perform tail fitting on Σ(iω) to improve the high-frequency behavior.
    6. Remove the static part Σ_HF from Σ(iω) to obtain the dynamic self-energy.

    Parameters
    ----------
    solver : CTSEG_Solver
        The CT-SEG solver instance containing the raw results and intermediate quantities.
    post_proc_params : dict
        A dictionary of parameters controlling the post-processing steps, which may include:
        - degenerate_blk: list of lists of block names that are degenerate (for symmetrization).
        - analytic_hf: bool, whether to compute Σ_HF analytically or via tail fitting.
        - perform_tail_fit: bool, whether to perform tail fitting on Σ(iω).
        - fit_min_n, fit_max_n: int, range of Matsubara frequencies to use for tail fitting.
        - fit_min_w, fit_max_w: float, range of frequencies to use for tail fitting (alternative to fit_min_n and fit_max_n).
        - fit_max_moment: int, maximum order of moments to use in tail fitting.

    Returns
    -------
    dict
        A dictionary containing the post-processed self-energy and related quantities. Keys include:
        - 'Sigma_iw': the post-processed self-energy on the imaginary frequency axis.
        - 'Sigma_HartreeFock': the static part of the self-energy (Σ_HF) extracted via tail fitting.
    """
    
    # 0. initialization
    mesh = MeshImFreq(beta = solver.beta, S="Fermion", n_iw = solver.n_iw)
    Sigma_iw = BlockGf(mesh = mesh, gf_struct = solver.gf_struct)
    Sigma_iw.zero()
    Sigma_moments = None
    G_iw = Sigma_iw.copy()
    G0_iw = Sigma_iw.copy()
    
    # 1. Fourier transform G(tau) to G(iw)
    Gf_known_moments = make_zero_tail(G_iw, n_moments=2)
    for i, bl in enumerate(G_iw.indices):
        Gf_known_moments[i][1] = np.eye(G_iw[bl].target_shape[0])
        G_iw[bl].set_from_fourier(solver.results.G_tau[bl], Gf_known_moments[i])
    G_iw << make_hermitian(G_iw)

    if symmetrize_func is not None and post_proc_params['degenerate_blk']:
        G_iw << symmetrize_func(G_iw, post_proc_params['degenerate_blk'])

    # 2. Compute fermionic Weiss field g(iw)
    Delta_iw = BlockGf(mesh = mesh, gf_struct = solver.gf_struct)
    Delta_known_moments = make_zero_tail(Delta_iw, n_moments=1)
    for i, bl in enumerate(solver.Delta_tau.indices):
        Delta_iw[bl].set_from_fourier(solver.Delta_tau[bl], Delta_known_moments[i])
        G0_iw[bl] << inverse(iOmega_n - solver.h_loc0_mat[i] - Delta_iw[bl])
    G0_iw << make_hermitian(G0_iw)

    if symmetrize_func is not None and post_proc_params['degenerate_blk']:
        G0_iw << symmetrize_func(G0_iw, post_proc_params['degenerate_blk'])

    # 3. Compute the HF self-energy as the first moment of the self-energy
    if post_proc_params['analytic_hf']:
        Sigma_HartreeFock = compute_sigma_hartreefock(solver)

        if symmetrize_func is not None and post_proc_params['degenerate_blk']:
            Sigma_HF_list = symmetrize_func(list(Sigma_HartreeFock.values()), post_proc_params['degenerate_blk'])
            for i, blk_name in enumerate(Sigma_HartreeFock.keys()):
                Sigma_HartreeFock[blk_name] = Sigma_HF_list[i]

        solver.Sigma_moments = {}
        for blk_name, hf_val in Sigma_HartreeFock.items():
            mpi.report(f"Σ_HF {blk_name}:")
            mpi.report(f"    {hf_val}")
            solver.Sigma_moments[blk_name] = np.array([hf_val], dtype=complex)
        mpi.report("")
    else:
        Sigma_HartreeFock = None
        solver.Sigma_moments = None

    # 4. Compute self-energy
    if solver.results.F_tau is None:
        mpi.report("F(tau) is not measured -> Compute the self-energy via the Dyson equation.\n")
        Sigma_iw = inverse(G0_iw) - inverse(G_iw)
    else:
        mpi.report("F(tau) is measured -> Compute the self-energy via the improved estimator.\n")
        F_iw = G_iw.copy()
        F_iw << 0.0
        F_known_moments = make_zero_tail(F_iw, n_moments=1)
        for i, bl in enumerate(F_iw.indices):
            F_iw[bl].set_from_fourier(solver.results.F_tau[bl], F_known_moments[i])
        F_iw << make_hermitian(F_iw)

        if symmetrize_func is not None and post_proc_params['degenerate_blk']:
            F_iw << symmetrize_func(F_iw, post_proc_params['degenerate_blk'])

        for block, fw in F_iw:
            for iw in fw.mesh:
                Sigma_iw[block][iw] = fw[iw] / G_iw[block][iw]
    
    Sigma_iw << make_hermitian(Sigma_iw)
    
    if symmetrize_func is not None and post_proc_params['degenerate_blk']:
        Sigma_iw << symmetrize_func(Sigma_iw, post_proc_params['degenerate_blk'])

    if post_proc_params['perform_tail_fit']:
        # tail fitting for the self-energy 
        Sigma_iw = tail_fit(
            Sigma_iw, 
            TailFitParams(fit_min_n=post_proc_params['fit_min_n'],
                          fit_max_n=post_proc_params['fit_max_n'],
                          fit_min_w=post_proc_params['fit_min_w'],
                          fit_max_w=post_proc_params['fit_max_w'],
                          fit_max_moment=post_proc_params['fit_max_moment']),
            known_moments=Sigma_moments
        )

    Sigma_iw << make_hermitian(Sigma_iw)
    if symmetrize_func is not None and post_proc_params['degenerate_blk']:
        Sigma_iw << symmetrize_func(Sigma_iw, post_proc_params['degenerate_blk'])

    # 5. Update G(iw) with the fitted self-energy
    G_iw << inverse( inverse(G0_iw) - Sigma_iw )
    G_iw << make_hermitian(G_iw)

    if symmetrize_func is not None and post_proc_params['degenerate_blk']:
        G_iw << symmetrize_func(G_iw, post_proc_params['degenerate_blk'])

    if Sigma_HartreeFock is None:
        Sigma_HartreeFock = {}
        for block, gf in Sigma_iw:
            tail, err = gf.fit_hermitian_tail()
            Sigma_HartreeFock[block] = tail[0]
        mpi.report("Extracting the static self-energy via tail fitting:")
        for blk_name, hf_val in Sigma_HartreeFock.items():
            mpi.report(f"Σ_HF {blk_name}:")
            mpi.report(f"    {hf_val}")
        mpi.report("")

    # 6. Extract the dynamic part (Σ - Σ_HartreeFock)
    Sigma_dynamic = Sigma_iw.copy()
    for bl, g in Sigma_dynamic:
        g << g - Sigma_HartreeFock[bl]

    return {
        'G_iw': G_iw,
        'G_tau': G_tau,
        'Sigma_iw': Sigma_iw,
        'Sigma_dynamic': Sigma_dynamic,
        'Sigma_HartreeFock': list(Sigma_HartreeFock.values()),
    }
    

# TODO move to utils.py
def extract_screen_matrix_from_D0_tau(blk2_D0_tau, gf_struct, return_4idx=False):
    n_color = 0
    for _, blk_dim in gf_struct:
        n_color += blk_dim

    block_name = []
    index_in_block = []
    for color in range(n_color):
        block_name.append(find_block_name(color, gf_struct))
        index_in_block.append(find_index_in_block(color, gf_struct))

    mesh = blk2_D0_tau[block_name[0], block_name[0]].mesh
    D0_tau = Gf(mesh=mesh, target_shape=(n_color, n_color))
    for c1 in range(n_color):
        for c2 in range(n_color):
            D0_tau.data[:, c1, c2] = (
                blk2_D0_tau[block_name[c1], block_name[c2]].data[:, index_in_block[c1], index_in_block[c2]]
            )

    w0_mesh = MeshImFreq(beta = D0_tau.mesh.beta, S="Boson", n_iw = 1)
    D0_iw = Gf(mesh=w0_mesh, target_shape=D0_tau.target_shape)
    D0_iw.set_from_fourier(D0_tau, make_zero_tail(D0_iw, n_moments=2))
    Dw0_dd = D0_iw.data[0].real

    if not return_4idx:
        return Dw0_dd

    n_orb = Dw0_dd.shape[0]//2
    Dw0_iijj = Dw0_dd[:n_orb, n_orb:2*n_orb]
    Dw0_ijij = Dw0_iijj - Dw0_dd[:n_orb, :n_orb]

    # construct full Uijkl tensor for static interaction
    Dw0_ijkl = np.zeros((n_orb, n_orb, n_orb, n_orb), dtype=complex)

    for i, j, k, l in product(range(n_orb), repeat=4):
        if i == j == k == l:  # iiii
            Dw0_ijkl[i, i, i, i] = Dw0_iijj[i, j]
        elif i == k and j == l:  # ijij
            Dw0_ijkl[i, j, i, j] = Dw0_iijj[i, j]
        elif i == l and j == k:  # ijji
            Dw0_ijkl[i, j, j, i] = Dw0_ijij[i, j]
        elif i == j and k == l:  # iijj
            Dw0_ijkl[i, i, k, k] = Dw0_ijij[i, k]

    return Dw0_ijkl


def compute_sigma_hartreefock(solver):
    """
    Compute the static Hartree-Fock self-energy with density-density interactions

    Parameters
    ----------
    solver : CTSEG_Solver
        The CT-SEG solver instance containing impurity densities and Coulomb interactions. 

    Returns
    -------
    Sigma_HartreeFock : dict
        A block matrix containing the static Hartree-Fock self-energy following the block structure 
        of the impurity problem. 
    """
    mpi.report('\nEvaluating static impurity self-energy analytically in density-density basis:')
    Sigma_HartreeFock = {}

    # initialization
    n_color = 0
    for _, blk_dim in solver.gf_struct:
        n_color += blk_dim

    block_name       = []
    index_in_block   = []
    for color in range(n_color):
        block_name.append(find_block_name(color, solver.gf_struct))
        index_in_block.append(find_index_in_block(color, solver.gf_struct))

    # Bare Coulomb in density-density basis
    V = extract_u_tensor_from_h_int(h_int=solver.h_int, gf_struct=solver.gf_struct)
    if solver.D0_tau is not None:
        # Add screening function at iw=0 in density-density basis
        V += extract_screen_matrix_from_D0_tau(blk2_D0_tau=solver.D0_tau, gf_struct=solver.gf_struct)

    # impurity density
    densities = np.zeros(n_color, dtype=float)
    for c1 in range(n_color):
        densities[c1] = solver.results.densities[block_name[c1]][index_in_block[c1]]

    # sigma_hartreefock
    c1 = 0
    for blk_name, blk_dim in solver.gf_struct:
        Sigma_HartreeFock[blk_name] = np.zeros((blk_dim, blk_dim), dtype=float)

        for iorb in range(blk_dim):
            for c2 in range(n_color):
                Sigma_HartreeFock[blk_name][iorb, iorb] += densities[c2] * V[c1, c2].real
            c1 += 1

    return Sigma_HartreeFock

def find_block_name(color, gf_struct):
    bl, colors_so_far = 0, 0
    for blk_name, blk_dim in gf_struct:
        colors_so_far += blk_dim
        if color < colors_so_far: 
            return blk_name
        bl+=1
    raise ValueError(f"Color index {color} out of bounds for gf_struct of total size {colors_so_far}")


def find_index_in_block(color, gf_struct):
    colors_so_far = 0
    for blk_name, blk_dim in gf_struct:
        colors_so_far += blk_dim
        if color < colors_so_far:
            return color - (colors_so_far - blk_dim)
    raise ValueError(f"Color index {color} out of bounds for gf_struct of total size {colors_so_far}")


def find_orbital_index(color, gf_struct):
    n_color = 0
    for _, blk_dim in gf_struct:
        n_color += blk_dim
    n_orb = n_color // 2
    
    colors_so_far = 0
    o_up, o_dn = 0, 0
    for blk_name, blk_dim in gf_struct:
        colors_so_far += blk_dim
        if blk_name[:2] == "up":
            if color < colors_so_far:
                return o_up + ( color - (colors_so_far - blk_dim) )
            else:
                o_up += blk_dim
            assert o_up <= n_orb, f"Spin up orbital index {o_up} excceds band range {n_orb}"
        else:
            if color < colors_so_far:
                return o_dn + ( color - (colors_so_far - blk_dim) )
            else:
                o_dn += blk_dim
            assert o_dn <= n_orb, f"Spin down orbital index {o_dn} excceds band range {n_orb}"    

    raise ValueError(f"Color index {color} out of bounds for gf_struct of total size {colors_so_far}")


def check_spectrum(A, radius=1.0, tol=0.0, truncation=False):
    """
    1) Compute eigenvalues of A
    2) Remove eigenvalues with |lambda| > radius (+ tol)
    3) Reconstruct A from the remaining eigenpairs using a pseudoinverse

    Parameters
    ----------
    A : (n, n) array_like
        Square matrix.
    radius : float
        Eigenvalue cutoff.
    tol : float
        Extra tolerance.
    truncation : bool
        If True, truncate unqualified eigenvalues and reconstruct A.

    Returns
    -------
    A_trunc : (n,n) ndarray if truncation is True else A
    """
    A = np.asarray(A)
    if A.ndim != 2 or A.shape[0] != A.shape[1]:
        raise ValueError("A must be square.")

    w, V = np.linalg.eig(A)

    # Qualification mask
    keep = np.abs(w) < (radius + tol)
    drop = ~keep

    w_keep = w[keep]
    w_drop = w[drop]
    V_keep = V[:, keep]

    if w_drop.size > 0:
        mpi.report(f"Unqualified eigenvalues (|λ| > {radius}):")
        for lam in w_drop:
            mpi.report(f"  {lam}")
        mpi.report("")

    if not truncation or w_keep.size == w.size:
        return A
    else:
        mpi.report(f"Reconstructing matrix with eigenvalues |λ| <= {radius} only.\n")
        n = A.shape[0]
        if w_keep.size == 0:
            return np.zeros((n, n), dtype=np.result_type(A, 1j))

        # least-squares reconstruction
        A_trunc = V_keep @ np.diag(w_keep) @ np.linalg.pinv(V_keep)
        return A_trunc