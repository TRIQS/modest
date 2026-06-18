// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once

#include <nda/nda.hpp>
#include <triqs/gfs.hpp>
#include "./downfolding.hpp"

namespace triqs::modest {

  /// Store data of spectral functions.
  struct spectral_function_kw {
    nda::array<double, 3> total;     ///< \f$ A^\sigma(k,\omega) \f$.
    nda::array<double, 4> projected; ///< \f$ A^\sigma_{mm}(k,\omega) \f$ (diagonal in \f$m\f$).
    static std::string hdf5_format() { return "SpectralFunctionKw"; }
    friend std::ostream &operator<<(std::ostream &out, spectral_function_kw const &x);
  };

  /// Store data of spectral functions.
  struct spectral_function_w {
    nda::array<double, 2> total;     ///< \f$ A^\sigma(\omega) \f$.
    nda::array<double, 4> projected; ///< \f$ A^{\sigma}_{mm'}(\omega) \f$.
    static std::string hdf5_format() { return "SpectralFunctionW"; }
    friend std::ostream &operator<<(std::ostream &out, spectral_function_w const &x);
  };

  /// h5 read and writes
  void h5_read(h5::group g, std::string const &name, spectral_function_kw &x);
  void h5_write(h5::group g, std::string const &name, spectral_function_kw const &x);
  void h5_read(h5::group g, std::string const &name, spectral_function_w &x);
  void h5_write(h5::group g, std::string const &name, spectral_function_w const &x);

  /**
   * @ingroup post
   * @brief Compute momentum-resolved spectral function \f$ A^\sigma(k, \omega) \f$ along high-symmetry path.
   *
   * @param obe One-body elements on grid created from one_body_elements_on_high_symmetry_path.
   * @param mu Chemical potential.
   * @param Sigma_w Self-energy in real-frequencies.
   * @param broadening Spectral broadening.
   * @return Momentum-resolved spectral function.
   */
  spectral_function_kw spectral_function_on_high_symmetry_path(one_body_elements_on_grid const &obe, double mu,
                                                               block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, double broadening = 0.01);

  /**
   * @ingroup post
   * @brief Compute the atom- and orbital-resolved spectral function (interacting density of states).
   *
   * @details Common case: `obe.P` is also the projector that upfolds the self-energy.
   * Uses the rank-reduced Woodbury fast path when `obe.H.matrix_valued == false`.
   *
   * @param obe One-body elements on grid.
   * @param mu Chemical potential.
   * @param Sigma_w Self-energy in real-frequencies.
   * @param broadening Spectral broadening.
   * @return Atom- and orbital-resolved spectral function.
   */
  spectral_function_w projected_spectral_function(one_body_elements_on_grid const &obe, double mu,
                                                  block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, double broadening = 0.01);

  /**
   * @ingroup post
   * @brief Two-projector overload: uses a downfolding projector to upfold Σ and a partial projector project G_{\nu\nu'}(\omega) into the
   * W space instead of the C space. 
   *
   * @param obe One-body elements on grid created from one_body_elements_with_partial_projectors.
   * @param Proj Downfolding projector defined in the correlated space used to upfold the DMFT self-energies.
   * @param mu Chemical potential.
   * @param Sigma_w Self-energy in real-frequencies.
   * @param broadening Spectral broadening.
   * @return Atom- and orbital-resolved spectral function.
   */
  spectral_function_w projected_spectral_function(one_body_elements_on_grid const &obe, downfolding_projector const &Proj, double mu,
                                                  block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, double broadening = 0.01);

} // namespace triqs::modest
