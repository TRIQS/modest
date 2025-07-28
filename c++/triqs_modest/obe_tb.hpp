#pragma once
#include <triqs/lattice/tb_hamiltonian.hpp>
#include <stdexcept>
#include "loaders.hpp"
#include "triqs/utility/integration/bz_integrators.hpp"
#include "triqs/lattice/gloc.hpp"
#include "triqs_modest/utils/gf_supp.hpp"

namespace triqs::modest {

  struct one_body_elements_tb {
    std::vector<tb_hamiltonian> H; // positibility to have two spin channels for spin pol
    local_space C_space;
    //downfolding_projector P;
    //C2PY_IGNORE std::optional<ibz_symmetry_ops> ibz_symm_ops = {}; //< IBZ symmetrizer after a k-sum
  };

  /** Construct a obe_tb from Wannier90 in the case of a single spin index 
   * @param wannier_file_path string to Wannier90 files, including the prefix 
   * @param spin_kind enum telling us the spintype 
   * @param shells list of atomic shells input by the user 
   */
  one_body_elements_tb one_body_elements_from_wannier90(std::string const &wannier_file_path, spin_kind_e const &spin_kind,
                                                        std::vector<atomic_shell_t> const &atomic_shells);

  one_body_elements_tb one_body_elements_from_wannier90(std::string const &wannier_file_path_up, std::string const &wannier_file_path_dn,
                                                        spin_kind_e const &spin_kind, std::vector<atomic_shell_t> const &atomic_shells);

  one_body_elements_tb make_obe_from_tb(std::vector<tb_hamiltonian> const tb_H_sigma, spin_kind_e const &spin_kind,
                                        std::vector<atomic_shell_t> const atomic_shells);

  /** @brief Compute local Green's function from a one_body_elements_tb object.
    *    
    * @param one_body_elements_tb A one_body_elements object containing the tb_hamiltonian
    * @param mu Chemical potential 
    * @param Sigma_embed Self energy in the embedded basis? TODO I don't think this was relevant 
    * @param Sigma_DC ? TODO JC would like this to be optional 
    * @param bz_int_options Option of the BZ integration
    * @param d_H ? for magnetic fields etc? 
    * @return gloc[alpha][sigma] = gf on the mesh, as a vector of length alpha, storing block gfs dim sigma 
    */
  template <typename Mesh>
  block2_gf<Mesh, matrix_valued> gloc(one_body_elements_tb const &obe_tb, double mu, block2_gf<Mesh, matrix_valued> const &Sigma,
                                      triqs::bz_int::bz_int_options const &opt) {

    block2_gf<Mesh, matrix_valued> result = triqs::gfs::make_block2_gf(Sigma(0, 0).mesh(), triqs::gfs::get_struct(Sigma));
    auto n_alpha                          = result.size1();
    auto n_sigma                          = result.size2();

    if(n_sigma != obe_tb.H.size()) {
      throw std::runtime_error("Mismatch between the spin channels in Sigma and spin channels in Hamiltonian.");
    }

    for (auto alpha : range(n_alpha)) {   // block index
      for (auto sigma : range(n_sigma)) { // spin index
        result(alpha, sigma) = gloc(obe_tb.H[sigma], mu, Sigma(alpha, sigma), opt);
      }
    }
    return result;
  }
} // namespace triqs::modest
