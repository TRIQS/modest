// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "./postprocess.hpp"

namespace triqs::modest {

  namespace detail {

    constexpr auto upfold_self_energy_at_freq(one_body_elements_on_grid const &obe, downfolding_projector const &Proj, auto const &Sigma_w,
                                              long w_idx, long k_idx, long sigma_idx) {
      auto N_nu = obe.H.N_nu(sigma_idx, k_idx);
      auto out  = nda::zeros<dcomplex>(N_nu, N_nu);
      for (auto &&[alpha, R] : enumerated_sub_slices(get_struct(Sigma_w).dims(r_all, 0) | tl::to<std::vector>())) {
        auto P = Proj.P(sigma_idx, k_idx)(R, r_all);
        out(r_all, r_all) += dagger(P) * nda::matrix<dcomplex>{Sigma_w(alpha, sigma_idx).data()(w_idx, r_all, r_all)} * P;
      }
      return out;
    }

    template <bool Trace>
    constexpr auto lattice_gf_at_k(auto const &mesh, one_body_elements_on_grid const &obe, downfolding_projector const &Proj, double mu,
                                   auto const &Sigma_w, dcomplex broadening) {
      return [&, mu, broadening](auto &k_idx, auto &sigma) {
        if constexpr (Trace) {
          auto out = gf{mesh};
          for (auto &&[n, w] : enumerate(mesh)) {
            auto PSP      = upfold_self_energy_at_freq(obe, Proj, Sigma_w, n, k_idx, sigma);
            out.data()(n) = trace(inverse(w + broadening + mu - obe.H.H(sigma, k_idx) - PSP));
          }
          return out;
        } else {
          auto N_nu = obe.H.N_nu(sigma, k_idx);
          auto out  = gf{mesh, {N_nu, N_nu}};
          for (auto &&[n, w] : enumerate(mesh)) {
            auto PSP                    = upfold_self_energy_at_freq(obe, Proj, Sigma_w, n, k_idx, sigma);
            out.data()(n, r_all, r_all) = inverse(w + broadening + mu - obe.H.H(sigma, k_idx) - PSP);
          }
          return out;
        }
      };
    }

    constexpr auto local_gf_at_k(auto const &mesh, one_body_elements_on_grid const &obe, downfolding_projector const &Proj, double mu,
                                 auto const &Sigma_w, dcomplex broadening) {
      return [&, mu, broadening](auto &k_idx, auto &sigma) {
        auto n_M = obe.C_space.dim();
        auto out = gf{mesh, {n_M, n_M}};
        auto P   = obe.P.P(sigma, k_idx);
        for (auto &&[n, w] : enumerate(mesh)) {
          auto PSP                    = upfold_self_energy_at_freq(obe, Proj, Sigma_w, n, k_idx, sigma);
          out.data()(n, r_all, r_all) = P * inverse(w + broadening + mu - obe.H.H(sigma, k_idx) - PSP) * dagger(P);
        }
        return out;
      };
    }

  } // namespace detail

  spectral_function_w projected_spectral_function(one_body_elements_on_grid const &obe_theta, downfolding_projector const &Proj, double mu,
                                                  block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, double broadening) {

    auto const &mesh = Sigma_w(0, 0).mesh();

    auto n_sigma = obe_theta.C_space.n_sigma();
    auto n_k     = obe_theta.H.n_k();

    auto gloc_at_k = detail::local_gf_at_k(mesh, obe_theta, Proj, mu, Sigma_w, dcomplex(0, broadening));

    auto gloc_result = make_block2_gf(mesh, obe_theta.C_space.Gc_block_shape());

    for (auto const &k_idx : range(n_k)) {
      for (auto const &sigma : range(n_sigma)) {
        gloc_result(0, sigma).data()(r_all, r_all, r_all) += obe_theta.H.k_weights(k_idx) * gloc_at_k(k_idx, sigma).data();
      }
    }

    gloc_result = mpi::all_reduce(gloc_result);

    if (auto const &S = obe_theta.ibz_symm_ops; S) { gloc_result = S->symmetrize(gloc_result, obe_theta.C_space.atomic_decomposition()); }

    auto n_M = obe_theta.C_space.dim();
    auto n_w = mesh.size();

    auto total     = nda::array<double, 2>(n_sigma, n_w);
    auto per_theta = nda::array<double, 4>(n_sigma, n_w, n_M, n_M);

    for (auto const &sigma : nda::range(n_sigma)) {
      auto g  = gloc_result(0, sigma).data();
      auto gC = conj(gloc_result(0, sigma)).data();
      for (auto [n, w] : enumerate(mesh)) {
        // auto g = gloc_result(0, sigma).data()(n, r_all, r_all);
        //FIXME: can trace ignore the mesh dimension (or in general the first axis dimension and broadcast over all the w-mesh points)
        total(sigma, n) = (-1.0 / M_PI) * imag(trace(g(n, r_all, r_all)));
        // missing sign?
        per_theta(sigma, n, r_all, r_all) = real(dcomplex(0, 1.0) * (g(n, r_all, r_all) - transpose(gC(n, r_all, r_all))) / (2.0 * M_PI));
      }
    }
    return {.total = total, .per_theta = per_theta};
  }

  spectral_function_kw spectral_function_on_high_symmetry_path(one_body_elements_on_grid const &obe, double mu,
                                                               block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, double broadening) {
    auto const &mesh = Sigma_w(0, 0).mesh();
    auto n_sigma     = obe.C_space.n_sigma();
    auto n_w         = mesh.size();
    auto n_k         = obe.H.n_k();
    auto n_M         = obe.C_space.dim();

    auto data      = nda::array<double, 3>(n_sigma, n_k, n_w);
    auto proj_data = nda::array<double, 5>(n_sigma, n_k, n_w, n_M, n_M);

    auto trglatt_at_k = detail::lattice_gf_at_k<true>(mesh, obe, obe.P, mu, Sigma_w, broadening);
    auto glatt_at_k   = detail::lattice_gf_at_k<false>(mesh, obe, obe.P, mu, Sigma_w, broadening);

    for (auto k_idx : range(n_k)) {
      for (auto sigma : range(n_sigma)) {
        data(sigma, k_idx, r_all) = (-1.0 / M_PI) * imag(trglatt_at_k(k_idx, sigma).data());
        auto P                    = obe.P.P(sigma, k_idx);

        for (auto n : range(n_w)) {
          auto PGP                                 = P * nda::matrix<dcomplex>{glatt_at_k(k_idx, sigma).data()(n, r_all, r_all)} * dagger(P);
          proj_data(sigma, k_idx, n, r_all, r_all) = (-1.0 / M_PI) * imag(PGP);
        }
      }
    }
    return {.data = data, .proj_data = proj_data};
  }

} // namespace triqs::modest