#pragma once
#include <nda/nda.hpp>
#include <triqs/utility/generator.hpp>

using itertools::range;

namespace triqs {
  /**
   * @brief 
   * 
   * @param sub_div List of block dimensions as longs (vector, range, generator) 
   * @return Generator of (block dim, corresponding range in the matrix)
   */
  inline generator<std::pair<long, nda::range>> enumerated_sub_slices(auto sub_div) {
    long m_start = 0, n = 0;
    for (auto p : sub_div) {
      co_yield {n, {m_start, m_start + p}};
      m_start += p;
      ++n;
    }
  }
} // namespace triqs