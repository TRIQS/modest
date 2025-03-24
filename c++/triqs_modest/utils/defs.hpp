// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <cassert>
#include <fmt/core.h>
#include <tl/to.hpp>
#include <nda/nda.hpp>
#include <triqs/gfs.hpp>
#include "./range_supp.hpp"

#ifdef __clang__
#define C2PY_IGNORE __attribute__((annotate("c2py_ignore")))
#else
#define C2PY_IGNORE
#endif
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
