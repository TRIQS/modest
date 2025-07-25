from triqs import version
from h5 import HDFArchive
from triqs.operators.util.U_matrix import U_matrix_slater, spherical_to_cubic

spherical_to_cubic_conventions = ['wien2k', 'qe', 'vasp', 'wannier90']
U_int  = 10.0
J_hund = 1.0

with HDFArchive("u_matrix_slater.ref.h5", "a") as ar:
    ar.create_group('triqs-details')
    ar['triqs-details']['version'] = version.version
    ar['triqs-details']['git_hash'] = version.git_hash
    ar['U_int'] = U_int
    ar['J_hund'] = J_hund
    for l in [1,2,3]:
        ar.create_group(str(l))
        ar[str(l)]['spherical'] = U_matrix_slater(l, U_int=U_int, J_hund=J_hund, basis='spherical')
    for l in [1,2]:
        for convention in spherical_to_cubic_conventions:
            T = spherical_to_cubic(l, convention=convention)
            ar[str(l)][convention] = U_matrix_slater(l, U_int=U_int, J_hund=J_hund, basis='other', T=T)
