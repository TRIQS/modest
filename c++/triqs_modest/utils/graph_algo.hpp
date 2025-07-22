#pragma once
#include <algorithm>
#include <vector>
#include <deque>
#include <nda/nda.hpp>

namespace nda {
  /**
   * @brief Find a block structure of a Matrix 
   * 
   * @param m A matrix (typically abs(some_matrix)). 
   * @param threshold Any value below this threshold is considered as 0
   * @return List of the groups of rows/cols which would make the matrix block diagonal
   */
  std::vector<std::vector<int>> find_blocks(nda::matrix_view<double> m, double threshold) {
    auto n = m.extent(0);
    std::vector<bool> visited(n, false);
    std::vector<std::vector<int>> components; // the result
    using std::abs;

    std::vector<std::vector<int>> adjacency_list(n);
    for (int i = 0; i < n; ++i)
      for (int j = 0; j < n; ++j)
        if (abs(m(i, j)) > threshold) adjacency_list[i].push_back(j);

    auto bfs = [&](int node) {
      std::deque<int> queue = {node};
      std::vector<int> component;
      while (!queue.empty()) {
        int curr = queue.front();
        queue.pop_front();
        if (!visited[curr]) {
          visited[curr] = true;
          component.push_back(curr);
          for (int neighbor : adjacency_list[curr]) {
            if (!visited[neighbor]) queue.push_back(neighbor);
          }
        }
      }
      return component;
    };

    for (int i = 0; i < n; ++i)
      if (!visited[i]) components.push_back(bfs(i));

    // Sort each component
    for (auto &c : components) std::ranges::sort(c);
    return components;
  }
} // namespace nda