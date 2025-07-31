#pragma once
#include "./downfolding.hpp"
#include <triqs/utility/streams.hpp>
#include "utils/nda_supp.hpp"

namespace triqs::modest {

  /**
   * @ingroup obe_factories
   * @brief Create a one-body elements object with orthonormalized projectors.
   *
   * @details Using the data from the "dft_input" group, the band_dispersion, local_space, 
   * downfolding_projector, and optional ibz_symmetry_ops are prepared to create a one-body 
   * elements object. This object is intended to be used in DMFT calculations.
   * 
   * @param filename Hdf5 file from dft_tools converter.
   * @param threshold Off-diagonal threshold for discovery of symmetries.
   * @param diagonalize_hloc Diagonalize the local non-interacting hamiltonian.
   * @param verbosity Log steps of creating the one-body elements.
   * @return The total electron density and a one-body elements.
   */
  std::pair<double, one_body_elements_on_grid> one_body_elements_from_dft_converter(std::string const &filename, double threshold = 1.e-5,
                                                                                    bool diagonalize_hloc = false);
  // bool verbosity        = true);
  /**
 * @ingroup obe_factories
 * @brief Create a one-body elements object with the Θ projectors.
 * 
 * @details Using the data from the "dft_parproj_input" group, the local_space, downfolding_projectors, 
 * and optional ibz_symmetry_ops are prepared to create a one-body elements object.  This object is 
 * intended to be used for post-processing the atom- and orbitally-resolved k-summed spectral functions.
 * 
 * @param filename Hdf5 file from DFTtools converter with dft_parproj_input group.
 * @param obe One-body elements that was used in the DMFT calculation.
 * @return One-body elements using the Θ projectors.
 */
  one_body_elements_on_grid one_body_elements_with_theta_projectors(std::string const &filename, one_body_elements_on_grid const &obe);
  /**
 * @ingroup obe_factories
 * @brief Create a one-body elements object along specific k-grid. 
 *
 * @details Using the data from the "dft_bands_input" group, the band_disperion and downfolding_projector
 * are prepared to create one-body elements object. This object is intended to be used for post-processing
 * the momentum-resolved spectral function.
 * 
 * @param filename  Hdf5 file from the dft_tools converter.
 * @param obe One-body elements that was ued in the DMFT calculation.
 * @return One-body elements along high-symmetry k-path.
 */
  one_body_elements_on_grid one_body_elements_on_high_symmetry_path(std::string const &filename, one_body_elements_on_grid const &obe);

  /** 
  * @brief Find symmetries of the R = 0 component of a Hamiltonian to determine a GF block structure. 
  * 
  * @details Discovers (approximate) irreducible symmetries for Green's function from the non-interacting part of the local
  * Hamiltonian (H0 = ∑k P(k) Hνν' P†(k) ), which represents the block structure of the TRIQS Gf.
  * 
  * @param Hloc0 the R = 0 part of the Hamiltonian as a vector of [n_atoms, n_sigma] 
  * @param atomic_shells the list of atomic shells used to index Hloc 
  * @param block_threshold the threshold of accuracy at which a symmetry is considered found
  * @param diagonalize_hloc whether or not to diagonalize hloc
  * @return decomposition, U_rotation describing the block structure of GF based on Hloc
  */
  std::pair<nda::array<std::vector<long>, 2>, nda::array<nda::matrix<dcomplex>, 2>>
  discover_symmetries(nda::array<nda::matrix<dcomplex>, 2> const &Hloc0, std::vector<atomic_shell_t> const &atomic_shells, double block_threshold,
                      bool diagonalize_hloc);

} // namespace triqs::modest