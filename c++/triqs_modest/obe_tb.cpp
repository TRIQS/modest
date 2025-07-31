#include <nda/layout/range.hpp>
#include <triqs/gfs.hpp>
#include <triqs/utility/exceptions.hpp>
#include <type_traits>
#include "./local_space.hpp"
#include <triqs/lattice/wannier_loader.hpp>
#include "downfolding.hpp"
#include <stdexcept>
#include "./obe_tb.hpp"

// DANGEROUS : no way to check that wannier hamiltonian has atoms in the same order if user specifies their positions/
// the atomic shell info. We are relying on them to get this right somehow, so there should be some print check
//
// TODO feels like discover_symmetries should have a default threshold; almost always sensible with that kind of thing

namespace triqs::modest {

  /** Construct a obe_tb from Wannier90 in the case of a single spin index 
   * @param wannier_file_path string to Wannier90 files, including the prefix // FIXME example 
   * @param spin_kind enum telling us the spintype 
   * @param shells list of atomic shells input by the user 
   */
  one_body_elements one_body_elements_from_wannier90(std::string const &wannier_file_path, spin_kind_e const &spin_kind,
                                                     std::vector<atomic_shell_t> atomic_shells) {
    if (spin_kind == spin_kind_e::Polarized) {
      TRIQS_RUNTIME_ERROR << "If performing a spin-polarized calculation, you need to supply two Wannier file paths for up and down channels.\n";
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
      TRIQS_RUNTIME_ERROR << "For a non-spin polarized calculation, you should specify only one Wannier Hamiltonian.\n";
    }

    // call the wannier90 loader and set up tb_hamiltonian list
    std::vector<tb_hamiltonian> tb_H;
    for (auto file : {wannier_file_path_up, wannier_file_path_dn}) {
      auto [R, HR, _] = read_wannier90_tb_data(file);
      tb_H.emplace_back(R, HR);
    }
    return make_obe_from_tb(std::move(tb_H), spin_kind, std::move(atomic_shells));
  }

  // -----------------------------------------------------------------------

  one_body_elements make_obe_from_tb(std::vector<tb_hamiltonian> H_sigma, spin_kind_e const &spin_kind, std::vector<atomic_shell_t> atomic_shells) {

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
      auto iR0 = H.get_R_idx({0, 0, 0});

      // look for a function like "enumerate_slice"
      // this needs to have dimensions [nshells, nspin].
      long start_orb = 0;
      for (auto &&[ishell, shell] : enumerate(atomic_shells)) {
        auto Hloc0_ab = nda::zeros<dcomplex>(shell.dim, shell.dim);
        // need to create a matrix of dim x dim for each
        for (auto iorb : nda::range(shell.dim)) {
          for (auto jorb : nda::range(shell.dim)) { Hloc0_ab(iorb, jorb) = H.hoppings()[iR0](start_orb + iorb, start_orb + jorb); }
          start_orb += shell.dim;
        }
        Hloc(ishell, isigma) = Hloc0_ab;
      }
    }
    // construct the H0 and detect symmetries from it
    double block_threshold  = 1e-6;
    bool diagonalize_hloc   = false; // CHECK this would mean looking at band energies at k = 0?
    auto [decomposition, U] = discover_symmetries(Hloc, atomic_shells, block_threshold, diagonalize_hloc);

    // call constructor for local space, for now ignoring rotation_from_spherical_to_dft_basis
    auto LS = local_space(spin_kind, atomic_shells, decomposition, U, {});

    // construct and return obe_tb
    return one_body_elements{.C_space = std::move(LS), .H = std::move(H_sigma)};
  }

  // -----------------------------------------------------------------------

  one_body_elements fold(lattice::superlattice const &sl, one_body_elements const &obe) {

    auto new_H = obe.H | stdv::transform([&](auto x) { return fold(sl, x); }) | tl::to<std::vector>();

    auto sh = obe.C_space.atomic_shells();
    decltype(sh) new_atomic_shells;
    auto const &dec = obe.C_space.atoms_block_decomposition();
    nda::array<std::vector<long>, 2> new_dec(dec.extent(0) * sl.n_cluster_sites(), dec.extent(1));

    for (auto i : nda::range(sl.n_cluster_sites())) {
      for (auto const &shell : sh) { new_atomic_shells.emplace_back(shell); }
      new_dec(i, r_all) = dec(i, r_all);
    }

    return {.C_space = local_space{obe.C_space.spin_kind(), std::move(new_atomic_shells), new_dec, {}, {}}, .H = std::move(new_H)};
  }
} // namespace triqs::modest
