#include "./density.hpp"

namespace triqs::modest {

  double density_nk(one_body_elements_on_grid const &obe, double mu, double beta) {

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

  // ---------------------------------------------------------------

  double density(one_body_elements_on_grid const &obe, double mu,
                 // add magnetic field,
                 block2_gf<mesh::dlr_imfreq, matrix_valued> const &Sigma_dynamic,
                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree) {

    auto n_sigma     = Sigma_dynamic.size2();
    auto const &mesh = Sigma_dynamic(0, 0).mesh();
    double beta      = mesh.beta();
    auto corr        = gf{mesh}; // Compute the correction term on all dlr mesh points
    double KS_term   = 0;        // The first Kohn Sham term, cf Notes

    // OP : we don't have a Fermi function in TRIQS ??
    auto Fermi                = [](double x) { return (x > 0 ? exp(-x) / (1 + exp(-x)) : 1 / (1 + exp(x))); };
    auto calc_correction_term = detail::trace_G_B_m_G_KS(obe, mu, Sigma_dynamic, Sigma_hartree);

    // ---------
    for (auto sigma : range(n_sigma)) {
      // FIXME JC : put openmp
      for (auto k_idx : nda::range(obe.H.n_k())) {

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
    // OP, JC : Fix the mpi. NB :  KS_term : REDUCE TOO
    //corr = mpi::all_reduce(corr);
    return KS_term + real(density(corr));
  }

  double find_chemical_potential(double const target_density, one_body_elements_on_grid const &obe, double beta,
                                 std::string method, double x_init, double precision, double delta_x, long max_loops,
                                 std::string x_name, std::string y_name, bool verbosity) {
    return std::get<0>(triqs::root_finder(
       method, [&obe, beta](double x) { return density_nk(obe, x, beta); }, x_init, target_density, precision, delta_x,
       max_loops, x_name, y_name, verbosity));
  }

  double find_chemical_potential(double const target_density, one_body_elements_on_grid const &obe,
                                 block2_gf<imfreq, matrix_valued> const &Sigma_dynamic,
                                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree, std::string method,
                                 double x_init, double precision, double delta_x, long max_loops, std::string x_name,
                                 std::string y_name, bool verbosity) {
    std::function<double(double)> f = [&obe, &Sigma_dynamic, &Sigma_hartree](double x) {
      return density_slow(obe, x, Sigma_dynamic, Sigma_hartree);
    };
    return std::get<0>(
       triqs::root_finder(method, f, x_init, target_density, precision, delta_x, max_loops, x_name, y_name, verbosity));
  }

  double find_chemical_potential(double const target_density, one_body_elements_on_grid const &obe,
                                 block2_gf<dlr_imfreq, matrix_valued> const &Sigma_dynamic,
                                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree, std::string method,
                                 double x_init, double precision, double delta_x, long max_loops, std::string x_name,
                                 std::string y_name, bool verbosity) {
    std::function<double(double)> f = [&obe, &Sigma_dynamic, &Sigma_hartree](double x) {
      return density(obe, x, Sigma_dynamic, Sigma_hartree);
    };
    return std::get<0>(
       triqs::root_finder(method, f, x_init, target_density, precision, delta_x, max_loops, x_name, y_name, verbosity));
  }
} // namespace triqs::modest