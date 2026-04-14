import sys, glob
import numpy as np

from h5 import HDFArchive

from triqs.gf import BlockGf, GfImFreq, MeshImFreq, iOmega_n, inverse

from triqs_dft_tools.sumk_dft import SumkDFT

from copy import deepcopy

np.random.seed(10001)

def from_dict_to_vector(sum_k, obj):
    out_obj = [ [ None for sp in sum_k.spin_block_names[sum_k.SO] ] for atom in range(sum_k.n_corr_shells) ]
    for iatom, out in enumerate(out_obj):
        for isig, sigma in enumerate(sum_k.spin_block_names[sum_k.SO]):
            out_obj[iatom][isig] = obj[sum_k.corr_to_inequiv[iatom]][sigma]
    return out_obj

flatten = lambda x : [ [ y[sp] for sp in y ] for y in x]

def generate_reference_data(filename : str, n_iw=30):
    print(f'generating reference data for {filename}...')

    mesh = MeshImFreq(beta=40, n_iw=n_iw, S='Fermion')
    sum_k = SumkDFT(filename, mesh=mesh)

    for i, r in enumerate(sum_k.rot_mat):
        svd = np.linalg.svd(r)
        sum_k.rot_mat[i] = svd.U @ svd.Vh

    ref_density = sum_k.total_density(mu=0, with_dc=False, with_Sigma=False)

    Gloc_dft = sum_k.extract_G_loc(with_Sigma=False, transform_to_solver_blocks=False)
    Hloc0_dft = from_dict_to_vector(sum_k, sum_k.eff_atomic_levels())

    dens_mat = [gf.density() for gf in Gloc_dft]
    sum_k.analyse_block_structure(dm=dens_mat)

    Gloc_dft2 = sum_k.extract_G_loc(with_Sigma=False)
    dens_mat = [gf.density() for gf in Gloc_dft2]
    dc_types = ['sFLL', 'cHeld','sAMF']
    dc_terms= {}
    for dc in dc_types:
        for ineq in range(sum_k.n_inequiv_shells):
            sum_k.calc_dc(dens_mat[ineq], orb=ineq,  U_interact=2.0, J_hund=1.0, use_dc_formula=dc, transform=False)
        dc_terms[dc] =  {'dc_imp' : flatten(sum_k.dc_imp), 'E_dc' : deepcopy(sum_k.dc_energ) }

    Sigma_imp_solver_const = [sum_k.block_structure.create_gf(ish=ineq, space='solver', mesh=sum_k.mesh) for ineq in range(sum_k.n_inequiv_shells)]
    for sigma in Sigma_imp_solver_const:
        for block, gf in sigma: sigma[block].data[:] += np.random.rand(*gf.data.shape)

    sum_k.put_Sigma(Sigma_imp_solver_const)
    Gloc_const_sigma = sum_k.extract_G_loc(with_Sigma=True, transform_to_solver_blocks=False, with_dc=False)

    Sigma_imp_solver = [sum_k.block_structure.create_gf(ish=ineq, space='solver', mesh=sum_k.mesh) for ineq in range(sum_k.n_inequiv_shells)]
    for sigma in Sigma_imp_solver:
        for block, gf in sigma:
            for (iom, om) in enumerate(gf.mesh):
                sigma[block].data[iom,:,:] += np.random.rand(*gf.data.shape[1:]) + np.random.rand(*gf.data.shape[1:])**2 / complex(om)
    sum_k.put_Sigma(Sigma_imp_solver)
    Gloc_sigma = sum_k.extract_G_loc(with_Sigma=True, transform_to_solver_blocks=False, with_dc=False)

    with HDFArchive(filename, 'a') as ar:
        if 'ref_data' not in ar: ar.create_group('ref_data');
        ar['ref_data']['hloc0']      = Hloc0_dft
        ar['ref_data']['Gloc_DFT']   = Gloc_dft
        ar['ref_data']['Gloc_constSigma'] = Gloc_const_sigma
        ar['ref_data']['const_Sigma_input'] = Sigma_imp_solver_const
        ar['ref_data']['Sigma_input'] = Sigma_imp_solver
        ar['ref_data']['Gloc_Sigma'] = Gloc_sigma
        if sum_k.symm_op: ar['ref_data']['permutations'] =  sum_k.symmcorr.orb_map

        if 'dc' not in ar['ref_data']: ar['ref_data'].create_group('dc')
        for key in dc_terms.keys():
            if key not in ar['ref_data']['dc']: ar['ref_data']['dc'].create_group(key)
            for key2 in dc_terms[key].keys(): ar['ref_data']['dc'][key][key2] = dc_terms[key][key2]

        ar['ref_data']['total_density_zero_mu'] =  ref_density

    return None

if __name__ == '__main__':
    ref_filenames = glob.glob("ref_data/*.ref.h5") + glob.glob("ref_data_lfs/*.ref.h5") 
    skip_files = ['ref_data/u_matrix_slater.ref.h5', 'ref_data_lfs/la5ni3o11-wien2k-large-mesh.ref.h5']
    for filename in ref_filenames:
        if filename in skip_files: continue
        generate_reference_data(filename)
    generate_reference_data("ref_data_lfs/la5ni3o11-wien2k-large-mesh.ref.h5", n_iw=50)
