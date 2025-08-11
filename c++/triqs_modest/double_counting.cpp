// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include <utility>
#include "./double_counting.hpp"
#include "utils/enumerate_slice.hpp"

using nda::range;

namespace triqs::modest {

  //------------------------------------------------------------------------------------
  /// double counting formulas parameterized by density, U, and J
  std::pair<double, double> dc_formulas(std::string const method, double const N_tot, double const N_sigma, long const n_orb, double const U,
                                        double const J) {
    double Mag     = N_sigma - (N_tot - N_sigma);
    double L_orbit = 0.5 * (double(n_orb) - 1.0);
    if (method == "cFLL") {
      return {(U * (N_tot - 0.5) - 0.5 * J * (N_tot - 1.0)), 0.5 * U * N_tot * (N_tot - 1.0) - 0.5 * J * N_tot * (0.5 * N_tot - 1.0)};
    } else if (method == "sFLL") {
      return {(U * (N_tot - 0.5) - J * (N_sigma - 0.5)),
              0.5 * U * N_tot * (N_tot - 1.0) - 0.5 * J * N_tot * (N_tot * 0.5 - 1.0) - 0.25 * J * Mag * Mag};
    } else if (method == "cAMF") {
      double C = (U + 2.0 * J * L_orbit) / (2 * L_orbit + 1.0);
      return {
         (U * N_tot - 0.5 * N_tot * C),       // DC
         (0.5 * U - 0.25 * C) * N_tot * N_tot //Energy
      };
    } else if (method == "sAMF") {
      return {(U * N_tot - ((U + 2.0 * L_orbit * J) / (2.0 * L_orbit + 1.0)) * N_sigma),                         // DC
              0.5 * U * N_tot * N_tot - 0.25 * ((U + 2.0 * L_orbit * J) / (2.0 * L_orbit + 1.0)) * N_tot * N_tot // Energy
                 - 0.25 * ((U + 2.0 * L_orbit * J) / (2.0 * L_orbit + 1.0)) * Mag * Mag};
    } else if (method == "cHeld") {
      // double C     = double(n_orb) - 1.0;
      // double Umean = (U + ((U - 2.0 * J) + (U - 3.0 * J)) * C) / (2.0 * double(n_orb) - 1);
      double U_mean = (U + (n_orb - 1) * (U - 2 * J) + (n_orb - 1) * (U - 3 * J)) / (2 * n_orb - 1);
      return {
         (U_mean * (N_tot - 0.5)),            //DC
         0.5 * U_mean * N_tot * (N_tot - 1.0) //Energy
      };
    } else {
      throw std::runtime_error("Not implemented! Complain to the developers");
    }
  }
  //------------------------------------------------------------------------------------
  /**
 * @brief compute double counting correction (H_DC) for a dc_type (method) from the G𝓒 and interaction parameters.
 * 
 * @param GC Green's function in C space
 * @param U_int Coulomb interaction parameter
 * @param J_hund Hund's coupling interaction parameter
 * @param method dc_formula (sFLL, cFLL, sAMF, cAMF, cHeld)
 * @return H_DC nda::array<dc_t, 2> 
 */
  std::pair<nda::array<nda::matrix<double>, 2>, nda::matrix<double>>
  //double_counting_from_gf(block2_gf<imfreq, matrix_valued> const &GC, double U_int, double J_hund,
  double_counting(nda::array<nda::matrix<dcomplex>, 2> const &density_matrix, double U_int, double J_hund, std::string const method) {

    auto [n_atoms, n_sigma] = density_matrix.shape();
    auto Sigma_DC           = nda::array<nda::matrix<double>, 2>(n_atoms, n_sigma);
    auto E_DC               = nda::matrix<double>(n_atoms, n_sigma);

    //for (auto [atom, sigma] : indices(GC)) {
    for (auto atom : nda::range(n_atoms))
      for (auto sigma : nda::range(n_sigma)) {
        //auto n_orb            = GC(atom, sigma).target_shape()[0];
        auto n_orb            = density_matrix(atom, sigma).shape()[0];
        Sigma_DC(atom, sigma) = nda::eye<double>(n_orb);
      }

    for (auto atom : range(n_atoms)) {
      auto Nsigma = nda::zeros<double>(n_sigma);
      //for (auto sigma : range(n_sigma)) { Nsigma(sigma) += real(trace(density(GC(atom, sigma)))); }
      for (auto sigma : range(n_sigma)) { Nsigma(sigma) += real(trace(density_matrix(atom, sigma))); }
      auto Ntot = stdr::fold_left(Nsigma, 0, std::plus<>());
      for (auto sigma : range(n_sigma)) {
        if (n_sigma == 2) {
          Nsigma(sigma) = Ntot / n_sigma;
        } else if (n_sigma == 1) {
          Nsigma(sigma) = 0.5 * Ntot;
        }
      }

      for (auto sigma : range(n_sigma)) {
        auto n_orb           = density_matrix(atom, sigma).shape()[0];
        n_orb                = (n_sigma == 2) ? n_orb : static_cast<long>(n_orb / 2);
        auto [DC_val, E_val] = dc_formulas(method, Ntot, Nsigma(sigma), n_orb, U_int, J_hund);
        Sigma_DC(atom, sigma) *= DC_val;
        E_DC(atom, sigma) = E_val;
      }
    }
    return {Sigma_DC, E_DC};
  }

  std::pair<double, nda::vector<double>> get_total_density(nda::array<nda::matrix<dcomplex>, 2> const &density_matrix) {
    auto [n_blocks, n_sigma] = density_matrix.shape();
    auto N_per_sigma         = nda::zeros<double>(n_sigma);
    for (auto sigma : range(n_sigma)) {
      for (auto bl : range(n_blocks)) { N_per_sigma(sigma) += real(trace(density_matrix(bl, sigma))); }
    }
    auto N_total = stdr::fold_left(N_per_sigma, 0, std::plus<>());
    for (auto sigma : range(n_sigma)) {
      if (n_sigma == 2) {
        N_per_sigma(sigma) = N_total / n_sigma;
      } else if (n_sigma == 1) {
        N_per_sigma(sigma) = 0.5 * N_total;
      }
    }
    return {N_total, N_per_sigma};
  }

  nda::array<nda::matrix<double>, 2> double_counting_sigma_dc(nda::array<nda::matrix<dcomplex>, 2> const &density_matrix, double U_int, double J_hund,
                                                              std::string const method) {

    auto [N_total, N_per_sigma] = get_total_density(density_matrix);

    auto [n_blocks, n_sigma] = density_matrix.shape();

    auto n_orb =
       stdr::fold_left(density_matrix(r_all, 0) | stdv::transform([](auto x) { return x.shape()[0]; }) | tl::to<std::vector>(), 0, std::plus<>());
    n_orb = (n_sigma == 2) ? n_orb : static_cast<long>(n_orb / 2);

    auto Sigma_DC = nda::array<nda::matrix<double>, 2>(n_blocks, n_sigma);
    for (auto bl : range(n_blocks))
      for (auto sigma : range(n_sigma))
        Sigma_DC(bl, sigma) = std::get<0>(dc_formulas(method, N_total, N_per_sigma(sigma), n_orb, U_int, J_hund))
           * nda::eye<double>(density_matrix(bl, sigma).shape()[0]);
    return Sigma_DC;
  }

  nda::matrix<double> double_counting_energy_dc(nda::array<nda::matrix<dcomplex>, 2> const &density_matrix, double U_int, double J_hund,
                                                std::string const method) {

    auto [N_total, N_per_sigma] = get_total_density(density_matrix);

    auto [n_blocks, n_sigma] = density_matrix.shape();
    auto n_orb =
       stdr::fold_left(density_matrix(r_all, 0) | stdv::transform([](auto x) { return x.shape()[0]; }) | tl::to<std::vector>(), 0, std::plus<>());
    n_orb = (n_sigma == 2) ? n_orb : static_cast<long>(n_orb / 2);

    auto E_DC = nda::matrix<double>(n_blocks, n_sigma);
    for (auto bl : range(n_blocks))
      for (auto sigma : range(n_sigma)) E_DC(bl, sigma) = std::get<1>(dc_formulas(method, N_total, N_per_sigma(sigma), n_orb, U_int, J_hund));
    return E_DC;
  }

  //------------------------------------------------------------------------------------
  dc_solver::dc_solver(long n_sigma, std::string method, double U_int, double J_hund)
     : n_sigma{n_sigma}, method{std::move(method)}, U_int{U_int}, J_hund{J_hund} {};

  nda::array<nda::matrix<double>, 2> dc_solver::get_density_matrix_from_gf(block_gf<imfreq, matrix_valued> const &gimp) {
    auto n_blocks       = gimp.size() / n_sigma;
    auto density_matrix = nda::array<nda::matrix<double>, 2>(n_blocks, n_sigma);
    for (auto bl : range(n_blocks))
      for (auto sigma : range(n_sigma)) density_matrix(bl, sigma) = real(density(gimp[bl + sigma * n_blocks]));
    return density_matrix;
  }

  std::vector<nda::matrix<dcomplex>> dc_solver::dc_self_energy(block_gf<imfreq, matrix_valued> const &gimp) {
    auto Sigma_DC       = std::vector<nda::matrix<dcomplex>>(gimp.size());
    auto density_matrix = get_density_matrix_from_gf(gimp);
    auto n_blocks       = density_matrix.extent(0);
    auto Sigma_dc_mat   = double_counting_sigma_dc(density_matrix, U_int, J_hund, method);
    for (auto bl : range(n_blocks))
      for (auto sigma : range(n_sigma)) Sigma_DC[bl + sigma * n_blocks] = Sigma_dc_mat(bl, sigma);
    return Sigma_DC;
  }

  nda::matrix<double> dc_solver::dc_energy(block_gf<imfreq, matrix_valued> const &gimp) {
    return double_counting_energy_dc(get_density_matrix_from_gf(gimp), U_int, J_hund, method);
  }

} // namespace triqs::modest
