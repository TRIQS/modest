#include "./common.hpp"
#include "triqs_modest/loaders.hpp"

using namespace triqs;
using namespace triqs::modest;

auto test_compute_hloc(std::string filename) {
  auto [_, obe]  = one_body_elements_from_dft_converter(filename);
  auto hloc_0    = obe.C_space.atomic_view(modest::impurity_levels(obe));
  auto hloc0_ref = sort_keys_as_int(h5::proxy{filename, 'r'}["ref_data"]["hloc0"])
     | stdv::transform([](auto const &g) { return to_vector<cmat_t>(sort_keys_as_int(g)); }) | tl::to<std::vector>();
  auto U = obe.C_space.rotation_from_dft_to_local_basis();
  for (auto [a, s] : hloc_0.indices()) { EXPECT_ARRAY_NEAR(Udag_X_U(U(a, s), hloc_0(a, s)), hloc0_ref[a][s], 1.e-14); }
};

TEST(hloc0_tests, liv2o4_r3m_strained_wien2k) { // NOLINT
  test_compute_hloc("ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5");
}

TEST(hloc0_tests, compute_hloc0_w90_fixed_grid) { // NOLINT
  auto h5_files = {"ref_data_lfs/la327-1313-p30-w90-fixedgrid.ref.h5", "ref_data_lfs/la327-2222-strain-w90-fixedgrid.ref.h5"};
  for (auto &&file : h5_files) test_compute_hloc(file);
}

TEST(hloc0_tests, compute_hloc0_wien2k_fixed_grid) { // NOLINT
  auto h5_files = {"ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5", "ref_data/svo-wien2k.ref.h5"};
  for (auto &&file : h5_files) test_compute_hloc(file);
}

TEST(hloc0_tests, compute_hloc0_vasp_fixed_grid) { // NOLINT
  auto h5_files = {"ref_data/prnio3-vasp.ref.h5", "ref_data/lunio3-vasp.ref.h5"};
  for (auto &&file : h5_files) test_compute_hloc(file);
}

// // HL: need to check this test more carefully!
// TEST(hloc0_tests, compute_hloc0_vasp_nio) { // NOLINT
//   test_compute_hloc("ref_data/nio.ref.h5");
// }

//------------------------------------------------
// SOC test: fails because of merge of rotations
//------------------------------------------------
// TEST(hloc0_tests, compute_hloc0_wien2k_soc_fixed_grid) { // NOLINT
//   auto h5_files = {"ref_data_lfs/sriro3-wien2k-soc.ref.h5", "ref_data_lfs/sr2mgoso6-wien2k-soc.ref.h5"};
//   for (auto &&file : h5_files) test_compute_hloc(file);
// }
