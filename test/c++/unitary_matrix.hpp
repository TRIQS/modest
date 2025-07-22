#pragma once
#include <nda/nda.hpp>
using dcomplex = std::complex<double>;
namespace triqs::modest {

  inline void Udag_X_U_in_place(auto const &U, nda::matrix_view<dcomplex> M) { M = dagger(U) * M * U; }
  inline void U_X_Udag_in_place(auto const &U, nda::matrix_view<dcomplex> M) { M = U * M * dagger(U); }

  inline void Udag_X_U_in_place(auto const &U, nda::array_view<dcomplex, 3> M) {
    auto _a_ = nda::range::all;
    for (auto i : nda::range(M.extent(0))) Udag_X_U_in_place(U, M(i, _a_, _a_));
  }

  inline void U_X_Udag_in_place(auto const &U, nda::array_view<dcomplex, 3> M) {
    auto _a_ = nda::range::all;
    for (auto i : nda::range(M.extent(0))) U_X_Udag_in_place(U, M(i, _a_, _a_));
  }

  inline nda::matrix<dcomplex> Udag_X_U(auto const &U, nda::matrix<dcomplex> M) {
    Udag_X_U_in_place(U, M);
    return M;
  }
  inline nda::matrix<dcomplex> U_X_Udag(auto const &U, nda::matrix<dcomplex> M) {
    U_X_Udag_in_place(U, M);
    return M;
  }

} // namespace triqs::modest
