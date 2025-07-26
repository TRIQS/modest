#include <nda/layout/range.hpp>
#include <triqs/gfs.hpp>
#include <triqs/utility/exceptions.hpp>
#include <type_traits>
#include "./local_space.hpp"
#include <triqs/lattice/tb_hamiltonian.hpp>
#include <triqs/lattice/wannier_loader.hpp>
#include "downfolding.hpp"
#include <stdexcept>
#include "./obe_tb.hpp"

// TODO questions:
// Wannier converter: I need to try an SOC / spin pol version
// https://wannier90.readthedocs.io/en/latest/tutorials/tutorial_8/
//
// DANGEROUS : no way to check that wannier hamiltonian has atoms in the same order if user specifies their positions/
// the atomic shell info. We are relying on them to get this right somehow, so there should be some print check
//
// TODO feels like discover_symmetries should have a default threshold; almost always sensible with that kind of thing
// TODO is this an ok way to account for different spintypes

namespace triqs::modest {

  /** Construct a obe_tb from Wannier90 in the case of a single spin index 
   * @param wannier_file_path string to Wannier90 files, including the prefix 
   * @param spin_kind enum telling us the spintype 
   * @param shells list of atomic shells input by the user 
   */
  one_body_elements_tb one_body_elements_from_wannier90(std::string const &wannier_file_path, spin_kind_e const &spin_kind,
                                                        std::vector<atomic_shell_t> const &atomic_shells) {
    if (spin_kind == spin_kind_e::Polarized) {
      TRIQS_RUNTIME_ERROR << "If performing a spin-polarized calculation, you need to supply two Wannier file paths for up and down channels.\n";
    }
    // call the wannier90 loader and set up tb_hamiltonian
    auto [R, HR, _] = read_wannier90_tb_data(wannier_file_path);
    //std::cout << HR[0] << std::endl;
    std::vector<tb_hamiltonian> tb_H;
    tb_H.emplace_back(R, HR);
    return make_obe_from_tb(tb_H, spin_kind, atomic_shells);
  };

  one_body_elements_tb one_body_elements_from_wannier90(std::string const &wannier_file_path_up, std::string const &wannier_file_path_dn,
                                                        spin_kind_e const &spin_kind, std::vector<atomic_shell_t> const &atomic_shells) {
    if (spin_kind != spin_kind_e::Polarized) {
      TRIQS_RUNTIME_ERROR << "For a non-spin polarized calculation, you should specify only one Wannier Hamiltonian.\n";
    }
    // call the wannier90 loader and set up tb_hamiltonian list
    std::vector<tb_hamiltonian> tb_H;
    for (auto file : {wannier_file_path_up, wannier_file_path_dn}) {
      auto [R, HR, _] = read_wannier90_tb_data(file);
      tb_H.emplace_back(R, HR);
    }
    return make_obe_from_tb(tb_H, spin_kind, atomic_shells);
  }

  one_body_elements_tb make_obe_from_tb(std::vector<tb_hamiltonian> const &H_sigma, spin_kind_e const &spin_kind,
                                        std::vector<atomic_shell_t> const &atomic_shells) {

    nda::array<nda::matrix<dcomplex>, 2> Hloc(atomic_shells.size(), 1);

    for (auto [isigma, H] : enumerate(H_sigma)) {

      // check that HR and atomic shells list have the same total size
      long n_orb = 0;
      for (auto shell : atomic_shells) { n_orb += shell.dim; }
      if (H.n_orbitals() != n_orb) {
        TRIQS_RUNTIME_ERROR << "Wannier Hamiltonian does not have the same number of orbitals as the provided atomic shells: HR " << H.n_orbitals()
                            << " , atomic_shells total " << n_orb << "\n";
      }

      // find the home cell of the TB file to get H0
      nda::array<long, 1> R0{0, 0, 0};
      // TODO do we want to check to make sure we found 0,0,0 and throw an error if not?
      //auto R   = H.R();
      //long iR0 = std::ranges::distance(R.begin(), std::ranges::find(R, R0));
      long iR0 = -1;
      for (auto iR : nda::range(H.R().shape(0))) {
        if (H.R()(iR, nda::ellipsis()) == R0(nda::ellipsis())) { iR0 = iR; }
      }
      if (iR0 == -1) { TRIQS_RUNTIME_ERROR << "Could not locate the R = [0,0,0] cell of the Wannier Hamiltonian.\n"; }

      // this needs to have dimensions [nshells, nspin].
      long start_orb = 0;
      for (auto &&[ishell, shell] : enumerate(atomic_shells)) {
        auto Hloc0_ab = nda::zeros<dcomplex>(shell.dim, shell.dim);
        // need to create a matrix of dim x dim for each
        for (auto iorb : nda::range(shell.dim)) {
          for (auto jorb : nda::range(shell.dim)) { Hloc0_ab(iorb, jorb) = H.hoppings()(iR0, start_orb + iorb, start_orb + jorb); }
          start_orb += shell.dim;
        }
        Hloc(ishell, isigma) = Hloc0_ab;
      }
    }
    // construct the H0 and detect symmetries from it
    double block_threshold  = 1e-6;
    bool diagonalize_hloc   = false; // TODO this would mean looking at band energies at k =0?
    auto [decomposition, U] = discover_symmetries(Hloc, atomic_shells, block_threshold, diagonalize_hloc);

    // call constructor for local space, for now ignoring rotation_from_spherical_to_dft_basis
    auto LS = local_space(spin_kind, atomic_shells, decomposition, U, {});

    // construct and return obe_tb
    return one_body_elements_tb{.H = H_sigma, .C_space = LS};
  }
} // namespace triqs::modest
