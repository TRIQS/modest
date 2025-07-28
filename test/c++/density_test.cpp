#include "./common.hpp"
#include "triqs_modest/loaders.hpp"

using namespace triqs;

void fill_random(nda::MemoryArray auto &A) {
  std::mt19937 gen(std::random_device{}());
  std::uniform_real_distribution<double> dist(0, 1);
  for (auto &x : A) { x = dist(gen); }
}

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

  auto E = make_embedding_with_equivalences(obe.C_space);

  auto eps   = 1.e-12;
  auto w_max = 5.0 * nda::max_element(abs(obe.H.H_k)); // FIXME
  std::cout << fmt::format("Sigma created with random DLR coefficents: w_max= {} eps= {}\n", w_max, eps);
#if 0

  auto Sigma_imp_dlr =
     make_vec_block_gf(mesh::dlr{40.0, triqs::mesh::statistic_enum::Fermion, w_max, eps}, E.imp_gf_struct());
  for (auto &bg : Sigma_imp_dlr)
    for (auto &g : bg) fill_random(g.data());

  auto make_sigma_dlr_imfreq = [](auto &x) {
    auto out = make_gf_dlr_imfreq<>(x);
    for (auto &g : out) { g.data() += random_constant(); }
    return out;
  };

  auto Sigma_embed = E.embed(Sigma_imp_dlr | stdv::transform(make_sigma_dlr_imfreq) | tl::to<std::vector>());
#else
  auto Sigma_imp_dlr = make_vec_block_gf(mesh::dlr_imfreq{40.0, triqs::mesh::statistic_enum::Fermion, w_max, eps}, E.imp_block_shape());
  auto const &mesh   = Sigma_imp_dlr[0][0].mesh();
  for (auto &bg : Sigma_imp_dlr) {
    for (auto &g : bg) {
      auto A = 0.0 * random_constant();
      auto B = random_constant();
      for (auto &&[n, iw] : enumerate(mesh)) { g.data()(n, r_all, r_all) = A + B / iw; }
    }
  }
  auto Sigma_hartree = Sigma_imp_dlr | stdv::transform([](auto &x) { return std::get<0>(split_self_energy(x)); }) | tl::to<std::vector>();
  auto Sigma_dynamic = Sigma_imp_dlr | stdv::transform([](auto &x) { return std::get<1>(split_self_energy(x)); }) | tl::to<std::vector>();
  auto Sigma_embed   = E.embed(Sigma_dynamic);
  auto Sigma_H_embed = E.embed(Sigma_hartree);
#endif
  return {target_density, obe, Sigma_embed, Sigma_H_embed};
}

std::tuple<double, one_body_elements_on_grid, block2_gf<dlr_imfreq, matrix_valued>, double> load_data(std::string filename) {
  auto root                     = h5::proxy{filename, 'r'};
  auto n_ref                    = as<double>(root["ref_data"]["total_density_zero_mu"]);
  auto load                     = one_body_elements_from_dft_converter(filename);
  auto target_density           = load.first;
  one_body_elements_on_grid obe = load.second;

  auto E     = make_embedding_with_equivalences(obe.C_space);
  auto w_max = 10.0 * nda::max_element(abs(obe.H.H_k));
  auto eps   = 1.e-12;
  auto mesh  = mesh::dlr_imfreq{40.0, triqs::mesh::statistic_enum::Fermion, w_max, eps};
  // zero self-energy
  std::cout << fmt::format("Sigma created with random DLR coefficents: w_max= {:.2f} eps= {} n_dlr= {}\n", w_max, eps, mesh.size());
  auto Sigma_embed = make_block2_gf(mesh, E.sigma_embed_block_shape());
  return {target_density, obe, Sigma_embed, n_ref};
}

auto calc_density_test_case(std::string filename) {
  //   auto [_, obe, Sigma_embed, n_ref] = load_data(filename);
  auto [_, obe, Sigma_embed, Sigma_H_embed] = load_data_w_selfenergy(filename);

  // auto Sigma_DC = nda::array<nda::matrix<double>, 2>(Sigma_embed.size1(), Sigma_embed.size2());
  // for (auto [a, s] : indices(Sigma_embed)) { Sigma_DC(a, s) = nda::zeros<double>(Sigma_embed(a, s).target_shape()); }
  double mu   = 1.0;
  auto n_wood = density(obe, mu, Sigma_embed, Sigma_H_embed);
  std::cout << fmt::format("n_wood(dlr)= {}\n", n_wood);
  //   auto n_slow = density_slow(obe, 0.0, Sigma_embed);
  //std::cout << fmt::format("n_slow(dlr)= {}\n", n_slow);
  for (auto niw : std::vector<long>{50, 250, 500}) { //, 4096, 8192}) {
    auto n_slow_full = density_slow(obe, mu, make_gf_imfreq<>(Sigma_embed, niw), Sigma_H_embed);
    std::cout << fmt::format("n_slow(iwn)= {} rel.error (iwn-dlr)/iwn = {:.6f}% (n_iw={})\n", n_slow_full,
                             std::abs((n_wood - n_slow_full) / n_wood) * 100, niw);
  }
  std::cout << fmt::format("n_wood(dlr)= {}\n", n_wood);
  //   EXPECT_NEAR(n_slow_full_mesh, n_ref, 1.e-10); // PASSES
  //   EXPECT_NEAR(n_slow, n_ref, 1.e-4);            // FAILS
};

auto root_finder_test_case(std::string filename) {
  auto package        = load_data_w_selfenergy(filename);
  auto target_density = std::get<0>(package);
  auto obe            = std::get<1>(package);
  auto Sigma_embed    = std::get<2>(package);
  auto Sigma_H_embed  = std::get<3>(package);

  // auto Sigma_DC = nda::array<nda::matrix<double>, 2>(Sigma_embed.size1(), Sigma_embed.size2());
  // for (auto [a, s] : indices(Sigma_embed)) { Sigma_DC(a, s) = nda::zeros<double>(Sigma_embed(a, s).target_shape()); }

  auto f = [obe, Sigma_embed, Sigma_H_embed](auto x) { return density_slow(obe, x, Sigma_embed, Sigma_H_embed); };

  auto [mu1, d1] = root_finder("dichotomy", f, 0.0, target_density, 1.e-4, 0.5, 1000, "Chemical Potential", "Total Density", false);
  auto [mu2, d2] = root_finder("bisection", f, 0.0, target_density, 1.e-4, 0.5, 1000, "Chemical Potential", "Total Density", false);
  EXPECT_NEAR(mu1, mu2, 1.e-3);
};

TEST(density_tests, density_prnio3_vasp) { // NOLINT
  calc_density_test_case("ref_data/prnio3-vasp.ref.h5");
}

TEST(density_tests, lunio3_vasp) { // NOLINT
  calc_density_test_case("ref_data/lunio3-vasp.ref.h5");
}

TEST(density_tests, nio_wien2k) { // NOLINT
  calc_density_test_case("ref_data/nio.ref.h5");
}

TEST(density_tests, root_prnio3_vasp) { // NOLINT
  root_finder_test_case("ref_data/prnio3-vasp.ref.h5");
}

TEST(density_tests, root_lunio3_vasp) { // NOLINT
  root_finder_test_case("ref_data/lunio3-vasp.ref.h5");
}

TEST(density_tests, root_svo_vasp) { // NOLINT
  //root_finder_test_case("ref_data/SrVO3-cubic-t2g.ref.h5");
  auto [target_density, obe_dft] = one_body_elements_from_dft_converter("ref_data/SrVO3-cubic-t2g.ref.h5");
  auto mu                        = find_chemical_potential(target_density, obe_dft, 10.0);
  std::cout << mu << std::endl;
  auto mu2 = find_chemical_potential(target_density + 0.1, obe_dft, 10.0);
  std::cout << mu2 << std::endl;
}

// TEST(density_tests, lavo3_wien2k) { // NOLINT
//   calc_density_test_case("ref_data/LaVO3-Pnma-t2g.ref.h5");
// }

// TEST(density_tests, la5ni3o11_wien2k) { // NOLINT
//   calc_density_test_case("ref_data/la5ni3o11-wien2k.ref.h5");
// }
