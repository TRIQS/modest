import numpy as np; np.set_printoptions(3)

import triqs_modest as tm

# Here we should how the loaded obe has off-digaonal elements due to the octahedral rotation.
# This means the cubic basis that we have used to construct the orbitals is not the most ideal one.
# Here we will show how to circumvent.
# To make the problem more explict, we calculate the hybridization function Δ that it has non-zero off-diagonal components.
# This is not good for some impurity solvers like CT-HYB or CT-SEG which are based on expansions of the hybridization function.
#           | x | 0 | 0 | 0 | 0 |
#           | 0 | x | x | 0 | 0 |
#  Delta =  | 0 | x | x | 0 | 0 |  <--- The block structure of Δ
#           | 0 | 0 | 0 | x | 0 |
#           | 0 | 0 | 0 | 0 | x |
# The resulting embedding will be a 5 orbital impurity problem, but if we look at the electronic structure
# only the t2g-like orbitals are relevant and therefore those are the ones that we would like to treat as correlated.
# Inspect the impurity_levels (tm.impurity_levels(obe1)). Will reveal the energy levels and the block structure.
_, obe1 = tm.one_body_elements_from_dft_converter("Sr2MgOsO6.h5"); print(obe1)

# We provide a function rotate_to_local_basis(R, obe), which can rotate an obe by any unitary matrix.
# You can find this unitary matrix by diagaonlizing the local impurity levels or the density matrix.
# Try this: 
# Find U from diagonalizing hloc0 = impurity_levels(obe), then use U to rotate to a new obe.
# Luckily, in modest we provide the ability to diagonalize_hloc upon loading the obe. The rotation matrices are stored
# in obe.C_space.rotation_matrices_from_dft_to_local_basis. These are helpful when constructing the interaction.
_, obe2 = tm.one_body_elements_from_dft_converter("Sr2MgOsO6.h5", diagonalize_hloc=True); print(obe2)
# If you look at the impurity levels of this rotated obe you will see that it was diagonalized based on the original block structure
# that was discovered. Let us check if the hybridization function is diagonal. Indeed, in this rotated (crystal-field basis) it has become diagonal.
# Now at this point, we can actually make a third obe for ultimate control.
_, obe3 = tm.one_body_elements_from_dft_converter("Sr2MgOsO6.h5", diagonalize_hloc=True, threshold=-1); print(obe3)
# This obe was diagonalized without using the original block structure. By checking the impurity levels you can see that they are the same but the order 
# is diffrent. This obe3 is convenient to work with because it automatically orders the crystal-field levels in increasing order.
# We can construct an embedding from this obe3.
E3 = tm.make_embedding(obe3.C_space); print(E3.description(True))
# But we want to manipulate it to reflect the symmetries of our problem.
E4 = E3.split_imp_block(0, 0, [1,1,1,2])
print(E4.description(True))
E5 = E4.split_imp(0, [0,1,2])
print(E5.description(True))
E  = E5.drop_imp(1)
# 5 --> 3 [1,1,1] x 2 
# separate [3] , [2] into 2 impurities
# drop the second impurity, we only care about the first 3 orbitals
print(E.description(True))
