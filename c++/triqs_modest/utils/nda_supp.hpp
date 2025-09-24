// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <nda/nda.hpp>
using dcomplex = std::complex<double>;
namespace nda {

  // FIXME : to be replace by linalg
  inline void Ainv_B(nda::matrix<dcomplex, nda::F_layout> A, nda::matrix_view<dcomplex, nda::F_layout> B) {
    nda::vector<int> ipiv(A.extent(0)); //
    lapack::getrf(A, ipiv);             // Perform LU factorization of A
    lapack::getrs(A, B, ipiv);          // Solve AX = B (X = A^{-1} B)
  }

  // M(p[j], j) = 1 for all j
  template <typename T> nda::matrix<T> make_matrix_from_permutation(std::vector<int> const &p) {
    int N    = int(p.size());
    auto res = nda::matrix<T>::zeros(N, N);
    for (int j = 0; j < N; ++j) res(p[j], j) = 1;
    return res;
  }

  template <typename T> std::vector<T> flatten(const std::vector<std::vector<T>> &nested) {
    std::vector<T> flat;
    for (const auto &sub : nested) { flat.insert(flat.end(), sub.begin(), sub.end()); }
    return flat;
  }

  template <typename T> bool is_diagonal(nda::matrix<T> const &M) {
    auto dim = M.shape()[0];
    for (size_t i = 0; i < dim; i++)
      for (size_t j = i + 1; j < dim; j++)
        if (abs(M(i, j)) > 1.e-14) return false;
    return true;
  }

  template <typename T> nda::array<T, 2> block_diag(std::vector<nda::matrix<T>> const &matrices) {
    auto n = stdr::fold_left(matrices | stdv::transform([](auto const &m) { return m.extent(0); }) | tl::to<std::vector>(), 0, std::plus<>());
    auto U = nda::zeros<T>(n, n);
    auto i = 0;
    for (auto const &M : matrices) {
      auto s                                        = M.extent(0);
      U(nda::range(i, i + s), nda::range(i, i + s)) = M;
      i += s;
    }
    return U;
  }

} // namespace nda