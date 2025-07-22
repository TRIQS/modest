#pragma once
#include <ranges>

namespace triqs {

  /**
   * @brief Determines if all elements in the given range are equal.
   * 
   * @tparam R A type that satisfies the std::ranges::input_range concept.
   * @param r The input range to check.
   * @return  True if all elements in the range are equal, false otherwise.  
   * 
   * @details This function uses std::ranges::adjacent_find with a predicate 
   *          that checks for inequality. If no such adjacent elements are 
   *          found, it implies all elements in the range are equal.
   */
  template <std::ranges::input_range R> bool all_equal(R const &r) {
    return std::ranges::adjacent_find(r, std::not_equal_to{}) == std::ranges::end(r);
  }
} // namespace triqs
