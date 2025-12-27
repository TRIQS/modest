// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "./hamiltonians.hpp"
#include <itertools/enumerate.hpp>
#include <triqs/operators/many_body_operator.hpp>
#include <algorithm>
#include "utils/defs.hpp"

namespace triqs {

  namespace detail {

    auto n(auto const &x) { return operators::n(x.first, x.second); };
    auto c_dag(auto const &x) { return operators::c_dag(x.first, x.second); };
    auto c(auto const &x) { return operators::c(x.first, x.second); };

    //-----------------------------------------

    double three_j_symbol(std::tuple<long, long, long, long, long, long> jms) {
      auto [j1, m1, j2, m2, j3, m3] = jms;
      auto fact                     = [](auto x) { return std::tgamma(x + 1); };
      if ((m1 + m2 + m3 != 0) || (m1 < -j1) || (m1 > j1) || (m2 < -j2) || (m2 > j2) || (m3 < -j3) || (m3 > j3) || (j3 > j1 + j2)
          || j3 < std::abs(j1 - j2)) {
        return 0.0;
      }
      auto three_j_sym = ((j1 - j2 - m3) % 2) ? -1.0 : 1.0;
      three_j_sym *= sqrt((fact(j1 + j2 - j3) * fact(j1 - j2 + j3) * fact(-j1 + j2 + j3)) / fact(j1 + j2 + j3 + 1));
      three_j_sym *= sqrt(fact(j1 - m1) * fact(j1 + m1) * fact(j2 - m2) * fact(j2 + m2) * fact(j3 - m3) * fact(j3 + m3));

      auto t_min = std::max({j2 - j3 - m1, j1 - j3 + m2, static_cast<long>(0)});
      auto t_max = std::min({j1 - m1, j2 + m2, j1 + j2 - j3});
      auto t_sum = 0.0;
      for (auto t : range(t_min, t_max + 1)) {
        t_sum += ((t % 2) ? -1.0 : 1.0)
           / (fact(t) * fact(j3 - j2 + m1 + t) * fact(j3 - j1 - m2 + t) * fact(j1 + j2 - j3 - t) * fact(j1 - m1 - t) * fact(j2 + m2 - t));
      }
      three_j_sym *= t_sum;
      return three_j_sym;
    }

    /// calculate the angular matrix element
    double angular_matrix_element(long l, long k, long m1, long m2, long m3, long m4) {
      auto squared = [](auto x) { return x * x; };

      auto ang_mat_ele = 0.0;
      for (auto q : range(-k, k + 1)) {
        ang_mat_ele += three_j_symbol({l, -m1, k, q, l, m3}) * three_j_symbol({l, -m2, k, -q, l, m4}) * (((m1 + q + m2) % 2) ? -1.0 : 1.0);
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

  std::map<op_name, op_name> make_op_map(auto const &spin_names, auto const &orb_dims) {
    std::map<op_name, op_name> op_map;
    for (auto const &spin : spin_names) {
      long flat_idx = 0;
      for (auto [bl_idx, bl_size] : enumerate(orb_dims)) {
        for (auto orb : range(bl_size)) { op_map[{spin, flat_idx++}] = {fmt::format("{}_{}", spin, bl_idx), orb}; }
      }
    }
    return op_map;
  }

  operators::many_body_operator rename_op(operators::many_body_operator const &op, std::map<op_name, op_name> const &op_map) {

    auto rename_monomial = [&](auto const &m) -> operators::many_body_operator {
      operators::many_body_operator nop = operators::many_body_operator(1.0);
      for (auto const &b : m) {
        auto name = op_map.at({std::get<std::string>(b.indices[0]), std::get<long>(b.indices[1])});
        nop *= (b.dagger) ? detail::c_dag(name) : detail::c(name);
      }
      return nop;
    };

    auto new_op = operators::many_body_operator();
    for (auto const &term : op) { new_op += term.coef * rename_monomial(term.monomial); }

    return new_op;
  }

  /// construct the Kanamori two-index interaction matrix for parallel and anti-parallel spins.
  std::pair<nda::matrix<double>, nda::matrix<double>> U_matrix_kanamori(long n_orb, double U_int, double U_prime, double J_hund) {
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

  nda::array<double, 4> U_matrix_slater_spherical(long l, double U_int, double J_hund) {
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

  nda::array<dcomplex, 4> rotate_U_matrix_slater(nda::array<double, 4> const &Uspherical, nda::matrix<dcomplex> sph_to_local) {
    auto s2lC    = conj(sph_to_local);
    auto s2lT    = transpose(sph_to_local);
    auto N       = Uspherical.extent(0);
    auto U_local = nda::zeros<dcomplex>(N, N, N, N);
    for (auto [i, k, n, p, j, q, m, o] : product(range(N), range(N), range(N), range(N), range(N), range(N), range(N), range(N))) {
      auto left  = s2lC(i, j) * s2lC(k, q);
      auto right = (s2lT(m, n)) * s2lT(o, p);
      U_local(i, k, n, p) += left * Uspherical(j, q, m, o) * right;
    }
    return U_local;
  }

  nda::array<dcomplex, 4> U_matrix_slater_local(long l, nda::matrix<dcomplex> sph_to_local, double U_int, double J_hund) {
    auto Uspherical = U_matrix_slater_spherical(l, U_int, J_hund); // construct U in spherical basis
    return rotate_U_matrix_slater(Uspherical, sph_to_local);       // rotate spherical U to local basis
  }

  operators::many_body_operator h_int_kanamori(nda::matrix<double> const &Umat, nda::matrix<double> const &Upmat, double const &J_hund,
                                               int const &n_orb, std::vector<std::string> const &spin_names, kanamori_params const &params) {

    auto h_int = operators::many_body_operator();

    // density-density terms
    for (auto &&[s1, s2] : itertools::product(spin_names, spin_names)) {
      for (auto &&[a1, a2] : product(range(n_orb), range(n_orb))) {
        h_int += 0.5 * ((s1 == s2) ? Umat(a1, a2) : Upmat(a1, a2)) * detail::n(std::make_pair(s1, a1)) * detail::n(std::make_pair(s2, a2));
      }
    }

    // spin-flip terms
    if (params.spin_flip) {
      for (auto &&[s1, s2] : itertools::product(spin_names, spin_names)) {
        if (s1 == s2) continue;
        for (auto &&[a1, a2] : product(range(n_orb), range(n_orb))) {
          if (a1 == a2) continue;
          h_int += -0.5 * J_hund * detail::c_dag(std::make_pair(s1, a1)) * detail::c(std::make_pair(s2, a1)) * detail::c_dag(std::make_pair(s2, a2))
             * detail::c(std::make_pair(s1, a2));
        }
      }
    }

    // pair-hopping terms
    if (params.pair_hopping) {
      for (auto &&[s1, s2] : itertools::product(spin_names, spin_names)) {
        if (s1 == s2) continue;
        for (auto &&[a1, a2] : product(range(n_orb), range(n_orb))) {
          if (a1 == a2) continue;
          h_int += 0.5 * J_hund * detail::c_dag(std::make_pair(s1, a1)) * detail::c_dag(std::make_pair(s2, a1)) * detail::c(std::make_pair(s2, a2))
             * detail::c(std::make_pair(s1, a2));
        }
      }
    }
    return h_int;
  }

  operators::many_body_operator h_int_density(nda::matrix<double> const &Umat, nda::matrix<double> const &Upmat, double const &J_hund,
                                              int const &n_orb, std::vector<std::string> const &spin_names) {
    kanamori_params params{.spin_flip = false, .pair_hopping = false};
    return h_int_kanamori(Umat, Upmat, J_hund, n_orb, spin_names, params);
  }

  // ----------------------------------------------------

  operators::many_body_operator h_int_slater(nda::array<dcomplex, 4> const &Umatrix, int const &n_orb, std::vector<std::string> const &spin_names) {
    auto h_int = operators::many_body_operator();
    for (auto &&[s1, s2] : itertools::product(spin_names, spin_names)) {
      for (auto [m1, m2, m3, m4] : product(range(n_orb), range(n_orb), range(n_orb), range(n_orb))) {
        h_int += 0.5 * real(Umatrix(m1, m2, m3, m4)) * detail::c_dag(std::make_pair(s1, m1)) * detail::c_dag(std::make_pair(s2, m2))
           * detail::c(std::make_pair(s2, m4)) * detail::c(std::make_pair(s1, m3));
      }
    }
    return h_int;
  }

  // ----------------------------------------------------
  operators::many_body_operator make_kanamori(std::vector<std::string> const &tau_names, std::vector<long> const &dim_gamma, double U_int,
                                              double U_prime, double J_hund, bool spin_flip, bool pair_hopping) {

    // compute number of orbitals
    long n_orb = stdr::fold_left(dim_gamma, 0, std::plus<>());

    // construct operator mapping
    auto op_map = make_op_map(tau_names, dim_gamma);

    // construct kanamori U matrices
    auto [Umat, Upmat] = U_matrix_kanamori(n_orb, U_int, U_prime, J_hund);

    // construct kanamori params
    kanamori_params params{.spin_flip = spin_flip, .pair_hopping = pair_hopping};

    // construct h_int and rename operators
    return rename_op(h_int_kanamori(Umat, Upmat, J_hund, n_orb, tau_names, params), op_map);
  }

  operators::many_body_operator make_density_density(const std::vector<std::string> &tau_names, const std::vector<long> &dim_gamma, double U_int,
                                                     double U_prime, double J_hund) {
    // make kanamori without spin-flip and pair-hopping
    return make_kanamori(tau_names, dim_gamma, U_int, U_prime, J_hund, false, false);
  }

  operators::many_body_operator make_slater(std::vector<std::string> const &tau_names, std::vector<long> const &dim_gamma, double U_int,
                                            double J_hund, nda::matrix<dcomplex> const &spherical_to_dft,
                                            std::optional<nda::matrix<dcomplex>> const &dft_to_local) {

    // compute number of orbitals
    long n_orb = stdr::fold_left(dim_gamma, 0, std::plus<>());

    // construct operator mapping
    auto op_map = make_op_map(tau_names, dim_gamma);

    // construct U matrix and rotate to local basis
    long l         = (n_orb - 1) / 2;
    auto transform = (dft_to_local) ? spherical_to_dft * dft_to_local.value() : spherical_to_dft;
    auto U_matrix  = U_matrix_slater_local(l, transform, U_int, J_hund);

    // construct h_int and rename operators
    return rename_op(h_int_slater(U_matrix, n_orb, tau_names), op_map);
  }
} // namespace triqs