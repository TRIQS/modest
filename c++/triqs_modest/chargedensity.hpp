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
    auto wb          = detail::make_woodbury_setup(Sigma_dynamic, Sigma_static);

#pragma omp parallel for collapse(2) default(none) shared(n_k, n_sigma, mesh, obe, mu, wb, N_nu_nup_k, r_all)
    for (auto k_idx : range(n_k)) {
      for (auto sigma : range(n_sigma)) {
        auto N_nu_k = obe.H.N_nu(sigma, k_idx);

        // Build G_k(ω) in the band basis via Woodbury, then call density() on it.
        auto Glatt   = gf{mesh, std::array<long, 2>{N_nu_k, N_nu_k}};
        Glatt.data() = detail::build_G_band(obe, wb.active, wb.Sa_per_sigma[sigma], wb.omegas, mu, k_idx, sigma);

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
