#pragma once
#include <nda/nda.hpp>
#include <triqs/gfs.hpp>
#include <itertools/omp_chunk.hpp>
#include <triqs/lattice/brillouin_zone.hpp>
#include "./downfolding.hpp"
#include "./root_finder.hpp"
#include "utils/nda_supp.hpp"

namespace triqs::modest {

  // ============================================
  namespace detail {
    // impl detail : compute (1- Y1 Sigma)^{-1} Y2 in M x M space, with Sigma by block
    nda::matrix<dcomplex, nda::F_layout> calc_inv_G_G0(long M, auto embedding_decomp, auto const &Sigma_dynamic,
                                                       auto const &Sigma_hartree, auto om, long sigma,
                                                       nda::matrix_view<dcomplex> Y1, nda::matrix_view<dcomplex> Y2) {
      // Y Sigma. NB Sigma is by blocks.
      // NB: F_layout because of getrs in Ainv_B below
      auto YS = nda::matrix<dcomplex, nda::F_layout>::zeros(M, M);
      // auto Sigma_m_Sigma_DC = [&](auto alpha, auto sigma) {
      //   if (dH_C) return nda::matrix<dcomplex>{Sigma(alpha, sigma)[om] - dH_C.value()(alpha, sigma)};
      //   return nda::matrix<dcomplex>{Sigma(alpha, sigma)[om]};
      // };
      for (auto &&[alpha, R] : enumerated_sub_slices(embedding_decomp)) {
        auto Sigma = nda::matrix<dcomplex>{Sigma_dynamic(alpha, sigma)[om] + Sigma_hartree(alpha, sigma)};
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
                          block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                          nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree) {

      auto M                = obe.C_space.dim();
      auto &mesh            = Sigma_dynamic(0, 0).mesh();
      auto omegas           = mesh | tl::to<std::vector<dcomplex>>();
      auto embedding_decomp = get_struct(Sigma_dynamic).dims(r_all, 0) | tl::to<std::vector>();

      return [=, &obe, &Sigma_dynamic, &Sigma_hartree](long sigma, long k_idx) {
        auto result = nda::zeros<dcomplex>(omegas.size());
        auto Y1     = detail::G0_C_k_sigma(obe, mu, k_idx, sigma, omegas, false); // Y1 = G0_𝓒
        auto Y2     = detail::G0_C_k_sigma(obe, mu, k_idx, sigma, omegas, true);  // Y2 = + ∂_μ G0_𝓒

        for (auto &&[n, om] : itertools::enumerate(mesh)) {
          // Compute (1- Y1 Sigma)^{-1} Y2
          auto B = calc_inv_G_G0(M, embedding_decomp, Sigma_dynamic, Sigma_hartree, om, sigma, Y1(n, r_all, r_all),
                                 Y2(n, r_all, r_all));

          // Tr (Sigma * B)
          dcomplex tr_Sigma_B = 0;
          for (auto &&[alpha, R] : enumerated_sub_slices(embedding_decomp)) {
            auto [m, mp] = Sigma_dynamic(alpha, sigma).target_shape();
            auto A       = Sigma_dynamic(alpha, sigma).data()(n, r_all, r_all) + Sigma_hartree(alpha, sigma);
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
 * @brief  Compute the density of the lattice Green's function (Woodbury)
 * 
 * @param obe one body elements on grid 
 * @param mu chemical potential
 * @param Sigma Embedded self-energy
 * @return double 
 */
  double density(one_body_elements_on_grid const &obe, double mu,
                 // add magnetic field,
                 block2_gf<mesh::dlr_imfreq, matrix_valued> const &Sigma_dynamic,
                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree);

  // ------------------------------------------------------------------------------------
  ///Compute number of particles n = ∑f(β(e(k)-μ))
  double density_nk(one_body_elements_on_grid const &obe, double mu, double beta);

  // ------------------------------------------------------------------------------------
  /**
 * @brief  compute density of lattice Green's function (slowly)
 * 
 * @tparam Mesh 
 * @param obe one_body_elements_on_grid
 * @param mu chemical potential
 * @param Sigma Embedded self-energy
 * @return double 
 */
  template <typename Mesh>
  double density_slow(one_body_elements_on_grid const &obe, double mu,
                      block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                      nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree) {
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
        out(r_all, r_all) += dagger(P)
           * nda::matrix<dcomplex>{Sigma_dynamic(alpha, sigma).data()(iw, r_all, r_all) + Sigma_hartree(alpha, sigma)}
           * P;
      }
      return out;
    };

    auto Glatt = [&](auto &k_idx, auto &sigma) {
      auto out = gf{mesh};
      for (auto &&[n, w] : enumerate(mesh)) {
        out.data()(n) = trace(inverse(w + mu - obe.H.H(sigma, k_idx) - PSP(n, k_idx, sigma))
                              - inverse(w + mu - obe.H.H(sigma, k_idx)));
      }
      return out;
    };

    for (auto sigma : nda::range(obe.C_space.n_sigma())) {
      for (auto k_idx : nda::range(obe.H.n_k())) {
        result.data() += obe.H.k_weights(k_idx) * Glatt(k_idx, sigma).data();
      }
    }

    return density_nk(obe, mu, beta) + real(density(result));
  }

  double find_chemical_potential(double const target_density, one_body_elements_on_grid const &obe, double beta,
                                 std::string method = "dichotomy", double x_init = 0.0, double precision = 1.e-5,
                                 double delta_x = 0.5, long max_loops = 1000, std::string x_name = "Chemical Potential",
                                 std::string y_name = "Total Density", bool verbosity = true);

  double find_chemical_potential(double const target_density, one_body_elements_on_grid const &obe,
                                 block2_gf<imfreq, matrix_valued> const &Sigma_dynamic,
                                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree,
                                 std::string method = "dichotomy", double x_init = 0.0, double precision = 1.e-5,
                                 double delta_x = 0.5, long max_loops = 1000, std::string x_name = "Chemical Potential",
                                 std::string y_name = "Total Density", bool verbosity = true);

  double find_chemical_potential(double const target_density, one_body_elements_on_grid const &obe,
                                 block2_gf<dlr_imfreq, matrix_valued> const &Sigma_dynamic,
                                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree,
                                 std::string method = "dichotomy", double x_init = 0.0, double precision = 1.e-5,
                                 double delta_x = 0.5, long max_loops = 1000, std::string x_name = "Chemical Potential",
                                 std::string y_name = "Total Density", bool verbosity = true);

  /** @cond DOXYGEN_SKIP_THIS */
  template double density_slow(one_body_elements_on_grid const &obe, double mu,
                               block2_gf<imfreq, matrix_valued> const &Sigma_dynamic,
                               nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree);
  //  nda::array<nda::matrix<double>, 2> const &Sigma_DC);

  /** @endcond */

} // namespace triqs::modest
