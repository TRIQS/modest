#pragma once
#include <nda/nda.hpp>
#include "utils/defs.hpp"
#include "triqs/gfs.hpp"

using namespace triqs::gfs;

namespace triqs::modest {

  /**
 * @ingroup dc_formula
 * @brief Double counting formulas.
 * 
 * @param method the double counting formula to use.
 * @param N_tot the total density
 * @param N_sigma the total density per spin
 * @param n_orb  the number of orbitals
 * @param U the Hubbard U
 * @param J the Hund's coupling J
 * @return std::pair<double, double> 
 */
  std::pair<double, double> dc_formulas(std::string const method, double const N_tot, double const N_sigma, long const n_orb, double const U,
                                        double const J);
  /**
 * @brief compute double counting correction (H_DC) for a dc_type (method) from the G𝓒 and interaction parameters.
 * 
 * @param GC Green's function in C space
 * @param U_int Coulomb interaction parameter
 * @param J_hund Hund's coupling interaction parameter
 * @param method dc_formula (sFLL, cFLL, sAMF, cAMF, cHeld)
 * @return H_DC nda::array<dc_t, 2> 
 */
  std::pair<nda::array<nda::matrix<double>, 2>, nda::array<double, 2>> double_counting(nda::array<nda::matrix<dcomplex>, 2> const &density_matrix,
                                                                                       double U_int, double J_hund, std::string const method);

  /**
 * @ingroup double_counting
 * @brief Double counting "solver" implements the double counting correction for DFT+DMFT, which is a phenomenlogical introduced
 * double counting the interactions already taken into account at the mean-field level within DFT. This class implements several
 * double counting formulas (all of which are functions of the density) relevant for different scenarios.
 *
 */
  class dc_solver {
    private:
    std::vector<std::string> spin_names;
    std::string method;
    double U_int;
    double J_hund;

    /**
    * @brief Get the density matrix from gf object
    * 
    * @param gimp The impurity Green's function which is used to calculate the orbital-resolved density matrices to evaluate the double counting formula.
    * @return nda::array<nda::matrix<double>, 2> 
    */
    nda::array<nda::matrix<double>, 2> get_density_matrix_from_gf(block_gf<imfreq, matrix_valued> const &gimp);

    public:
    /**
     * @ingroup double_counting
     * @brief Construct a double counting "solver".
     * 
     * @param spin_names The names of the spin indices 
     * @param method The double counting formula (method) to call (options: cFLL, sFLL, cAMF, sAMF, cHeld).
     * @param U_int  The Hubbard U to use in the DC formula.
     * @param J_hund The Hund's coupling J to use in the DC formula.
     */
    dc_solver(std::vector<std::string> spin_names, std::string method, double U_int, double J_hund);

    /**
    * @ingroup double_counting
    * @brief Compute the double-counting self-energy
    * 
    * @param gimp The impurity Green's function which is used to calculate the orbital-resolved density matrices to evaluate the double counting formula.
    * @return block_gf<imfreq, matrix_valued> 
    */
    block_gf<imfreq, matrix_valued> dc_self_energy(block_gf<imfreq, matrix_valued> const &gimp);
    /**
    * @ingroup double_counting
     * @brief Compute the double counting correction to the energy.
     * 
     * @param gimp The impurity Green's function which is used to calculate the orbital-resolved density matrices to evaluate the double counting formula.
     * @return nda::array<double, 2> 
     */
    nda::array<double, 2> dc_energy(block_gf<imfreq, matrix_valued> const &gimp);
  };

} // namespace triqs::modest