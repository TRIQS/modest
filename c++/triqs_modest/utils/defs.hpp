#pragma once
#include <cassert>
#include <fmt/core.h>
#include <nda/nda.hpp>
#include <triqs/gfs.hpp>
#include "./range_supp.hpp"

#define C2PY_IGNORE __attribute__((annotate("c2py_ignore")))
//#include <c2py/user_api.hpp>

namespace stdv = std::views;
namespace stdr = std::ranges;
using namespace std::literals;

namespace triqs {

  // To be cleaned
  using fmt::format;
  using gfs::block2_gf;
  using gfs::block_gf;
  using gfs::gf;
  using gfs::gf_struct_t;
  using gfs::make_block2_gf;
  using gfs::matrix_valued;
  using itertools::enumerate;
  using itertools::zip; // OR STD ?

  using cmat_t                = nda::matrix<dcomplex>;
  using cmat_vt               = nda::matrix_view<dcomplex>;
  static constexpr auto r_all = nda::range::all;

} // namespace triqs