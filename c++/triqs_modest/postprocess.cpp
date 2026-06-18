// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "./postprocess.hpp"
#include "./lattice_gf_helpers.hpp"
#include "utils/defs.hpp"
#include <iostream>
#include <chrono>

namespace triqs::modest {

  // All `detail::` helpers used here (upfold_self_energy_all_freq, detect_active_subspace,
  // compute_bare_projected, compute_sigma_active, apply_K, …) live in `lattice_gf_helpers.hpp`.

  // Single-projector overload — delegates to the two-projector implementation with Proj = obe.P.
  spectral_function_w projected_spectral_function(one_body_elements_on_grid const &obe, double mu,
                                                  block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, double broadening) {
    return projected_spectral_function(obe, obe.P, mu, Sigma_w, broadening);
  }

  // Two-projector overload — the canonical implementation. Used directly for cases where Σ is
  // upfolded by a `Proj` distinct from `obe.P`; the single-projector overload above just calls
  // through with Proj = obe.P.
  spectral_function_w projected_spectral_function(one_body_elements_on_grid const &obe, downfolding_projector const &Proj, double mu,
                                                  block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, double broadening) {
    auto const &mesh = Sigma_w(0, 0).mesh();
    auto n_sigma     = obe.C_space.n_sigma();
    auto n_k         = obe.H.n_k();
    auto n_M         = obe.C_space.dim();
    auto n_w         = mesh.size();
    auto im          = dcomplex(0, 1.0);
    auto delta       = im * broadening;

    // Accumulate local Green's function over k-points (MPI-chunked).
    auto gloc_result       = make_block2_gf(mesh, obe.C_space.Gc_block_shape());
    mpi::communicator comm = {};

    if (obe.H.matrix_valued) {
      // Matrix-valued H(k): direct N_ν × N_ν inversion fallback.
      using nda::linalg::inv;
      for (auto k_idx : mpi::chunk(range(n_k), comm)) {
        for (auto sigma : range(n_sigma)) {
          auto P       = obe.P.P(sigma, k_idx);
          auto Pdag    = dagger(P);
          auto H_k     = obe.H.H(sigma, k_idx);
          auto w_k     = obe.H.k_weights(k_idx);
          auto PSP_all = detail::upfold_self_energy_all_freq(obe, Proj, Sigma_w, k_idx, sigma);
          for (auto &&[n, w] : enumerate(mesh)) {
            auto G_k = nda::matrix<dcomplex>{inv(w + delta + mu - H_k - PSP_all(n, r_all, r_all))};
            gloc_result(0, sigma).data()(n, r_all, r_all) += w_k * (P * G_k * Pdag);
          }
        }
      }
    } else {
      // Diagonal H(k): rank-reduced Woodbury per (k, σ, ω), uses two projectors:
      //   obe.P  for the output projection (M × N_ν)
      //   Proj   for the active rows that upfold Σ  (its first `rank` active rows form Q).
      //
      // Per ω:
      //   K = Σ_a · (I − Q M₀⁻¹ Q† · Σ_a)⁻¹                                  (rank × rank)
      //   gloc(σ, ω) += w_k · ( obe.P · M₀⁻¹ · obe.P†  +  L · K · R ),
      //   L = obe.P · M₀⁻¹ · Q†,  R = Q · M₀⁻¹ · obe.P†   (R ≠ L† for complex ω).
      auto decomp = get_struct(Sigma_w).dims(r_all, 0) | tl::to<std::vector>();
      auto active = detail::detect_active_subspace(Sigma_w, decomp);
      long rank   = active.rank;

      auto omegas       = mesh | stdv::transform([&](auto w) { return dcomplex(w) + delta; }) | tl::to<std::vector>();
      auto Sa_per_sigma = range(n_sigma) | stdv::transform([&](long sigma) { return detail::compute_sigma_active(Sigma_w, active, sigma); }) //
         | tl::to<std::vector>();

      if (comm.rank() == 0)
        std::cerr << "projected_spectral_function: starting  (n_k=" << n_k << " n_sigma=" << n_sigma //
                  << " n_w=" << n_w << " M=" << n_M << " rank=" << rank << ")\n";
      auto t_start = std::chrono::steady_clock::now();

      for (auto k_idx : mpi::chunk(range(n_k), comm)) {
        for (auto sigma : range(n_sigma)) {
          auto bare        = detail::compute_bare_projected(obe, Proj, mu, k_idx, sigma, omegas, active);
          auto w_k         = obe.H.k_weights(k_idx);
          auto const &Sa_n = Sa_per_sigma[sigma];

          // Σ ≡ 0 short-circuit: G_loc^C = G0_PP, no correction. Avoids the 0 × 0 K-solve.
          if (rank == 0) {
            for (long n = 0; n < n_w; ++n) gloc_result(0, sigma).data()(n, r_all, r_all) += w_k * bare.G0_PP(n, r_all, r_all);
            continue;
          }

          for (long n = 0; n < n_w; ++n) {
            auto Sa      = nda::matrix<dcomplex>{Sa_n(n, r_all, r_all)};
            auto Yaa     = nda::matrix<dcomplex>{bare.G0_QQ(n, r_all, r_all)};
            auto KR      = detail::apply_K(Sa, Yaa, bare.G0_QPdag(n, r_all, r_all));
            auto L_n     = nda::matrix<dcomplex>{bare.G0_PQ(n, r_all, r_all)};
            auto contrib = nda::matrix<dcomplex>{bare.G0_PP(n, r_all, r_all) + L_n * KR};
            gloc_result(0, sigma).data()(n, r_all, r_all) += w_k * contrib;
          }
        }
      }

      auto t_end  = std::chrono::steady_clock::now();
      double secs = std::chrono::duration<double>(t_end - t_start).count();
      if (comm.rank() == 0) std::cerr << "projected_spectral_function: total " << secs << " s\n";
    }

    // Combine per-rank partial sums.  Each rank has accumulated its own k-chunk into gloc_result.
    gloc_result = mpi::all_reduce(gloc_result);

    if (auto const &S = obe.ibz_symm_ops; S) { gloc_result = S->symmetrize(gloc_result, obe.C_space.atomic_decomposition()); }

    // Extract spectral functions from the local Green's function.
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

  spectral_function_kw spectral_function_on_high_symmetry_path(one_body_elements_on_grid const &obe, double mu,
                                                               block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, double broadening) {

    auto const &mesh = Sigma_w(0, 0).mesh();
    auto n_sigma     = obe.C_space.n_sigma();
    auto n_w         = mesh.size();
    auto n_k         = obe.H.n_k();
    auto n_M         = obe.C_space.dim();
    auto delta       = dcomplex(0, broadening);

    auto total     = nda::zeros<double>(n_sigma, n_k, n_w);
    auto projected = nda::zeros<double>(n_sigma, n_k, n_w, n_M);

    // Each k-point's outputs land in independent slots of (total, projected).  Each MPI rank fills
    // only its own k-chunk and leaves the other slots zero, so all_reduce(+) acts as a gather —
    // not a sum over k in the mathematical sense.
    mpi::communicator comm = {};

    // Matrix-valued H(k): direct N_ν × N_ν inversion.
    if (obe.H.matrix_valued) {
      using nda::linalg::inv;
      auto mesh_vec = mesh | tl::to<std::vector>();
#pragma omp parallel for collapse(2) default(none) shared(comm, n_k, n_sigma, n_M, obe, mu, delta, Sigma_w, mesh_vec, total, projected, r_all)
      for (auto k_idx : mpi::chunk(range(n_k), comm)) {
        for (auto sigma : range(n_sigma)) {
          auto P       = obe.P.P(sigma, k_idx);
          auto Pdag    = dagger(P);
          auto H_k     = obe.H.H(sigma, k_idx);
          auto PSP_all = detail::upfold_self_energy_all_freq(obe, obe.P, Sigma_w, k_idx, sigma);

          for (auto &&[n, w] : enumerate(mesh_vec)) {
            auto G_k               = inv(w + delta + mu - H_k - PSP_all(n, r_all, r_all));
            total(sigma, k_idx, n) = (-1.0 / M_PI) * imag(trace(G_k));
            auto PGP               = nda::matrix<dcomplex>{P * nda::matrix<dcomplex>{G_k} * Pdag};
            for (auto m : range(n_M)) projected(sigma, k_idx, n, m) = (-1.0 / M_PI) * imag(PGP(m, m));
          }
        }
      }
      total     = mpi::all_reduce(total);
      projected = mpi::all_reduce(projected);
      return {.total = total, .projected = projected};
    }

    // Diagonal H(k): rank-reduced Woodbury per (k, σ, ω).
    auto decomp = get_struct(Sigma_w).dims(r_all, 0) | tl::to<std::vector>();
    auto active = detail::detect_active_subspace(Sigma_w, decomp);
    long rank   = active.rank;

    auto omegas = mesh | stdv::transform([&](auto w) { return dcomplex(w) + delta; }) | tl::to<std::vector>();
    // Σ_active across all ω, per σ. Independent of k, so build once and reuse for every k.
    auto Sa_per_sigma = range(n_sigma) | stdv::transform([&](long sigma) { return detail::compute_sigma_active(Sigma_w, active, sigma); }) //
       | tl::to<std::vector>();

    if (comm.rank() == 0)
      std::cerr << "spectral_function_on_high_symmetry_path: starting  (n_k=" << n_k << " n_sigma=" << n_sigma //
                << " n_w=" << n_w << " M=" << n_M << " rank=" << rank << ")\n";
    auto t_start = std::chrono::steady_clock::now();

#pragma omp parallel for collapse(2) default(none)                                                                                                   \
   shared(comm, n_k, n_sigma, n_M, n_w, rank, obe, mu, omegas, Sigma_w, active, Sa_per_sigma, total, projected, r_all)
    for (auto k_idx : mpi::chunk(range(n_k), comm)) {
      for (auto sigma : range(n_sigma)) {
        auto bare        = detail::compute_bare_projected(obe, obe.P, mu, k_idx, sigma, omegas, active);
        auto const &Sa_n = Sa_per_sigma[sigma];

        // Σ ≡ 0 short-circuit: only the bare terms contribute. Avoids the 0 × 0 K-solve.
        if (rank == 0) {
          for (long n = 0; n < n_w; ++n) {
            total(sigma, k_idx, n) = -imag(bare.G0_band_trace(n)) / M_PI;
            for (auto m : range(n_M)) projected(sigma, k_idx, n, m) = -imag(bare.G0_PP(n, m, m)) / M_PI;
          }
          continue;
        }

        for (long n = 0; n < n_w; ++n) {
          // Rank-reduced Woodbury without inverting Σ:  K · X = Σ_a · (I − Y_aa · Σ_a)⁻¹ · X.
          // Solve once for both right-hand sides — trace and band-projection — by stacking
          //   (I − Y_aa · Σ_a) · Y = [ G0_QQ_sq | G0_QPdag ],
          // then K · [G0_QQ_sq | G0_QPdag] = Σ_a · Y.
          auto Sa  = nda::matrix<dcomplex>{Sa_n(n, r_all, r_all)};
          auto Yaa = nda::matrix<dcomplex>{bare.G0_QQ(n, r_all, r_all)};
          auto LHS = nda::matrix<dcomplex, nda::F_layout>{-Yaa * Sa};
          for (auto i : range(rank)) LHS(i, i) += 1.0;

          auto RHS                            = nda::matrix<dcomplex, nda::F_layout>::zeros(rank, rank + n_M);
          RHS(r_all, range(0, rank))          = bare.G0_QQ_sq(n, r_all, r_all);
          RHS(r_all, range(rank, rank + n_M)) = bare.G0_QPdag(n, r_all, r_all);
          Ainv_B(LHS, RHS);

          auto SaRHS   = nda::matrix<dcomplex>{Sa * nda::matrix<dcomplex>{RHS}};
          auto X_trace = SaRHS(r_all, range(0, rank));          // K · G0_QQ_sq   (rank × rank)
          auto KR      = SaRHS(r_all, range(rank, rank + n_M)); // K · G0_QPdag   (rank × M)

          // Total: trace(G_k) = trace(M₀⁻¹) + trace(K · G0_QQ_sq).
          total(sigma, k_idx, n) = -imag(bare.G0_band_trace(n) + nda::trace(X_trace)) / M_PI;

          // Orbital diag: diag(P G_k P†) = diag(G0_PP) + diag(L · KR),  L = G0_PQ.
          //   (L · KR)_{mm} = Σ_c L_n(m, c) · KR(c, m).
          for (auto m : range(n_M)) {
            dcomplex corr_m = 0;
            for (auto c : range(rank)) corr_m += bare.G0_PQ(n, m, c) * KR(c, m);
            projected(sigma, k_idx, n, m) = -imag(bare.G0_PP(n, m, m) + corr_m) / M_PI;
          }
        }
      }
    }
    auto t_end        = std::chrono::steady_clock::now();
    double total_secs = std::chrono::duration<double>(t_end - t_start).count();
    if (comm.rank() == 0) std::cerr << "spectral_function_on_high_symmetry_path: total " << total_secs << " s\n";
    total     = mpi::all_reduce(total);
    projected = mpi::all_reduce(projected);
    return {.total = total, .projected = projected};
  }

} // namespace triqs::modest