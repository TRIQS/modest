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
  // implicity reads on rank 0 and broadcasts
  auto [target_density, obe] = one_body_elements_from_dft_converter("ref_data/SrVO3-cubic-t2g.ref.h5");
  // read on all ranks the data
  auto [ref_density, ref_obe] = read_obe_from_dft_converter_hdf5("ref_data/SrVO3-cubic-t2g.ref.h5");
  EXPECT_DOUBLE_EQ(target_density, ref_density);
  EXPECT_EQ(obe, ref_obe);
}

TEST_F(MpiTest, BroadcastOneBodyElementsOnGridWithHighSymmPath) {
  std::string const filename = "ref_data/SrVO3-cubic-t2g.ref.h5";
  auto [target_density, obe] = one_body_elements_from_dft_converter(filename);
  // implicity reads on rank 0 and broadcasts
  auto obe2 = one_body_elements_on_high_symmetry_path(filename, obe);
  // read on all ranks the data
  auto ref_obe_bands = read_data_on_high_symm_path_for_obe(filename, obe);
  EXPECT_EQ(obe2, ref_obe_bands);
}

TEST_F(MpiTest, BroadcastOneBodyElementsOnGridWithThetaProjectors) {
  std::string const filename = "ref_data/SrVO3-cubic-t2g.ref.h5";
  auto [target_density, obe] = one_body_elements_from_dft_converter(filename);
  // implicity reads on rank 0 and broadcasts
  auto obe2 = one_body_elements_with_theta_projectors(filename, obe);
  // read on all ranks the data
  auto ref_obe = read_theta_projectors_for_obe(filename, obe);
  EXPECT_EQ(obe2, ref_obe);
}

MPI_TEST_MAIN
