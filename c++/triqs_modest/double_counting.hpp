#pragma once
#include <nda/nda.hpp>
#include "utils/defs.hpp"
#include "triqs/gfs.hpp"

using namespace triqs::gfs;

namespace triqs::modest {

  /// double counting formulas parameterized by density, U, and J
  std::pair<double, double> dc_formulas(std::string const method, double const N_tot, double const N_sigma,
                                        long const n_orb, double const U, double const J);
  /**
 * @brief compute double counting correction (H_DC) for a dc_type (method) from the G𝓒 and interaction parameters.
 * 
 * @param GC Green's function in C space
 * @param U_int Coulomb interaction parameter
 * @param J_hund Hund's coupling interaction parameter
 * @param method dc_formula (sFLL, cFLL, sAMF, cAMF, cHeld)
 * @return H_DC nda::array<dc_t, 2> 
 */
  std::pair<nda::array<nda::matrix<double>, 2>, nda::array<double, 2>>
  double_counting(nda::array<nda::matrix<dcomplex>, 2> const &density_matrix, double U_int, double J_hund,
                  std::string const method);

  class dc_solver {
    public:
    std::vector<std::string> spin_names;
    std::string method;
    double U_int;
    double J_hund;

    dc_solver(std::vector<std::string> spin_names, std::string method, double U_int, double J_hund);

    block_gf<imfreq, matrix_valued> dc_self_energy(block_gf<imfreq, matrix_valued> const &gimp);
    nda::array<double, 2> dc_energy(block_gf<imfreq, matrix_valued> const &gimp);

    private:
    nda::array<nda::matrix<double>, 2> get_density_matrix_from_gf(block_gf<imfreq, matrix_valued> const &gimp);
  };

} // namespace triqs::modest