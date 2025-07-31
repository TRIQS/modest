#include <nda/layout/range.hpp>
#include <nda/matrix_functions.hpp>
#include <triqs/gfs.hpp>
#include <triqs/utility/exceptions.hpp>
#include <type_traits>
#include "./local_space.hpp"
#include <triqs/lattice/wannier_loader.hpp>
#include "downfolding.hpp"
#include <stdexcept>
#include "./obe_tb.hpp"
#include "triqs_modest/utils/defs.hpp"

// DANGEROUS : no way to check that wannier hamiltonian has atoms in the same order if user specifies their positions/
// the atomic shell info. We are relying on them to get this right somehow, so there should be some print check
//
// TODO feels like discover_symmetries should have a default threshold; almost always sensible with that kind of thing
// TODO what do about sigma_hartree... forward it to gloc in triqs as something optional?

namespace triqs::modest {

  /** Construct a obe_tb from Wannier90 in the case of a single spin index 
   * @param wannier_file_path string to Wannier90 files, including the prefix // FIXME example 
   * @param spin_kind enum telling us the spintype 
   * @param shells list of atomic shells input by the user 
   */
  one_body_elements one_body_elements_from_wannier90(std::string const &wannier_file_path, spin_kind_e const &spin_kind,
                                                     std::vector<atomic_shell_t> atomic_shells) {
    if (spin_kind == spin_kind_e::Polarized) {
      throw std::runtime_error("If performing a spin-polarized calculation, you need to supply two Wannier file paths for up and down channels.\n");
    }
    // call the wannier90 loader and set up tb_hamiltonian
    // FIXME check if tb or hr exists -- write a lambda that checks which exists and returns just one of the two
    auto [R, HR, _] = read_wannier90_tb_data(wannier_file_path);
    std::vector<tb_hamiltonian> tb_H;
    tb_H.emplace_back(R, HR);
    return make_obe_from_tb(std::move(tb_H), spin_kind, std::move(atomic_shells));
  };

  one_body_elements one_body_elements_from_wannier90(std::string const &wannier_file_path_up, std::string const &wannier_file_path_dn,
                                                     spin_kind_e const &spin_kind, std::vector<atomic_shell_t> atomic_shells) {
    if (spin_kind != spin_kind_e::Polarized) {
      throw std::runtime_error("For a non-spin polarized calculation, you should specify only one Wannier Hamiltonian.\n");
    }

    // call the wannier90 loader and set up tb_hamiltonian list
    std::vector<tb_hamiltonian> tb_H;
    for (auto file : {wannier_file_path_up, wannier_file_path_dn}) {
      auto [R, HR, _] = read_wannier90_tb_data(file);
      tb_H.emplace_back(R, HR);
    }
    if (tb_H[0].n_orbitals() != tb_H[1].n_orbitals()) {
      throw std::runtime_error(
         "Cannot construct a one_body_elements "
         "using up and down H_k that have a different number of orbtials.");
    }
    return make_obe_from_tb(std::move(tb_H), spin_kind, std::move(atomic_shells));
  }

  // -----------------------------------------------------------------------

  //TODO docstring
  /** Helper function to calculate Hloc given a vector of H with length sigma (contained in Wannier OBE)
   */
  nda::array<nda::matrix<dcomplex>, 2> Hloc(std::vector<tb_hamiltonian> const &H_sigma, std::vector<atomic_shell_t> const &atomic_shells) {

    // return Hloc with shape [atomic_shells, nsigma]
    nda::array<nda::matrix<dcomplex>, 2> Hloc_result(atomic_shells.size(), H_sigma.size());

    for (auto [isigma, H] : enumerate(H_sigma)) {

      // check that HR and atomic shells list have the same total size
      long n_orb = 0;
      for (auto shell : atomic_shells) { n_orb += shell.dim; }
      if (H.n_orbitals() != n_orb) {
        throw std::runtime_error("Wannier Hamiltonian does not have the same number of orbitals as the provided atomic shells: HR " + std::to_string(H.n_orbitals()) + 
                            " , atomic_shells total " + std::to_string(n_orb));
      }

      // find the home cell of the TB file to get H0
      auto iR0 = H.get_R_idx({0, 0, 0});

      // Hloc needs to have dimensions [nshells, nspin].
      // REFACTOR -- need opposite of enumerate_sub_slice --
      // NOTE: we cannot use enumerated_subslice here because we do not have a C_space!
      // this function computes Hloc so we can determine the C_space in the first place.
      long start_orb = 0;
      for (auto &&[ishell, shell] : enumerate(atomic_shells)) {
        auto Hloc0_ab = nda::zeros<dcomplex>(shell.dim, shell.dim);
        // need to create a matrix of dim x dim for each
        for (auto iorb : nda::range(shell.dim)) {
          for (auto jorb : nda::range(shell.dim)) { Hloc0_ab(iorb, jorb) = H.hoppings()[iR0](start_orb + iorb, start_orb + jorb); }
          start_orb += shell.dim;
        }
        Hloc_result(ishell, isigma) = Hloc0_ab;
      }
    }
    return Hloc_result;
  }

  /** Creates an Hloc function from an OBE based on a tight binding Hamiltonian 
   */
  nda::array<nda::matrix<dcomplex>, 2> Hloc(one_body_elements const &obe) {

    // TODO is this working as we think it will... FIXME need num atoms here
    auto n_atoms = obe.C_space.n_atoms();
    auto n_sigma = obe.C_space.n_sigma();
    nda::array<nda::matrix<dcomplex>, 2> Hloc_result(n_atoms, n_sigma);

    for (auto sigma : range(n_sigma)) {

      // find the home cell of the TB file to get H0
      auto iR0 = obe.H[sigma].get_R_idx({0, 0, 0});

      for (auto const &[atom, R_atom] : enumerated_sub_slices(obe.C_space.atomic_decomposition())) {
        //auto shell               = obe.C_space.atomic_shells()[atom];
        Hloc_result(atom, sigma) = obe.H[sigma].hoppings()[iR0](R_atom, R_atom);
      }
    }
    return Hloc_result;
  }

  // -----------------------------------------------------------------------

  one_body_elements make_obe_from_tb(std::vector<tb_hamiltonian> H_sigma, spin_kind_e const &spin_kind, std::vector<atomic_shell_t> atomic_shells) {

    // calculate Hloc using helper function -- Hloc here is dim [nshells, nsigma]
    nda::array<nda::matrix<dcomplex>, 2> hloc = Hloc(H_sigma, atomic_shells);

    // construct block structure using Hloc
    double block_threshold  = 1e-6;
    bool diagonalize_hloc   = false; // CHECK this would mean looking at band energies at k = 0?
    auto [decomposition, U] = discover_symmetries(hloc, atomic_shells, block_threshold, diagonalize_hloc);

    // call constructor for local space, for now ignoring rotation_from_spherical_to_dft_basis
    auto LS = local_space(spin_kind, atomic_shells, decomposition, U, {});

    // construct and return obe_tb
    return one_body_elements{.C_space = std::move(LS), .H = std::move(H_sigma)};
  }

  /** Compute the local impurity levels from the single-particle dispersion. 
  * This simply computes Hloc and returns it, but exists to match the syntax used in the fixed_grid case. 
  */
  nda::array<nda::matrix<dcomplex>, 2> impurity_levels(one_body_elements const &obe) { return Hloc(obe); }

} // namespace triqs::modest
