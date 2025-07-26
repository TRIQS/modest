#pragma once
#include <triqs/lattice/tb_hamiltonian.hpp>
#include "downfolding.hpp"
#include <stdexcept>
#include "loaders.hpp"

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
  one_body_elements_tb one_body_elements_from_wannier90(std::string const &wannier_file_path,
                                                        spin_kind_e const &spin_kind,
                                                        std::vector<atomic_shell_t> const &atomic_shells);

  one_body_elements_tb one_body_elements_from_wannier90(std::string const &wannier_file_path_up,
                                                        std::string const &wannier_file_path_dn,
                                                        spin_kind_e const &spin_kind,
                                                        std::vector<atomic_shell_t> const &atomic_shells);

  one_body_elements_tb make_obe_from_tb(std::vector<tb_hamiltonian> const &tb_H_sigma, spin_kind_e const &spin_kind,
                                        std::vector<atomic_shell_t> const &atomic_shells);
} // namespace triqs::modest
