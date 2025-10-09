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
   * @{
   */

  /**
   * @ingroup hamiltonian
   * @brief Construct a density-density interaction Hamiltonian.
   *
   * @details Create a density-density Hamiltonian
   * \f[
   *   H_{\mathrm{int}} = \frac{1}{2} \sum_{(i\sigma)\neq(j\sigma^{\prime})} U_{ij}^{\sigma\sigma^{\prime}}n_{i\sigma}
   *   n_{j\sigma^{\prime}}.
   * \f]
   *
   * @param tau_names Names of tau indices ['up', 'down'].
   * @param dim_gamma Dimension of the blocks \f$ \gamma \f$.
   * @param U_int Hubbard \f$ U \f$.
   * @param U_prime \f$ U' \f$ (typically \f$ U' = U - 2J \f$).
   * @param J_hund Kanamori \f$ J \f$.
   * @return Many-body operator representing the Hamiltonian.
   */
  operators::many_body_operator make_density_density(std::vector<std::string> const &tau_names, std::vector<long> const &dim_gamma, double U_int,
                                                     double U_prime, double J_hund);

  /**
   * @ingroup hamiltonian
   * @brief Construct a Hubbard-Kanamori Hamiltonian.
   *
   * @details Create a Hubbard-Kanamori Hamiltonian using the density-density, spin-flip, and pair-hopping interactions,
   * \f[
   *   H_{\mathrm{int}} = \frac{1}{2} \sum_{(i\sigma)\neq(j\sigma^{\prime})} U_{ij}^{\sigma\sigma^{\prime}}n_{i\sigma}
   *   n_{j\sigma^{\prime}} - \sum_{i\neq j}Jc_{i\uparrow}^{\dagger}c_{i\downarrow}c_{j\downarrow}^{\dagger}
   *   c_{j\uparrow} + \sum_{i\neq j} J c_{i\uparrow}^{\dagger}c_{i\downarrow}^{\dagger}c_{j\downarrow}c_{j\uparrow}.
   * \f]
   *
   * @param tau_names Names of tau indices ['up', 'down'].
   * @param dim_gamma Dimension of the blocks \f$ \gamma \f$.
   * @param U_int  Hubbard \f$ U \f$.
   * @param U_prime \f$ U' \f$ (typically \f$ U' = U - 2J \f$).
   * @param J_hund Kanamori \f$ J \f$.
   * @param spin_flip Spin flip term.
   * @param pair_hopping Pair-hopping term.
   * @return Many-body operator representing the Hamiltonian.
   */
  operators::many_body_operator make_kanamori(std::vector<std::string> const &tau_names, std::vector<long> const &dim_gamma, double U_int,
                                              double U_prime, double J_hund, bool spin_flip = true, bool pair_hopping = true);
  /**
   * @ingroup hamiltonian
   * @brief Construct a Slater Hamiltonian.
   *
   * @details Create a Slater Hamiltonian using fully rotationally-invariant four-index interactions:
   * \f[
   *   H_{\mathrm{int}} = \frac{1}{2} \sum_{ijkl, \sigma\sigma^{\prime}} U_{ijkl}c^{\dagger}_{i\sigma}
   *   c^{\dagger}_{j\sigma^{\prime}}c_{l\sigma^{\prime}}c_{k\sigma}.
   * \f]
   *
   * @param tau_names Names of tau indices ['up', 'down'].
   * @param dim_gamma Dimension of the blocks \f$ \gamma \f$.
   * @param U_int  Hubbard \f$ U \f$.
   * @param J_hund Hund's \f$ J \f$.
   * @param spherical_to_dft Rotation matrices from spherical \f$ Y_l^m \f$ basis to DFT orbital basis.
   * @param dft_to_local Rotation matrices from DFT basis to the local impurity basis.
   */
  operators::many_body_operator make_slater(std::vector<std::string> const &tau_names, std::vector<long> const &dim_gamma, double U_int,
                                            double J_hund, nda::matrix<dcomplex> const &spherical_to_dft,
                                            std::optional<nda::matrix<dcomplex>> const &dft_to_local);
  ///@}

  /** @name Coulomb tensor utilities
   * Utility functions for creating and working with the four index Coulomb tensor.
   * @{
   */

  /**
   * @ingroup hamiltonian
   * @brief Construct a four-index Coulomb tensor in the basis of spherical harmonics.
   *
   * @details We typically construct the four-index Coulomb tensor in the basis of spherical harmonics,
   * \f[
   *   U_{m_{1}m_{2}m_{3}m_{4}}^{\mathrm{spherical}} = \sum_{k=0}^{2l} F_{k} \alpha (l, k, m_{1}, m_{2}, m_{3}, m_{4}),
   * \f]
   * where \f$ F_{k} \f$ are radial Slater integrals and \f$ \alpha(l, k, m_{1}, m_{2}, m_{3}, m_{4}) \f$ denote angular
   * Racah-Wigner numbers for a spherically symmetric interaction tensor.
   *
   * @param l Angular quantum number.
   * @param U_int Screened Hubbard interaction.
   * @param J_hund Hund's coupling.
   * @return Coulomb tensor.
   */
  nda::array<double, 4> U_matrix_in_spherical_basis(long l, double U_int, double J_hund);

  /**
   * @ingroup hamiltonian
   * @brief Construct a four-index Coulomb tensor in a specific orbital basis.
   *
   * @param l Angular quantum number.
   * @param s2l Spherical to local basis rotation.
   * @param U_int Screened Hubbard interaction.
   * @param J_hund Hund's coupling.
   * @return Coulomb tensor.
   */
  nda::array<dcomplex, 4> U_matrix_in_local_basis(long l, nda::matrix<dcomplex> s2l, double U_int, double J_hund);

  ///@}

} // namespace triqs
