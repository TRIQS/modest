// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "defs.hpp"
#include "gf_supp.hpp"
#include "nda_supp.hpp"
#include "range_supp.hpp"
#include "enumerate_slice.hpp"

namespace triqs::modest::detail {
  auto constexpr r_all = nda::range::all;

  template <int Rank> constexpr bool has_frequency = (Rank == 3 || Rank == 5);

  // Rank 2: T(r, r)
  template <typename T> auto extract_diagonal(nda::array<T, 2> const &M, nda::range r) { return M(r, r); }
  template <typename T> void embed_diagonal(nda::array<T, 2> &M, nda::range r, nda::array<T, 2> const &block) { M(r, r) = block; }

  // Rank 3: T(r_all, r, r) - frequency on axis 0
  template <typename T> auto extract_diagonal(nda::array<T, 3> const &M, nda::range r) { return M(r_all, r, r); }
  template <typename T> void embed_diagonal(nda::array<T, 3> &M, nda::range r, nda::array<T, 3> const &block) { M(r_all, r, r) = block; }

  // Rank 4: T(r, r, r, r)
  template <typename T> auto extract_diagonal(nda::array<T, 4> const &M, nda::range r) { return M(r, r, r, r); }
  template <typename T> void embed_diagonal(nda::array<T, 4> &M, nda::range r, nda::array<T, 4> const &block) { M(r, r, r, r) = block; }

  // Rank 5: T(r_all, r, r, r, r) - frequency on axis 0
  template <typename T> auto extract_diagonal(nda::array<T, 5> const &M, nda::range r) { return M(r_all, r, r, r, r); }
  template <typename T> void embed_diagonal(nda::array<T, 5> &M, nda::range r, nda::array<T, 5> const &block) { M(r_all, r, r, r, r) = block; }

  // Constrauct zero array of given rank and type
  template <int Rank, typename T> auto make_zero_array(long dim, long n_w = 0) {
    if constexpr (Rank == 2) {
      return nda::zeros<T>(dim, dim);
    } else if constexpr (Rank == 3) {
      return nda::zeros<T>(n_w, dim, dim);
    } else if constexpr (Rank == 4) {
      return nda::zeros<T>(dim, dim, dim, dim);
    } else if constexpr (Rank == 5) {
      return nda::zeros<T>(n_w, dim, dim, dim, dim);
    }
  }

  // From a BlockGf return it's data as a vector (blocks) of arrays
  template <typename Mesh> std::vector<nda::array<dcomplex, 3>> make_data_view_from_block_gf(block_gf<Mesh, matrix_valued> const &gf) {
    std::vector<nda::array<dcomplex, 3>> data;
    for (auto block_idx = 0; block_idx < gf.size(); ++block_idx) { data.push_back(gf[block_idx].data()); }
    return data;
  }

  // From a vector of BlockGf return it's data as a vector (impurities) of vector (blocks) of arrays
  template <typename Mesh>
  std::vector<std::vector<nda::array<dcomplex, 3>>> make_data_view_from_block_gfs(std::vector<block_gf<Mesh, matrix_valued>> const &gfs) {
    return gfs | stdv::transform([&](auto g) { return make_data_view_from_block_gf(g); }) | tl::to<std::vector>();
  }

  // Gather the blocks from a Block2Gf (alpha, sigma) in a vector (sigma) of data arrays
  template <typename Mesh> std::vector<nda::array<dcomplex, 3>> gather_blocks_to_data_view(block2_gf<Mesh, matrix_valued> const &g) {
    auto n_sigma = g.size2();
    auto decomp  = get_struct(g).dims(r_all, 0) | tl::to<std::vector>();

    auto n_w = g(0, 0).mesh().size();
    auto dim = stdr::fold_left(decomp, 0, std::plus<>());

    auto data = range(n_sigma) | stdv::transform([dim, n_w](auto) { return nda::array<dcomplex, 3>(n_w, dim, dim); }) | tl::to<std::vector>();
    for (auto sigma : range(n_sigma)) {
      for (auto &&[index, sli] : enumerated_sub_slices(decomp)) { data[sigma](r_all, sli, sli) = g(index, sigma).data(); }
    };
    return data;
  }

  template <typename T> auto gather_blocks_to_data_view(nda::array<nda::matrix<T>, 2> const &M) {
    auto n_sigma = M.extent(1);
    auto decomp  = range(M.extent(0)) | stdv::transform([&](auto const &alpha) { return M(alpha, 0).shape()[0]; }) | tl::to<std::vector>();
    auto dim     = stdr::fold_left(decomp, 0, std::plus<>());
    nda::array<nda::array<T, 2>, 2> data(1, n_sigma);
    for (auto sigma : range(n_sigma)) {
      data(0, sigma) = nda::zeros<T>(dim, dim);
      for (auto &&[index, sli] : enumerated_sub_slices(decomp)) { data(0, sigma)(sli, sli) = M(index, sigma); }
    };
    return data;
  }

  // Make a BlockGf from data
  template <typename Mesh>
  block_gf<Mesh, matrix_valued> make_block_gf_from_data_view(std::vector<nda::array<dcomplex, 3>> const &data_view, gf_struct_t const &gf_struct,
                                                             Mesh const &mesh) {
    auto gf = block_gf<Mesh, matrix_valued>(mesh, gf_struct);
    for (auto &&[block_idx, block] : enumerate(data_view)) { gf[block_idx].data() = block; }
    return gf;
  }

  // Make a Block2Gf from data
  template <typename Mesh>
  block2_gf<Mesh, matrix_valued> make_block2_gf_from_data_view(std::vector<nda::array<dcomplex, 3>> const &data_view, gf_struct2_t const &gf_struct2,
                                                               Mesh const &mesh) {
    auto gf2                = make_block2_gf(mesh, gf_struct2);
    auto n_sigma            = gf2.size2();
    auto sigma_embed_decomp = get_struct(gf2).dims(r_all, 0) | tl::to<std::vector>();
    for (auto [alpha, r_alpha] : enumerated_sub_slices(sigma_embed_decomp)) {
      for (auto sigma : range(n_sigma)) { gf2(alpha, sigma).data() = data_view[sigma](r_all, r_alpha, r_alpha); }
    }
    return gf2;
  }

  // Convert between matrix<T> and array<T,2>
  template <typename T> auto matrix_to_array(std::vector<std::vector<nda::matrix<T>>> const &data) {
    auto tmp_data = std::vector<std::vector<nda::array<T, 2>>>(data.size());
    for (auto imp : range(data.size())) {
      for (auto &&[block, block_data] : enumerate(data[imp])) { tmp_data[imp].emplace_back(block_data); }
    }
    return tmp_data;
  }

  // Convert between array<T,2> and matrix<T>
  template <typename T> auto array_to_matrix(std::vector<std::vector<nda::array<T, 2>>> const &data) {
    auto tmp_data = std::vector<std::vector<nda::matrix<T>>>(data.size());
    for (auto imp : range(data.size())) {
      for (auto &&[block, block_data] : enumerate(data[imp])) { tmp_data[imp].emplace_back(block_data); }
    }
    return tmp_data;
  }

  // Reshape data array to a block2 (nda::array<array_t, 2>) using decomposition
  template <typename T> auto data_array_to_block2_array(std::vector<nda::array<T, 2>> const &data_view, std::vector<long> const &sigma_embed_decomp) {
    auto n_alpha      = long(sigma_embed_decomp.size());
    auto n_sigma      = long(data_view.size());
    auto block2_array = nda::array<nda::array<T, 2>, 2>(n_alpha, n_sigma);
    for (auto &&[alpha, r_alpha] : enumerated_sub_slices(sigma_embed_decomp)) {
      for (auto sigma : range(n_sigma)) { block2_array(alpha, sigma) = data_view[sigma](r_alpha, r_alpha); }
    }
    return block2_array;
  }

} // namespace triqs::modest::detail
