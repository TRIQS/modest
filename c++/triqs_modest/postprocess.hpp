#pragma once

#include <nda/nda.hpp>
#include <triqs/gfs.hpp>
#include "./downfolding.hpp"

namespace triqs::modest {

  //-------------------------------------------------------------------------------------------

  /// Returns Tr (A) [σ,k,ω] for all k points in obe grid and all omega in Sigma mesh
  // nda::array<dcomplex, 3> spectral_function(one_body_elements_on_grid const &obe, double mu,
  //                                           // add magnetic field,
  //                                           block2_gf<mesh::refreq, matrix_valued> const &Sigma,
  //                                           nda::array<nda::matrix<double>, 2> const &Sigma_DC) {
  //   auto n_sigma       = Sigma.size2();
  //   auto const &mesh   = Sigma(0, 0).mesh();
  //   auto calc_trace_GB = detail::trace_G_B_m_G_KS(obe, mu, Sigma, Sigma_DC);
  //   auto result        = nda::zeros<dcomplex>(n_sigma, obe.H.n_k(), mesh.size());

  //   // WRONG : need to add KS TERM ?
  //   // FIXME : should we do it this way ???? cancelation of poles ???

  //   for (auto sigma : range(n_sigma)) {
  //     // FIXME JC : put openmp
  //     for (auto k_idx : nda::range(obe.H.n_k()))
  //       result(sigma, k_idx, r_all) = obe.H.k_weights(k_idx) * calc_trace_GB(sigma, k_idx);
  //   }
  //   result = -1 / M_PI * imag(result);
  //   return result;
  // }

  struct spectral_function_kw {
    nda::array<double, 3> data; // A[σ](k,w)
  };

  /**
 * @brief Compute momentum-resolved spectral function A(k, ω) along high-symmetry path.
 * 
 * @param obe  one-body elements on grid created from one_body_elements_on_high_symmetry_path (see obe factories)
 * @param mu   chemical potential
 * @param Sigma_w  self-energy on real-frequency mesh 
 * @param Sigma_DC double-counting term
 * @param broadening spectral broadening
 * @return spectral_function_kw 
 */
  spectral_function_kw spectral_function_on_high_symmetry_path(one_body_elements_on_grid const &obe, double mu,
                                                               block2_gf<mesh::refreq, matrix_valued> const &Sigma_w,
                                                               nda::array<nda::matrix<double>, 2> const &Sigma_DC,
                                                               double broadening = 0.01);
  struct spectral_function_w {
    nda::array<double, 2> total;     // A[σ](w)
    nda::array<double, 4> per_theta; // A[a,σ](m,m',w)
  };

  /**
 * @brief  Compute the atom- and orbital-resolved spectral function (interacting density of states).
 * 
 * @param obe_theta one-body elements on grid created from one_body_elements_with_theta_projectors (see obe factories).
 * @param Pk downfolding_projector defined in the correlated space using to upfold the DMFT self-energies.
 * @param mu chemical potential
 * @param Sigma_w self-energy on real-frequency mesh
 * @param Sigma_DC double counting term
 * @param broadening spectral broadening
 * @return spectral_function_w 
 */
  spectral_function_w projected_spectral_function(one_body_elements_on_grid const &obe_theta,
                                                  downfolding_projector const &Proj, double mu,
                                                  block2_gf<mesh::refreq, matrix_valued> const &Sigma_w,
                                                  nda::array<nda::matrix<double>, 2> const &Sigma_DC,
                                                  double broadening = 0.01);

} // namespace triqs::modest

// ------------------------------------------------------

/** @cond DOXYGEN_SKIP_THIS */
/** @endcond */
