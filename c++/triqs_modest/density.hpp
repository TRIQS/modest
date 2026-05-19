// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
//#include <mpi/generic_communication.hpp>
#include <nda/nda.hpp>
#include <triqs/gfs.hpp>
#include <itertools/omp_chunk.hpp>
#include <triqs/lattice/brillouin_zone.hpp>
#include "./downfolding.hpp"
#include <triqs/experimental/utility/root_finder.hpp>

namespace triqs::modest {

#pragma omp declare reduction(gf_sum : gf<imfreq, scalar_valued> : omp_out += omp_in) initializer(omp_priv = gf{omp_orig.mesh()})
#pragma omp declare reduction(gf_sum : gf<dlr_imfreq, scalar_valued> : omp_out += omp_in) initializer(omp_priv = gf{omp_orig.mesh()})

  // ============================================
  namespace detail {
    // impl detail : compute (1- Y1 Sigma)^{-1} Y2 in M x M space, with Sigma by block
    nda::matrix<dcomplex, nda::F_layout> calc_inv_G_G0(long M, auto embedding_decomp, auto const &Sigma_dynamic, auto const &Sigma_static, auto om,
                                                       long sigma, nda::matrix_view<dcomplex> Y1, nda::matrix_view<dcomplex> Y2) {
      // Y Sigma. NB Sigma is by blocks.
      // NB: F_layout because of getrs in Ainv_B below
      auto YS = nda::matrix<dcomplex, nda::F_layout>::zeros(M, M);

      for (auto &&[alpha, R] : enumerated_sub_slices(embedding_decomp)) {
        auto Sigma = nda::matrix<dcomplex>{Sigma_dynamic(alpha, sigma)[om] + Sigma_static(alpha, sigma)};
        nda::blas::gemm(-1, Y1(r_all, R), Sigma, 0, YS(r_all, R));
      }

      // Z = (1 -YS)^{-1} * Y
      for (auto m : range(M)) YS(m, m) += 1;
      auto B = nda::matrix<dcomplex, nda::F_layout>{Y2(r_all, r_all)};
      //B      = inverse(YS) * B;
      Ainv_B(YS, B);
      return B;
    }

    //-------------------------------------------------------------------------------------------
    // Returns a lambda (sigma, k_idx ) -> [Tr_nu G_B(k, omega) - Tr_nu G_KS(k, omega) for omega in Sigma.mesh] as nda::array
    template <typename Mesh>
    auto trace_G_B_m_G_KS(one_body_elements_on_grid const &obe, double mu,
                          // add magnetic field,
                          block2_gf<Mesh, matrix_valued> const &Sigma_dynamic, nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static) {

      auto M                = obe.C_space.dim();
      auto &mesh            = Sigma_dynamic(0, 0).mesh();
      auto omegas           = mesh | tl::to<std::vector<dcomplex>>();
      auto embedding_decomp = get_struct(Sigma_dynamic).dims(r_all, 0) | tl::to<std::vector>();

      return [=, &obe, &Sigma_dynamic, &Sigma_static](long sigma, long k_idx) {
        auto result = nda::zeros<dcomplex>(omegas.size());
        auto Y1     = detail::G0_C_k_sigma(obe, mu, k_idx, sigma, omegas, false); // Y1 = G0_𝓒
        auto Y2     = detail::G0_C_k_sigma(obe, mu, k_idx, sigma, omegas, true);  // Y2 = + ∂_μ G0_𝓒

        for (auto &&[n, om] : itertools::enumerate(mesh)) {
          // Compute (1- Y1 Sigma)^{-1} Y2
          auto B = calc_inv_G_G0(M, embedding_decomp, Sigma_dynamic, Sigma_static, om, sigma, Y1(n, r_all, r_all), Y2(n, r_all, r_all));

          // Tr (Sigma * B)
          dcomplex tr_Sigma_B = 0;
          for (auto &&[alpha, R] : enumerated_sub_slices(embedding_decomp)) {
            auto [m, mp] = Sigma_dynamic(alpha, sigma).target_shape();
            auto A       = Sigma_dynamic(alpha, sigma).data()(n, r_all, r_all) + Sigma_static(alpha, sigma);
            auto C       = B(R, R);
            for (auto m1 : range(m))
              for (auto m2 : range(mp)) tr_Sigma_B += A(m1, m2) * C(m2, m1);
          }
          result(n) -= obe.H.k_weights(k_idx) * tr_Sigma_B;
        }
        return result;
      };
    }
  } // namespace detail

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

    auto n_sigma     = Sigma_dynamic.size2();
    auto n_k         = obe.H.n_k();
    auto const &mesh = Sigma_dynamic(0, 0).mesh();
    double beta      = mesh.beta();
    auto corr        = gf{mesh}; // Compute the correction term on all dlr mesh points
    double KS_term   = 0;        // The first Kohn Sham term, cf Notes

    // OP : we don't have a Fermi function in TRIQS ??
    auto Fermi                = [](double x) { return (x > 0 ? exp(-x) / (1 + exp(-x)) : 1 / (1 + exp(x))); };
    auto calc_correction_term = detail::trace_G_B_m_G_KS(obe, mu, Sigma_dynamic, Sigma_static);

    // ---------
    mpi::communicator comm = {}; // for now using default comm in MPI
#pragma omp parallel for collapse(2) reduction(gf_sum : corr) reduction(+ : KS_term) default(none)                                                   \
   shared(n_k, comm, n_sigma, obe, Fermi, mu, calc_correction_term, beta)
    for (auto k_idx : mpi::chunk(range(n_k), comm)) {
      for (auto sigma : range(n_sigma)) {

        // 1- KS term
        double KS_term_acc = 0;
        auto eps           = obe.H.H(sigma, k_idx);
        for (auto nu : range(obe.H.N_nu(sigma, k_idx)))        // we must cut at N_nu, do not add nu with eps =0
          KS_term_acc += Fermi(beta * real(eps(nu, nu) - mu)); // FIXME : H SHOULD BE REAL in OBE !!
        KS_term += obe.H.k_weights(k_idx) * KS_term_acc;

        //  2- Correction term
        corr.data() += calc_correction_term(sigma, k_idx);
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
    // nda::array<nda::matrix<double>, 2> const &Sigma_DC) {
    auto const &mesh = Sigma_dynamic(0, 0).mesh();
    auto beta        = mesh.beta();

    auto result = gf{mesh}; // Compute the correction term on all dlr mesh points

    auto PSP = [&](auto &iw, auto &k_idx, auto &sigma) {
      auto N_nu             = obe.H.N_nu(sigma, k_idx);
      auto out              = nda::zeros<dcomplex>(N_nu, N_nu);
      auto embedding_decomp = get_struct(Sigma_dynamic).dims(r_all, 0) | tl::to<std::vector>();
      for (auto &&[alpha, R] : enumerated_sub_slices(embedding_decomp)) {
        auto P = obe.P.P(sigma, k_idx)(R, r_all);
        out(r_all, r_all) += dagger(P) * nda::matrix<dcomplex>{Sigma_dynamic(alpha, sigma).data()(iw, r_all, r_all) + Sigma_static(alpha, sigma)} * P;
      }
      return out;
    };

    auto Glatt = [&](auto &k_idx, auto &sigma) {
      using nda::linalg::inv;
      auto out = gf{mesh};
      for (auto &&[n, w] : enumerate(mesh)) {
        out.data()(n) = trace(inv(w + mu - obe.H.H(sigma, k_idx) - PSP(n, k_idx, sigma)) - inv(w + mu - obe.H.H(sigma, k_idx)));
      }
      return out;
    };

    mpi::communicator comm = {}; // for now using default comm in MPI
#pragma omp parallel for collapse(2) reduction(gf_sum : result) default(none) shared(obe, Glatt, comm)
    for (auto k_idx : mpi::chunk(range(obe.H.n_k()), comm)) {
      for (auto sigma : range(obe.C_space.n_sigma())) { result.data() += obe.H.k_weights(k_idx) * Glatt(k_idx, sigma).data(); }
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
    return std::get<0>(triqs::experimental::utility::root_finder(method, f, 0.0, target_density, precision, 0.5, 1000, "Chemical Potential", "Total Density", verbosity));
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
    return std::get<0>(triqs::experimental::utility::root_finder(method, f, 0.0, target_density, precision, 0.5, 1000, "Chemical Potential", "Total Density", verbosity));
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
