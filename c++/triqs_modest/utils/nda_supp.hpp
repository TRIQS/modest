#pragma once
#include <nda/nda.hpp>
using dcomplex = std::complex<double>;
namespace nda {

  CLEF_MAKE_FNT_LAZY(inverse);

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
} // namespace nda