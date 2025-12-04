// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "./common.hpp"
#include "triqs_modest/loaders.hpp"

using namespace triqs::modest;

struct MpiTest : public ::testing::Test {
  protected:
  mpi::communicator comm = {};
  int root               = 0;
};

TEST_F(MpiTest, BroadcastOneBodyElementsOnGrid) {
  double target_density = 0;
  one_body_elements_on_grid obe;

  // Load data only on root
  if (comm.rank() == root) { std::tie(target_density, obe) = one_body_elements_from_dft_converter("ref_data/SrVO3-cubic-t2g.ref.h5"); }

  // Broadcast to all ranks
  mpi::broadcast(target_density, comm, root);
  mpi::broadcast(obe, comm, root);

  // Load reference on all ranks and compare
  auto [ref_density, ref_obe] = one_body_elements_from_dft_converter("ref_data/SrVO3-cubic-t2g.ref.h5");

  EXPECT_DOUBLE_EQ(target_density, ref_density);
  EXPECT_EQ(obe, ref_obe);
}

MPI_TEST_MAIN
