#pragma once
#include <triqs/gfs.hpp>
#include <triqs/operators.hpp>
#include <triqs/operators/many_body_operator.hpp>

using namespace triqs::gfs;

namespace triqs {
  /**
 * @ingroup impurity
 * @brief Construct a density-density interation
 * 
 * @param tau_names names of tau indices ['up', 'down']
 * @param dim_gamma dimension of the blocks γ
 * @param U_int Hubbard U
 * @param U_prime U' (typically U' = U - 2J)
 * @param J_hund Kanamori J
 */
  operators::many_body_operator make_density_density(std::vector<std::string> const &tau_names,
                                                     std::vector<long> const &dim_gamma, double U_int, double U_prime,
                                                     double J_hund);

  /**
 * @ingroup impurity
 * @brief Construct a Hubbard-Kanamori Hamiltonian
 * 
 * @param tau_names names of tau indices ['up', 'down']
 * @param dim_gamma dimension of the blocks γ
 * @param U_int  Hubbard U
 * @param U_prime  U' typical U' = U - 2J
 * @param J_hund  Kanamori J
 * @param spin_flip  spin flip term
 * @param pair_hopping pair hopping term
 */
  operators::many_body_operator make_kanamori(std::vector<std::string> const &tau_names,
                                              std::vector<long> const &dim_gamma, double U_int, double U_prime,
                                              double J_hund, bool spin_flip = true, bool pair_hopping = true);
  /**
  * @ingroup impurity
  * @brief Construct a new operators::many body operator make slater object
  * 
  * @param tau_names 
  * @param dim_gamma 
  * @param U_int 
  * @param J_hund 
  * @param spherical_to_dft 
  * @param dft_to_local 
  */
  operators::many_body_operator make_slater(std::vector<std::string> const &tau_names,
                                            std::vector<long> const &dim_gamma, double U_int, double J_hund,
                                            nda::matrix<dcomplex> const &spherical_to_dft,
                                            std::optional<nda::matrix<dcomplex>> const &dft_to_local);
  /**
 * @ingroup impurity_utilities
 * @brief 
 * 
 * @param l 
 * @param U_int 
 * @param J_hund 
 * @return nda::array<double, 4> 
 */
  nda::array<double, 4> U_matrix_in_spherical_basis(long l, double U_int, double J_hund);

  /**
 * @ingroup impurity_utilities
 * @brief 
 * 
 * @param l 
 * @param s2l 
 * @param U_int 
 * @param J_hund 
 * @return nda::array<dcomplex, 4> 
 */
  nda::array<dcomplex, 4> U_matrix_in_local_basis(long l, nda::matrix<dcomplex> s2l, double U_int, double J_hund);
} // namespace triqs
