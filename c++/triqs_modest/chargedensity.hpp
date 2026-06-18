#pragma once
#include "utils/defs.hpp"
#include "utils/gf_supp.hpp"
#include <triqs/mesh.hpp>
#include "downfolding.hpp"
#include "./lattice_gf_helpers.hpp"

namespace triqs::modest {

  /**
 * @brief Compute the charge density correction from DMFT
 *
 * Compute the charge density correction in the band basis \f$ N_{\nu\nu'}(\mathbf{k}) \f$ from the lattice Green's function.
 *
 * @tparam Mesh The mesh type
 * @param obe The one-body elements on the grid
 * @param mu The chemical potential
 * @param Sigma_dynamic The dynamic part of the self-energy
 * @param Sigma_static The static part of the self-energy
 * @return The charge density correction in the band basis \f$ N_{\nu\nu'}(\mathbf{k}) \f$
 */
  template <typename Mesh>
  nda::array<dcomplex, 4> charge_density_correction(one_body_elements_on_grid const &obe, double mu,
                                                    block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                                                    nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static) {

    auto n_sigma = obe.C_space.n_sigma();
    auto n_k     = obe.H.n_k();

    // Output: band-basis density matrix per (k, σ).  Use n_nu = max over k of N_ν(0, k).
    auto n_nu_k     = range(n_k) | stdv::transform([&obe](auto k_idx) { return obe.H.N_nu(0, k_idx); }) | tl::to<std::vector>();
    auto n_nu       = *std::max_element(begin(n_nu_k), end(n_nu_k));
    auto N_nu_nup_k = nda::zeros<dcomplex>(n_k, n_sigma, n_nu, n_nu);

    // Matrix-valued H(k): direct N_ν × N_ν inversion fallback (no rank-reduction benefit).
    if (obe.H.matrix_valued) {
      auto glatt_at_k = detail::lattice_gf_at_k(obe, mu, Sigma_dynamic, Sigma_static);
#pragma omp parallel for collapse(2) default(none) shared(n_k, n_sigma, obe, glatt_at_k, N_nu_nup_k, r_all)
      for (auto k_idx : range(n_k)) {
        for (auto sigma : range(n_sigma)) {
          auto r_window                                = nda::range(0, obe.H.N_nu(sigma, k_idx));
          N_nu_nup_k(k_idx, sigma, r_window, r_window) = density(glatt_at_k(k_idx, sigma));
        }
      }
      return N_nu_nup_k;
    }

    // Diagonal H(k): rank-reduced Woodbury, band-basis G_k built per (k, σ, ω)
    auto const &mesh = Sigma_dynamic(0, 0).mesh();
    auto Sigma_total = detail::make_sigma_total(Sigma_dynamic, Sigma_static);

    auto omegas = mesh | tl::to<std::vector<dcomplex>>();
    auto decomp = get_struct(Sigma_total).dims(r_all, 0) | tl::to<std::vector>();
    auto active = detail::detect_active_subspace(Sigma_total, decomp);
    auto Sa_per_sigma =
       range(n_sigma) | stdv::transform([&](long sigma) { return detail::compute_sigma_active(Sigma_total, active, sigma); }) | tl::to<std::vector>();

#pragma omp parallel for collapse(2) default(none) shared(n_k, n_sigma, mesh, omegas, obe, mu, active, Sa_per_sigma, N_nu_nup_k, r_all)
    for (auto k_idx : range(n_k)) {
      for (auto sigma : range(n_sigma)) {
        auto N_nu_k = obe.H.N_nu(sigma, k_idx);
        auto Hk     = obe.H.H(sigma, k_idx);
        auto P_full = obe.P.P(sigma, k_idx);

        // Q = obe.P[active, :]   (rank × N_ν_k)   and   D(n, ν) = 1/(ω_n + μ − ε_ν(k)).
        auto Q = nda::matrix<dcomplex>(active.rank, N_nu_k);
        for (auto c : range(active.rank))
          for (auto nu : range(N_nu_k)) Q(c, nu) = P_full(active.c_indices[c], nu);

        auto D = nda::matrix<dcomplex>(omegas.size(), N_nu_k);
        for (long n = 0; n < long(omegas.size()); ++n)
          for (auto nu : range(N_nu_k)) D(n, nu) = 1.0 / (omegas[n] + mu - Hk(nu, nu));

        // Build G_k(ω) in band basis via Woodbury, then call density() on it.
        auto Glatt = gf{mesh, std::array<long, 2>{N_nu_k, N_nu_k}};

        if (active.rank == 0) {
          // Σ ≡ 0: G_k = M₀⁻¹ (diagonal in band basis).
          for (long n = 0; n < long(omegas.size()); ++n)
            for (auto nu : range(N_nu_k)) Glatt.data()(n, nu, nu) = D(n, nu);
        } else {
          auto bare        = detail::compute_bare_projected(obe, obe.P, mu, k_idx, sigma, omegas, active);
          auto const &Sa_n = Sa_per_sigma[sigma];

          for (long n = 0; n < long(omegas.size()); ++n) {
            // G_k(ν, ν') = D(ν) δ_νν' + D(ν) · (Q† · K · Q)(ν, ν') · D(ν').
            auto Sa       = nda::matrix<dcomplex>{Sa_n(n, r_all, r_all)};
            auto Yaa      = nda::matrix<dcomplex>{bare.G0_QQ(n, r_all, r_all)};
            auto KQ       = detail::apply_K(Sa, Yaa, Q);
            auto Q_dag_KQ = nda::matrix<dcomplex>{dagger(Q) * KQ};
            for (auto nu : range(N_nu_k))
              for (auto nup : range(N_nu_k)) Glatt.data()(n, nu, nup) = D(n, nu) * Q_dag_KQ(nu, nup) * D(n, nup);
            for (auto nu : range(N_nu_k)) Glatt.data()(n, nu, nu) += D(n, nu);
          }
        }

        auto r_window                                = nda::range(0, N_nu_k);
        N_nu_nup_k(k_idx, sigma, r_window, r_window) = density(Glatt);
      }
    }

    return N_nu_nup_k;
  }

  template nda::array<dcomplex, 4> charge_density_correction(one_body_elements_on_grid const &obe, double mu,
                                                             block2_gf<imfreq, matrix_valued> const &Sigma_dynamic,
                                                             nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static);

} // namespace triqs::modest
