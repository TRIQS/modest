// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "./downfolding.hpp"
#include <triqs/utility/streams.hpp>
#include "utils/nda_supp.hpp"

namespace triqs::modest {

  /** @name OBE factories using a fixed grid
 *  Factory functions to create one_body_elements_on_grid
 *  @{
 */

  /**
   * @ingroup one_body_elements
   * @brief Create a one-body elements object with orthonormalized projectors.
   *
   * Using the data from the "dft_input" group, the band dispersion, local space, downfolding projector, and optional 
   * IBZ symmetry ops are prepared to create a one-body elements object. This object is intended to be used in DMFT 
   * calculations.
   *
   * Our strategy is to decompose the \f${\cal C}\f$ space using the suitable basis for embedding. Each block of 
   * embedded self-energy will then be mapped to corresponding impurity models self-energy.
   *
   * The projectors are obtained from the DFT code or Wannier90, in some global coordinate system of the crystal.
   * Some basis transformations are required before performing the embedding.
   *
   * 1. A coordinate system rotation \f$R^{a}_{m_{a},m_{a}'}\f$ from the global coordinate system of atom (site) \f$a\f$ 
   *    into the local coordinate system of the crystal. This rotations ensure that equivalent atoms have the same 
   *    self-energy in the new basis, hence can be solved by the same impurity model.
   *
   * 2. Optionally, we perform a second rotation \f$ U \f$ to infer the irreps from the local Hamiltonian.
   *    Unfortunately, at this stage, the proper information about irreps is not retrieved from the electronic structure 
   *    code. A workaround has been to examine the local non-interacting Hamiltonian:
   *    \f[
   *      [H_{\mathrm{loc}}^{0}]_{m_{a} m_{a}'}^{a,\sigma} \equiv \sum_{\mathbf{k}} P_{(a,m_{a})\nu}^{\sigma}
   *      (\mathbf{k}) \varepsilon_{\nu\nu'}^{\sigma}(\mathbf{k}) [P_{(a',m_{\alpha}')\nu'}^{\sigma}
   *      (\mathbf{k})]^{\dagger}.
   *    \f]
   *
   * Its block structure is infered (up to a user-defined threshold), by discovering  a permutation of the orbitals 
   * \f$m_{a}\f$ which renders \f$H_{\mathrm{loc}}^{0}\f$ block-diagonal. Optionally, we can diagonalize these smaller 
   * blocks of the local non-interacting Hamiltonian (to reduce the off-diagonal elements in the impurity model 
   * hybridization functions \f$\Delta_{mm'}^{\sigma}\f$).
   *
   * The Wannier basis is therefore transformed as:
   * \f[
   *   P^{\sigma}_{m\nu}(\mathbf{k}) \leftarrow (RU)^{\dagger}_{m, m'} P^{\sigma}_{m'\nu}(\mathbf{k}).
   * \f]
   *
   * @param filename Hdf5 file from dft_tools converter.
   * @param threshold Off-diagonal threshold for discovery of symmetries.
   * @param diagonalize_hloc Diagonalize the local non-interacting hamiltonian.
   * @return The total electron density and a one-body elements.
   */
  std::pair<double, one_body_elements_on_grid> one_body_elements_from_dft_converter(std::string const &filename, double threshold = 1.e-5,
                                                                                    bool diagonalize_hloc = false);
  // bool verbosity        = true);
  /**
 * @ingroup one_body_elements
 * @brief Create a one-body elements object with the Θ projectors.
 *
 * @details Using the data from the "dft_parproj_input" group, the local_space, downfolding_projectors,
 * and optional ibz_symmetry_ops are prepared to create a one-body elements object.  This object is
 * intended to be used for post-processing the atom- and orbitally-resolved k-summed spectral functions.
 *
 * @param filename Hdf5 file from DFTtools converter with dft_parproj_input group.
 * @param obe One-body elements that was used in the DMFT calculation.
 * @return One-body elements using the Θ projectors.
 */
  one_body_elements_on_grid one_body_elements_with_theta_projectors(std::string const &filename, one_body_elements_on_grid const &obe);
  /**
 * @ingroup one_body_elements
 * @brief Create a one-body elements object along specific k-grid.
 *
 * @details Using the data from the "dft_bands_input" group, the band_disperion and downfolding_projector
 * are prepared to create one-body elements object. This object is intended to be used for post-processing
 * the momentum-resolved spectral function.
 *
 * @param filename  Hdf5 file from the dft_tools converter.
 * @param obe One-body elements that was ued in the DMFT calculation.
 * @return One-body elements along high-symmetry k-path.
 */
  one_body_elements_on_grid one_body_elements_on_high_symmetry_path(std::string const &filename, one_body_elements_on_grid const &obe);

  /** @} */ // Fixed grid factories

  /**
  * @brief Find symmetries of the R = 0 component of a Hamiltonian to determine a GF block structure.
  *
  * @details Discovers (approximate) irreducible symmetries for Green's function from the non-interacting part of the local
  * Hamiltonian (H0 = ∑k P(k) Hνν' P†(k) ), which represents the block structure of the TRIQS Gf.
  *
  * @param Hloc0 the R = 0 part of the Hamiltonian as a vector of [n_atoms, n_sigma]
  * @param atomic_shells the list of atomic shells used to index Hloc
  * @param block_threshold the threshold of accuracy at which a symmetry is considered found
  * @param diagonalize_hloc whether or not to diagonalize hloc
  * @return decomposition, U_rotation describing the block structure of GF based on Hloc
  */
  std::pair<nda::array<std::vector<long>, 2>, nda::array<nda::matrix<dcomplex>, 2>>
  discover_symmetries(nda::array<nda::matrix<dcomplex>, 2> const &Hloc0, std::vector<atomic_orbs> const &atomic_shells, double block_threshold,
                      bool diagonalize_hloc);

} // namespace triqs::modest