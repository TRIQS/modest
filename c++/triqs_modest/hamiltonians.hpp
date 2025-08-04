// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <triqs/gfs.hpp>
#include <triqs/operators.hpp>
#include <triqs/operators/many_body_operator.hpp>

using namespace triqs::gfs;

namespace triqs {

  /** @name Hamiltonians
  * Functions to create the impurity interaction using many-body operators.
  */
  ///@{
  /**
 * @ingroup hamiltonian
 * @brief Construct a density-density interation
 * @details Create a density-density Hamiltonian
 * $$ H_{\mathrm{int}} = \frac{1}{2} \sum_{(i\sigma)\neq(j\sigma^{\prime})} U_{ij}^{\sigma\sigma^{\prime}}n_{i\sigma}n_{j\sigma^{\prime}}.$$
 * 
 * @param tau_names names of tau indices ['up', 'down']
 * @param dim_gamma dimension of the blocks γ
 * @param U_int Hubbard U
 * @param U_prime U' (typically U' = U - 2J)
 * @param J_hund Kanamori J
 */
  operators::many_body_operator make_density_density(std::vector<std::string> const &tau_names, std::vector<long> const &dim_gamma, double U_int,
                                                     double U_prime, double J_hund);

  /**
 * @ingroup hamiltonian
 * @brief Construct a Hubbard-Kanamori Hamiltonian
 * 
 * @details Create a Hubbard-Kanamori Hamiltonian using the density-density, spin-flip, and pair-hopping interactions,
 * $$ H_{\mathrm{int}} = \frac{1}{2} \sum_{(i\sigma)\neq(j\sigma^{\prime})} U_{ij}^{\sigma\sigma^{\prime}}n_{i\sigma}n_{j\sigma^{\prime}} - \sum_{i\neq j}Jc_{i\uparrow}^{\dagger}c_{i\downarrow}c_{j\downarrow}^{\dagger}c_{j\uparrow} + \sum_{i\neq j} J c_{i\uparrow}^{\dagger}c_{i\downarrow}^{\dagger}c_{j\downarrow}c_{j\uparrow}.$$
 * 
 * @param tau_names names of tau indices ['up', 'down']
 * @param dim_gamma dimension of the blocks γ
 * @param U_int  Hubbard U
 * @param U_prime  U' typical U' = U - 2J
 * @param J_hund  Kanamori J
 * @param spin_flip  spin flip term
 * @param pair_hopping pair hopping term
 */
  operators::many_body_operator make_kanamori(std::vector<std::string> const &tau_names, std::vector<long> const &dim_gamma, double U_int,
                                              double U_prime, double J_hund, bool spin_flip = true, bool pair_hopping = true);
  /**
 * @ingroup hamiltonian
  * @brief Construct a new operators::many body operator make slater object
  * @details Create a Slater Hamiltonian using fully rotationally-invariant four-index interactions:
  * $$H_{\mathrm{int}} = \frac{1}{2} \sum_{ijkl, \sigma\sigma^{\prime}} U_{ijkl}c^{\dagger}_{i\sigma}c^{\dagger}_{j\sigma^{\prime}}c_{l\sigma^{\prime}}c_{k\sigma}.$$
  * 
  * @param tau_names 
  * @param dim_gamma 
  * @param U_int 
  * @param J_hund 
  * @param spherical_to_dft 
  * @param dft_to_local 
  */
  operators::many_body_operator make_slater(std::vector<std::string> const &tau_names, std::vector<long> const &dim_gamma, double U_int,
                                            double J_hund, nda::matrix<dcomplex> const &spherical_to_dft,
                                            std::optional<nda::matrix<dcomplex>> const &dft_to_local);
  ///@}

  /** @name Coulomb tensor utilities
  * Utility functions for creating and working with the four index Coulomb tensor.
  */
  ///@{
  /**
 * @ingroup hamiltonian
 * @brief Construct a four-index Coulomb tensor in the basis of spherical harmonics.
 * @details We typically construct the four-index Coulomb tensor in the basis of spherical harmonics,
 * $$ U_{m_{1}m_{2}m_{3}m_{4}}^{\mathrm{spherical}} = \sum_{k=0}^{2l} F_{k} \alpha (l, k, m_{1}, m_{2}, m_{3}, m_{4}),$$
 * where $F_{k}$ are radial Slater integrals and \f$\alpha(l, k, m_{1}, m_{2}, m_{3}, m_{4})\f$ denote angular Racah-Wigner numbers for a spherically symmetric
 * interaction tensor.
 * 
 * @param l angular quantum number
 * @param U_int screend Hubbard interaction
 * @param J_hund Hund's coupling
 * @return nda::array<double, 4> 
 */
  nda::array<double, 4> U_matrix_in_spherical_basis(long l, double U_int, double J_hund);

  /**
 * @ingroup hamiltonian
 * @brief Construct a four-index Coulomb tensor in a specific orbital basis.
 * 
 * @param l  angular quantum number
 * @param s2l spherical to local basis rotation
 * @param U_int screened Hubbard interaction
 * @param J_hund Hund's coupling
 * @return nda::array<dcomplex, 4> 
 */
  nda::array<dcomplex, 4> U_matrix_in_local_basis(long l, nda::matrix<dcomplex> s2l, double U_int, double J_hund);
  ///@}

} // namespace triqs
