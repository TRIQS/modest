// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "./lattice_gf_helpers.hpp"
#include "utils/scoped_timer.hpp"
#include <triqs/mesh.hpp>
#include "utils/gf_supp.hpp"

namespace triqs::modest {

// omp reduction operation for block2_gf
#pragma omp declare reduction(block2_gf_sum : block2_gf<imfreq, matrix_valued> : omp_out += omp_in)                                                  \
   initializer(omp_priv = make_block2_gf(omp_orig(0, 0).mesh(), get_struct(omp_orig)))
#pragma omp declare reduction(block2_gf_sum : block2_gf<dlr_imfreq, matrix_valued> : omp_out += omp_in)                                              \
   initializer(omp_priv = make_block2_gf(omp_orig(0, 0).mesh(), get_struct(omp_orig)))

  /** @cond DOXYGEN_SKIP_THIS */
  /**
  * @brief [INTERNAL] compute G𝓒 local Green's function on Mesh(MxM) for cases where H(k) is not diagonal.
  *
  * @tparam Mesh The mesh type (triqs::mesh::{dlr_imfreq,imfreq})
  * @param obe one_body_elements_on_grid
  * @param mu chemical potential
  * @param Sigma_dynamic The dynamic part of the embedded self-energy in the embedded view.
  * @param Sigma_static The static part of the embedded self-energy in the embedded view.
  * @return block2_gf<Mesh, matrix_valued>
   */
  template <typename Mesh>
  block2_gf<Mesh, matrix_valued> gloc_for_matrix_valued_dispersion_impl(one_body_elements_on_grid const &obe, double mu,
                                                                        block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                                                                        nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static) {

    auto n_sigma = obe.C_space.n_sigma();
    auto n_kpts  = long(obe.H.n_k());

    auto gloc_k      = detail::local_gf_at_k(obe, mu, obe.P, Sigma_dynamic, Sigma_static);
    auto gloc_result = make_block2_gf(Sigma_dynamic(0, 0).mesh(), obe.C_space.Gc_block_shape());

    mpi::communicator comm = {};
#pragma omp parallel for collapse(2) reduction(block2_gf_sum : gloc_result) default(none) shared(comm, gloc_k, n_kpts, n_sigma, obe, r_all)
    for (auto k_idx : mpi::chunk(range(n_kpts), comm)) {
      for (auto sigma : range(n_sigma)) { gloc_result(0, sigma).data()(r_all, r_all, r_all) += obe.H.k_weights(k_idx) * gloc_k(k_idx, sigma).data(); }
    }

    gloc_result = mpi::all_reduce(gloc_result);

    if (auto const &S = obe.ibz_symm_ops; S) { gloc_result = S->symmetrize(gloc_result, obe.C_space.atomic_decomposition()); }
    return gloc_result;
  }
  /** @endcond */

  // ------------------------------------------------------------------

  /** @name Local Green's function using a fixed k-grid
  */
  ///@{

  /**
   * @ingroup gloc
   * @brief Compute local Green's function on a \f$ M \times M \f$ mesh.
   *
   * @details When the one-body dispersion is defined as fixed k-grid, which is the case when working with DFT codes
   * (e.g., VASP, Wien2k, Elk) or performing charge self-consistent calculations with any DFT code, \f$H(\mathbf{k})\f$
   * is diagonal in the band basis and reduces to \f$\varepsilon_{\nu}^{\sigma}(\mathbf{k})\f$. The local Green's
   * function becomes:
   * \f[
   *   [ G_{\mathrm{loc}}^{\sigma} ]_{m m'} = \sum_{\mathbf{k}} P_{m\nu}^{\sigma}(\mathbf{k}) \Big [ (\omega + \mu -
   *   \varepsilon_{\nu}^{\sigma}(\mathbf{k}))\delta_{\nu\nu'} - [P_{m\nu}^{\sigma}]^{\dagger}\Sigma_{\mathrm{embed}}
   *   P_{m'\nu'}^{\sigma}(\mathbf{k}) \Big ]^{-1} [P_{m'\nu'}^{\sigma}]^{\dagger}.
   * \f]
   * For performance reasons, we can avoid performing the matrix inverstion in the larger band basis (\f$N_{\nu}\f$)
   * using the Woodbury formula which allows us to perform the matrix inversion in the smaller orbital basis \f$N_{M}\f$.
   *
   * @tparam Mesh The mesh type (triqs::mesh::{dlr_imfreq,imfreq}).
   * @param obe One-body elements on a fixed grid.
   * @param mu Chemical potential \f$ \mu \f$.
   * @param Sigma_dynamic The dynamic part of the embedded self-energy in the embedded view,
   * \f$ \Sigma_{\text{dynamic}}[\alpha, \sigma] \f$.
   * @param Sigma_static The static part of the embedded self-energy in the embedded view,
   * \f$ \Sigma_{\text{static}}[\alpha,\sigma] \f$.
   * @return \f$ G_{\mathrm{loc}}^{\sigma} \f$, the local Green's function in the full \f$ \mathcal{C} \f$ space.
   */
  template <typename Mesh>
  block2_gf<Mesh, matrix_valued> gloc(one_body_elements_on_grid const &obe, double mu, block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                                      nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static) {

    // intercept if the dispersion in obe is matrix valued. The Woodbury offers no performance gain for this case.
    if (obe.H.matrix_valued) return gloc_for_matrix_valued_dispersion_impl(obe, mu, Sigma_dynamic, Sigma_static);

    auto Sigma_total = detail::make_sigma_total(Sigma_dynamic, Sigma_static);

    auto const &mesh = Sigma_total(0, 0).mesh();
    auto n_sigma     = Sigma_total.size2();
    auto n_kpts      = long(obe.H.n_k());
    auto gloc_result = make_block2_gf(mesh, obe.C_space.Gc_block_shape());

    auto omegas = mesh | tl::to<std::vector<dcomplex>>();
    auto decomp = get_struct(Sigma_total).dims(r_all, 0) | tl::to<std::vector>();
    auto active = detail::detect_active_subspace(Sigma_total, decomp);
    auto Sa_per_sigma =
       range(n_sigma) | stdv::transform([&](long sigma) { return detail::compute_sigma_active(Sigma_total, active, sigma); }) | tl::to<std::vector>();

    auto timer             = scoped_timer{};
    mpi::communicator comm = {};
#pragma omp parallel for collapse(2) reduction(block2_gf_sum : gloc_result) default(none)                                                            \
   shared(comm, r_all, n_kpts, n_sigma, obe, mu, omegas, mesh, active, Sa_per_sigma)
    for (auto k_idx : mpi::chunk(range(n_kpts), comm)) {
      for (auto sigma : range(n_sigma)) {
        auto bare        = detail::compute_bare_projected(obe, obe.P, mu, k_idx, sigma, omegas, active);
        auto const &Sa_n = Sa_per_sigma[sigma];
        auto w_k         = obe.H.k_weights(k_idx);

        // Σ ≡ 0 short-circuit: G_loc^C = G0_PP, no correction.
        if (active.rank == 0) {
          for (long n = 0; n < long(omegas.size()); ++n) gloc_result(0, sigma).data()(n, r_all, r_all) += w_k * bare.G0_PP(n, r_all, r_all);
          continue;
        }

        for (long n = 0; n < long(omegas.size()); ++n) {
          // Rank-reduced Woodbury:  G_loc += w_k · ( G0_PP + L · K · R ), L = G0_PQ, R = G0_QPdag.
          auto Sa      = nda::matrix<dcomplex>{Sa_n(n, r_all, r_all)};
          auto Yaa     = nda::matrix<dcomplex>{bare.G0_QQ(n, r_all, r_all)};
          auto KR      = detail::apply_K(Sa, Yaa, bare.G0_QPdag(n, r_all, r_all));
          auto L_n     = nda::matrix<dcomplex>{bare.G0_PQ(n, r_all, r_all)};
          auto contrib = nda::matrix<dcomplex>{bare.G0_PP(n, r_all, r_all) + L_n * KR};
          gloc_result(0, sigma).data()(n, r_all, r_all) += w_k * contrib;
        }
      }
    }
    gloc_result = mpi::all_reduce(gloc_result);
    if (auto const &S = obe.ibz_symm_ops; S) gloc_result = S->symmetrize(gloc_result, obe.C_space.atomic_decomposition());
    return gloc_result;
  }

  /**
   * @ingroup gloc
   * @brief Compute the local Green's function without a self-energy.
   *
   * @details See other overloads (gloc) for more details.
   *
   * @tparam Mesh The mesh type.
   * @param mesh (DLR) imaginary frequency mesh.
   * @param obe One-body elements on a fixed grid.
   * @param mu Chemical potential \f$ \mu \f$.
   * @return \f$ G_{\mathrm{loc}}^{\sigma} \f$, the local Green's function.
   */
  template <typename Mesh> block2_gf<Mesh, matrix_valued> gloc(Mesh const &mesh, one_body_elements_on_grid const &obe, double mu) {
    auto Sigma_dynamic = make_block2_gf(mesh, obe.C_space.Gc_block_shape());
    auto Sigma_static  = nda::array<nda::matrix<dcomplex>, 2>(1, obe.C_space.n_sigma());
    for (auto [i, j] : Sigma_static.indices()) { Sigma_static(i, j) = nda::zeros<dcomplex>(obe.C_space.dim(), obe.C_space.dim()); }
    return gloc(obe, mu, Sigma_dynamic, Sigma_static);
  }
  ///@}

  /**
   * @ingroup hybridization
   * @brief Compute the hybridization function from the effective impurity levels, the local Green's function, and the impurity self-energy.
   *
   * @tparam Mesh The mesh type.
   * @param epsilon_levels The effective impurity levels.
   * @param Gloc The local Green's function of the impurity.
   * @param Sigma_dynamic The frequency dependent part of the impurity self-energy.
   * @param Sigma_static The static term of the impurity self-energy.
   * @return Hybridization function \f$ \Delta \f$.
   */
  template <typename Mesh>
  block_gf<Mesh, matrix_valued> hybridization(std::vector<nda::matrix<dcomplex>> const &epsilon_levels, block_gf<Mesh, matrix_valued> const &Gloc,
                                              block_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                                              std::vector<nda::matrix<dcomplex>> const &Sigma_static) {
    auto gf_struct = Gloc.gf_struct();
    auto mesh      = Gloc[0].mesh();
    auto Delta     = block_gf{mesh, gf_struct};
    auto n_blocks  = gf_struct.size();
    for (auto bl : range(n_blocks)) {
      for (auto &&[n, w] : enumerate(mesh))
        Delta[bl].data()(n, r_all, r_all) =
           (w - epsilon_levels[bl] - inverse(Gloc[bl]).data()(n, r_all, r_all) - (Sigma_dynamic[bl].data()(n, r_all, r_all) + Sigma_static[bl]));
    }
    return Delta;
  }

  /**
   * @ingroup hybridization
   * @brief Compute the hybridization function from the effective impurity levels and the local Green's function.
   *
   * @tparam Mesh The mesh type.
   * @param epsilon_levels The effective impurity levels.
   * @param Gloc The local Green's function of the impurity.
   * @return Hybridization function \f$ \Delta \f$.
   */
  template <typename Mesh>
  block_gf<Mesh, matrix_valued> hybridization(std::vector<nda::matrix<dcomplex>> const &epsilon_levels, block_gf<Mesh, matrix_valued> const &Gloc) {
    auto Sigma_static = Gloc.gf_struct() | stdv::transform([](auto &x) { return nda::zeros<dcomplex>(x.second, x.second); })
       | tl::to<std::vector<nda::matrix<dcomplex>>>();
    auto Sigma_dynamic = block_gf{Gloc[0].mesh(), Gloc.gf_struct()};
    return hybridization(epsilon_levels, Gloc, Sigma_dynamic, Sigma_static);
  }

  // ------------------------------------------------------

  template block2_gf<imfreq, matrix_valued> gloc(one_body_elements_on_grid const &one_body, double mu,
                                                 block2_gf<imfreq, matrix_valued> const &Sigma_dynamic,
                                                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static);
  template block2_gf<imfreq, matrix_valued> gloc(imfreq const &mesh, one_body_elements_on_grid const &obe, double mu);
  template block2_gf<dlr_imfreq, matrix_valued> gloc(one_body_elements_on_grid const &one_body, double mu,
                                                     block2_gf<dlr_imfreq, matrix_valued> const &Sigma_dynamic,
                                                     nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static);
  template block2_gf<dlr_imfreq, matrix_valued> gloc(dlr_imfreq const &mesh, one_body_elements_on_grid const &obe, double mu);
  template block_gf<imfreq, matrix_valued> hybridization(std::vector<nda::matrix<dcomplex>> const &epsilon_levels,
                                                         block_gf<imfreq, matrix_valued> const &Gloc);
  template block_gf<imfreq, matrix_valued> hybridization(std::vector<nda::matrix<dcomplex>> const &epsilon_levels,
                                                         block_gf<imfreq, matrix_valued> const &Gloc,
                                                         block_gf<imfreq, matrix_valued> const &Sigma_dynamic,
                                                         std::vector<nda::matrix<dcomplex>> const &Sigma_static);
  template block_gf<dlr_imfreq, matrix_valued> hybridization(std::vector<nda::matrix<dcomplex>> const &epsilon_levels,
                                                             block_gf<dlr_imfreq, matrix_valued> const &Gloc);
  template block_gf<dlr_imfreq, matrix_valued> hybridization(std::vector<nda::matrix<dcomplex>> const &epsilon_levels,
                                                             block_gf<dlr_imfreq, matrix_valued> const &Gloc,
                                                             block_gf<dlr_imfreq, matrix_valued> const &Sigma_dynamic,
                                                             std::vector<nda::matrix<dcomplex>> const &Sigma_static);

} // namespace triqs::modest
