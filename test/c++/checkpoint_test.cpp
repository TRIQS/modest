// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "./common.hpp"
#include "triqs_modest/checkpoint.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

namespace fs = std::filesystem;
using namespace triqs;

// ---------------------------------------------------------------------------
// Helper: minimal iteration_data (1x1 block GF, beta=1, 5 freqs)
// ---------------------------------------------------------------------------

iteration_data make_test_data(double mu) {
  auto mesh    = mesh::imfreq{1.0, triqs::mesh::Fermion, 5};
  auto sig     = gfs::gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>{mesh, {1, 1}};
  sig()        = 0.0;
  auto Sigma   = gfs::make_block_gf({"up"}, std::vector{sig});
  auto sig_h   = nda::matrix<dcomplex>{1, 1};
  sig_h()      = 0.0;
  auto Sigma_H = std::vector<nda::matrix<dcomplex>>{sig_h};
  return {.mu = mu, .Sigma_imp_list = {Sigma}, .Sigma_hartree_list = {Sigma_H}};
}

// ---------------------------------------------------------------------------
// Fixture: fresh temp directory per test
// ---------------------------------------------------------------------------

struct CheckpointTest : public ::testing::Test {
  std::string dir = "checkpoint_test_tmp";
  void SetUp() override { fs::remove_all(dir); }
  void TearDown() override { fs::remove_all(dir); }
};

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST_F(CheckpointTest, CreateNew) { // NOLINT
  auto chkpt = checkpoint<iteration_data>{dir};
  EXPECT_TRUE(chkpt.empty());
  EXPECT_EQ(chkpt.size(), 0);
  EXPECT_TRUE(fs::exists(dir));
  EXPECT_TRUE(fs::exists(dir + "/iterations.h5"));
}

TEST_F(CheckpointTest, AppendAndReadBack) { // NOLINT
  auto chkpt = checkpoint<iteration_data>{dir};
  for (auto &&val : {1.5, 2.5}) chkpt.append(make_test_data(val));

  EXPECT_EQ(chkpt.size(), 2);
  EXPECT_FALSE(chkpt.empty());

  EXPECT_NEAR(chkpt[0].mu, 1.5, 1e-10);
  EXPECT_NEAR(chkpt[1].mu, 2.5, 1e-10);

  EXPECT_EQ(chkpt[0].Sigma_imp_list.size(), 1u);
  EXPECT_EQ(chkpt[0].Sigma_hartree_list.size(), 1u);
}

TEST_F(CheckpointTest, NegativeIndexing) { // NOLINT
  auto chkpt = checkpoint<iteration_data>{dir};
  for (auto &&val : {1.5, 2.5}) chkpt.append(make_test_data(val));
  EXPECT_NEAR(chkpt[-1].mu, 2.5, 1e-10);
  EXPECT_NEAR(chkpt[-2].mu, 1.5, 1e-10);
}

TEST_F(CheckpointTest, Iterator) { // NOLINT
  auto chkpt = checkpoint<iteration_data>{dir};

  for (auto &&val : {1.0, 2.0, 3.0}) chkpt.append(make_test_data(val));

  std::vector<double> mus;
  for (auto const &it : chkpt) mus.push_back(it.mu);

  ASSERT_EQ(mus.size(), 3u);
  for (auto it : range(3)) EXPECT_NEAR(mus[it], (it + 1.0), 1e-10);
}

TEST_F(CheckpointTest, Last) { // NOLINT
  auto chkpt = checkpoint<iteration_data>{dir};
  for (auto &&val : {1.5, 2.5}) chkpt.append(make_test_data(val));
  EXPECT_NEAR(chkpt.last().mu, 2.5, 1e-10);
}

TEST_F(CheckpointTest, ReopenAndContinue) { // NOLINT

  auto chkpt = checkpoint<iteration_data>{dir};
  for (auto &&val : {1.5, 2.5}) chkpt.append(make_test_data(val));

  auto chkpt2 = checkpoint<iteration_data>{dir};
  EXPECT_EQ(chkpt2.size(), 2);
  EXPECT_NEAR(chkpt2[-1].mu, 2.5, 1e-10);
  EXPECT_NEAR(chkpt2[-2].mu, 1.5, 1e-10);

  chkpt2.append(make_test_data(3.5));
  EXPECT_EQ(chkpt2.size(), 3);
  EXPECT_NEAR(chkpt2[-1].mu, 3.5, 1e-10);
}

TEST_F(CheckpointTest, PathIsFileThrows) { // NOLINT
  // dirname points to a regular file, not a directory — must throw
  std::ofstream{dir}; // create a regular file at that path
  EXPECT_THROW((checkpoint<iteration_data>{dir}), std::runtime_error);
}
