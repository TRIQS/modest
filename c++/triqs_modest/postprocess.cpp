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

<<<<<<< HEAD
    auto data      = nda::zeros<double>(n_sigma, n_k, n_w);
    auto proj_data = nda::zeros<double>(n_sigma, n_k, n_w, n_M, n_M);

#pragma omp parallel for default(none) shared(n_k, n_sigma, n_w, obe, mu, delta, Sigma_w, broadening, mesh, data, proj_data, r_all)
    for (auto k_idx : range(n_k)) {
=======
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
>>>>>>> 894f1e0 (WIP)
      for (auto sigma : range(n_sigma)) {
        auto P    = obe.P.P(sigma, k_idx);
        auto Pdag = dagger(P);
        auto H_k  = obe.H.H(sigma, k_idx);

<<<<<<< HEAD
        // Precompute upfolded self-energy for all frequencies
        auto PSP_all = detail::upfold_self_energy_all_freq(obe, obe.P, Sigma_w, k_idx, sigma);
=======
        // Σ ≡ 0 short-circuit: only the bare terms contribute. Avoids the 0 × 0 K-solve.
        if (rank == 0) {
          for (long n = 0; n < n_w; ++n) {
            total(sigma, k_idx, n) = -imag(bare.G0_band_trace(n)) / M_PI;
            for (auto m : range(n_M)) projected(sigma, k_idx, n, m) = -imag(bare.G0_PP(n, m, m)) / M_PI;
          }
          continue;
        }
>>>>>>> 894f1e0 (WIP)

        for (auto &&[n, w] : enumerate(mesh)) {
          auto G_k = inv(w + delta + mu - H_k - PSP_all(n, r_all, r_all));

          // Total spectral function from trace
          data(sigma, k_idx, n) = (-1.0 / M_PI) * imag(trace(G_k));

<<<<<<< HEAD
          // Orbital-resolved from projection
          auto PGP                                 = P * nda::matrix<dcomplex>{G_k} * Pdag;
          proj_data(sigma, k_idx, n, r_all, r_all) = (-1.0 / M_PI) * imag(PGP);
        }
      }
    }

    return {.data = data, .proj_data = proj_data};
=======
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
>>>>>>> 894f1e0 (WIP)
  }

} // namespace triqs::modest