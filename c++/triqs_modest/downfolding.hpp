// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <triqs/gfs.hpp>
#include <type_traits>
#include "./local_space.hpp"
#include "./ibz_symmetry_ops.hpp"
#include "utils/nda_supp.hpp"

namespace triqs::modest {

  // In NonPolarized case, σ logically has 2 values, but
  // the data are the same, so we store only on.
  // this method maps σ to its index for the data arrays
  [[nodiscard]] inline long sigma_to_data_idx(spin_kind_e spin_kind, long sigma) { return (spin_kind == spin_kind_e::Polarized) ? sigma : 0; };

  /**
 * @ingroup one_body_elements
 * @brief The one-body dispersion as a function of momentum.
 *
 * @details A band dispersion containing the DFT band structure ε(ν,k,σ), weights in the Brillouin zone, and the spin kind used in the DFT calculation.
 * 
 */
  struct band_dispersion {
    spin_kind_e spin_kind;             ///< Spin kind of the one-body data
    nda::array<dcomplex, 4> H_k;       ///< H_k [k_idx, σ', nu, nu']
    nda::array<long, 2> n_bands_per_k; ///< n_bands_per_k [k_idx, σ'] = # of nu
    nda::array<double, 1> k_weights;   ///< k_weights[k_idx]
    bool matrix_valued;                ///< Is the dispersion matrix-valued?

    // public:
    // /// Constructor
    // band_dispersion(spin_kind_e spin_kind, nda::array<dcomplex, 4> H_k, nda::array<long, 2> n_bands_per_k,
    //                 nda::array<double, 1> k_weights)
    //    : spin_kind(spin_kind),
    //      H_k(std::move(H_k)),
    //      n_bands_per_k(std::move(n_bands_per_k)),
    //      _k_weights(std::move(k_weights)) {
    //   // TODO add some checks on the data ?
    // }

    /// H^σ(k)_ν, returned as a MATRIX in (ν, ν)
    [[nodiscard]] nda::matrix_const_view<dcomplex> H(long sigma, long k_idx) const {
      auto sigma_p = sigma_to_data_idx(spin_kind, sigma);
      auto R_nu    = nda::range(n_bands_per_k(k_idx, sigma_p));
      return H_k(k_idx, sigma_p, R_nu, R_nu);
    }

    /// Number of bands #ν
    [[nodiscard]] long N_nu(long sigma, long k_idx) const { return n_bands_per_k(k_idx, sigma_to_data_idx(spin_kind, sigma)); }

    /// Number of k points in the grid
    [[nodiscard]] long n_k() const { return H_k.extent(0); }

    // /// Weight of k point (typically from DFT code).
    // [[nodiscard]] double k_weights(long k_idx) const { return _k_weights(k_idx); }

    /// printing
    friend std::ostream &operator<<(std::ostream &out, band_dispersion const &bd);
  };

  /**
 * @ingroup one_body_elements
 * @brief The projector that downfolds the one-body dispersion (ν) onto local orbitals (m).
 * 
 * @details A downfoldin projector contains the projector, the kind of spin used in the projection, and the number of bands per k-point for cases
 * when a band goes outside of the projection window.
 */
  struct downfolding_projector {
    spin_kind_e spin_kind;
    nda::array<dcomplex, 4> P_k;       ///< Pk[alpha][k_idx, σ', m_alpha, nu]
    nda::array<long, 2> n_bands_per_k; ///< n_bands_per_k [k_idx, σ'] = # of nu

    // public:
    // /// Constructor
    // downfolding_projector(spin_kind_e spin_kind, nda::array<dcomplex, 4> P_k, nda::array<long, 2> n_bands_per_k)
    //    : spin_kind(spin_kind), P_k(std::move(P_k)), n_bands_per_k(std::move(n_bands_per_k)) {}

    // ----------------- accessors --------------------
    // FIXME : SWAP k and sigma in data
    /// P^σ(k)_mν, returned as a matrix in (m ν)
    [[nodiscard]] nda::matrix_const_view<dcomplex> P(long sigma, long k_idx) const {
      auto sigma_p = sigma_to_data_idx(spin_kind, sigma);
      auto R_nu    = nda::range(n_bands_per_k(k_idx, sigma_p));
      return P_k(k_idx, sigma_p, r_all, R_nu);
    }
    /// Rotates the local basis of the downfolding projector
    downfolding_projector rotate_local_basis(nda::array<nda::matrix<dcomplex>, 2> const &U) const;
    /// printing
    friend std::ostream &operator<<(std::ostream &out, downfolding_projector const &proj);
  };

  // ------------------------------------------------------------
  /**
 * @ingroup one_body_elements
 * @brief A one-body elements struct where all of the underlying data exists on a fixed momentum grid.
 */
  struct one_body_elements_on_grid {
    band_dispersion H;
    local_space C_space;
    downfolding_projector P;
    C2PY_IGNORE std::optional<ibz_symmetry_ops> ibz_symm_ops = {}; ///< IBZ symmetrizer after a k-sum
  };

  std::ostream &operator<<(std::ostream &out, one_body_elements_on_grid const &);
  void h5_read(h5::group g, std::string const &name, one_body_elements_on_grid &x);
  void h5_write(h5::group g, std::string const &name, one_body_elements_on_grid const &x);

  //-------------------------------------------------------------
  one_body_elements_on_grid permute_local_space(std::vector<std::vector<long>> const &atom_partition, one_body_elements_on_grid const &x);

  // -------------------------------------------------------------
  /// Rotates the local basis of the downfolding projector
  one_body_elements_on_grid rotate_local_basis(nda::array<nda::matrix<dcomplex>, 2> const &U, one_body_elements_on_grid const &x);
  // -------------------------------------------------------------

  /// Compute the local impurity levels from the single-particle dispersion.

  /**
   * @ingroup hybridization
   * @brief Compute the atomic (impurity) levels from an obe.
   * 
   * @param obe One-body elements
   * @return nda::array<nda::matrix<dcomplex>, 2> 
   */
  nda::array<nda::matrix<dcomplex>, 2> impurity_levels(one_body_elements_on_grid const &obe);
  // -------------------------------------------------------------

  // These functions are not part of the public API: not documented, not wrapped.
  namespace detail {
    /// The projected bare function on the C space, cf notes.
    nda::array<dcomplex, 3> G0_C_k_sigma(one_body_elements_on_grid const &obe, double mu, long k_idx, long sigma, std::vector<dcomplex> const &omegas,
                                         bool mu_derivative = false);
  } // namespace detail

} // namespace triqs::modest