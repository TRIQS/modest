#include "./common.hpp"
#include "triqs_modest/loaders.hpp"

using namespace triqs;

double random_constant() {
  std::mt19937 gen(std::random_device{}());
  std::uniform_real_distribution<double> dist(0, 1);
  return dist(gen);
}

// FIXME
std::tuple<double, one_body_elements_on_grid, block2_gf<dlr_imfreq, matrix_valued>, nda::array<nda::matrix<dcomplex>, 2>>
load_data_w_selfenergy(std::string filename) {
  auto root                  = h5::proxy{filename, 'r'};
  auto [target_density, obe] = one_body_elements_from_dft_converter(filename);
  auto E                     = make_embedding_with_equivalences(obe.C_space);
  auto eps                   = 1.e-12;
  auto w_max                 = 5.0 * nda::max_element(abs(obe.H.H_k)); // FIXME
  // std::cout << fmt::format("Sigma created with random DLR coefficents: w_max= {} eps= {}\n", w_max, eps);
  auto Sigma_imp_dlr = make_vec_block_gf(mesh::dlr_imfreq{40.0, triqs::mesh::statistic_enum::Fermion, w_max, eps}, E.imp_block_shape());
  auto const &mesh   = Sigma_imp_dlr[0][0].mesh();
  for (auto &bg : Sigma_imp_dlr) {
    for (auto &g : bg) {
      auto A = random_constant();
      auto B = random_constant();
      for (auto &&[n, iw] : enumerate(mesh)) { g.data()(n, r_all, r_all) = A + B / iw; }
    }
  }
  auto Sigma_static  = Sigma_imp_dlr | stdv::transform([](auto &x) { return std::get<0>(split_self_energy(x)); }) | tl::to<std::vector>();
  auto Sigma_dynamic = Sigma_imp_dlr | stdv::transform([](auto &x) { return std::get<1>(split_self_energy(x)); }) | tl::to<std::vector>();
  auto Sigma_embed   = E.embed(Sigma_dynamic);
  auto Sigma_H_embed = E.embed(Sigma_static);
  return {target_density, obe, Sigma_embed, Sigma_H_embed};
}

auto calc_density_test_case(std::string filename) {
  auto [target_density, obe, Sigma_embed, Sigma_H_embed] = load_data_w_selfenergy(filename);
  auto n_wood                                            = density(obe, 0.0, Sigma_embed, Sigma_H_embed);
  auto n_slow                                            = density_slow(obe, 0.0, make_gf_imfreq<>(Sigma_embed, 250), Sigma_H_embed);
  EXPECT_NEAR(n_slow, n_wood, 1.e-2);
  // std::cout << fmt::format("n_wood(dlr)= {}\n", n_wood);
  //   auto n_slow = density_slow(obe, 0.0, Sigma_embed);
  //std::cout << fmt::format("n_slow(dlr)= {}\n", n_slow);
  // for (auto niw : std::vector<long>{50, 250, 500}) { //, 4096, 8192}) {
  //   std::cout << fmt::format("n_slow(iwn)= {} rel.error (iwn-dlr)/iwn = {:.6f}% (n_iw={})\n", n_slow_full,
  //                            std::abs((n_wood - n_slow_full) / n_wood) * 100, niw);
  // }
  // std::cout << fmt::format("n_wood(dlr)= {}\n", n_wood);
  //   EXPECT_NEAR(n_slow_full_mesh, n_ref, 1.e-10); // PASSES
  //   EXPECT_NEAR(n_slow, n_ref, 1.e-4);            // FAILS
};

auto root_finder_test_case(std::string filename) {
  auto [target_density, obe, Sigma_embed, Sigma_H_embed] = load_data_w_selfenergy(filename);

  auto beta = 10.0;

  auto mu1 = find_chemical_potential(target_density, obe, beta, "dichotomy", 1e-2, false);
  auto mu2 = find_chemical_potential(target_density, obe, beta, "bisection", 1e-2, false);
  EXPECT_NEAR(mu1, mu2, 1.e-2);

  EXPECT_NEAR(target_density, density_nk(obe, mu1, beta), 1.e-2);
  EXPECT_NEAR(target_density, density_nk(obe, mu2, beta), 1.e-2);

  auto mu3 = find_chemical_potential(target_density, obe, Sigma_embed, Sigma_H_embed, "dichotomy", 1e-2, false);
  auto mu4 = find_chemical_potential(target_density, obe, Sigma_embed, Sigma_H_embed, "bisection", 1e-2, false);
  EXPECT_NEAR(mu3, mu4, 1.e-2);

  EXPECT_NEAR(target_density, density_slow(obe, mu3, Sigma_embed, Sigma_H_embed), 1.e-2);
  EXPECT_NEAR(target_density, density_slow(obe, mu4, Sigma_embed, Sigma_H_embed), 1.e-2);
};

TEST(density_tests, density_prnio3_vasp) { // NOLINT
  calc_density_test_case("ref_data/prnio3-vasp.ref.h5");
}

TEST(density_tests, root_prnio3_vasp) { // NOLINT
  root_finder_test_case("ref_data/prnio3-vasp.ref.h5");
}

TEST(density_tests, lunio3_vasp) { // NOLINT
  calc_density_test_case("ref_data/lunio3-vasp.ref.h5");
}

TEST(density_tests, root_lunio3_vasp) { // NOLINT
  root_finder_test_case("ref_data/lunio3-vasp.ref.h5");
}

TEST(density_tests, nio_wien2k) { // NOLINT
  calc_density_test_case("ref_data/nio.ref.h5");
}

TEST(density_tests, root_nio_wien2k) { // NOLINT
  root_finder_test_case("ref_data/nio.ref.h5");
}

TEST(density_tests, la5ni3o11_wien2k) { // NOLINT
  calc_density_test_case("ref_data/la5ni3o11-wien2k.ref.h5");
}

TEST(density_tests, root_la5ni3o11_wien2k) { // NOLINT
  root_finder_test_case("ref_data/la5ni3o11-wien2k.ref.h5");
}