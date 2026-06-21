// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once

#include <nda/nda.hpp>
#include <triqs/gfs.hpp>
#include "./downfolding.hpp"
#include "./obe_tb.hpp"

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

  //-------------------------------------------------------------------------------------------
  // OBE tight-binding overloads
  //-------------------------------------------------------------------------------------------

  /**
   * @ingroup post
   * @brief Compute the atom- and orbital-resolved spectral function (interacting DOS) from a tight-binding OBE.
   *
   * Accumulates the local Green's function on a uniform \f$\Gamma\f$-centered Monkhorst-Pack grid built from
   * `opt.k_grid`, projects to the C-space by index-slicing the first `C_space.dim()` rows/cols of the full
   * n_orb × n_orb Wannier matrix (the same embedding convention used by `gloc(one_body_elements_tb, ...)`),
   * and reports both the C-space trace (`total`) and the full C-space matrix (`projected`). Adaptive
   * integration is not supported: `opt.run_adaptive` must be `false`.
   *
   * @param obe One-body elements built from a tight-binding Hamiltonian.
   * @param mu Chemical potential.
   * @param Sigma_w Self-energy in real-frequencies.
   * @param opt Container for BZ-integration options; `opt.k_grid` defines the uniform mesh.
   * @param broadening Spectral broadening.
   * @return Atom- and orbital-resolved spectral function.
   */
  spectral_function_w projected_spectral_function(one_body_elements_tb const &obe, double mu, block2_gf<mesh::refreq, matrix_valued> const &Sigma_w,
                                                  bz_int_options const &opt, double broadening = 0.01);

  /**
   * @ingroup post
   * @brief Compute the momentum-resolved spectral function \f$A^\sigma(k,\omega)\f$ along a user-supplied path.
   *
   * H(k) is evaluated on the fly at each row of `k_list` (fractional coordinates, units of reciprocal lattice
   * vectors). Use `triqs.lattice.utils.k_space_path` to build a path. The returned `projected` field is the
   * diagonal in the C-space orbital index (shape `[n_sigma, n_k, n_w, n_M]`), matching the on-grid path version.
   *
   * @param obe One-body elements built from a tight-binding Hamiltonian.
   * @param k_list Array of shape `[nk, 3]` of fractional k-points.
   * @param mu Chemical potential.
   * @param Sigma_w Self-energy in real-frequencies.
   * @param broadening Spectral broadening.
   * @return Momentum-resolved spectral function (total Wannier trace + C-space projection).
   */
  spectral_function_kw spectral_function_on_high_symmetry_path(one_body_elements_tb const &obe, nda::array<double, 2> const &k_list, double mu,
                                                               block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, double broadening = 0.01);

} // namespace triqs::modest
