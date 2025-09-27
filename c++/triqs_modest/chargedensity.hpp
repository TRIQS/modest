#pragma once
#include "utils/defs.hpp"
#include "utils/gf_supp.hpp"
#include <triqs/mesh.hpp>
#include "downfolding.hpp"
#include "gloc_fixed_grid.hpp"

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

    auto n_sigma = obe.C_space.n_sigma(); // number of spin channesl
    auto n_k     = obe.H.n_k();           // number of k-points

    // maximum number of bands
    auto n_nu_k = range(n_k) | stdv::transform([&obe](auto k_idx) { return obe.H.N_nu(0, k_idx); }) | tl::to<std::vector>();
    auto n_nu   = *std::max_element(begin(n_nu_k), end(n_nu_k));

    // setup data structure for return
    auto N_nu_nup_k = nda::zeros<dcomplex>(n_k, n_sigma, n_nu, n_nu);

    // calculator for lattice Green's function at given k and σ
    auto glatt_at_k = detail::lattice_gf_at_k(obe, mu, Sigma_dynamic, Sigma_static);

    // loop over k and σ
    // TODO: parallelize over k and gather
    for (auto k_idx : range(n_k)) {
      for (auto const &sigma : range(n_sigma)) {
        auto r_window                                = nda::range(0, obe.H.N_nu(sigma, k_idx));
        N_nu_nup_k(k_idx, sigma, r_window, r_window) = density(glatt_at_k(k_idx, sigma));
      }
    }

    return N_nu_nup_k;
  }

  template nda::array<dcomplex, 4> charge_density_correction(one_body_elements_on_grid const &obe, double mu,
                                                             block2_gf<imfreq, matrix_valued> const &Sigma_dynamic,
                                                             nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static);

} // namespace triqs::modest