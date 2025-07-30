#include "./common.hpp"
#include "triqs_modest/loaders.hpp"
#include <random>

using namespace triqs;

void fill_random(nda::MemoryArray auto &A) {
  std::mt19937 gen(std::random_device{}());
  std::uniform_real_distribution<double> dist(0, 1);
  for (auto &x : A) { x = dist(gen); }
}

void test_embed_desc_from_dft(std::string filename, double threshold) {
  std::cout << "filename= " << filename << std::endl;
  auto [_, obe] = one_body_elements_from_dft_converter(filename, threshold);
  auto E        = make_embedding_with_equivalences(obe.C_space);
  std::cout << E << std::endl;
  auto mesh      = triqs::mesh::imfreq(40., triqs::mesh::Fermion);
  auto gf_solver = make_vec_block_gf(mesh, E.imp_block_shape());
  auto gf_embed  = make_block2_gf(mesh, E.sigma_embed_block_shape());
  EXPECT_BLOCK2_GF_NEAR(E.embed(gf_solver), gf_embed, 1.e-12);
  // randomize and check extract embed are inverse of each other
  for (auto &bg : gf_solver)
    for (auto &g : bg) fill_random(g.data());
  auto gf_embed_from_solver = E.embed(gf_solver);
  auto gf_solver_from_embed = E.extract(gf_embed_from_solver);
  for (auto &&[g1, g2] : zip(gf_solver_from_embed, gf_solver)) EXPECT_BLOCK_GF_NEAR(g1, g2, 1.e-12);
};

template <typename Mesh> void test_embed_desc_embed(std::string filename, double threshold) {
  std::cout << "filename= " << filename << std::endl;
  auto [_, obe] = one_body_elements_from_dft_converter(filename, threshold);
  auto E        = make_embedding_with_equivalences(obe.C_space);
  std::cout << E << std::endl;
  auto root      = h5::proxy{filename, 'r'};
  auto Sigma_imp = to_vector<block_gf<Mesh, matrix_valued>>(sort_keys_as_int(root["ref_data"]["const_Sigma_input"]));
}

void test_embed_desc_rotate_hloc0(std::string filename, double threshold) {
  std::cout << "filename= " << filename << std::endl;
  auto [_, obe] = one_body_elements_from_dft_converter(filename, threshold, true);
  auto basis    = obe.C_space.rotation_from_dft_to_local_basis();
  //auto hloc0    = modest::detail::hloc0(obe);
  auto hloc0 = obe.C_space.atomic_view(modest::impurity_levels(obe));
  for (auto [H, U] : zip(hloc0, basis)) {
    auto H_back = U_X_Udag(U, Udag_X_U(U, H));
    EXPECT_ARRAY_NEAR(H, H_back, 1.e-14);
  }
};

TEST(embed_desc_tests, lunio3_vasp) { // NOLINT
  auto filename = "ref_data/lunio3-vasp.ref.h5";
  test_embed_desc_from_dft(filename, 1.e-6);
  test_embed_desc_rotate_hloc0(filename, 1.e-6);
}

TEST(embed_desc_tests, nio_vasp) { // NOLINT
  auto filename = "ref_data/nio.ref.h5";
  test_embed_desc_from_dft(filename, 1.e-5);
}

TEST(embed_desc_tests, lavo3_wien2k) { // NOLINT
  auto filename = "ref_data/LaVO3-Pnma-t2g.ref.h5";
  test_embed_desc_from_dft(filename, 1.e-4);
  test_embed_desc_rotate_hloc0(filename, 1.e-4);
}

TEST(embed_desc_tests, svo_wien2k) { // NOLINT
  auto filename = "ref_data/svo-wien2k.ref.h5";
  test_embed_desc_from_dft(filename, 1.e-3);
  test_embed_desc_rotate_hloc0(filename, 1.e-3);
}

TEST(embed_desc_tests, extract_matrix) {
  std::string filename       = "ref_data/SrVO3-cubic-t2g.ref.h5";
  auto [target_density, obe] = one_body_elements_from_dft_converter(filename);
  auto E                     = make_embedding_with_no_equivalences(obe.C_space);
  auto hloc_C                = impurity_levels(obe);
  auto h_imp                 = E.extract(hloc_C);
  auto n_orb                 = obe.C_space.dim();
  auto n_sigma               = obe.C_space.n_sigma();
  for (auto a : range(obe.C_space.n_atoms()))
    for (auto o : range(n_orb))
      for (auto s : range(n_sigma)) EXPECT_COMPLEX_NEAR(hloc_C(a, s)(o, o), h_imp[a][o + s * o](0, 0));
}

TEST(embed_desc_tests, cluster) { // NOLINT
  auto [target_density, obe] = one_body_elements_from_dft_converter("ref_data/nio.ref.h5");

  auto partition        = std::vector<std::vector<long>>{{0, 1}};
  auto [obe_cluster, E] = make_embedding_with_clusters(obe, partition);
  std::cout << obe_cluster << std::endl;
  std::cout << E << std::endl;
}

#if LFS
TEST(embed_desc_tests, sio_wien2k_soc) { // NOLINT
  auto filename = "ref_data_lfs/sriro3-wien2k-soc.ref.h5";
  test_embed_desc_from_dft(filename, 1.e-6);
  test_embed_desc_rotate_hloc0(filename, 1.e-6);
}

TEST(embed_desc_tests,
     lvo_wien2k_diag_hloc) { // NOLINT
  auto filename = "ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5";
  test_embed_desc_from_dft(filename, 1.e-6);
  test_embed_desc_rotate_hloc0(filename, 1.e-6);
}

TEST(embed_desc_tests, la327_2222) { // NOLINT
  auto filename = "ref_data_lfs/la327-2222-strain-w90-fixedgrid.ref.h5";
  test_embed_desc_from_dft(filename, 1.e-5);
  test_embed_desc_rotate_hloc0(filename, 1.e-5);
}

TEST(embed_desc_tests, la327_1313) { // NOLINT
  auto filename = "ref_data_lfs/la327-1313-p30-w90-fixedgrid.ref.h5";
  test_embed_desc_from_dft(filename, 1.e-5);
  test_embed_desc_rotate_hloc0(filename, 1.e-5);
}

TEST(embed_desc_tests, sriro3_soc) { test_embed_desc_from_dft("ref_data_lfs/sriro3-wien2k-soc.ref.h5", 1.e-6); }

TEST(embed_desc_tests, sriro3_soc_embed_sigma) { test_embed_desc_embed<triqs::mesh::imfreq>("ref_data_lfs/sriro3-wien2k-soc.ref.h5", 1.e-6); }

TEST(embed_desc_tests, sr2mgoso6_soc) { test_embed_desc_from_dft("ref_data_lfs/sr2mgoso6-wien2k-soc.ref.h5", 1.e-6); }

TEST(embed_desc_tests, api) {
  std::string filename = "ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5";
  std::cout << "filename= " << filename << std::endl;
  auto [_, obe] = one_body_elements_from_dft_converter(filename);
  auto E        = make_embedding_with_equivalences(obe.C_space);
  std::cout << E << std::endl;
  //auto E2 = E.split(0, {{"up_2", 2}, {"down_2", 2}}).split(1, {{"up_1", 2}, {"down_1", 2}}).drop(2).drop(2);
  auto E2 = E.split(0, {2});
  std::cout << "==============\n split E.split(0, {up_2', 'down_2'} " << E2 << std::endl;
  auto E5 = E2.drop(0);
  std::cout << "==============\n drop 0 " << E2 << E5 << std::endl;
  auto E3 = E2.flip_spin(0);
  std::cout << "==============\n flip_sigma 0" << E2 << E3 << std::endl;
  auto E4 = E.split(0, {2}).split(1, {1});
  std::cout << "==============\n multiple split : E.split(0, { up_2 , down_2 }).split(1, { up_1 ,  down_1}) \n" << E << E4 << std::endl;
}

//---------------------------------------------------------------
// Embedding API tests: fails in Python
//---------------------------------------------------------------
TEST(embed_desc_tests, api2) {
  std::string filename = "ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5";
  std::cout << "filename= " << filename << std::endl;
  auto [_, obe] = one_body_elements_from_dft_converter(filename);
  auto E        = make_embedding_with_equivalences(obe.C_space);
  std::cout << E << std::endl;
  auto Enew = E.split(0, {2}).drop(0).split(1, {1}).drop(1);
  std::cout << Enew << std::endl;

  auto mesh      = triqs::mesh::imfreq(40., triqs::mesh::Fermion);
  auto gf_solver = make_vec_block_gf(mesh, E.imp_block_shape());
  auto gf_embed  = make_block2_gf(mesh, E.sigma_embed_block_shape());

  // randomize and check extract embed are inverse of each other
  for (auto &bg : gf_solver)
    for (auto &g : bg) fill_random(g.data());
  auto gf_embed_from_solver = E.embed(gf_solver);
  auto gf_solver_from_embed = E.extract(gf_embed_from_solver);
  for (auto &&[g1, g2] : zip(gf_solver_from_embed, gf_solver)) EXPECT_BLOCK_GF_NEAR(g1, g2, 1.e-15);
}

// TEST(embed_desc_tests, api2) {
//   std::string filename = "la327-1313-p30-w90-fixedgrid.ref.h5";
//   std::cout << "filename= " << filename << std::endl;
//   auto [_, data] = load_one_body_elements_from_h5(filename);
//   auto [D, U]    = find_block_structure(data, 1.e-4);
//   auto E         = make_embed_from_obe(data, D);
//   std::cout << E << std::endl;
// }

// TEST(embed_desc_tests, cluster) {
//   auto filename = "ref_data_lfs/la327-2222-strain-w90-fixedgrid.ref.h5";
//   std::cout << "Loading ref_data from data in HDF5 " << filename << std::endl;
//   auto [density, data] = load_one_body_elements_from_h5(filename);
//   auto cluster_alphas  = std::vector<long>{0, 1};
//   auto cluster_data    = clusterize_one_body_elements(data, cluster_alphas);
//   // auto E               = make_embed_from_dft(cluster_data, 1.e-8);
//   // std::cout << E << std::endl;
// }

// TEST(embed_desc_tests, dimer) {
//   auto filename = "ref_data_lfs/moocl2-dimer-w90-fix-grid.ref.h5";
//   std::cout << "Loading ref_data from data in HDF5 " << filename << std::endl;
//   auto [density, data] = load_one_body_elements_from_h5(filename);
//   auto cluster_alphas  = std::vector<long>{0, 1};
//   auto cluster_data    = clusterize_one_body_elements(data, cluster_alphas);
//   embedding_desc E     = make_embed_from_dft(cluster_data, 1.e-4, true);
//   std::cout << E << std::endl;
//   auto hloc0 = compute_hloc0(cluster_data);
//   auto Up    = E.blocks(0, 0).U;
//   auto H     = Udag_X_U(Up, hloc0[0][0]);
//   // auto [eval, evecs] = nda::linalg::eigenelements(H);
//   // std::cout << fmt::format("{}", hloc0[0][0]) << std::endl;
//   //std::cout << fmt::format("{}", H) << std::endl;
//   // auto H2 = Udag_X_U(evecs, H);
//   // std::cout << fmt::format("{}", H2) << std::endl;
//   // std::cout << fmt::format("{}", Udag_X_U(Up * evecs, hloc0[0][0])) << std::endl;
// }

// TEST(embed_desc_tests, dimer2) {
//   auto filename = "ref_data_lfs/la327-2222-strain-w90-fixedgrid.ref.h5";
//   std::cout << "Loading ref_data from data in HDF5 " << filename << std::endl;
//   auto root            = h5::proxy{filename, 'r'};
//   auto g_dft           = root["dft_input"];
//   auto kpts            = as<nda::array<double, 2>>(g_dft["kpts"]);
//   auto [density, data] = load_one_body_elements_from_h5(filename);
//   auto cluster_alphas  = std::vector<long>{0, 1};
//   auto cluster_data    = clusterize_one_body_elements(data, cluster_alphas);
//   auto Hloc0           = compute_hloc0(cluster_data);
//   auto Hloc0_1         = compute_intersite_terms(cluster_data, kpts, nda::vector<double>{1.0, 1.0, -1.0});
//   auto Hloc            = Hloc0;
//   for (auto i : range(Hloc0.size()))
//     for (auto j : range(Hloc0[i].size())) Hloc[i][j] = Hloc0[i][j] + Hloc0_1[i][j] + dagger(Hloc0_1[i][j]);
//   //std::cout << fmt::format("{}", Hloc0_2[0][0]) << std::endl;
//   // auto H0 = nda::matrix<dcomplex>{Hloc0[0][0] + Hloc0_1[0][0] + dagger(Hloc0_1[0][0])};
//   // std::cout << fmt::format("{}", H0) << std::endl;
//   embedding_desc E = make_embed_from_dft(cluster_data, Hloc, 1.e-3, true);
//   std::cout << E << std::endl;
// }
#endif
