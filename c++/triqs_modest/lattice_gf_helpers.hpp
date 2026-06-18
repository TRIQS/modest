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
  // Apply K = Σ_a · (I − Y_aa · Σ_a)⁻¹ to a right-hand side R, returning K · R.
  // Computed without inverting Σ_a (which is singular at large iω_n / for static-only Σ).
  //   1) Solve  (I − Y_aa · Σ_a) · X = R              [LAPACK getrf + getrs via Ainv_B]
  //   2) K · R = Σ_a · X
  // ===================================================================================
  inline nda::matrix<dcomplex> apply_K(nda::matrix<dcomplex> const &Sa, nda::matrix<dcomplex> const &Yaa, nda::matrix_const_view<dcomplex> R) {
    long rank = Sa.extent(0);
    auto LHS  = nda::matrix<dcomplex, nda::F_layout>{-Yaa * Sa};
    for (auto i : range(rank)) LHS(i, i) += 1.0;
    auto X = nda::matrix<dcomplex, nda::F_layout>{R};
    Ainv_B(LHS, X);                       // X ← (I − Yaa · Sa)⁻¹ · R
    return nda::matrix<dcomplex>{Sa * X}; // K · R = Σ_a · X
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

  // ===================================================================================
  // Rank-reduced Woodbury — for diagonal H(k) and block-sparse Σ.
  //
  // Σ is structurally sparse in C-space: non-zero only on a few rows × columns indexed
  // by the active impurity blocks. The whole calculation collapses
  // from M-dim (full C-space) to rank-dim (active rows only).
  //
  //     M₀(ω, ν) = (ω + iδ + μ − ε_ν(k))     [diagonal, since H_k is diagonal]
  //     Q        = P_active[active C rows, :] (rank × N_ν)
  //     K        = (Σ_active⁻¹ − Q M₀⁻¹ Q†)⁻¹  (rank × rank)
  //
  //     trace(G_k)        = trace(M₀⁻¹)            +  trace(K · Q M₀⁻² Q†)
  //     diag(P G_k P†)_m  = diag(P M₀⁻¹ P†)_m      +  ((P M₀⁻¹ Q†) · K · (Q M₀⁻¹ P†))_{mm}
  // ===================================================================================

  /// The active subspace: which embed blocks of Σ are non-zero and where they sit in C-space.
  struct active_subspace_t {
    std::vector<long> alphas;     ///< α indices into Sigma_w that have non-zero data
    std::vector<long> block_dims; ///< m_α for each active α
    std::vector<long> c_indices;  ///< C-space row indices: ⋃ R(α) over active α
    long rank;                    ///< = c_indices.size()
  };

  active_subspace_t detect_active_subspace(auto const &Sigma_w, std::vector<long> const &decomp, double tol = 1e-15) {
    active_subspace_t A{};
    long n_sigma = Sigma_w.size2();
    long offset  = 0;
    for (long alpha = 0; alpha < long(decomp.size()); ++alpha) {
      long m_a     = decomp[alpha];
      bool nonzero = false;
      for (long s = 0; s < n_sigma && !nonzero; ++s) {
        auto const &dat = Sigma_w(alpha, s).data();
        for (long in = 0; in < dat.extent(0) && !nonzero; ++in)
          for (long i = 0; i < m_a && !nonzero; ++i)
            for (long j = 0; j < m_a && !nonzero; ++j)
              if (std::abs(dat(in, i, j)) > tol) nonzero = true;
      }
      if (nonzero) {
        A.alphas.push_back(alpha);
        A.block_dims.push_back(m_a);
        for (long i = 0; i < m_a; ++i) A.c_indices.push_back(offset + i);
      }
      offset += m_a;
    }
    A.rank = long(A.c_indices.size());
    return A;
  }

  /// Bare projected propagators at fixed (k, σ), batched over ω.
  /// `G0_PP` is the full M × M block; the diagonal is just `G0_PP(n, m, m)`.
  struct bare_projected_t {
    nda::array<dcomplex, 3> G0_PP;         ///< (n_ω, M, M)        = P M₀⁻¹ P†                FULL
    nda::array<dcomplex, 3> G0_PQ;         ///< (n_ω, M, rank)     = P M₀⁻¹ Q†
    nda::array<dcomplex, 3> G0_QPdag;      ///< (n_ω, rank, M)     = Q M₀⁻¹ P†   (≠ dagger(G0_PQ) for iδ)
    nda::array<dcomplex, 3> G0_QQ;         ///< (n_ω, rank, rank)  = Q M₀⁻¹ Q†
    nda::array<dcomplex, 3> G0_QQ_sq;      ///< (n_ω, rank, rank)  = Q M₀⁻² Q†
    nda::array<dcomplex, 1> G0_band_trace; ///< (n_ω,)             = trace(M₀⁻¹) = Σ_ν M₀⁻¹(ω, ν)
  };

  /// Compute bare projected propagators using two projectors: `obe.P` (M-side / output) and
  /// `P_active` (rank-side). When `P_active == obe.P` this matches the single-projector case;
  /// when different it implements the two-projector kernel.
  inline bare_projected_t compute_bare_projected(one_body_elements_on_grid const &obe, downfolding_projector const &P_active, double mu, long k_idx,
                                                 long sigma, std::vector<dcomplex> const &omegas, active_subspace_t const &A) {
    auto n_nu          = obe.H.N_nu(sigma, k_idx);
    auto R_nu          = nda::range(n_nu);
    auto M             = obe.C_space.dim();
    auto rank          = A.rank;
    auto n_omega       = long(omegas.size());
    auto P_obe         = obe.P.P(sigma, k_idx);
    auto P_act         = P_active.P(sigma, k_idx);
    auto Hk            = obe.H.H(sigma, k_idx);
    auto const &active = A.c_indices;

    // Dinv(n, ν) = 1/(ω_n + μ − ε_ν), Dinv²(n, ν), band trace tr0(n) = Σ_ν Dinv(n, ν).
    auto Dinv  = nda::matrix<dcomplex>(n_omega, n_nu);
    auto Dinv2 = nda::matrix<dcomplex>(n_omega, n_nu);
    auto tr0   = nda::vector<dcomplex>(n_omega);
    for (auto n : range(n_omega)) {
      dcomplex tr0_n = 0;
      for (auto nu : R_nu) {
        auto d       = 1.0 / (omegas[n] + mu - Hk(nu, nu));
        Dinv(n, nu)  = d;
        Dinv2(n, nu) = d * d;
        tr0_n += d;
      }
      tr0(n) = tr0_n;
    }

    // Index tensors over band index ν:
    //   PP(ν, m, m') = obe.P(m, ν)         · conj(obe.P(m', ν))           — full M × M
    //   PQ(ν, m, c)  = obe.P(m, ν)         · conj(P_act(active[c], ν))
    //   QP(ν, c, m)  = P_act(active[c], ν) · conj(obe.P(m, ν))            — ≠ PQ when D complex
    //   QQ(ν, a, b)  = P_act(active[a], ν) · conj(P_act(active[b], ν))
    auto PP = nda::zeros<dcomplex>(n_nu, M, M);
    for (auto m : range(M))
      for (auto mp : range(M))
        for (auto nu : R_nu) PP(nu, m, mp) = P_obe(m, nu) * conj(P_obe(mp, nu));
    auto PQ = nda::zeros<dcomplex>(n_nu, M, rank);
    for (auto m : range(M))
      for (auto c : range(rank))
        for (auto nu : R_nu) PQ(nu, m, c) = P_obe(m, nu) * conj(P_act(active[c], nu));
    auto QP = nda::zeros<dcomplex>(n_nu, rank, M);
    for (auto c : range(rank))
      for (auto m : range(M))
        for (auto nu : R_nu) QP(nu, c, m) = P_act(active[c], nu) * conj(P_obe(m, nu));
    auto QQ = nda::zeros<dcomplex>(n_nu, rank, rank);
    for (auto a : range(rank))
      for (auto b : range(rank))
        for (auto nu : R_nu) QQ(nu, a, b) = P_act(active[a], nu) * conj(P_act(active[b], nu));

    // Five batched gemms: each is (n_ω × n_ν) × (n_ν × …).
    // When rank == 0 the QQ/PQ/QP arrays have a zero-extent dim and as_2d() of a
    // 3D tensor with a zero-extent inner dim produces a grouped view with min_stride == 0,
    // which trips nda's BLAS precondition.  Skip those gemms — callers short-circuit on rank==0.
    auto G0_PP    = nda::zeros<dcomplex>(n_omega, M, M);
    auto G0_PQ    = nda::zeros<dcomplex>(n_omega, M, rank);
    auto G0_QPdag = nda::zeros<dcomplex>(n_omega, rank, M);
    auto G0_QQ    = nda::zeros<dcomplex>(n_omega, rank, rank);
    auto G0_QQ_sq = nda::zeros<dcomplex>(n_omega, rank, rank);
    nda::blas::gemm(1, Dinv, as_2d(PP), 0, as_2d(G0_PP));
    if (rank > 0) {
      nda::blas::gemm(1, Dinv, as_2d(PQ), 0, as_2d(G0_PQ));
      nda::blas::gemm(1, Dinv, as_2d(QP), 0, as_2d(G0_QPdag));
      nda::blas::gemm(1, Dinv, as_2d(QQ), 0, as_2d(G0_QQ));
      nda::blas::gemm(1, Dinv2, as_2d(QQ), 0, as_2d(G0_QQ_sq));
    }

    return {.G0_PP         = std::move(G0_PP),
            .G0_PQ         = std::move(G0_PQ),
            .G0_QPdag      = std::move(G0_QPdag),
            .G0_QQ         = std::move(G0_QQ),
            .G0_QQ_sq      = std::move(G0_QQ_sq),
            .G0_band_trace = std::move(tr0)};
  }

  /// Σ_active at every ω, packed as a dense (n_ω, rank, rank) tensor with block-diagonal structure.
  /// Independent of k: built once per σ, sliced inside the inner ω loop.
  ///
  /// NOTE: we use Σ_active *directly* (not its inverse) so the downstream kernel can solve
  /// `(I − Y_aa Σ_active) X = Σ_active · …` instead of `(Σ_active⁻¹ − Y_aa) K = …`.  This avoids
  /// inverting Σ.
  nda::array<dcomplex, 3> compute_sigma_active(auto const &Sigma_w, active_subspace_t const &A, long sigma) {
    long n_w = Sigma_w(0, 0).mesh().size();
    auto out = nda::zeros<dcomplex>(n_w, A.rank, A.rank);
    for (long n = 0; n < n_w; ++n) {
      long off = 0;
      for (long bi = 0; bi < long(A.alphas.size()); ++bi) {
        long m_a     = A.block_dims[bi];
        auto R       = nda::range(off, off + m_a);
        out(n, R, R) = Sigma_w(A.alphas[bi], sigma).data()(n, r_all, r_all);
        off += m_a;
      }
    }
    return out;
  }

} // namespace triqs::modest::detail
