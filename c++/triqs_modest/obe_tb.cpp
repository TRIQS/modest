// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

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

namespace triqs::modest {

  one_body_elements_tb one_body_elements_from_wannier90(std::string const &wannier_file_path, spin_kind_e spin_kind,
                                                        std::vector<atomic_orbs> atomic_shells) {
    if (spin_kind == spin_kind_e::Polarized) {
      throw std::runtime_error("If performing a spin-polarized calculation, you need to supply two Wannier file paths for up and down channels.\n");
    }
    // call the wannier90 loader and set up tb_hamiltonian
    // FIXME check if tb or hr exists -- write a lambda that checks which exists and returns just one of the two
    auto [R, HR, _] = read_wannier90_tb_data(wannier_file_path);
    std::vector<tb_hamiltonian> tb_H;
    tb_H.emplace_back(R, HR);
    // put the same H a second time for two spin channels
    if (spin_kind == spin_kind_e::NonPolarized) { tb_H.emplace_back(R, HR); }

    return make_obe_from_tb(std::move(tb_H), spin_kind, std::move(atomic_shells));
  };

  one_body_elements_tb one_body_elements_from_wannier90(std::string const &wannier_file_path_up, std::string const &wannier_file_path_dn,
                                                        spin_kind_e spin_kind, std::vector<atomic_orbs> atomic_shells) {
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

  nda::array<nda::matrix<dcomplex>, 2> Hloc(std::vector<tb_hamiltonian> const &H_sigma, std::vector<atomic_orbs> const &atomic_shells) {

    // group the shells into atom indices
    // makes a vector containing the dim of each atomic shell...
    //auto shell_decomposition = atomic_shells | stdv::transform([](auto const &s) { return s.dim; });

    // return Hloc with shape [n_atoms, nsigma]
    nda::array<nda::matrix<dcomplex>, 2> Hloc_result(atomic_shells.size(), H_sigma.size());

    for (auto [isigma, H] : enumerate(H_sigma)) {

      // FIXME : REFACTOR decomposition from atomic shells
      // check that HR and atomic shells list have the same total size
      long n_orb = 0;
      for (auto shell : atomic_shells) { n_orb += shell.dim; }
      if (H.n_orbitals() != n_orb) {
        throw std::runtime_error("Wannier Hamiltonian does not have the same number of orbitals as the provided atomic shells: HR "
                                 + std::to_string(H.n_orbitals()) + " , atomic_shells total " + std::to_string(n_orb));
      }

      // find the home cell of the TB file to get H0
      auto iR0 = H.get_R_idx(std::array<long, 3>{0, 0, 0});

      // Hloc needs to have dimensions [n_atomic_shells, nspin].
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

  nda::array<nda::matrix<dcomplex>, 2> impurity_levels(one_body_elements_tb const &obe) {

    auto n_atoms = obe.C_space.n_atoms();
    auto n_sigma = obe.C_space.n_sigma();
    nda::array<nda::matrix<dcomplex>, 2> Hloc_result(n_atoms, n_sigma);

    for (auto sigma : range(n_sigma)) {

      // find the home cell of the TB file to get H0
      auto iR0 = obe.H[sigma].get_R_idx({0, 0, 0});

      for (auto const &[atom, R_atom] : enumerated_sub_slices(obe.C_space.atomic_decomposition())) {
        Hloc_result(atom, sigma) = obe.H[sigma].hoppings()[iR0](R_atom, R_atom);
      }
    }
    return Hloc_result;
  }

  // -----------------------------------------------------------------------

  one_body_elements_tb make_obe_from_tb(std::vector<tb_hamiltonian> H_sigma, spin_kind_e spin_kind, std::vector<atomic_orbs> atomic_shells) {

    // calculate Hloc using helper function -- Hloc here is dim [nshells, nsigma]
    nda::array<nda::matrix<dcomplex>, 2> hloc = Hloc(H_sigma, atomic_shells);

    // construct block structure using Hloc
    double block_threshold  = 1e-6;
    bool diagonalize_hloc   = false; // CHECK this would mean looking at band energies at k = 0?
    auto [decomposition, U] = discover_symmetries(hloc, atomic_shells, block_threshold, diagonalize_hloc);

    // call constructor for local space, for now ignoring rotation_from_spherical_to_dft_basis
    auto LS = local_space(spin_kind, atomic_shells, decomposition, U, {});

    // construct and return obe_tb
    return one_body_elements_tb{.C_space = std::move(LS), .H = std::move(H_sigma)};
  }

  // -----------------------------------------------------------------------

  one_body_elements_tb fold(lattice::superlattice const &sl, one_body_elements_tb const &obe) {
    auto new_H = obe.H | stdv::transform([&](auto x) { return fold(sl, x); }) | tl::to<std::vector>();
    auto sh    = obe.C_space.atomic_shells();
    decltype(sh) new_atomic_shells;
    auto const &dec = obe.C_space.atoms_block_decomposition();
    nda::array<std::vector<long>, 2> new_dec(dec.extent(0) * sl.n_cluster_sites(), dec.extent(1));
    for (auto i : nda::range(sl.n_cluster_sites())) {
      for (auto &&[j, shell] : enumerate(sh)) {
        new_atomic_shells.emplace_back(shell);
        new_dec(i * sh.size() + j, r_all) = dec(j, r_all);
      }
    }
    return {.C_space = local_space{obe.C_space.spin_kind(), std::move(new_atomic_shells), new_dec, {}, {}}, .H = std::move(new_H)};
  }

  // -----------------------------------------------------------------------

  one_body_elements_tb rotate(one_body_elements_tb const &obe, nda::matrix<dcomplex> const &U) {

    auto rotate = [](tb_hamiltonian const &H, nda::matrix<dcomplex> const &U) {
      if (U.extent(0) != H.n_orbitals()) {
        throw std::runtime_error(
           "Cannot rotate a tb_hamiltonian with a unitary matrix that has a different number of rows than the number of orbitals in the Hamiltonian.");
      }

      auto const &Rs = H.get_R_list();
      std::vector<nda::array<dcomplex, 2>> rotated_hoppings;
      for (auto const &tR : H.hoppings()) { rotated_hoppings.emplace_back(U * nda::matrix<dcomplex>(tR) * dagger(U)); }
      return tb_hamiltonian{Rs, std::move(rotated_hoppings)};
    };

    auto new_H = obe.H | stdv::transform([&](auto x) { return rotate(x, U); }) | tl::to<std::vector>();

    return {.C_space = obe.C_space, .H = std::move(new_H)};
  }

} // namespace triqs::modest
