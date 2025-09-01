// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once

#include <nda/nda.hpp>
#include <triqs/gfs.hpp>
#include "./downfolding.hpp"

namespace triqs::modest {

  //-------------------------------------------------------------------------------------------

  /// Returns Tr (A) [σ,k,ω] for all k points in obe grid and all omega in Sigma mesh
  // nda::array<dcomplex, 3> spectral_function(one_body_elements_on_grid const &obe, double mu,
  //                                           // add magnetic field,
  //                                           block2_gf<mesh::refreq, matrix_valued> const &Sigma,
  //                                           nda::array<nda::matrix<double>, 2> const &Sigma_DC) {
  //   auto n_sigma       = Sigma.size2();
  //   auto const &mesh   = Sigma(0, 0).mesh();
  //   auto calc_trace_GB = detail::trace_G_B_m_G_KS(obe, mu, Sigma, Sigma_DC);
  //   auto result        = nda::zeros<dcomplex>(n_sigma, obe.H.n_k(), mesh.size());

  //   // WRONG : need to add KS TERM ?
  //   // FIXME : should we do it this way ???? cancelation of poles ???

  //   for (auto sigma : range(n_sigma)) {
  //     // FIXME JC : put openmp
  //     for (auto k_idx : nda::range(obe.H.n_k()))
  //       result(sigma, k_idx, r_all) = obe.H.k_weights(k_idx) * calc_trace_GB(sigma, k_idx);
  //   }
  //   result = -1 / M_PI * imag(result);
  //   return result;
  // }

  /// Store data of spectral functions.
  struct spectral_function_kw {
    nda::array<double, 3> data; ///< \f$ A^\sigma(k,\omega) \f$.
  };

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

  /// Store data of spectral functions.
  struct spectral_function_w {
    nda::array<double, 2> total;     ///< \f$ A^\sigma(\omega) \f$.
    nda::array<double, 4> per_theta; ///< \f$ A^{\sigma}_{mm'}(\omega) \f$.
  };

  /**
   * @ingroup post
   * @brief Compute the atom- and orbital-resolved spectral function (interacting density of states).
   * 
   * @param obe_theta One-body elements on grid created from one_body_elements_with_theta_projectors.
   * @param Proj Downfolding projector defined in the correlated space using to upfold the DMFT self-energies.
   * @param mu Chemical potential.
   * @param Sigma_w Self-energy in real-frequencies.
   * @param broadening Spectral broadening.
   * @return Atom- and orbital-resolved spectral function.
   */
  spectral_function_w projected_spectral_function(one_body_elements_on_grid const &obe_theta, downfolding_projector const &Proj, double mu,
                                                  block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, double broadening = 0.01);

} // namespace triqs::modest