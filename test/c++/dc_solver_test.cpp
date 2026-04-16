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
  auto double_counting = dc_solver(obe.C_space.spin_kind(), method, 2.0, 1.0);
  auto Sigma_DC        = Gimp | stdv::transform([&double_counting](auto &x) { return double_counting.dc_self_energy(x); }) | tl::to<std::vector>();
  auto E_DC            = Gimp | stdv::transform([&double_counting](auto &x) { return double_counting.dc_energy(x); }) | tl::to<std::vector>();
  // HL : need to loop over all of the data but the reference data has a different shape. TODO: update the reference data
  EXPECT_NEAR(real(Sigma_DC[0][0](0, 0)), dc_ref[0][0](0, 0), 1.e-5);
  EXPECT_NEAR(E_DC[0], Edc_ref[0], 1.e-5);
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

//--------------------------------
// Hardcoded-value tests for dc_formulas
//--------------------------------
TEST(dc_solver_tests, dc_formulas_cFLL) { // NOLINT
  // N_tot=4, N_sigma=2, n_orb=5, U=4, J=1
  // DC_val = U*(N_tot-0.5) - 0.5*J*(N_tot-1) = 4*3.5 - 0.5*1*3 = 12.5
  // E_val  = 0.5*U*N_tot*(N_tot-1) - 0.5*J*N_tot*(0.5*N_tot-1) = 24 - 2 = 22
  auto [DC_val, E_val] = dc_formulas("cFLL", 4.0, 2.0, 5, 4.0, 1.0);
  EXPECT_NEAR(DC_val, 12.5, 1.e-12);
  EXPECT_NEAR(E_val, 22.0, 1.e-12);
}

TEST(dc_solver_tests, dc_formulas_sFLL) { // NOLINT
  // N_tot=4, N_sigma=2.5, n_orb=5, U=4, J=1
  // Mag = 2*2.5 - 4 = 1
  // DC_val = U*(N_tot-0.5) - J*(N_sigma-0.5) = 14 - 2 = 12
  // E_val  = 24 - 2 - 0.25*J*Mag^2 = 21.75
  auto [DC_val, E_val] = dc_formulas("sFLL", 4.0, 2.5, 5, 4.0, 1.0);
  EXPECT_NEAR(DC_val, 12.0, 1.e-12);
  EXPECT_NEAR(E_val, 21.75, 1.e-12);
}

TEST(dc_solver_tests, dc_formulas_cAMF) { // NOLINT
  // N_tot=4, N_sigma=2, n_orb=5, U=4, J=1
  // L_orbit = 0.5*(5-1) = 2
  // C = (U + 2*J*L_orbit) / (2*L_orbit + 1) = 8/5 = 1.6
  // DC_val = U*N_tot - 0.5*N_tot*C = 16 - 3.2 = 12.8
  // E_val  = (0.5*U - 0.25*C)*N_tot^2 = 1.6*16 = 25.6
  auto [DC_val, E_val] = dc_formulas("cAMF", 4.0, 2.0, 5, 4.0, 1.0);
  EXPECT_NEAR(DC_val, 12.8, 1.e-12);
  EXPECT_NEAR(E_val, 25.6, 1.e-12);
}

TEST(dc_solver_tests, dc_formulas_sAMF) { // NOLINT
  // N_tot=4, N_sigma=2.5, n_orb=5, U=4, J=1
  // L_orbit = 2, C_factor = 8/5 = 1.6, Mag = 2*2.5 - 4 = 1
  // DC_val = U*N_tot - C_factor*N_sigma = 16 - 4 = 12
  // E_val  = 0.5*U*N_tot^2 - 0.25*C_factor*N_tot^2 - 0.25*C_factor*Mag^2 = 32 - 6.4 - 0.4 = 25.2
  auto [DC_val, E_val] = dc_formulas("sAMF", 4.0, 2.5, 5, 4.0, 1.0);
  EXPECT_NEAR(DC_val, 12.0, 1.e-12);
  EXPECT_NEAR(E_val, 25.2, 1.e-12);
}

TEST(dc_solver_tests, dc_formulas_cHeld) { // NOLINT
  // N_tot=4, N_sigma=2, n_orb=5, U=4, J=1
  // U_mean = (4 + 4*(4-2) + 4*(4-3)) / 9 = (4+8+4)/9 = 16/9
  // DC_val = 16/9 * 3.5 = 56/9
  // E_val  = 0.5 * 16/9 * 4 * 3 = 96/9
  auto [DC_val, E_val] = dc_formulas("cHeld", 4.0, 2.0, 5, 4.0, 1.0);
  EXPECT_NEAR(DC_val, 56.0 / 9.0, 1.e-12);
  EXPECT_NEAR(E_val, 96.0 / 9.0, 1.e-12);
}

//--------------------------------
// Density matrix overload tests
//--------------------------------
TEST(dc_solver_tests, density_matrix_overload) { // NOLINT
  // Create a density matrix with known traces: 1 block, 2 spins, 5 orbitals each
  // N_up = 2.0, N_down = 2.0 => N_tot = 4.0
  auto density_matrix = nda::array<nda::matrix<dcomplex>, 2>(1, 2);
  density_matrix(0, 0) = 0.4 * nda::eye<dcomplex>(5); // trace = 2.0
  density_matrix(0, 1) = 0.4 * nda::eye<dcomplex>(5); // trace = 2.0

  auto solver = dc_solver(spin_kind_e::NonPolarized, "cFLL", 4.0, 1.0);
  auto Sigma_DC = solver.dc_self_energy(density_matrix);
  auto E_DC     = solver.dc_energy(density_matrix);

  // NonPolarized: N_per_sigma = N_tot/2 = 2.0 for both spins
  // cFLL: DC_val = 12.5, E_val = 22.0
  // vector layout: [bl + sigma * n_blocks], n_blocks=1 => [0]=sigma0, [1]=sigma1
  EXPECT_NEAR(real(Sigma_DC[0](0, 0)), 12.5, 1.e-12);
  EXPECT_NEAR(real(Sigma_DC[1](0, 0)), 12.5, 1.e-12);
  EXPECT_NEAR(E_DC, 22.0, 1.e-12);
}

//--------------------------------
// Polarized vs NonPolarized sFLL test (bug fix validation)
//--------------------------------
TEST(dc_solver_tests, polarized_vs_nonpolarized_sFLL) { // NOLINT
  // N_up=2.5, N_down=1.5 => N_tot=4.0
  auto density_matrix = nda::array<nda::matrix<dcomplex>, 2>(1, 2);
  density_matrix(0, 0) = 0.5 * nda::eye<dcomplex>(5);  // trace = 2.5
  density_matrix(0, 1) = 0.3 * nda::eye<dcomplex>(5);  // trace = 1.5

  // NonPolarized: overrides N_per_sigma to N_tot/2 = 2.0 for both spins
  // => sFLL becomes identical to cFLL: DC_val = 12.5 for both spins
  auto solver_np = dc_solver(spin_kind_e::NonPolarized, "sFLL", 4.0, 1.0);
  auto Sigma_np  = solver_np.dc_self_energy(density_matrix);
  auto E_np      = solver_np.dc_energy(density_matrix);
  EXPECT_NEAR(real(Sigma_np[0](0, 0)), 12.5, 1.e-12);
  EXPECT_NEAR(real(Sigma_np[1](0, 0)), 12.5, 1.e-12);
  EXPECT_NEAR(E_np, 22.0, 1.e-12);

  // Polarized: preserves per-spin densities
  // Spin up:   N_sigma=2.5 => DC_val = U*(N_tot-0.5) - J*(N_sigma-0.5) = 14 - 2 = 12
  // Spin down: N_sigma=1.5 => DC_val = U*(N_tot-0.5) - J*(N_sigma-0.5) = 14 - 1 = 13
  auto solver_p = dc_solver(spin_kind_e::Polarized, "sFLL", 4.0, 1.0);
  auto Sigma_p  = solver_p.dc_self_energy(density_matrix);
  auto E_p      = solver_p.dc_energy(density_matrix);
  EXPECT_NEAR(real(Sigma_p[0](0, 0)), 12.0, 1.e-12);
  EXPECT_NEAR(real(Sigma_p[1](0, 0)), 13.0, 1.e-12);

  // Polarized sFLL energy: Mag = 2*N_up - N_tot = 1, E = 24 - 2 - 0.25*1 = 21.75
  EXPECT_NEAR(E_p, 21.75, 1.e-12);

  // Verify that Polarized and NonPolarized give DIFFERENT DC self-energies for sFLL
  EXPECT_GT(std::abs(Sigma_p[0](0, 0) - Sigma_np[0](0, 0)), 0.1);
}

//--------------------------------
// NonColinear spin kind test
//--------------------------------
TEST(dc_solver_tests, noncolinear_cFLL) { // NOLINT
  // NonColinear: 1 spin channel, orbitals include spin => n_orb_matrix = 10 for 5 orbitals
  // density_matrix has shape [1, 1] with a 10x10 matrix (spin folded into orbital index)
  // trace = 4.0 => N_total = 4.0, N_per_sigma = N_total/2 = 2.0, n_orb = 10/2 = 5
  auto density_matrix = nda::array<nda::matrix<dcomplex>, 2>(1, 1);
  density_matrix(0, 0) = 0.4 * nda::eye<dcomplex>(10); // trace = 4.0

  auto solver = dc_solver(spin_kind_e::NonColinear, "cFLL", 4.0, 1.0);
  auto Sigma_DC = solver.dc_self_energy(density_matrix);
  auto E_DC     = solver.dc_energy(density_matrix);

  // cFLL with N_tot=4, N_sigma=2, n_orb=5, U=4, J=1: DC_val=12.5, E_val=22.0
  EXPECT_NEAR(real(Sigma_DC[0](0, 0)), 12.5, 1.e-12);
  EXPECT_NEAR(E_DC, 22.0, 1.e-12);
  // Vector has only 1 entry (1 block * 1 spin channel)
  EXPECT_EQ(Sigma_DC.size(), 1u);
}

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