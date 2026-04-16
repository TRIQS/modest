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
  // Compute total density N_total and per-spin densities N_per_sigma from a density matrix.
  //
  // The spin_kind determines how per-spin densities are treated:
  //   - Polarized:    N_per_sigma retains the actual per-spin traces from the density matrix.
  //                   This allows spin-dependent DC formulas (sFLL, sAMF) to produce
  //                   different corrections for each spin channel.
  //   - NonPolarized: N_per_sigma is overridden to N_total/2 for both spins, making
  //                   spin-dependent formulas reduce to their charge-only counterparts.
  //   - NonColinear:  Single spin channel (n_sig=1); N_per_sigma is set to N_total/2
  //                   so that dc_formulas sees the per-spin density as half the total.
  static std::pair<double, nda::vector<double>> get_total_density(spin_kind_e spin_kind,
                                                                  nda::array<nda::matrix<dcomplex>, 2> const &density_matrix) {
    auto [n_blocks, n_sig] = density_matrix.shape();
    auto N_per_sigma       = nda::zeros<double>(n_sig);
    for (auto sigma : range(n_sig))
      for (auto bl : range(n_blocks)) N_per_sigma(sigma) += real(trace(density_matrix(bl, sigma)));
    auto N_total = stdr::fold_left(N_per_sigma, 0.0, std::plus<>());

    switch (spin_kind) {
      case spin_kind_e::Polarized:
        break;
      case spin_kind_e::NonPolarized:
        for (auto sigma : range(n_sig)) N_per_sigma(sigma) = 0.5 * N_total;
        break;
      case spin_kind_e::NonColinear:
        N_per_sigma(0) = 0.5 * N_total;
        break;
    }
    return {N_total, N_per_sigma};
  }

  //------------------------------------------------------------------------------------
  dc_solver::dc_solver(spin_kind_e spin_kind, std::string method, double U_int, double J_hund)
     : _spin_kind{spin_kind}, n_sigma{n_sigma_from_spin_kind(spin_kind)}, method{std::move(method)}, U_int{U_int}, J_hund{J_hund} {};

  //------------------------------------------------------------------------------------
  nda::array<nda::matrix<dcomplex>, 2> dc_solver::get_density_matrix_from_gf(block_gf<imfreq, matrix_valued> const &gimp) {
    auto n_blocks       = gimp.size() / n_sigma;
    auto density_matrix = nda::array<nda::matrix<dcomplex>, 2>(n_blocks, n_sigma);
    for (auto bl : range(n_blocks))
      for (auto sigma : range(n_sigma)) density_matrix(bl, sigma) = density(gimp[bl + sigma * n_blocks]);
    return density_matrix;
  }

  //------------------------------------------------------------------------------------
  std::vector<nda::matrix<dcomplex>> dc_solver::dc_self_energy(nda::array<nda::matrix<dcomplex>, 2> const &density_matrix) {
    auto [N_total, N_per_sigma] = get_total_density(_spin_kind, density_matrix);
    auto [n_blocks, n_sig]      = density_matrix.shape();

    long n_orb = 0;
    for (auto bl : range(n_blocks)) n_orb += density_matrix(bl, 0).shape()[0];
    if (_spin_kind == spin_kind_e::NonColinear) n_orb /= 2;

    auto Sigma_DC = std::vector<nda::matrix<dcomplex>>(n_blocks * n_sig);
    for (auto bl : range(n_blocks))
      for (auto sigma : range(n_sig))
        Sigma_DC[bl + sigma * n_blocks] = std::get<0>(dc_formulas(method, N_total, N_per_sigma(sigma), n_orb, U_int, J_hund))
           * nda::eye<dcomplex>(density_matrix(bl, sigma).shape()[0]);
    return Sigma_DC;
  }

  //------------------------------------------------------------------------------------
  double dc_solver::dc_energy(nda::array<nda::matrix<dcomplex>, 2> const &density_matrix) {
    auto [N_total, N_per_sigma] = get_total_density(_spin_kind, density_matrix);
    auto [n_blocks, n_sig]      = density_matrix.shape();

    long n_orb = 0;
    for (auto bl : range(n_blocks)) n_orb += density_matrix(bl, 0).shape()[0];
    if (_spin_kind == spin_kind_e::NonColinear) n_orb /= 2;

    auto E_DC = std::vector<double>(n_sig);
    for (auto sigma : range(n_sig)) E_DC[sigma] = std::get<1>(dc_formulas(method, N_total, N_per_sigma(sigma), n_orb, U_int, J_hund));

    // E_DC must be independent of sigma (Mag appears only as Mag^2 in all current formulas).
    // Guard against future formulas where this assumption may not hold.
    if (n_sig > 1)
      for (auto sigma : range(1, n_sig))
        if (std::abs(E_DC[sigma] - E_DC[0]) > 1.e-10)
          throw std::runtime_error("dc_energy: E_DC depends on spin channel");

    return E_DC[0];
  }

  //------------------------------------------------------------------------------------
  std::vector<nda::matrix<dcomplex>> dc_solver::dc_self_energy(block_gf<imfreq, matrix_valued> const &gimp) {
    return dc_self_energy(get_density_matrix_from_gf(gimp));
  }

  //------------------------------------------------------------------------------------
  double dc_solver::dc_energy(block_gf<imfreq, matrix_valued> const &gimp) {
    return dc_energy(get_density_matrix_from_gf(gimp));
  }

} // namespace triqs::modest
