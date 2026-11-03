// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "./downfolding.hpp"
//#include "triqs_modest/ibz_symmetry_ops.hpp"
#include <ranges>
#include "utils/defs.hpp"
//#include "utils/to_vector.hpp"
//#include "utils/h5_proxy.hpp"
//#include "utils/graph_algo.hpp"
//#include "utils/unitary_matrix.hpp"
// ranges::to : gcc >= 14 https://godbolt.org/z/8TofKvb5a

using namespace triqs;

namespace triqs::modest {

  // -------------------------------------------------------------------------------------------

  // FIXME : we can easily avoid the copy with deducing this : pass self and forward it to res.
  downfolding_projector downfolding_projector::rotate_local_basis(nda::array<nda::matrix<dcomplex>, 2> const &U) const {
    auto res     = *this;
    auto n_k     = res.P_k.extent(0);
    auto n_sigma = res.P_k.extent(1);
    for (auto isig : range(n_sigma)) {
      auto decomp = range(U.extent(0)) | stdv::transform([&](auto &&m) { return U(m, isig).extent(0); }) | tl::to<std::vector>();
      for (auto ik : range(n_k)) {
        for (auto const &[a, sli] : enumerated_sub_slices(decomp)) {
          // P <- dagger(U) * P
          res.P_k(ik, isig, sli, r_all) = dagger(U(a, isig)) * matrix_const_view<dcomplex>{this->P_k(ik, isig, sli, r_all)};
          // I can not use x.P(isig,k)(sli, r_all) because of the slice on the n_bands_per_k in this P }
        }
      }
    }
    return res;
  }

  // -------------------------------------------------------------------------------------------

  one_body_elements_on_grid rotate_local_basis(nda::array<nda::matrix<dcomplex>, 2> const &U, one_body_elements_on_grid const &x) {
    auto res = x;
    res.P    = x.P.rotate_local_basis(U);
    if (res.ibz_symm_ops) res.ibz_symm_ops = rotate_local_basis(U, x.ibz_symm_ops.value());
    return res;
  }
  // -------------------------------------------------------------------------------------------

  one_body_elements_on_grid permute_local_space(std::vector<std::vector<long>> const &atom_partition, one_body_elements_on_grid const &obe) {
    auto atom_indices = nda::flatten(atom_partition);

    auto spin_kind     = obe.C_space.spin_kind();
    auto atomic_shells = atom_indices | stdv::transform([&](auto i) { return obe.C_space.atomic_shells()[i]; }) | tl::to<std::vector>();

    auto new_rot_from_dft_to_local_basis = obe.C_space.rotation_from_dft_to_local_basis();
    auto new_rot_from_sph_to_dft_basis   = obe.C_space.rotation_from_spherical_to_dft_basis();
    for (auto &&[iatom, atom] : enumerate(atom_indices)) {
      new_rot_from_dft_to_local_basis(iatom, r_all) = obe.C_space.rotation_from_dft_to_local_basis()(atom, r_all);
      new_rot_from_sph_to_dft_basis(iatom)          = obe.C_space.rotation_from_spherical_to_dft_basis()(atom);
    }

    auto n_atoms = atom_partition.size();
    auto n_sigma = obe.C_space.n_sigma();

    auto irreps_per_atom = nda::array<std::vector<long>, 2>(n_atoms, n_sigma);
    for (auto &&[ipart, partition] : enumerate(atom_partition)) {
      if (partition.size() == 1) {
        irreps_per_atom(ipart, r_all) = obe.C_space.atoms_block_decomposition()(partition[0], r_all);
      } else {
        irreps_per_atom(ipart, r_all) = std::vector{
           stdr::fold_left(partition | stdv::transform([&](auto idx) { return obe.C_space.atomic_shells()[idx].dim; }), 0, std::plus<>())};
      }
    }

    auto new_C_space = local_space{spin_kind, atomic_shells, irreps_per_atom, new_rot_from_dft_to_local_basis, new_rot_from_sph_to_dft_basis};

    auto atom_decomp = obe.C_space.atomic_decomposition() | tl::to<std::vector>();

    auto [n_k, n_sig, n_M, n_nu] = obe.P.P_k.shape();

    auto P_k_list = atom_decomp | stdv::transform([&](auto dim) { return nda::zeros<dcomplex>(n_k, n_sig, dim, n_nu); }) | tl::to<std::vector>();
    for (auto &&[atom, sli] : enumerated_sub_slices(atom_decomp)) {
      P_k_list[atom](r_all, r_all, r_all, r_all) = obe.P.P_k(r_all, r_all, sli, r_all);
    }

    auto new_P_k_list = atom_indices | stdv::transform([&](auto i) { return P_k_list[i]; }) | tl::to<std::vector>();

    auto new_atom_decomp = new_C_space.atomic_decomposition() | tl::to<std::vector>();

    auto P_k = nda::zeros<dcomplex>(n_k, n_sig, n_M, n_nu);
    for (auto const &[atom, sli] : enumerated_sub_slices(new_atom_decomp)) {
      P_k(r_all, r_all, sli, r_all) = new_P_k_list[atom](r_all, r_all, nda::range(0, new_atom_decomp[atom]), r_all);
    }

    downfolding_projector new_P{.spin_kind = spin_kind, .P_k = P_k, .n_bands_per_k = obe.P.n_bands_per_k};

    if (!obe.ibz_symm_ops) return {.H = obe.H, .C_space = new_C_space, .P = new_P, .ibz_symm_ops = {}};

    std::vector<long> inv_atom_indices(atom_indices.size());
    for (long i = 0; i < atom_indices.size(); ++i) { inv_atom_indices[atom_indices[i]] = i; }

    auto new_ibz_symm_ops = obe.ibz_symm_ops.value();
    for (auto &op : new_ibz_symm_ops.ops) {
      auto old_perm = op.permutation;
      auto old_mats = op.mats;
      for (auto i : range(atom_indices.size())) {
        op.permutation[i] = inv_atom_indices[old_perm[atom_indices[i]]];
        op.mats[i]        = old_mats[atom_indices[i]];
      }
    }
    return {.H = obe.H, .C_space = new_C_space, .P = new_P, .ibz_symm_ops = new_ibz_symm_ops};
  }

  // -------------------------------------------------------------------------------------------

  nda::array<nda::matrix<dcomplex>, 2> impurity_levels(one_body_elements_on_grid const &obe) {
    auto n_atoms          = obe.C_space.n_atoms();
    auto n_sigma          = obe.C_space.n_sigma();
    auto results_per_atom = nda::array<nda::matrix<dcomplex>, 2>{n_atoms, n_sigma};
    // Hloc^{σ} = \sum_{k} P^{σ}_{mν}(k) H^{σ}_{νν'} P^{σ}_{m'ν'}(k)
    for (auto const &[atom_idx, R_atom] : enumerated_sub_slices(obe.C_space.atomic_decomposition())) {
      auto sh  = obe.C_space.atomic_shells()[atom_idx];
      auto M_a = sh.dim;
      for (auto sigma : range(n_sigma)) {
        auto hloc0 = nda::zeros<dcomplex>(M_a, M_a);
        for (auto ik : range(obe.H.n_k())) { // loop over k-points
          auto P = obe.P.P(sigma, ik)(R_atom, r_all);
          hloc0 += obe.H.k_weights(ik) * P * obe.H.H(sigma, ik) * dagger(P);
        }
        results_per_atom(atom_idx, sigma) = hloc0;
      }
    }

    if (auto const &S = obe.ibz_symm_ops; S) results_per_atom = S->symmetrize(results_per_atom);
    // FIXME : why are we doing this ? If so, why did we compute all of them before ?

    for (auto atom_idx : range(n_atoms)) {
      if (auto inequiv = obe.C_space.first_shell_of_its_equiv_cls(atom_idx); atom_idx != inequiv) {
        results_per_atom(atom_idx, r_all) = results_per_atom(inequiv, r_all);
      }
    }

    auto result = nda::array<nda::matrix<dcomplex>, 2>{1, n_sigma};
    for (auto sigma : range(n_sigma)) {
      result(0, sigma) = nda::zeros<dcomplex>(obe.C_space.dim(), obe.C_space.dim());
      for (auto const &[atom, r_atom] : enumerated_sub_slices(obe.C_space.atomic_decomposition())) {
        result(0, sigma)(r_atom, r_atom) = results_per_atom(atom, sigma);
      }
    }
    return result;
  }
  // -------------------------------------------------------------------------------------------

} // namespace triqs::modest
