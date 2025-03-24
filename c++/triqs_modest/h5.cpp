// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include <h5/h5.hpp>
#include "downfolding.hpp"
#include "embedding.hpp"
#include <H5Tpublic.h> // HDF5 type creation API

template <> h5::hid_t h5::detail::hid_t_of<triqs::modest::embedding::imp_block_t>() {
  using S    = triqs::modest::embedding::imp_block_t;
  hid_t type = H5Tcreate(H5T_COMPOUND, sizeof(S));
  H5Tinsert(type, "imp_idx", HOFFSET(S, imp_idx), H5T_NATIVE_INT);
  H5Tinsert(type, "gamma", HOFFSET(S, gamma), H5T_NATIVE_INT);
  H5Tinsert(type, "tau", HOFFSET(S, tau), H5T_NATIVE_INT);
  return type;
}

namespace triqs::modest {

  // spin_kind_e
  void h5_read(h5::group g, std::string const &name, spin_kind_e &spin_kind) {
    int value = h5_read<int>(g, name);           // Read the integer representation
    spin_kind = static_cast<spin_kind_e>(value); // Convert to spin_kind_e
  }

  void h5_write(h5::group g, std::string const &name, spin_kind_e const &spin_kind) {
    int value = static_cast<int>(spin_kind); // Convert to integer representation
    h5_write(g, name, value);                // Write the integer representation
  }

  // projector
  void h5_read(h5::group g, std::string const &name, downfolding_projector &proj) {
    auto subgroup = g.open_group(name);
    h5_read(subgroup, "spin_kind", proj.spin_kind);
    h5_read(subgroup, "P_k", proj.P_k);
    h5_read(subgroup, "n_bands_per_k", proj.n_bands_per_k);
  }

  void h5_write(h5::group g, std::string const &name, downfolding_projector const &proj) {
    auto subgroup = g.create_group(name);
    h5_write(subgroup, "spin_kind", proj.spin_kind);
    h5_write(subgroup, "P_k", proj.P_k);
    h5_write(subgroup, "n_bands_per_k", proj.n_bands_per_k);
  }

  // band_dispersion
  void h5_read(h5::group g, std::string const &name, band_dispersion &bd) {
    auto subgroup = g.open_group(name);
    h5_read(subgroup, "spin_kind", bd.spin_kind);
    h5_read(subgroup, "H_k", bd.H_k);
    h5_read(subgroup, "n_bands_per_k", bd.n_bands_per_k);
    h5_read(subgroup, "k_weights", bd.k_weights);
  }
  void h5_write(h5::group g, std::string const &name, band_dispersion const &bd) {
    auto subgroup = g.create_group(name);
    h5_write(subgroup, "spin_kind", bd.spin_kind);
    h5_write(subgroup, "H_k", bd.H_k);
    h5_write(subgroup, "n_bands_per_k", bd.n_bands_per_k);
    h5_write(subgroup, "k_weights", bd.k_weights);
  }

  // atomic_shell
  void h5_read(h5::group g, std::string const &name, atomic_shell_t &shell) {
    auto subgroup = g.open_group(name);
    h5_read(subgroup, "dim", shell.dim);
    h5_read(subgroup, "l", shell.l);
    h5_read(subgroup, "cls_idx", shell.cls_idx);
    h5_read(subgroup, "dft_idx", shell.dft_idx);
  }
  void h5_write(h5::group g, std::string const &name, atomic_shell_t const &shell) {
    auto subgroup = g.create_group(name);
    h5_write(subgroup, "dim", shell.dim);
    h5_write(subgroup, "l", shell.l);
    h5_write(subgroup, "cls_idx", shell.cls_idx);
    h5_write(subgroup, "dft_idx", shell.dft_idx);
  }

  // local_space
  void h5_read(h5::group g, std::string const &name, local_space &ls) {
    auto subgroup = g.open_group(name);
    h5_read(subgroup, "spin_kind", ls._spin_kind);
    h5_read(subgroup, "atomic_shells", ls._atomic_shells);
    h5_read(subgroup, "block_decomposition", ls._irreps_decomp_per_atom);
  }

  void h5_write(h5::group g, std::string const &name, local_space const &ls) {
    auto subgroup = g.create_group(name);
    h5_write(subgroup, "spin_kind", ls._spin_kind);
    h5_write(subgroup, "atomic_shells", ls._atomic_shells);
    h5_write(subgroup, "block_decomposition", ls._irreps_decomp_per_atom);
  }

  // embedding
  void h5_read(h5::group g, std::string const &name, embedding &embed) {
    auto subgroup = g.open_group(name);
    h5_read(subgroup, "sigma_embed_decomp", embed.sigma_embed_decomp);
    h5_read(subgroup, "imp_decomps", embed.imp_decomps);
    //h5_read(subgroup, "psi", embed.psi);
    h5_read(subgroup, "sigma_names", embed._sigma_names);
  }

  void h5_write(h5::group g, std::string const &name, embedding const &embed) {
    auto subgroup = g.create_group(name);
    h5_write(subgroup, "sigma_embed_decomp", embed.sigma_embed_decomp);
    h5_write(subgroup, "imp_decomps", embed.imp_decomps);
    //h5_write(subgroup, "psi", embed.psi);
    h5_write(subgroup, "sigma_names", embed._sigma_names);
  }

  // one_body_elements_on_grid
  void h5_read(h5::group g, std::string const &name, one_body_elements_on_grid &obe) {
    auto subgroup = g.open_group(name);
    h5_read(subgroup, "H", obe.H);
    h5_read(subgroup, "C_space", obe.C_space);
    h5_read(subgroup, "P", obe.P);
    //h5_read(subgroup, "ibz_symm_ops", obe.ibz_symm_ops);
  }
  void h5_write(h5::group g, std::string const &name, one_body_elements_on_grid const &obe) {
    auto subgroup = g.create_group(name);
    h5_write(subgroup, "H", obe.H);
    h5_write(subgroup, "C_space", obe.C_space);
    h5_write(subgroup, "P", obe.P);
    //h5_write(subgroup, "ibz_symm_ops", obe.ibz_symm_ops);
  }
} // namespace triqs::modest