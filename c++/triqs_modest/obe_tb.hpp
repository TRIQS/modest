#pragma once
#include <triqs/lattice/tb_hamiltonian.hpp>
#include <stdexcept>
#include <triqs/mesh/imfreq.hpp>
#include "loaders.hpp"
#include "triqs/lattice/bz_integrators.hpp"
#include "triqs/lattice/gloc.hpp"
#include "triqs_modest/utils/gf_supp.hpp"
#include <nda/nda.hpp>
#include <triqs/gfs/functions/density.hpp>

namespace nda::temp {
  CLEF_MAKE_FNT_LAZY(trace);
}

namespace triqs::modest {

  struct one_body_elements {
    local_space C_space;
    C2PY_IGNORE std::vector<tb_hamiltonian> H; // positibility to have two spin channels for spin pol
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

  // TODO docstring
  nda::array<nda::matrix<dcomplex>, 2> Hloc(std::vector<tb_hamiltonian> const &H_sigma, std::vector<atomic_shell_t> const &atomic_shells);
  nda::array<nda::matrix<dcomplex>, 2> Hloc(one_body_elements const &obe);
  nda::array<nda::matrix<dcomplex>, 2> impurity_levels(one_body_elements_on_grid const &obe);

  C2PY_IGNORE one_body_elements make_obe_from_tb(std::vector<tb_hamiltonian> const tb_H_sigma, spin_kind_e const &spin_kind,
                                                 std::vector<atomic_shell_t> atomic_shells);

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
                                      nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree, triqs::lattice::bz_int_options const &opt) {

    // Need a sigma Hartree
    auto &mesh       = Sigma_dynamic(0, 0).mesh();
    auto n_sigma     = Sigma_dynamic.size2();
    auto gloc_result = make_block2_gf(mesh, obe.C_space.Gc_block_shape());
  // TODO also check safety of orbital space sizes...
  if (n_sigma != Sigma_hartree.shape(0)){ throw std::runtime_error("Mismatch between the spin channels in Sigma_Static and Sigma_Dynamic"); }
  if (n_sigma != obe.H.size()) { throw std::runtime_error("Mismatch between the spin channels in Sigma and spin channels in Hamiltonian."); }
  
    // Embedding decomposition from structure of Sigma -- provides a list of block names
    auto embedding_decomp = get_struct(Sigma_dynamic).dims(r_all, 0) | tl::to<std::vector>();

    for (auto sigma : range(n_sigma)) {

      // spin index
      auto Sigma_full_space = gfs::gf(mesh, {obe.H[sigma].n_orbitals(), obe.H[sigma].n_orbitals()}); //
      //for (auto &&[alpha, R] : enumerated_sub_slices(embedding_decomp)) {
      //  auto Sigma = nda::matrix<dcomplex>{Sigma_dynamic(alpha, sigma)[om] + Sigma_hartree(alpha, sigma)};
      //std::cout << Sigma_dynamic(0, sigma).data()(r_all).shape() << std::endl;
      //std::cout << "Sigma_hartree " << Sigma_hartree(0, sigma) << std::endl;
      // FIXME how else can I add these two things ?
      for (auto &&[block, R] : enumerated_sub_slices(embedding_decomp)) { 
        for(auto [n,w] : enumerate(mesh)) {
          Sigma_full_space.data()(n, R, R) = Sigma_dynamic(block, sigma).data()(n, r_all, r_all) + Sigma_hartree(block, sigma); 
        }
      }
      // Call the TRIQS version of this function
      gloc_result(0, sigma) = gloc(obe.H[sigma], mu, Sigma_full_space, opt);
    }
    return gloc_result;
  }

  template <typename Mesh>
  double density(one_body_elements const &obe, double mu, block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree, triqs::lattice::bz_int_options const &opt) {

    auto const &mesh = Sigma_dynamic(0, 0).mesh();
    auto beta        = mesh.beta();

    //auto n_blocks = Sigma_dynamic.size1();
    auto n_sigma = Sigma_dynamic.size2();

    double n  = 0;
    auto Gloc = gloc(obe, mu, Sigma_dynamic, Sigma_hartree, opt); // returns block2gf (1, nsigma, {norb, norb})
    // return type of Gloc is a B2GF with dimensions (1, nspin, {norb, norb})
    for (auto sigma : range(n_sigma)) { // spin index
      n += real(nda::trace(density(Gloc(0, sigma))));
    }
    return n;
  }

  // --------  instantiations --------------

  /** @cond DOXYGEN_SKIP_THIS */
  block2_gf<mesh::imfreq, matrix_valued> gloc(one_body_elements const &obe, double mu, block2_gf<mesh::imfreq, matrix_valued> const &Sigma_dynamic,
                                              nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree, double tolerance,
                                              triqs::lattice::bz_int_options const &opt);

  //double density(one_body_elements const &obe, double mu, block2_gf<mesh::imfreq, matrix_valued> const &Sigma_dynamic,
  //               nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree);

  /** @endcond */

} // namespace triqs::modest
