// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "./postprocess.hpp"
#include <iostream>
#include <atomic>
#include <stdexcept>

namespace triqs::modest {

  namespace detail {

    // Upfold self-energy for ALL frequencies at once for a given (k, sigma)
    // Returns array of shape (n_w, N_nu, N_nu)
    auto upfold_self_energy_all_freq(one_body_elements_on_grid const &obe, downfolding_projector const &Proj, auto const &Sigma_w, long k_idx,
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

  } // namespace detail

  spectral_function_w projected_spectral_function(one_body_elements_on_grid const &obe_theta, downfolding_projector const &Proj, double mu,
                                                  block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, double broadening) {
    using nda::linalg::inv;

    auto const &mesh = Sigma_w(0, 0).mesh();
    auto n_sigma     = obe_theta.C_space.n_sigma();
    auto n_k         = obe_theta.H.n_k();
    auto n_M         = obe_theta.C_space.dim();
    auto n_w         = mesh.size();

    // Accumulate local Green's function over k-points
    auto gloc_result = make_block2_gf(mesh, obe_theta.C_space.Gc_block_shape());

    for (auto k_idx : range(n_k)) {
      for (auto sigma : range(n_sigma)) {
        auto P    = Proj.P(sigma, k_idx);
        auto Pdag = dagger(P);
        auto H_k  = obe_theta.H.H(sigma, k_idx);
        auto w_k  = obe_theta.H.k_weights(k_idx);

        // Precompute upfolded self-energy for all frequencies
        auto PSP_all = detail::upfold_self_energy_all_freq(obe_theta, Proj, Sigma_w, k_idx, sigma);

        for (auto &&[n, w] : enumerate(mesh)) {
          auto G_k = inv(w + dcomplex(0, broadening) + mu - H_k - PSP_all(n, r_all, r_all));
          gloc_result(0, sigma).data()(n, r_all, r_all) += w_k * (P * nda::matrix<dcomplex>{G_k} * Pdag);
        }
      }
    }

    if (auto const &S = obe_theta.ibz_symm_ops; S) { gloc_result = S->symmetrize(gloc_result, obe_theta.C_space.atomic_decomposition()); }

    // Extract spectral functions from local Green's function
    auto total     = nda::array<double, 2>(n_sigma, n_w);
    auto per_theta = nda::array<double, 4>(n_sigma, n_w, n_M, n_M);

    for (auto sigma : range(n_sigma)) {
      auto g  = gloc_result(0, sigma).data();
      auto gC = conj(gloc_result(0, sigma)).data();
      for (auto &&[n, w] : enumerate(mesh)) {
        total(sigma, n)                   = (-1.0 / M_PI) * imag(trace(g(n, r_all, r_all)));
        per_theta(sigma, n, r_all, r_all) = real(dcomplex(0, 1.0) * (g(n, r_all, r_all) - transpose(gC(n, r_all, r_all))) / (2.0 * M_PI));
      }
    }
    return {.total = total, .per_theta = per_theta};
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

    for (auto sigma : range(n_sigma)) {
      for (auto k_idx : range(n_k)) {
        if (obe.H.N_nu(sigma, k_idx) != n_bands)
          throw std::runtime_error("spectral_function requires a fixed number of bands over all k and spin blocks");
      }
    }

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

    auto data      = nda::array<double, 3>(n_sigma, n_k, n_w);
    auto proj_data = nda::array<double, 5>(n_sigma, n_k, n_w, n_M, n_M);

#pragma omp parallel for default(none) shared(n_k, n_sigma, n_w, obe, mu, delta, Sigma_w, broadening, mesh, data, proj_data, r_all)
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
          data(sigma, k_idx, n) = (-1.0 / M_PI) * imag(trace(G_k));

          // Orbital-resolved from projection
          auto PGP                                 = P * nda::matrix<dcomplex>{G_k} * Pdag;
          proj_data(sigma, k_idx, n, r_all, r_all) = (-1.0 / M_PI) * imag(PGP);
        }
      }
    }

    return {.data = data, .proj_data = proj_data};
  }

} // namespace triqs::modest