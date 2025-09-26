// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "../dft_code_specific.hpp"
#include <stdexcept>

namespace triqs::modest::dft_code {

  nda::matrix<dcomplex> get_spherical_to_dft_rotation_Wien2k(long l) {
    if (l == 0) throw std::runtime_error("No l=0 implementation");

    auto sqrt2 = std::numbers::sqrt2;
    auto I     = dcomplex(0, 1.0);
    auto size  = 2 * l + 1;
    auto Ylm   = nda::zeros<dcomplex>(size, size);
    // l = 1 : "x", "y", "z"
    if (l == 1) {
      Ylm(0, 0) = 1.0 / sqrt2;
      Ylm(1, 0) = I / sqrt2;
      Ylm(2, 1) = 1.0;
      Ylm(0, 2) = -1.0 / sqrt2;
      Ylm(1, 2) = I / sqrt2;
    }
    // l = 2: "z^2", "x^2-y^2", "xy", "yz", "xz"
    else if (l == 2) {
      Ylm(0, 2) = 1.0;
      Ylm(1, 0) = 1.0 / sqrt2;
      Ylm(2, 0) = -1.0 / sqrt2;
      Ylm(3, 1) = 1.0 / sqrt2;
      Ylm(4, 1) = 1.0 / sqrt2;
      Ylm(1, 4) = 1.0 / sqrt2;
      Ylm(2, 4) = 1.0 / sqrt2;
      Ylm(3, 3) = -1.0 / sqrt2;
      Ylm(4, 3) = 1.0 / sqrt2;
    }
    // l = 3: "x(x^2-3y^2)","z(x^2-y^2)","xz^2","z^3","yz^2","xyz","y(3x^2-y^2)"
    else if (l == 3) {
      Ylm(0, 0) = 1.0 / sqrt2;
      Ylm(1, 1) = 1.0 / sqrt2;
      Ylm(2, 2) = 1.0 / sqrt2;
      Ylm(0, 6) = 1.0 / sqrt2;
      Ylm(1, 5) = 1.0 / sqrt2;
      Ylm(2, 4) = 1.0 / sqrt2;
      Ylm(3, 3) = 1.0;
      Ylm(4, 2) = I / sqrt2;
      Ylm(5, 1) = I / sqrt2;
      Ylm(6, 0) = I / sqrt2;
      Ylm(4, 4) = I / sqrt2;
      Ylm(5, 5) = -I / sqrt2;
      Ylm(6, 6) = I / sqrt2;
    }
    return Ylm;
  }
} // namespace triqs::modest::dft_code