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