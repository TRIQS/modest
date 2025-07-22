#pragma once

#include <chrono>
#include <iostream>

namespace triqs {

  /**
   * @class scoped_timer
   * @brief A utility class for measuring the execution time of a scope.
   *
   * The `scoped_timer` class starts a timer upon construction and stops it
   * upon destruction, printing the elapsed time to the standard output.
   * It is useful for profiling and measuring the performance of specific
   * code blocks.
   *
   * Example usage:
   * @code
   * {
   *     scoped_timer timer; // Timer starts here
   *     // Code to measure
   * } // Timer stops and prints elapsed time here
   * @endcode
   */
  class scoped_timer {
    std::chrono::time_point<std::chrono::high_resolution_clock> start;

    public:
    scoped_timer() : start(std::chrono::high_resolution_clock::now()) {}

    ~scoped_timer() {
      auto end                              = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> elapsed = end - start;
      std::cerr << "Execution time: " << elapsed.count() << " seconds\n";
    }
  };
} // namespace triqs