#pragma once
#include <triqs/lattice/tb_hamiltonian.hpp>
#include <stdexcept>
#include "loaders.hpp"
#include "triqs/lattice/bz_integrators.hpp"
#include "triqs/lattice/gloc.hpp"
#include "triqs/lattice/superlattice.hpp"
#include "triqs_modest/utils/gf_supp.hpp"

namespace triqs::modest {

  using triqs::lattice::superlattice;

  struct one_body_elements {
    local_space C_space;
    std::vector<tb_hamiltonian> H; // positibility to have two spin channels for spin pol
    //downfolding_projector P;
    //C2PY_IGNORE std::optional<ibz_symmetry_ops> ibz_symm_ops = {}; //< IBZ symmetrizer after a k-sum
  };

  /** Construct a obe_tb from Wannier90 in the case of a single spin index 
   * @param wannier_file_path string to Wannier90 files, including the prefix 
   * @param spin_kind enum telling us the spintype 
   * @param shells list of atomic shells input by the user 
   */
  one_body_elements one_body_elements_from_wannier90(std::string const &wannier_file_path, spin_kind_e const &spin_kind,
                                                     std::vector<atomic_shell_t> atomic_shells);

  one_body_elements one_body_elements_from_wannier90(std::string const &wannier_file_path_up, std::string const &wannier_file_path_dn,
                                                     spin_kind_e const &spin_kind, std::vector<atomic_shell_t> atomic_shells);

  one_body_elements make_obe_from_tb(std::vector<tb_hamiltonian> const tb_H_sigma, spin_kind_e const &spin_kind,
                                     std::vector<atomic_shell_t> atomic_shells);

  /// Folding with superlattice
  one_body_elements fold(superlattice const &sl, one_body_elements const &obe);

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
  block2_gf<Mesh, matrix_valued> gloc(one_body_elements const &obe, double mu, block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                                      triqs::lattice::bz_int_options const &opt) {

    // Need a sigma Hartree
    auto &mesh = Sigma_dynamic(0, 0).mesh();

    // CHECK Does this account for the possiblity of different norbitals in up and down?
    auto n_sigma     = Sigma_dynamic.size2();
    auto gloc_result = make_block2_gf(mesh, obe.C_space.Gc_block_shape());

    if (n_sigma != obe.H.size()) { throw std::runtime_error("Mismatch between the spin channels in Sigma and spin channels in Hamiltonian."); }

    // Embedding decomposition from structure of Sigma -- provides a list of block names
    auto embedding_decomp = get_struct(Sigma_dynamic).dims(r_all, 0) | tl::to<std::vector>();

    // REFACTOR norbtials in tb_obe
    auto Sigma_big_space = gfs::gf(mesh, {obe.H[0].n_orbitals(), obe.H[0].n_orbitals()}); //
    for (auto sigma : range(n_sigma)) {                                                   // spin index

      for (auto &&[iblock, R] : enumerated_sub_slices(embedding_decomp)) {
        Sigma_big_space.data()(r_all, R, R) = Sigma_dynamic(iblock, sigma).data();
      }

      // Call the TRIQS version of this function
      gloc_result(0, sigma) = gloc(obe.H[sigma], mu, Sigma_big_space, opt);
    }
    return gloc_result;
  }
} // namespace triqs::modest
