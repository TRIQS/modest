// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "./postprocess.hpp"
#include "./lattice_gf_helpers.hpp"
#include <iostream>
#include <atomic>
#include <stdexcept>

namespace triqs::modest {

  spectral_function_w projected_spectral_function(one_body_elements_on_grid const &obe, downfolding_projector const &Proj, double mu,
                                                  block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, double broadening) {
    using nda::linalg::inv;

    auto const &mesh = Sigma_w(0, 0).mesh();
    auto n_sigma     = obe.C_space.n_sigma();
    auto n_k         = obe.H.n_k();
    auto n_M         = obe.C_space.dim();
    auto n_w         = mesh.size();
    auto im          = dcomplex(0, 1.0);
    auto delta       = im * broadening;

    // Accumulate local Green's function over k-points
    auto gloc_result = make_block2_gf(mesh, obe.C_space.Gc_block_shape());

    for (auto k_idx : range(n_k)) {
      for (auto sigma : range(n_sigma)) {
        auto P    = obe.P.P(sigma, k_idx);
        auto Pdag = dagger(P);
        auto H_k  = obe.H.H(sigma, k_idx);
        auto w_k  = obe.H.k_weights(k_idx);

        // Precompute upfolded self-energy for all frequencies
        auto PSP_all = detail::upfold_self_energy_all_freq(obe, Proj, Sigma_w, k_idx, sigma);

        for (auto &&[n, w] : enumerate(mesh)) {
          auto G_k = nda::matrix<dcomplex>{inv(w + delta + mu - H_k - PSP_all(n, r_all, r_all))};
          gloc_result(0, sigma).data()(n, r_all, r_all) += w_k * (P * G_k * Pdag);
        }
      }
    }

    if (auto const &S = obe.ibz_symm_ops; S) { gloc_result = S->symmetrize(gloc_result, obe.C_space.atomic_decomposition()); }

    // Extract spectral functions from local Green's function
    auto total     = nda::zeros<double>(n_sigma, n_w);
    auto projected = nda::zeros<double>(n_sigma, n_w, n_M, n_M);

    for (auto sigma : range(n_sigma)) {
      auto const &G = gloc_result(0, sigma).data();
      for (auto &&[n, w] : enumerate(mesh)) {
        auto g                            = nda::matrix<dcomplex>{G(n, r_all, r_all)};
        total(sigma, n)                   = (-1.0 / M_PI) * imag(trace(g));
        projected(sigma, n, r_all, r_all) = real(im * (g - dagger(g)) / (2 * M_PI));
      }
    }

    return {.total = total, .projected = projected};
  }

  nda::array<double, 4> spectral_function(one_body_elements_on_grid const &obe, double mu, block2_gf<mesh::refreq, matrix_valued> const &Sigma_w,
                                          double broadening) {
    using nda::linalg::inv;

    auto const &mesh = Sigma_w(0, 0).mesh() | tl::to<std::vector>();
    auto n_sigma     = obe.C_space.n_sigma();
    auto n_k         = obe.H.n_k();
    auto n_w         = mesh.size();
    auto n_bands     = obe.H.N_nu(0, 0);
    auto delta       = dcomplex(0, broadening);

    auto data = nda::zeros<double>(n_sigma, n_w, n_bands, n_bands);

    for (auto k_idx : range(n_k)) {
      for (auto sigma : range(n_sigma)) {
        auto H_k      = obe.H.H(sigma, k_idx);
        auto k_weight = obe.H.k_weights(k_idx);

        auto PSP_all = detail::upfold_self_energy_all_freq(obe, obe.P, Sigma_w, k_idx, sigma);

        for (auto &&[n, w] : enumerate(mesh)) {
          auto G_k = inv(w + delta + mu - H_k - PSP_all(n, r_all, r_all));
          auto A_k = real(dcomplex(0, 1.0) * (G_k - dagger(G_k)) / (2.0 * M_PI));
          data(sigma, n, r_all, r_all) += k_weight * A_k;
        }
      }
    }

    return data;
  }

  spectral_function_kw spectral_function_on_high_symmetry_path(one_body_elements_on_grid const &obe, double mu,
                                                               block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, double broadening) {
    using nda::linalg::inv;

    auto const &mesh = Sigma_w(0, 0).mesh() | tl::to<std::vector>();
    auto n_sigma     = obe.C_space.n_sigma();
    auto n_w         = mesh.size();
    auto n_k         = obe.H.n_k();
    auto n_M         = obe.C_space.dim();
    auto delta       = dcomplex(0, broadening);

    auto total     = nda::zeros<double>(n_sigma, n_k, n_w);
    auto projected = nda::zeros<double>(n_sigma, n_k, n_w, n_M);

#pragma omp parallel for default(none) shared(n_k, n_sigma, n_M, obe, mu, delta, Sigma_w, mesh, total, projected, r_all)
    for (auto k_idx : range(n_k)) {
      for (auto sigma : range(n_sigma)) {
        auto P    = obe.P.P(sigma, k_idx);
        auto Pdag = dagger(P);
        auto H_k  = obe.H.H(sigma, k_idx);

        // Precompute upfolded self-energy for all frequencies
        auto PSP_all = detail::upfold_self_energy_all_freq(obe, obe.P, Sigma_w, k_idx, sigma);

        for (auto &&[n, w] : enumerate(mesh)) {
          auto G_k = inv(w + delta + mu - H_k - PSP_all(n, r_all, r_all));

          // Total spectral function from trace
          total(sigma, k_idx, n) = (-1.0 / M_PI) * imag(trace(G_k));

          // Orbital-resolved from projection, diagonal in m
          auto PGP = nda::matrix<dcomplex>{P * nda::matrix<dcomplex>{G_k} * Pdag};
          for (auto m : range(n_M)) projected(sigma, k_idx, n, m) = (-1.0 / M_PI) * imag(PGP(m, m));
        }
      }
    }

    return {.total = total, .projected = projected};
  }

} // namespace triqs::modest