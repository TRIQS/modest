// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

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
   * The print is gated by the `enabled` flag (default false), so the timer is
   * silent unless explicitly switched on. Under MPI the caller is the layer
   * that knows the rank context, so pass e.g. `comm.rank() == 0` to print only
   * on the master rank instead of once per rank.
   *
   * Example usage:
   * @code
   * {
   *     scoped_timer timer{comm.rank() == 0}; // Timer starts here, prints on rank 0 only
   *     // Code to measure
   * } // Timer stops and prints elapsed time here
   * @endcode
   */
  class scoped_timer {
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    bool enabled;

    public:
    scoped_timer(bool enabled = false) : start(std::chrono::high_resolution_clock::now()), enabled(enabled) {}

    ~scoped_timer() {
      if (!enabled) return;
      auto end                              = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> elapsed = end - start;
      std::cerr << "Execution time: " << elapsed.count() << " seconds\n";
    }
  };
} // namespace triqs