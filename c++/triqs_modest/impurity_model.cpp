#include "./impurity_model.hpp"
#include <itertools/enumerate.hpp>
#include <triqs/operators/many_body_operator.hpp>
#include <algorithm>
#include "utils/defs.hpp"

namespace triqs {

  namespace detail {

    auto n(auto const &x) { return operators::n(x.first, x.second); };
    auto c_dag(auto const &x) { return operators::c_dag(x.first, x.second); };
    auto c(auto const &x) { return operators::c(x.first, x.second); };

    /// build fermionic operator flavors for the interaction
    nda::array<std::pair<std::string, long>, 2> make_op_names(auto const &tau_names, auto const &dim_gamma) {
      auto n_sigma  = long(tau_names.size());
      auto n_orb    = stdr::fold_left(dim_gamma, 0, std::plus<>());
      auto op_names = nda::array<std::pair<std::string, long>, 2>(n_orb, n_sigma);
      for (auto [itau, tau] : enumerate(tau_names)) {
        auto pl = 0;
        for (auto [igamma, gamma] : enumerate(dim_gamma)) {
          for (auto orb : range(gamma)) {
            op_names(pl, itau) = {fmt::format("{}_{}", tau, igamma), orb};
            pl++;
          }
        }
      }
      return op_names;
    }

    //-----------------------------------------

    /// construct the Kanamori two-index interaction matrix for parallel and anti-parallel spins.
    std::pair<nda::matrix<double>, nda::matrix<double>> kanamori_umatrices(long n_orb, double U_int, double U_prime,
                                                                           double J_hund) {
      auto Umat  = nda::zeros<double>(n_orb, n_orb);
      auto Upmat = nda::zeros<double>(n_orb, n_orb);
      for (auto &&[m, mp] : product(range(n_orb), range(n_orb))) {
        if (m == mp) {
          Upmat(m, mp) = U_int;
        } else {
          Umat(m, mp)  = U_prime - J_hund;
          Upmat(m, mp) = U_prime;
        }
      }
      return {Umat, Upmat};
    }

    double three_j_symbol(std::tuple<long, long, long, long, long, long> jms) {
      auto [j1, m1, j2, m2, j3, m3] = jms;
      auto fact                     = [](auto x) { return std::tgamma(x + 1); };
      if ((m1 + m2 + m3 != 0) || (m1 < -j1) || (m1 > j1) || (m2 < -j2) || (m2 > j2) || (m3 < -j3) || (m3 > j3)
          || (j3 > j1 + j2) || j3 < std::abs(j1 - j2)) {
        return 0.0;
      }
      auto three_j_sym = ((j1 - j2 - m3) % 2) ? -1.0 : 1.0;
      three_j_sym *= sqrt((fact(j1 + j2 - j3) * fact(j1 - j2 + j3) * fact(-j1 + j2 + j3)) / fact(j1 + j2 + j3 + 1));
      three_j_sym *=
         sqrt(fact(j1 - m1) * fact(j1 + m1) * fact(j2 - m2) * fact(j2 + m2) * fact(j3 - m3) * fact(j3 + m3));

      auto t_min = std::max({j2 - j3 - m1, j1 - j3 + m2, static_cast<long>(0)});
      auto t_max = std::min({j1 - m1, j2 + m2, j1 + j2 - j3});
      auto t_sum = 0.0;
      for (auto t : range(t_min, t_max + 1)) {
        t_sum += ((t % 2) ? -1.0 : 1.0)
           / (fact(t) * fact(j3 - j2 + m1 + t) * fact(j3 - j1 - m2 + t) * fact(j1 + j2 - j3 - t) * fact(j1 - m1 - t)
              * fact(j2 + m2 - t));
      }
      three_j_sym *= t_sum;
      return three_j_sym;
    }

    /// calculate the angular matrix element
    double angular_matrix_element(long l, long k, long m1, long m2, long m3, long m4) {
      auto squared = [](auto x) { return x * x; };

      auto ang_mat_ele = 0.0;
      for (auto q : range(-k, k + 1)) {
        ang_mat_ele += three_j_symbol({l, -m1, k, q, l, m3}) * three_j_symbol({l, -m2, k, -q, l, m4})
           * (((m1 + q + m2) % 2) ? -1.0 : 1.0);
      }
      ang_mat_ele *= squared(2.0 * static_cast<double>(l) + 1) * squared(three_j_symbol({l, 0, k, 0, l, 0}));
      return ang_mat_ele;
    }

    nda::array<double, 1> radial_integrals(long l, double U_int, double J_hund) {
      auto F = nda::zeros<double>(l + 1);
      if (l == 1) {
        F(0) = U_int;
        F(1) = 5 * J_hund;
      } else if (l == 2) {
        F(0) = U_int;
        F(1) = 14.0 * J_hund / (1.0 + 0.63);
        F(2) = 0.63 * F(1);
      } else if (l == 3) {
        F(0) = U_int;
        F(1) = (6435.0 * J_hund) / (286.0 + 195.0 * 451.0 / 675.0 + 250.0 * 1001.0 / 2025.0);
        F(2) = 451.0 * F(1) / 675.0;
        F(3) = 1001.0 * F(1) / 2025.0;
      }
      return F;
    }
  } // namespace detail

  operators::many_body_operator make_density_density(const std::vector<std::string> &tau_names,
                                                     const std::vector<long> &dim_gamma, double U_int, double U_prime,
                                                     double J_hund) {
    return make_kanamori(tau_names, dim_gamma, U_int, U_prime, J_hund, false, false);
  }

  // ----------------------------------------------------
  operators::many_body_operator make_kanamori(std::vector<std::string> const &tau_names,
                                              std::vector<long> const &dim_gamma, double U_int, double U_prime,
                                              double J_hund, bool spin_flip, bool pair_hopping) {
    auto n_sigma = long(tau_names.size());
    auto n_orb   = stdr::fold_left(dim_gamma, 0, std::plus<>());

    auto op_names = detail::make_op_names(tau_names, dim_gamma);

    // construct kanamori U matricies
    auto [Umat, Upmat] = detail::kanamori_umatrices(n_orb, U_int, U_prime, J_hund);

    // construct h_int
    auto h_int = operators::many_body_operator();
    // density-density terms
    for (auto &&[s1, s2] : product(range(n_sigma), range(n_sigma))) {
      for (auto &&[a1, a2] : product(range(n_orb), range(n_orb))) {
        h_int += 0.5 * ((s1 == s2) ? Umat(a1, a2) : Upmat(a1, a2)) * detail::n(op_names(a1, s1))
           * detail::n(op_names(a2, s2));
      }
    }

    // spin-flip terms
    if (spin_flip) {
      for (auto &&[s1, s2] : product(range(n_sigma), range(n_sigma))) {
        if (s1 == s2) continue;
        for (auto &&[a1, a2] : product(range(n_orb), range(n_orb))) {
          if (a1 == a2) continue;
          h_int += -0.5 * J_hund * detail::c_dag(op_names(a1, s1)) * detail::c(op_names(a1, s2))
             * detail::c_dag(op_names(a2, s2)) * detail::c(op_names(a2, s1));
        }
      }
    }

    // pair-hopping terms
    if (pair_hopping) {
      for (auto &&[s1, s2] : product(range(n_sigma), range(n_sigma))) {
        if (s1 == s2) continue;
        for (auto &&[a1, a2] : product(range(n_orb), range(n_orb))) {
          if (a1 == a2) continue;
          h_int += 0.5 * J_hund * detail::c_dag(op_names(a1, s1)) * detail::c_dag(op_names(a1, s2))
             * detail::c(op_names(a2, s2)) * detail::c(op_names(a2, s1));
        }
      }
    }
    return h_int;
  }

  nda::array<double, 4> U_matrix_in_spherical_basis(long l, double U_int, double J_hund) {
    auto U_matrix         = nda::zeros<double>(2 * l + 1, 2 * l + 1, 2 * l + 1, 2 * l + 1);
    auto m_range          = nda::range(-l, l + 1);
    auto radial_integrals = detail::radial_integrals(l, U_int, J_hund);
    for (auto [n, F] : itertools::enumerate(radial_integrals)) {
      auto k = 2 * n;
      for (auto [m1, m2, m3, m4] : product(m_range, m_range, m_range, m_range)) {
        U_matrix(m1 + l, m2 + l, m3 + l, m4 + l) += F * detail::angular_matrix_element(l, k, m1, m2, m3, m4);
      }
    }
    return U_matrix;
  }

  nda::array<dcomplex, 4> U_matrix_in_local_basis(long l, nda::matrix<dcomplex> s2l, double U_int, double J_hund) {
    auto s2lC        = conj(s2l);
    auto s2lT        = transpose(s2l);
    auto N           = 2 * l + 1;
    auto U_spherical = U_matrix_in_spherical_basis(l, U_int, J_hund);
    auto U_local     = nda::zeros<dcomplex>(N, N, N, N);

    // FIXME: replace with nda::(tensor product)?
    for (auto i : range(N))
      for (auto k : range(N))
        for (auto n : range(N))
          for (auto p : range(N))
            for (auto j : range(N))
              for (auto q : range(N))
                for (auto m : range(N))
                  for (auto o : range(N)) {
                    auto left  = s2lC(i, j) * s2lC(k, q);
                    auto right = (s2lT(m, n)) * s2lT(o, p);
                    U_local(i, k, n, p) += left * U_spherical(j, q, m, o) * right;
                  }
    return U_local;
  }

  operators::many_body_operator make_slater(std::vector<std::string> const &tau_names,
                                            std::vector<long> const &dim_gamma, double U_int, double J_hund,
                                            nda::matrix<dcomplex> const &spherical_to_dft,
                                            std::optional<nda::matrix<dcomplex>> const &dft_to_local) {
    auto n_sigma = long(tau_names.size());
    auto n_orb   = stdr::fold_left(dim_gamma, 0, std::plus<>());

    auto op_names = detail::make_op_names(tau_names, dim_gamma);

    // construct U matrix
    long l         = (n_orb - 1) / 2;
    auto transform = (dft_to_local) ? spherical_to_dft * dft_to_local.value() : spherical_to_dft;
    auto U_matrix  = U_matrix_in_local_basis(l, transform, U_int, J_hund);

    // construct h_int
    auto h_int = operators::many_body_operator();
    for (auto [s1, s2] : product(range(n_sigma), range(n_sigma))) {
      for (auto [m1, m2, m3, m4] : product(range(n_orb), range(n_orb), range(n_orb), range(n_orb))) {
        h_int += 0.5 * real(U_matrix(m1, m2, m3, m4)) * detail::c_dag(op_names(m1, s1))
           * detail::c_dag(op_names(m2, s2)) * detail::c(op_names(m4, s2)) * detail::c(op_names(m3, s1));
      }
    }
    return h_int;
  }
} // namespace triqs