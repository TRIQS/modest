// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "triqs_modest/utils/defs.hpp"

using namespace triqs::gfs;

namespace triqs::modest {

  namespace detail {

    /// Union-Find: Data structure that efficiently tracks a collection of disjoint sets.
    struct union_find {
      std::vector<long> parent;

      union_find(long n) { parent = range(n) | tl::to<std::vector<long>>(); };

      /// returns the representative of the set containing x
      long find(long x) {
        if (parent[x] != x) { parent[x] = find(parent[x]); }
        return parent[x];
      }

      /// merges the set containing x and y
      void unite(long x, long y) {
        long px = find(x);
        long py = find(y);
        if (px != py) parent[px] = py;
      }
    };

  } // namespace detail

  // HL: TODO the dft_tools analyse_degenerate_blocks (or analyse_deg_shells) did the following steps:
  // 1. Make G(tau) hermitian
  // 2. diagonalize G(tau = 0)
  // 3. Compare eigenvalues.
  // 4. if eigenvalues are the same to epsilon
  // 5. Rotate all blocks using the eigenvectors which constitute a unitary transformation.
  // Do we agree with this algorithm and want the same?

  /**
   * @ingroup deg
   * @brief Find the generate blocks of a BlockGf by analyzing G(τ=0) or G(iω₀) using the union-find algorithm.
   * 
   * @details We use the union-find algorithm to group the blocks of the BlockGf (matrices) into equivalence
   * classes based on approximate numerical equality. If two matrices are approximately equal, their blocks are united
   * into the same set. The final result is a partitioning of the blocks (matrices) into degenerate groups.
   *
   * @param Gimp Block Green's function
   * @param threshold tolerance for equivalence of blocks
   * @return A list of equivalent blocks
   */
  inline std::vector<std::vector<long>> analyze_degenerate_blocks(block_gf<imfreq, matrix_valued> const &Gimp, double threshold = 1.e-5) {
    auto find_degenerate = [&](auto const &G) {
      auto are_matrices_equal = [threshold](auto const &A, auto const &B) {
        if (A.shape() != B.shape()) return false;
        return nda::all(nda::map([threshold](auto x) { return x < threshold; })(abs(A - B)));
      };
      // convert G to list of matrices at Mesh = 0
      auto midpt = static_cast<long>(G[0].mesh().size() / 2);
      auto mats  = G | stdv::transform([midpt](auto const &g) { return nda::matrix<dcomplex>{g(midpt)}; }) | tl::to<std::vector>();

      auto uf = detail::union_find(mats.size());

      std::map<long, std::vector<long>> groups_map;
      for (long i = 0; i < mats.size(); ++i) {
        for (long j = i + 1; j < mats.size(); ++j) {
          if (are_matrices_equal(mats[i], mats[j])) uf.unite(i, j);
        }
        groups_map[uf.find(i)].emplace_back(i);
      }

      return groups_map | stdv::values | stdv::filter([](auto const &g) { return g.size() > 1; }) | tl::to<std::vector>();
    };
    return find_degenerate(Gimp) | stdv::transform([](auto &x) { return x | stdv::transform([](auto &y) { return y; }) | tl::to<std::vector>(); })
       | tl::to<std::vector>();
  }

  /**
   * @ingroup deg
   * @brief Symmetrize the blocks of a Green's function given a list of it's degenerate blocks.
   * 
   * @details Average the degenerate blocks and replace the degenerate ones with their average.
   *
   * @param Gin Block Green's function
   * @param degenerate_blocks a list of the degenerate blocks.
   * @return The symmetrized Green's function.
   */
  inline block_gf<imfreq, matrix_valued> symmetrize_gf(block_gf<imfreq, matrix_valued> const &Gin, std::vector<std::vector<long>> degenerate_blocks) {
    auto Gsymm = Gin;
    auto mesh  = Gsymm[0].mesh();
    for (auto degenerate : degenerate_blocks) {
      auto n_deg = degenerate.size();
      auto dim   = Gin[degenerate[0]].target_shape()[0];
      auto gtmp  = gf{mesh, {dim, dim}};
      for (auto deg_bl : degenerate) { gtmp += Gin[deg_bl] / n_deg; }
      for (auto deg_bl : degenerate) { Gsymm[deg_bl] = gtmp; }
    }
    return Gsymm;
  }

} // namespace triqs::modest
