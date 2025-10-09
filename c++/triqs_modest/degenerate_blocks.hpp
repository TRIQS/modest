// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "utils/defs.hpp"

using namespace triqs::gfs;

namespace triqs::modest {

  /**
   * @ingroup deg
   * @brief Find the generate blocks of a block GF by analyzing \f$ G(\tau=0) \f$ or \f$ G(i\omega_0) \f$ using the
   * union-find algorithm.
   *
   * @details We use the union-find algorithm to group the blocks of the block GF (matrices) into equivalence
   * classes based on approximate numerical equality. If two matrices are approximately equal, their blocks are united
   * into the same set. The final result is a partitioning of the blocks (matrices) into degenerate groups.
   *
   * @tparam Mesh  The mesh type.
   * @param Gimp Block Green's function.
   * @param threshold Tolerance for equivalent of blocks.
   * @return A list of equivalent blocks.
   */
  template <typename Mesh>
  std::vector<std::vector<long>> analyze_degenerate_blocks(block_gf<Mesh, matrix_valued> const &Gimp, double threshold = 1.e-5) {

    auto n_mesh   = Gimp[0].mesh().size();
    auto n_blocks = Gimp.size();

    std::vector<std::vector<long>> groups;
    std::vector<bool> used(n_blocks, false);

    auto are_matrices_equal = [threshold, n_mesh](auto const &A, auto const &B) {
      if (A.target_shape() != B.target_shape()) return false;
      long n0 = A.target_shape()[0];
      for (auto mesh_idx : range(n_mesh)) {
        auto const &Am = A(mesh_idx);
        auto const &Bm = B(mesh_idx);
        for (size_t i = 0; i < n0; ++i) {
          for (size_t j = i + 1; j < n0; ++j) {
            if (std::abs(Am(i, j) - Bm(i, j)) > threshold) return false;
          }
        }
      }
      return true;
    };

    for (auto i = 0; i < n_blocks; ++i) {
      if (!used[i]) {
        std::vector<long> current_group = {i};
        used[i]                         = true;
        for (auto j = i + 1; j < n_blocks; ++j) {
          if (!used[j] && are_matrices_equal(Gimp[i], Gimp[j])) {
            current_group.push_back(j);
            used[j] = true;
          }
        }
        groups.push_back(std::move(current_group));
      }
    }
    return groups;
  }

  /**
   * @ingroup deg
   * @brief Symmetrize the blocks of a Green's function given a list of it's degenerate blocks.
   *
   * @details Average the degenerate blocks and replace the degenerate ones with their average.
   *
   * @tparam Mesh  The mesh type.
   * @param g Block Green's function.
   * @param deg_bls A list of the degenerate blocks.
   * @return The symmetrized Green's function.
  */
  template <typename Mesh>
  block_gf<Mesh, matrix_valued> symmetrize_gf(block_gf<Mesh, matrix_valued> const &g, std::vector<std::vector<long>> deg_bls) {
    auto gsymm       = g;
    auto const &mesh = gsymm[0].mesh();

    for (auto const &deg_bl : deg_bls) {
      auto n_deg = deg_bl.size();
      auto dim   = g[deg_bl[0]].target_shape()[0];

      auto gtmp = gf{mesh, {dim, dim}};
      for (auto bl : deg_bl) { gtmp += g[bl]; }

      gtmp /= n_deg;

      for (auto bl : deg_bl) { gsymm[bl] = gtmp; }
    }

    return gsymm;
  }

  template std::vector<std::vector<long>> analyze_degenerate_blocks(block_gf<dlr_imfreq, matrix_valued> const &G, double threshold);
  template std::vector<std::vector<long>> analyze_degenerate_blocks(block_gf<dlr_imtime, matrix_valued> const &G, double threshold);
  template std::vector<std::vector<long>> analyze_degenerate_blocks(block_gf<imfreq, matrix_valued> const &G, double threshold);
  template std::vector<std::vector<long>> analyze_degenerate_blocks(block_gf<imtime, matrix_valued> const &G, double threshold);

  template block_gf<imfreq, matrix_valued> symmetrize_gf(block_gf<imfreq, matrix_valued> const &g, std::vector<std::vector<long>> deg_bls);
  template block_gf<imtime, matrix_valued> symmetrize_gf(block_gf<imtime, matrix_valued> const &g, std::vector<std::vector<long>> deg_bls);
  template block_gf<dlr_imfreq, matrix_valued> symmetrize_gf(block_gf<dlr_imfreq, matrix_valued> const &g, std::vector<std::vector<long>> deg_bls);
  template block_gf<dlr_imtime, matrix_valued> symmetrize_gf(block_gf<dlr_imtime, matrix_valued> const &g, std::vector<std::vector<long>> deg_bls);
} // namespace triqs::modest
