// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <vector>

namespace triqs {

  template <typename T> auto to_vector(auto &&gen) {
    std::vector<T> res;
    for (auto &&x : gen)
      if constexpr (requires { as<T>(x); })
        res.push_back(as<T>(x));
      else
        res.push_back(T{x});
    return res;
  }

} // namespace triqs