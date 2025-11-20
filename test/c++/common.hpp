#pragma once
#include <ranges>
#include <ostream>
#include <gtest/gtest.h>

#include "triqs_modest/utils/h5_proxy.hpp"
#include "triqs_modest/utils/to_vector.hpp"
#include <triqs_modest/utils/nda_pretty_printer.hpp>

#include "triqs_modest/downfolding.hpp"
#include "triqs_modest/loaders.hpp"
#include "triqs_modest/embedding.hpp"
#include "triqs_modest/gloc_fixed_grid.hpp"
#include "triqs_modest/density.hpp"

#include "triqs_modest/utils/enumerate_slice.hpp"
#include "triqs_modest/utils/gf_supp.hpp"
#include "triqs_modest/double_counting.hpp"

#include <itertools/enumerate.hpp>
#include <triqs/test_tools/gfs.hpp>

#include "unitary_matrix.hpp"

using namespace triqs::modest;

template <typename Mesh>
std::pair<std::vector<nda::matrix<dcomplex>>, block_gf<Mesh, matrix_valued>> split_self_energy(block_gf<Mesh, matrix_valued> &Sigma) {
  auto Sigma_hartree = std::vector<nda::matrix<dcomplex>>{};
  auto Sigma_dynamic = Sigma;
  auto const &mesh   = Sigma_dynamic[0].mesh();
  for (auto &&[sig_full, sig_dyn] : zip(Sigma, Sigma_dynamic)) {
    auto S_H = nda::matrix<dcomplex>{sig_full(0)};
    for (auto n : range(mesh.size())) { sig_dyn.data()(n, r_all, r_all) -= S_H; } // FIXME: overload -= for dlr gf
    Sigma_hartree.emplace_back(S_H);
  }
  return {Sigma_hartree, Sigma_dynamic};
}

// NOTE: mpi bcast is important here, otherwise each process generates its own random number;
// this is problematic if one tests with MPI and possibily OMP
double random_constant() {
  double rc = 0;
  mpi::communicator world;
  if (world.rank() == 0) { // if this is head proc, generate number
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(0, 1);
    rc = dist(gen);
  }
  mpi::broadcast(rc);
  return rc;
}

namespace triqs {
  template <typename Mesh> std::vector<block_gf<Mesh, matrix_valued>> make_vec_block_gf(Mesh const &mesh, std::vector<gf_struct_t> const &s_vec) {
    auto r = std::vector<block_gf<Mesh, matrix_valued>>{};
    for (auto const &estruct : s_vec) r.emplace_back(block_gf<Mesh, matrix_valued>{mesh, estruct});
    return r;
  }

  // ---------------------------------------

  template <typename Mesh> generator<std::pair<long, long>> indices(block2_gf<Mesh, matrix_valued> const &g) {
    for (auto a : range(g.size1()))
      for (auto b : range(g.size2())) co_yield {a, b};
  }

} // namespace triqs
