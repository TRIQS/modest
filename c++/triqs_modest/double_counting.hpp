// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <nda/nda.hpp>
#include "utils/defs.hpp"
#include "triqs/gfs.hpp"

using namespace triqs::gfs;

namespace triqs::modest {

  /**
   * @ingroup double_counting
   * @brief Double counting formulas.
   * 
   * @param method Double counting formula to use.
   * @param N_tot Total density.
   * @param N_sigma Total density per spin.
   * @param n_orb Number of orbitals.
   * @param U Hubbard \f$ U \f$.
   * @param J Hund's coupling \f$ J \f$.
   * @return \f$ \Sigma_{DC} \f$ and \f$ E_{DC} \f$.
   */
  std::pair<double, double> dc_formulas(std::string const method, double const N_tot, double const N_sigma, long const n_orb, double const U,
                                        double const J);
  /**
   * @brief Compute double counting correction for a DC type (method) from the density matrix of a Green's function.
   * 
   * @param density_matrix Density matrix [orbital, spin] indices.
   * @param U_int Coulomb interaction parameter.
   * @param J_hund Hund's coupling interaction parameter.
   * @param method DC formula (`sFLL`, `cFLL`, `sAMF`, `cAMF`, `cHeld`).
   * @return A pair of \f$ \Sigma_{DC} \f$ and \f$ E_{DC} \f$.
   */
  std::pair<nda::array<nda::matrix<double>, 2>, nda::matrix<double>> double_counting(nda::array<nda::matrix<dcomplex>, 2> const &density_matrix,
                                                                                     double U_int, double J_hund, std::string const method);

  /**
   * @ingroup double_counting
   * @brief Double counting "solver".
   * 
   * @details It implements the double counting correction for DFT+DMFT, which is a phenomenlogical introduced double 
   * counting the interactions already taken into account at the mean-field level within DFT. This class implements 
   * several double counting formulas (all of which are functions of the density) relevant for different scenarios.
   */
  class dc_solver {

    private:
    // number of spins
    long n_sigma;
    // double counting method to use
    std::string method;
    // U term used in the double counting formulas
    double U_int;
    // J term used in the double counting formulas
    double J_hund;

    /**
    * @brief Get the density matrix from gf object
    * 
    * @param gimp The impurity Green's function which is used to calculate the orbital-resolved density matrices to evaluate the double counting formula.
    * @return Density matrix of Green's function in block_matrix format.
    */
    nda::array<nda::matrix<double>, 2> get_density_matrix_from_gf(block_gf<imfreq, matrix_valued> const &gimp);

    public:
    /**
     * @brief Construct a double counting "solver".
     * 
     * @param n_sigma Dimension of the \f$ \sigma \f$ index.
     * @param method Double counting formula (method) to call (options: `cFLL`, `sFLL`, `cAMF`, `sAMF`, `cHeld`).
     * @param U_int Hubbard \f$ U \f$ to use in the DC formula.
     * @param J_hund Hund's coupling \f$ J \f$ to use in the DC formula.
     */
    dc_solver(long n_sigma, std::string method, double U_int, double J_hund);

    /**
     * @brief Compute the double-counting self-energy.
     * 
     * @param gimp The impurity Green's function which is used to calculate the orbital-resolved density matrices to 
     * evaluate the double counting formula.
     * @return Double counting self-energy term.
     */
    std::vector<nda::matrix<dcomplex>> dc_self_energy(block_gf<imfreq, matrix_valued> const &gimp);

    /**
     * @brief Compute the double counting correction to the energy.
     * 
     * @param gimp The impurity Green's function which is used to calculate the orbital-resolved density matrices to 
     * evaluate the double counting formula.
     * @return Double counting energy term.
     */
    nda::matrix<double> dc_energy(block_gf<imfreq, matrix_valued> const &gimp);
  };

} // namespace triqs::modest