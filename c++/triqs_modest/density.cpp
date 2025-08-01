#include "./density.hpp"

namespace triqs::modest {

  double density_nk_matrix_valued_impl(one_body_elements_on_grid const &obe, double mu, double beta) {

    auto Fermi     = [](double x) { return (x > 0 ? exp(-x) / (1 + exp(-x)) : 1 / (1 + exp(x))); };
    double KS_term = 0;

    for (auto sigma : nda::range(obe.C_space.n_sigma()))
      for (auto k_idx : nda::range(obe.H.n_k())) {
        double KS_term_acc = 0;
        auto eps           = nda::linalg::eigenvalues(nda::matrix<dcomplex>{obe.H.H(sigma, k_idx)});
        for (auto nu : range(obe.H.N_nu(sigma, k_idx))) { KS_term_acc += Fermi(beta * (eps(nu) - mu)); }
        KS_term += obe.H.k_weights(k_idx) * KS_term_acc;
      }
    return KS_term;
  }

  double density_nk(one_body_elements_on_grid const &obe, double mu, double beta) {
    // if band dispersion is matrix valued need to diagonalize H(k) at every k.
    if (obe.H.matrix_valued) return density_nk_matrix_valued_impl(obe, mu, beta);

    auto Fermi     = [](double x) { return (x > 0 ? exp(-x) / (1 + exp(-x)) : 1 / (1 + exp(x))); };
    double KS_term = 0;

    for (auto sigma : nda::range(obe.C_space.n_sigma()))
      for (auto k_idx : nda::range(obe.H.n_k())) {
        double KS_term_acc = 0;
        auto eps           = obe.H.H(sigma, k_idx);
        for (auto nu : range(obe.H.N_nu(sigma, k_idx))) { KS_term_acc += Fermi(beta * real(eps(nu, nu) - mu)); }
        KS_term += obe.H.k_weights(k_idx) * KS_term_acc;
      }
    return KS_term;
  }

} // namespace triqs::modest