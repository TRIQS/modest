// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <nda/nda.hpp>
#include <triqs/gfs.hpp>
#include "./downfolding.hpp"
#include "utils/defs.hpp"
#include "utils/enumerate_slice.hpp"
#include "utils/nda_supp.hpp"

namespace triqs::modest::detail {

  // ===================================================================================
  // View an (n_ω, A, B) tensor as a 2D matrix (n_ω × A·B) for batched gemm.
  // ===================================================================================
  template <typename T> auto as_2d(T &&t) { return cmat_vt{nda::group_indices_view(std::forward<T>(t), nda::idx_group<0>, nda::idx_group<1, 2>)}; }

  // ===================================================================================
  // Σ_total(α, σ)(ω) = Σ_dynamic(α, σ)(ω) + Σ_static(α, σ).
  // ===================================================================================
  template <typename Mesh>
  block2_gf<Mesh, matrix_valued> make_sigma_total(block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                                                  nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static) {
    auto Sigma_total = make_block2_gf(Sigma_dynamic(0, 0).mesh(), get_struct(Sigma_dynamic));
    for (auto alpha : range(Sigma_dynamic.size1())) {
      for (auto sigma : range(Sigma_dynamic.size2())) {
        auto const &Sstat = Sigma_static(alpha, sigma);
        long n_w          = Sigma_dynamic(alpha, sigma).data().extent(0);
        for (long n = 0; n < n_w; ++n)
          Sigma_total(alpha, sigma).data()(n, r_all, r_all) = Sigma_dynamic(alpha, sigma).data()(n, r_all, r_all) + Sstat;
      }
    }
    return Sigma_total;
  }

  // ===================================================================================
  // Per-(k, σ, ω) Σ upfolding and lattice GF builders — direct N_ν × N_ν inversion path.
  // Used by the matrix-valued-H fallbacks (in `gloc` and the spectral functions)
  // ===================================================================================

  /// Upfold Σ at a single frequency w_idx: returns N_ν × N_ν.
  constexpr auto upfold_self_energy_at_freq(one_body_elements_on_grid const &obe, downfolding_projector const &Proj, auto const &Sigma_dynamic,
                                            nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static, long w_idx, long k_idx, long sigma_idx) {
    auto N_nu = obe.H.N_nu(sigma_idx, k_idx);
    auto out  = nda::zeros<dcomplex>(N_nu, N_nu);
    for (auto &&[alpha, R] : enumerated_sub_slices(get_struct(Sigma_dynamic).dims(r_all, 0) | tl::to<std::vector>())) {
      auto P = Proj.P(sigma_idx, k_idx)(R, r_all);
      out(r_all, r_all) +=
         dagger(P) * nda::matrix<dcomplex>{Sigma_dynamic(alpha, sigma_idx).data()(w_idx, r_all, r_all) + Sigma_static(alpha, sigma_idx)} * P;
    }
    return out;
  }

  /// Upfold Σ for ALL frequencies at once for a given (k, σ).  Returns (n_w, N_ν, N_ν).
  /// Used by the matrix-valued-H fallback paths that need PSP at every ω in one go.
  /// (`Sigma_static` is assumed zero here — postprocess's refreq fallbacks don't carry one.)
  inline auto upfold_self_energy_all_freq(one_body_elements_on_grid const &obe, downfolding_projector const &Proj, auto const &Sigma_w, long k_idx,
                                          long sigma_idx) {
    auto N_nu = obe.H.N_nu(sigma_idx, k_idx);
    auto n_w  = Sigma_w(0, 0).mesh().size();
    auto out  = nda::zeros<dcomplex>(n_w, N_nu, N_nu);

    for (auto &&[alpha, R] : enumerated_sub_slices(get_struct(Sigma_w).dims(r_all, 0) | tl::to<std::vector>())) {
      auto P         = Proj.P(sigma_idx, k_idx)(R, r_all);
      auto Pdag      = dagger(P);
      auto Sigma_blk = Sigma_w(alpha, sigma_idx).data();

      // Batch over all frequencies
      for (auto n : range(n_w)) { out(n, r_all, r_all) += Pdag * nda::matrix<dcomplex>{Sigma_blk(n, r_all, r_all)} * P; }
    }
    return out;
  }

  constexpr auto local_gf_at_k(one_body_elements_on_grid const &obe, double const &mu, downfolding_projector const &Proj, auto const &Sigma_dynamic,
                               nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static) {
    return [&](auto const &k_idx, auto const &sigma_idx) {
      using nda::linalg::inv;
      auto const n_M   = obe.C_space.dim();
      auto const &mesh = Sigma_dynamic(0, 0).mesh();
      auto out         = gf{mesh, {n_M, n_M}};
      auto P           = obe.P.P(sigma_idx, k_idx);
      for (auto &&[n, w] : enumerate(mesh)) {
        auto PSP                    = upfold_self_energy_at_freq(obe, Proj, Sigma_dynamic, Sigma_static, n, k_idx, sigma_idx);
        out.data()(n, r_all, r_all) = P * inv(w + mu - obe.H.H(sigma_idx, k_idx) - PSP) * dagger(P);
      }
      return out;
    };
  }

  constexpr auto lattice_gf_at_k(one_body_elements_on_grid const &obe, double const &mu, auto const &Sigma_dynamic,
                                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static) {
    return [&](auto const &k_idx, auto const &sigma_idx) {
      using nda::linalg::inv;
      auto const &mesh = Sigma_dynamic(0, 0).mesh();
      auto N_nu        = obe.H.N_nu(sigma_idx, k_idx);
      auto Glatt       = gf{mesh, {N_nu, N_nu}};
      for (auto &&[n, w] : enumerate(mesh)) {
        auto PSP                      = upfold_self_energy_at_freq(obe, obe.P, Sigma_dynamic, Sigma_static, n, k_idx, sigma_idx);
        Glatt.data()(n, r_all, r_all) = inv(w + mu - obe.H.H(sigma_idx, k_idx) - PSP);
      }
      return Glatt;
    };
  }

} // namespace triqs::modest::detail
