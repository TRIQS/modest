#include "./common.hpp"
#include "triqs_modest/loaders.hpp"

using namespace triqs;

// --------------------------- NOTICE --------------------------------------
// The TRIQS/ModEST implementation of the density methods is more accurate than the
// TRIQS/dft_tools implementation. This is because we subtract off the high-frequency tails of the self-energy
// decreasing the error. Therefore, we test the density funciton based on values pre-computed at μ = 0.0 for
// n_iw = 30.

double random_constant() {
  std::mt19937 gen(std::random_device{}());
  std::uniform_real_distribution<double> dist(0, 1);
  return dist(gen);
}

std::tuple<double, one_body_elements_on_grid, block2_gf<dlr_imfreq, matrix_valued>, nda::array<nda::matrix<dcomplex>, 2>>
load_data_w_selfenergy(std::string filename, bool zero_self = false) {
  auto root                  = h5::proxy{filename, 'r'};
  auto [target_density, obe] = one_body_elements_from_dft_converter(filename);
  auto E                     = make_embedding(obe.C_space);
  auto eps                   = 1.e-12;
  auto w_max                 = 5.0 * nda::max_element(abs(obe.H.H_k)); // FIXME
  // std::cout << fmt::format("Sigma created with random DLR coefficents: w_max= {} eps= {}\n", w_max, eps);

  auto Sigma_imp_dlr = make_vec_block_gf(mesh::dlr_imfreq{40.0, triqs::mesh::statistic_enum::Fermion, w_max, eps}, E.imp_block_shape());
  auto const &mesh   = Sigma_imp_dlr[0][0].mesh();
  for (auto &bg : Sigma_imp_dlr) {
    for (auto &g : bg) {
      auto A = (zero_self) ? 0.0 : random_constant();
      auto B = (zero_self) ? 0.0 : random_constant();
      for (auto &&[n, iw] : enumerate(mesh)) { g.data()(n, r_all, r_all) = A + B / iw; }
    }
  }
  auto Sigma_static  = Sigma_imp_dlr | stdv::transform([](auto &x) { return std::get<0>(split_self_energy(x)); }) | tl::to<std::vector>();
  auto Sigma_dynamic = Sigma_imp_dlr | stdv::transform([](auto &x) { return std::get<1>(split_self_energy(x)); }) | tl::to<std::vector>();
  auto Sigma_embed   = E.embed(Sigma_dynamic);
  auto Sigma_H_embed = E.embed(Sigma_static);
  return {target_density, obe, Sigma_embed, Sigma_H_embed};
}

std::tuple<double, double, double> calc_density_test_case(std::string filename) {
  auto [target_density, obe, Sigma_embed, Sigma_H_embed] = load_data_w_selfenergy(filename, true);
  return {density_nk(obe, 0.0, 40.0), density(obe, 0.0, make_gf_imfreq<>(Sigma_embed, 30), Sigma_H_embed),
          density(obe, 0.0, Sigma_embed, Sigma_H_embed)};
};

auto root_finder_test_case(std::string filename) {
  auto [target_density, obe, Sigma_embed, Sigma_H_embed] = load_data_w_selfenergy(filename);

  auto beta = 10.0;

  auto mu1 = find_chemical_potential(target_density, obe, beta, "dichotomy", 1e-4, false);
  auto mu2 = find_chemical_potential(target_density, obe, beta, "bisection", 1e-4, false);
  EXPECT_NEAR(mu1, mu2, 1.e-3);

  EXPECT_NEAR(target_density, density_nk(obe, mu1, beta), 1.e-3);
  EXPECT_NEAR(target_density, density_nk(obe, mu2, beta), 1.e-3);

  auto mu3 = find_chemical_potential(target_density, obe, Sigma_embed, Sigma_H_embed, "dichotomy", 1e-4, false);
  auto mu4 = find_chemical_potential(target_density, obe, Sigma_embed, Sigma_H_embed, "bisection", 1e-4, false);
  EXPECT_NEAR(mu3, mu4, 1.e-3);

  EXPECT_NEAR(target_density, density(obe, mu3, Sigma_embed, Sigma_H_embed), 1.e-3);
  EXPECT_NEAR(target_density, density(obe, mu4, Sigma_embed, Sigma_H_embed), 1.e-3);
};

TEST(density_tests, density_prnio3_vasp) { // NOLINT
  auto nref           = 6.3047097;
  auto [n0, ns1, ns2] = calc_density_test_case("ref_data/prnio3-vasp.ref.h5");
  EXPECT_NEAR(n0, ns1, 1.e-12);
  EXPECT_NEAR(ns1, ns2, 1.e-12);
  EXPECT_NEAR(n0, nref, 1.e-5);
}

TEST(density_tests, root_prnio3_vasp) { // NOLINT
  root_finder_test_case("ref_data/prnio3-vasp.ref.h5");
}

TEST(density_tests, lunio3_vasp) { // NOLINT
  auto nref           = 3.911859;
  auto [n0, ns1, ns2] = calc_density_test_case("ref_data/lunio3-vasp.ref.h5");
  EXPECT_NEAR(n0, ns1, 1.e-12);
  EXPECT_NEAR(ns1, ns2, 1.e-12);
  EXPECT_NEAR(n0, nref, 1.e-5);
}

TEST(density_tests, root_lunio3_vasp) { // NOLINT
  root_finder_test_case("ref_data/lunio3-vasp.ref.h5");
}

TEST(density_tests, nio_wien2k) { // NOLINT
  auto nref           = 14.11070;
  auto [n0, ns1, ns2] = calc_density_test_case("ref_data/nio.ref.h5");
  EXPECT_NEAR(n0, ns1, 1.e-12);
  EXPECT_NEAR(ns1, ns2, 1.e-12);
  EXPECT_NEAR(n0, nref, 1.e-5);
}

TEST(density_tests, root_nio_wien2k) { // NOLINT
  root_finder_test_case("ref_data/nio.ref.h5");
}

TEST(density_tests, la5ni3o11_wien2k) { // NOLINT
  auto nref           = 88.674939;
  auto [n0, ns1, ns2] = calc_density_test_case("ref_data/la5ni3o11-wien2k.ref.h5");
  EXPECT_NEAR(n0, ns1, 1.e-12);
  EXPECT_NEAR(ns1, ns2, 1.e-12);
  EXPECT_NEAR(n0, nref, 1.e-5);
}

TEST(density_tests, root_la5ni3o11_wien2k) { // NOLINT
  root_finder_test_case("ref_data/la5ni3o11-wien2k.ref.h5");
}

TEST(density_tests, lco_w90_fixed) { // NOLINT
  auto nref           = 0.0;
  auto [n0, ns1, ns2] = calc_density_test_case("ref_data/lco_qe_dp_w90_grid.ref.h5");
  EXPECT_NEAR(n0, ns1, 1.e-12);
  EXPECT_NEAR(ns1, ns2, 1.e-12);
  EXPECT_NEAR(n0, nref, 1.e-5);
}

TEST(density_tests, root_lco_w90_fixed) { // NOLINT
  root_finder_test_case("ref_data/lco_qe_dp_w90_grid.ref.h5");
}

MPI_TEST_MAIN;
