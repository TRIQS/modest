#include "./common.hpp"
#include "triqs_modest/double_counting.hpp"
#include <gtest/gtest.h>
#include <optional>

auto run_dc_solver_test_case(std::string filename, double threshold, std::string const method) {
  auto root     = h5::proxy{filename, 'r'};
  auto gloc_ref = to_vector<block_gf<imfreq, matrix_valued>>(sort_keys_as_int(root["ref_data"]["Gloc_DFT"]));
  auto dc_ref   = sort_keys_as_int(root["ref_data"]["dc"][method]["dc_imp"])
     | stdv::transform([](auto const &g) { return to_vector<nda::matrix<double>>(sort_keys_as_int(g)); }) | tl::to<std::vector>();
  auto Edc_ref = to_vector<double>(sort_keys_as_int(root["ref_data"]["dc"][method]["E_dc"]));

  auto [_, obe]        = one_body_elements_from_dft_converter(filename, threshold);
  auto E               = make_embedding(obe.C_space);
  auto Gimp            = E.extract(gloc(gloc_ref[0][0].mesh(), obe, 0.0));
  auto double_counting = dc_solver(E.sigma_names(), method, 2.0, 1.0);
  auto Sigma_DC        = Gimp | stdv::transform([&double_counting](auto &x) { return double_counting.dc_self_energy(x); }) | tl::to<std::vector>();
  auto E_DC            = Gimp | stdv::transform([&double_counting](auto &x) { return double_counting.dc_energy(x); }) | tl::to<std::vector>();
  // HL : need to loop over all of the data but the reference data has a different shape. TODO: update the reference data
  EXPECT_NEAR(real(Sigma_DC[0][0](0)(0, 0)), dc_ref[0][0](0, 0), 1.e-5);
  EXPECT_NEAR(E_DC[0](0, 0), Edc_ref[0], 1.e-5);
}

//--------------------------------
// SrVO3 (Wien2k)
//--------------------------------
TEST(dc_solver_tests, dc_wien2k_svo_sFLL) { // NOLINT
  run_dc_solver_test_case("ref_data/svo-wien2k.ref.h5", 1.e-6, "sFLL");
}
TEST(dc_solver_tests, dc_wien2k_svo_sAMF) { // NOLINT
  run_dc_solver_test_case("ref_data/svo-wien2k.ref.h5", 1.e-6, "sAMF");
}
TEST(dc_solver_tests, dc_wien2k_svo_cHeld) { // NOLINT
  run_dc_solver_test_case("ref_data/svo-wien2k.ref.h5", 1.e-6, "cHeld");
}
//--------------------------------

//--------------------------------
// PrNiO3 (VASP)
//--------------------------------
TEST(dc_solver_tests, dc_vasp_prnio3_sFLL) { // NOLINT
  run_dc_solver_test_case("ref_data/prnio3-vasp.ref.h5", 1.e-4, "sFLL");
}
TEST(dc_solver_tests, dc_vasp_prnio3_sAMF) { // NOLINT
  run_dc_solver_test_case("ref_data/prnio3-vasp.ref.h5", 1.e-4, "sAMF");
}
TEST(dc_solver_tests, dc_vasp_prnio3_cHeld) { // NOLINT
  run_dc_solver_test_case("ref_data/prnio3-vasp.ref.h5", 1.e-4, "cHeld");
}

//--------------------------------
// LaVO3 (Wien2k)
//--------------------------------
TEST(dc_solver_tests, dc_vasp_lavo3_sFLL) { // NOLINT
  run_dc_solver_test_case("ref_data/LaVO3-Pnma-t2g.ref.h5", 1.e-4, "sFLL");
}
TEST(dc_solver_tests, dc_vasp_lavo3_sAMF) { // NOLINT
  run_dc_solver_test_case("ref_data/LaVO3-Pnma-t2g.ref.h5", 1.e-4, "sAMF");
}
TEST(dc_solver_tests, dc_vasp_lavo3_cHeld) { // NOLINT
  run_dc_solver_test_case("ref_data/LaVO3-Pnma-t2g.ref.h5", 1.e-4, "cHeld");
}
//--------------------------------

//--------------------------------
// La5Ni3O11 (Wien2k) (large nu)
//--------------------------------
TEST(dc_solver_tests, dc_wien2k_large_nu_sFLL) { //NOLINT
  run_dc_solver_test_case("ref_data/la5ni3o11-wien2k.ref.h5", 0.00001, "sFLL");
}
TEST(dc_solver_tests, dc_wien2k_large_nu_sAMF) { //NOLINT
  run_dc_solver_test_case("ref_data/la5ni3o11-wien2k.ref.h5", 0.00001, "sAMF");
}
TEST(dc_solver_tests, dc_wien2k_large_nu_cHeld) { //NOLINT
  run_dc_solver_test_case("ref_data/la5ni3o11-wien2k.ref.h5", 0.00001, "cHeld");
}

//--------------------------------
// LuNiO3 (VASP) (Fails?)
//--------------------------------
// TEST(dc_tests, dc_vasp_lunio3_sFLL) { // NOLINT
//   run_dc_test_case<triqs::mesh::imfreq>("lunio3-vasp.ref.h5", 1.e-4, "sFLL");
// }
// TEST(dc_tests, dc_vasp_lunio3_sAMF) { // NOLINT
//   run_dc_test_case<triqs::mesh::imfreq>("lunio3-vasp.ref.h5", 1.e-4, "sAMF");
// }
// TEST(dc_tests, dc_vasp_lunio3_cHeld) { // NOLINT
//   run_dc_test_case<triqs::mesh::imfreq>("lunio3-vasp.ref.h5", 1.e-4, "cHeld");
// }
//--------------------------------

#ifdef LFS
//--------------------------------
// LiV2O4 (Wien2k)
//--------------------------------
TEST(dc_solver_tests, dc_wien2k_liv2o4_sFLL) { // NOLINT
  run_dc_solver_test_case("ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5", 1.e-6, "sFLL");
}
TEST(dc_solver_tests, dc_wien2k_liv2o4_sAMF) { // NOLINT
  run_dc_solver_test_case("ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5", 1.e-6, "sAMF");
}
TEST(dc_solver_tests, dc_wien2k_liv2o4_cHeld) { // NOLINT
  run_dc_solver_test_case("ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5", 1.e-6, "cHeld");
}
//--------------------------------
#endif