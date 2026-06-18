// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <nda/nda.hpp>
#include <triqs/gfs.hpp>
#include <itertools/omp_chunk.hpp>
#include <triqs/lattice/brillouin_zone.hpp>
#include "./downfolding.hpp"
#include "./lattice_gf_helpers.hpp"
#include <triqs/experimental/utility/root_finder.hpp>

namespace triqs::modest {

#pragma omp declare reduction(gf_sum : gf<imfreq, scalar_valued> : omp_out += omp_in) initializer(omp_priv = gf{omp_orig.mesh()})
#pragma omp declare reduction(gf_sum : gf<dlr_imfreq, scalar_valued> : omp_out += omp_in) initializer(omp_priv = gf{omp_orig.mesh()})

  //-------------------------------------------------------------------------------------------
  /**
   * @ingroup mu
   * @brief Compute the density of the lattice Green's function with a self-energy using Woodbury.
   *
   * @tparam Mesh The mesh type.
   * @param obe One-body elements.
   * @param mu Chemical potential.
   * @param Sigma_dynamic Dynamic part of the embedded self-energy.
   * @param Sigma_static Static part of the embedded self-energy.
   * @return Electron density of the lattice Green's function.
   */
  template <typename Mesh>
  double density(one_body_elements_on_grid const &obe, double mu,
                 // add magnetic field,
                 block2_gf<Mesh, matrix_valued> const &Sigma_dynamic, nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static) {

    if (obe.H.matrix_valued) return density_for_matrix_valued_impl(obe, mu, Sigma_dynamic, Sigma_static);

    auto const &mesh = Sigma_dynamic(0, 0).mesh();
    double beta      = mesh.beta();
    auto n_sigma     = Sigma_dynamic.size2();
    auto n_k         = obe.H.n_k();
    auto corr        = gf{mesh}; // Compute the correction term on all dlr mesh points
    double KS_term   = 0;        // The first Kohn Sham term, cf Notes

    // OP : we don't have a Fermi function in TRIQS ??
    auto Fermi = [](double x) { return (x > 0 ? exp(-x) / (1 + exp(-x)) : 1 / (1 + exp(x))); };

    auto Sigma_total = detail::make_sigma_total(Sigma_dynamic, Sigma_static);

    auto omegas       = mesh | tl::to<std::vector<dcomplex>>();
    auto decomp       = get_struct(Sigma_total).dims(r_all, 0) | tl::to<std::vector>();
    auto active       = detail::detect_active_subspace(Sigma_total, decomp);
    auto Sa_per_sigma = range(n_sigma)                                                                         //
       | stdv::transform([&](long sigma) { return detail::compute_sigma_active(Sigma_total, active, sigma); }) //
       | tl::to<std::vector>();

    mpi::communicator comm = {};
#pragma omp parallel for collapse(2) reduction(gf_sum : corr) reduction(+ : KS_term) default(none)                                                   \
   shared(comm, r_all, n_k, n_sigma, obe, mu, beta, Fermi, omegas, mesh, active, Sa_per_sigma)
    for (auto k_idx : mpi::chunk(range(n_k), comm)) {
      for (auto sigma : range(n_sigma)) {
        // 1- Kohn-Sham term (unchanged).
        double KS_term_acc = 0;
        auto eps           = obe.H.H(sigma, k_idx);
        for (auto nu : range(obe.H.N_nu(sigma, k_idx))) KS_term_acc += Fermi(beta * real(eps(nu, nu) - mu));
        KS_term += obe.H.k_weights(k_idx) * KS_term_acc;

        // 2- Correction term:  tr(G_band − M₀⁻¹) = tr(K · G0_QQ_sq) per (k, σ, ω).
        //    Σ ≡ 0 ⇒ correction is exactly zero, leaving density = KS_term.
        if (active.rank == 0) continue;

        auto bare        = detail::compute_bare_projected(obe, obe.P, mu, k_idx, sigma, omegas, active);
        auto const &Sa_n = Sa_per_sigma[sigma];
        auto w_k         = obe.H.k_weights(k_idx);

        for (long n = 0; n < long(omegas.size()); ++n) {
          auto Sa     = nda::matrix<dcomplex>{Sa_n(n, r_all, r_all)};
          auto Yaa    = nda::matrix<dcomplex>{bare.G0_QQ(n, r_all, r_all)};
          auto K_QQsq = detail::apply_K(Sa, Yaa, bare.G0_QQ_sq(n, r_all, r_all)); // K · G0_QQ_sq
          corr.data()(n) += w_k * nda::trace(K_QQsq);
        }
      }
    }
    KS_term = mpi::all_reduce(KS_term);
    corr    = mpi::all_reduce(corr);
    return KS_term + real(density(corr));
  }

  // ------------------------------------------------------------------------------------
  /// Compute number of particles \f$ n = \sum f(\beta(\varepsilon(k) - μ)) \f$.
  double density_nk(one_body_elements_on_grid const &obe, double mu, double beta);

  // ------------------------------------------------------------------------------------
  /** @cond DOXYGEN_SKIP_THIS */
  /**
   * @brief  Compute the density of the lattice Green's function with a self-energy.
   *
   * @tparam Mesh The mesh type
   * @param obe one-body elements
   * @param mu chemical potential
   * @param Sigma_dynamic dynamic part of the embedded self-energy.
   * @param Sigma_static static part of the embedded self-energy.
   * @return density of the lattice Green's function
   */
  template <typename Mesh>
  double density_for_matrix_valued_impl(one_body_elements_on_grid const &obe, double mu, block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                                        nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static) {
    auto const &mesh = Sigma_dynamic(0, 0).mesh();
    auto beta        = mesh.beta();
    auto result      = gf{mesh}; // tr(G_full − G_KS) per ω, summed over (k, σ).

    // Reuse the direct N_ν × N_ν inversion helper.  Returns a callable (k, σ) → gf on (mesh, {N_ν, N_ν}).
    auto glatt_at_k = detail::lattice_gf_at_k(obe, mu, Sigma_dynamic, Sigma_static);

    mpi::communicator comm = {};
#pragma omp parallel for collapse(2) reduction(gf_sum : result) default(none) shared(obe, mu, mesh, glatt_at_k, comm, r_all)
    for (auto k_idx : mpi::chunk(range(obe.H.n_k()), comm)) {
      for (auto sigma : range(obe.C_space.n_sigma())) {
        using nda::linalg::inv;
        auto G_band = glatt_at_k(k_idx, sigma); // gf on (mesh, {N_ν, N_ν})
        auto Hk     = obe.H.H(sigma, k_idx);
        for (auto &&[n, w] : enumerate(mesh)) {
          result.data()(n) += obe.H.k_weights(k_idx)
             * (trace(G_band.data()(n, r_all, r_all)) - trace(inv(nda::matrix<dcomplex>{w + mu - Hk})));
        }
      }
    }
    result = mpi::all_reduce(result);
    return density_nk(obe, mu, beta) + real(density(result));
  }
  /** @endcond */

  /**
   * @ingroup mu
   * @brief Find the chemical potenital from the local Green's function given a target density.
   *
   * @param target_density Total electron density.
   * @param obe One-body elements.
   * @param beta Inverse temperature (units 1/eV).
   * @param method Root finding method to use (default = `dichotomy`).
   * @param precision Precision to end search (default = 1e-5).
   * @param verbosity Printing of the root finder's progress (default = true).
   * @return Chemical potential corresponding to target density.
   */
  inline double find_chemical_potential(double const target_density, one_body_elements_on_grid const &obe, double beta,
                                        std::string method = "dichotomy", double precision = 1.e-5, bool verbosity = true) {
    std::function<double(double)> f = [&obe, beta](double x) { return density_nk(obe, x, beta); };
    return std::get<0>(triqs::experimental::utility::root_finder(method, f, 0.0, target_density, precision, 0.5, 1000, "Chemical Potential",
                                                                 "Total Density", verbosity));
  }

  /**
   * @ingroup mu
   * @brief Find the chemical potenital from the local Green's function and self-energy given a target density.
   *
   * @tparam Mesh The mesh type.
   * @param target_density Total electron density.
   * @param obe One-body elements.
   * @param Sigma_dynamic Dynamic part of the embedded self-energy.
   * @param Sigma_static Static part of the embedded self-energy.
   * @param method Root finding method to use (default = `dichotomy`).
   * @param precision Precision to end search (default = 1e-5).
   * @param verbosity Printing of the root finder's progress (default = true).
   * @return Chemical potential corresponding to target density.
   */
  template <typename Mesh>
  double find_chemical_potential(double const target_density, one_body_elements_on_grid const &obe,
                                 block2_gf<Mesh, matrix_valued> const &Sigma_dynamic, nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static,
                                 std::string method = "dichotomy", double precision = 1.e-5, bool verbosity = true) {
    std::function<double(double)> f = [&obe, &Sigma_dynamic, &Sigma_static](double x) { return density(obe, x, Sigma_dynamic, Sigma_static); };
    return std::get<0>(triqs::experimental::utility::root_finder(method, f, 0.0, target_density, precision, 0.5, 1000, "Chemical Potential",
                                                                 "Total Density", verbosity));
  }

  /** @cond DOXYGEN_SKIP_THIS */
  template double density(one_body_elements_on_grid const &obe, double mu, block2_gf<imfreq, matrix_valued> const &Sigma_dynamic,
                          nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static);
  template double density(one_body_elements_on_grid const &obe, double mu, block2_gf<dlr_imfreq, matrix_valued> const &Sigma_dynamic,
                          nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static);
  template double find_chemical_potential(double const target_density, one_body_elements_on_grid const &obe,
                                          block2_gf<imfreq, matrix_valued> const &Sigma_dynamic,
                                          nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static, std::string method = "dichotomy",
                                          double precision = 1.e-5, bool verbosity = true);
  template double find_chemical_potential(double const target_density, one_body_elements_on_grid const &obe,
                                          block2_gf<dlr_imfreq, matrix_valued> const &Sigma_dynamic,
                                          nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static, std::string method = "dichotomy",
                                          double precision = 1.e-5, bool verbosity = true);
  /** @endcond */

} // namespace triqs::modest
