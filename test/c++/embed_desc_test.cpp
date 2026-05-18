#include "./common.hpp"
#include "triqs_modest/loaders.hpp"
#include <random>

using namespace triqs;

void fill_random(nda::MemoryArray auto &A) {
  for (auto &x : A) { x = random_constant(); }
}

void test_embed_desc_from_dft(std::string filename, double threshold) {
  auto [target_density, obe] = one_body_elements_from_dft_converter(filename, threshold);
  auto E                     = make_embedding(obe.C_space);
  auto mesh                  = triqs::mesh::imfreq(40., triqs::mesh::Fermion);
  auto gf_solver             = make_vec_block_gf(mesh, E.imp_block_structure());
  auto gf_embed              = make_block2_gf(mesh, E.embed_block_structure());
  EXPECT_BLOCK2_GF_NEAR(E.embed(gf_solver), gf_embed, 1.e-12);
  // randomize and check extract embed are inverse of each other
  for (auto &bg : gf_solver)
    for (auto &g : bg) fill_random(g.data());
  auto gf_embed_from_solver = E.embed(gf_solver);
  auto gf_solver_from_embed = E.extract(gf_embed_from_solver);
  for (auto &&[g1, g2] : zip(gf_solver_from_embed, gf_solver)) EXPECT_BLOCK_GF_NEAR(g1, g2, 1.e-12);
};

template <typename Mesh> void test_embed_desc_embed(std::string filename, double threshold) {
  auto [target_density, obe] = one_body_elements_from_dft_converter(filename, threshold);
  auto E                     = make_embedding(obe.C_space);
  auto root                  = h5::proxy{filename, 'r'};
  auto Sigma_imp             = to_vector<block_gf<Mesh, matrix_valued>>(sort_keys_as_int(root["ref_data"]["const_Sigma_input"]));
}

void test_embed_desc_rotate_hloc0(std::string filename, double threshold) {
  auto [target_density, obe] = one_body_elements_from_dft_converter(filename, threshold, true);
  auto basis                 = obe.C_space.rotation_from_dft_to_local_basis();
  auto hloc0                 = obe.C_space.atomic_view(modest::impurity_levels(obe));
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
  auto E                     = make_embedding(obe.C_space, false);
  auto hloc_C                = impurity_levels(obe);
  auto h_imp                 = E.extract(hloc_C);
  auto n_orb                 = obe.C_space.dim();
  auto n_sigma               = obe.C_space.n_sigma();
  for (auto a : range(obe.C_space.n_atoms()))
    for (auto o : range(n_orb))
      for (auto s : range(n_sigma)) EXPECT_COMPLEX_NEAR(hloc_C(a, s)(o, o), h_imp[a][o + s * o](0, 0));
}

TEST(embed_desc_tests, embed_matrix) {
  auto [target_density, obe] = one_body_elements_from_dft_converter("ref_data/lco_qe_dp_w90_grid.ref.h5");
  auto E                     = make_embedding(obe.C_space).drop_imp(1);

  double beta          = 40.0;
  auto mesh            = mesh::imfreq{beta, statistic_enum::Fermion, 251};
  auto G               = E.extract(gloc(mesh, obe, find_chemical_potential(target_density, obe, beta, "bisection", 1e-3, false)));
  auto double_counting = dc_solver(obe.C_space.spin_kind(), "sFLL", 2.0, 1.0);
  auto Sigma_static    = G | stdv::transform([&double_counting](auto &x) { return double_counting.dc_self_energy(x); }) | tl::to<std::vector>();

  auto Sigma_static_C = E.embed(Sigma_static);
  EXPECT_ARRAY_EQ(Sigma_static_C(0, 0), Sigma_static[0][0]);
  EXPECT_ARRAY_EQ(Sigma_static_C(0, 1), Sigma_static[0][1]);

  for (auto i : range(1, 2))
    for (auto s : range(2)) EXPECT_ARRAY_ZERO(Sigma_static_C(i, s));
}

TEST(embed_desc_tests, cluster) { // NOLINT
  auto [target_density, obe] = one_body_elements_from_dft_converter("ref_data/nio.ref.h5");
  auto partition             = std::vector<std::vector<long>>{{0, 1}};
  auto [obe_cluster, E]      = make_embedding_with_clusters(obe, partition);
}

TEST(embed_desc_tests, split_imp_block) { // NOLINT
  auto E1_ref = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3, 2}, {3}, {3}, {3}}, std::vector<long>{0, 1, 2, 3});
  auto E0     = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{5}, {3}, {3}, {3}}, std::vector<long>{0, 1, 2, 3});
  auto E1     = E0.split_imp_block(0, 0, std::vector<long>{3, 2});
  EXPECT_EQ(E1, E1_ref);

  auto E2_ref =
     make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3, 2}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}}, std::vector<long>{0, 1, 1, 1});

  auto E3 = E0.split_imp_block(0, 0, {3, 2}).replace_imp(3, 1).drop_imp(3).replace_imp(2, 1).drop_imp(2).split_imp_block(1, 0, std::vector<long>{1, 1, 1});
  EXPECT_EQ(E3, E2_ref);
}

TEST(embed_desc_tests, spinless) {
  auto E_ref = make_embedding({"ud"}, std::vector<std::vector<long>>{{3, 2}}, std::vector<long>{0});
  auto E     = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3, 2}}, std::vector<long>{0});
  auto E1    = E.slice_sigma();
  EXPECT_EQ(E1, E_ref);
}

// Rank 2  (i j)
TEST(embed_desc_tests, Eij) {
  auto E              = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{1}, {1}}, std::vector<long>{0, 0});
  auto n_spin         = 2;
  auto n_orb          = 2;
  auto dummy_data     = nda::rand<dcomplex>(1, 1);
  auto Sigma_static_C = range(n_spin) | stdv::transform([&](auto) { return nda::zeros<dcomplex>(n_orb, n_orb); }) | tl::to<std::vector>();
  for (auto s : range(n_spin)) {
    for (auto j : range(n_orb)) Sigma_static_C[s](j, j) = dummy_data(0, 0);
  }
  auto Sigma_static = E.embed(E.extract(Sigma_static_C));
  for (auto s : range(n_spin)) { EXPECT_ARRAY_NEAR(Sigma_static[s], Sigma_static_C[s]); }
}

// Rank 3  (w i j)
TEST(embed_desc_tests, Ewij) {
  auto E          = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{1}, {1}}, std::vector<long>{0, 0});
  auto n_iw       = 100;
  auto n_spin     = 2;
  auto n_orb      = 2;
  auto dummy_data = nda::rand<dcomplex>(n_iw, 1, 1);
  auto Sigma_C    = range(n_spin) | stdv::transform([&](auto) { return nda::zeros<dcomplex>(n_iw, n_orb, n_orb); }) | tl::to<std::vector>();
  for (auto s : range(n_spin)) {
    for (auto j : range(n_orb)) Sigma_C[s](r_all, j, j) = dummy_data(r_all, 0, 0);
  }
  auto Sigma = E.embed(E.extract(Sigma_C));
  for (auto s : range(n_spin)) { EXPECT_ARRAY_NEAR(Sigma[s], Sigma_C[s]); }
}

// Rank 4 (i j k l)
TEST(embed_desc_tests, Eijkl) {
  auto E          = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{1}, {1}}, std::vector<long>{0, 0}).slice_sigma();
  auto n_spin     = 1;
  auto n_orb      = 2;
  auto dummy_data = nda::rand<dcomplex>(1, 1, 1, 1);
  auto U_static_C = range(n_spin) | stdv::transform([&](auto) { return nda::zeros<dcomplex>(n_orb, n_orb, n_orb, n_orb); }) | tl::to<std::vector>();
  for (auto s : range(n_spin)) {
    for (auto j : range(n_orb)) U_static_C[s](j, j, j, j) = dummy_data(0, 0, 0, 0);
  }
  auto U_static = E.embed(E.extract(U_static_C));
  for (auto s : range(n_spin)) { EXPECT_ARRAY_NEAR(U_static[s], U_static_C[s]); }
}

// Rank 5  (w i j k l)
TEST(embed_desc_tests, Ewijkl) {
  auto E          = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{1}, {1}}, std::vector<long>{0, 0}).slice_sigma();
  auto n_iw       = 100;
  auto n_spin     = 1;
  auto n_orb      = 2;
  auto dummy_data = nda::rand<dcomplex>(n_iw, 1, 1, 1, 1);
  auto Pi_C = range(n_spin) | stdv::transform([&](auto) { return nda::zeros<dcomplex>(n_iw, n_orb, n_orb, n_orb, n_orb); }) | tl::to<std::vector>();
  for (auto s : range(n_spin)) {
    for (auto j : range(n_orb)) Pi_C[s](r_all, j, j, j, j) = dummy_data(r_all, 0, 0, 0, 0);
  }
  auto Pi = E.embed(E.extract(Pi_C));
  for (auto s : range(n_spin)) { EXPECT_ARRAY_NEAR(Pi[s], Pi_C[s]); }
}

// ------- merge_embed_block_by_imp tests -------

TEST(embed_desc_tests, merge_embed_block_by_imp_simple) {
  // 1 impurity, 3 orbitals with 1x1 blocks → single [3] block
  auto E     = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{1, 1, 1}}, std::vector<long>{0});
  auto E_2p  = E.merge_embed_block_by_imp();
  auto E_ref = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3}}, std::vector<long>{0});
  EXPECT_EQ(E_2p, E_ref);
}

TEST(embed_desc_tests, merge_embed_block_by_imp_mixed_blocks) {
  // 1 impurity, mixed block sizes [3, 2] → [5]
  auto E     = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3, 2}}, std::vector<long>{0});
  auto E_2p  = E.merge_embed_block_by_imp();
  auto E_ref = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{5}}, std::vector<long>{0});
  EXPECT_EQ(E_2p, E_ref);
}

TEST(embed_desc_tests, merge_embed_block_by_imp_two_impurities) {
  // 2 impurities: [1,1,1] and [2,1]
  auto E     = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{1, 1, 1}, {2, 1}}, std::vector<long>{0, 1});
  auto E_2p  = E.merge_embed_block_by_imp();
  auto E_ref = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3}, {3}}, std::vector<long>{0, 1});
  EXPECT_EQ(E_2p, E_ref);
}

TEST(embed_desc_tests, merge_embed_block_by_imp_equivalent_atoms) {
  // 2 atoms mapped to the same impurity — must NOT merge across atom boundaries
  auto E     = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{1, 1, 1}, {1, 1, 1}}, std::vector<long>{0, 0});
  auto E_2p  = E.merge_embed_block_by_imp();
  auto E_ref = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3}, {3}}, std::vector<long>{0, 0});
  EXPECT_EQ(E_2p, E_ref);
}

TEST(embed_desc_tests, merge_embed_block_by_imp_mixed_equiv_and_inequiv) {
  // 2 equivalent atoms (imp 0) with [1,1,1] each + 1 inequivalent atom (imp 1) with [3,2]
  // After merge_embed_block_by_imp: imp 0 → [3] (shared solver), imp 1 → [5]
  auto E     = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{1, 1, 1}, {1, 1, 1}, {3, 2}}, std::vector<long>{0, 0, 1});
  auto E_2p  = E.merge_embed_block_by_imp();
  auto E_ref = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3}, {3}, {5}}, std::vector<long>{0, 0, 1});
  EXPECT_EQ(E_2p, E_ref);
}

TEST(embed_desc_tests, merge_embed_block_by_imp_idempotent) {
  // Already a single block per impurity → no change
  auto E    = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{5}}, std::vector<long>{0});
  auto E_2p = E.merge_embed_block_by_imp();
  EXPECT_EQ(E_2p, E);
}

TEST(embed_desc_tests, merge_embed_block_by_imp_then_spinless) {
  // Compose with slice_sigma
  auto E     = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{1, 1, 1}}, std::vector<long>{0});
  auto E_2p  = E.merge_embed_block_by_imp().slice_sigma();
  auto E_ref = make_embedding({"ud"}, std::vector<std::vector<long>>{{3}}, std::vector<long>{0});
  EXPECT_EQ(E_2p, E_ref);
}

TEST(embed_desc_tests, merge_embed_block_by_imp_with_disconnected) {
  // 2 impurities, drop the first → its α blocks become disconnected (-1).
  // The disconnected blocks must NOT be merged with each other or with
  // the remaining connected blocks.
  using block_t = embedding::imp_block_t;

  auto E      = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{1, 1, 1}, {2, 1}}, std::vector<long>{0, 1});
  auto E_drop = E.drop_imp(0); // imp 0 gone; its 3 α blocks are now {-1, 0, 0}

  // Verify precondition: first 3 α blocks are disconnected
  for (auto alpha : range(3))
    for (auto sigma : range(2)) { EXPECT_EQ(E_drop.psi_map()(alpha, sigma).imp_idx, -1); }

  auto E_2p = E_drop.merge_embed_block_by_imp();

  // Expected: 3 standalone disconnected blocks (sizes 1,1,1) + 1 merged block (size 3)
  // imp_decomps: only impurity 0 (the ex-impurity 1, renumbered after drop) with [3]
  // sigma_embed_decomp: {1, 1, 1, 3}
  // psi: first 3 rows → (-1, 0, 0), last row → (0, 0, σ)
  auto new_sigma_embed_decomp = std::vector<long>{1, 1, 1, 3};
  nda::array<block_t, 2> new_psi(4, 2);
  // clang-format off
  new_psi(0, 0) = block_t{-1, 0, 0};  new_psi(0, 1) = block_t{-1, 0, 0};
  new_psi(1, 0) = block_t{-1, 0, 0};  new_psi(1, 1) = block_t{-1, 0, 0};
  new_psi(2, 0) = block_t{-1, 0, 0};  new_psi(2, 1) = block_t{-1, 0, 0};
  new_psi(3, 0) = block_t{0, 0, 0};   new_psi(3, 1) = block_t{0, 0, 1};
  // clang-format on
  auto E_ref = embedding(new_sigma_embed_decomp, std::vector<std::vector<long>>{{3}}, new_psi, {"up", "down"});
  EXPECT_EQ(E_2p, E_ref);
}

// ------- Input validation tests -------

TEST(embed_desc_tests, extract_wrong_n_sigma) {
  auto E    = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3}}, std::vector<long>{0});
  auto data = std::vector<nda::array<dcomplex, 2>>{nda::zeros<dcomplex>(3, 3)}; // 1 spin channel, expect 2
  EXPECT_THROW(E.extract(data), std::runtime_error);
}

TEST(embed_desc_tests, extract_wrong_dimension) {
  auto E    = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3}}, std::vector<long>{0});
  auto data = std::vector<nda::array<dcomplex, 2>>{nda::zeros<dcomplex>(5, 5), nda::zeros<dcomplex>(5, 5)}; // dim 5, expect 3
  EXPECT_THROW(E.extract(data), std::runtime_error);
}

TEST(embed_desc_tests, embed_wrong_n_impurities) {
  auto E     = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3}, {2}}, std::vector<long>{0, 1});
  auto blocks = std::vector<std::vector<nda::array<dcomplex, 2>>>{{nda::zeros<dcomplex>(3, 3), nda::zeros<dcomplex>(3, 3)}}; // 1 imp, expect 2
  EXPECT_THROW(E.embed(blocks), std::runtime_error);
}

TEST(embed_desc_tests, embed_wrong_n_blocks) {
  auto E      = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3}}, std::vector<long>{0});
  auto blocks = std::vector<std::vector<nda::array<dcomplex, 2>>>{{nda::zeros<dcomplex>(3, 3)}}; // 1 block, expect 2 (up_0, down_0)
  EXPECT_THROW(E.embed(blocks), std::runtime_error);
}

TEST(embed_desc_tests, embed_wrong_block_dimension) {
  auto E      = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3}}, std::vector<long>{0});
  auto blocks = std::vector<std::vector<nda::array<dcomplex, 2>>>{{nda::zeros<dcomplex>(5, 5), nda::zeros<dcomplex>(5, 5)}}; // dim 5, expect 3
  EXPECT_THROW(E.embed(blocks), std::runtime_error);
}

TEST(embed_desc_tests, embed_matrix_wrong_n_impurities) {
  auto E      = make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3}, {2}}, std::vector<long>{0, 1});
  auto blocks = std::vector<block_matrix_t>{{nda::zeros<dcomplex>(3, 3), nda::zeros<dcomplex>(3, 3)}}; // 1 imp, expect 2
  EXPECT_THROW(E.embed(blocks), std::runtime_error);
}

// ------- make_embedding factory validation tests -------

TEST(embed_desc_tests, make_embedding_empty_spin_names) {
  EXPECT_THROW(make_embedding({}, std::vector<std::vector<long>>{{3}}, std::vector<long>{0}), std::runtime_error);
}

TEST(embed_desc_tests, make_embedding_atom_count_mismatch) {
  // 2 atoms in decomposition but only 1 entry in atom_to_imp
  EXPECT_THROW(make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3}, {2}}, std::vector<long>{0}), std::runtime_error);
}

TEST(embed_desc_tests, make_embedding_negative_imp_idx) {
  EXPECT_THROW(make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3}}, std::vector<long>{-1}), std::runtime_error);
}

TEST(embed_desc_tests, make_embedding_inconsistent_equiv_atoms) {
  // Two atoms mapped to imp 0 but with different block decompositions
  EXPECT_THROW(make_embedding({"up", "down"}, std::vector<std::vector<long>>{{3, 2}, {1, 1}}, std::vector<long>{0, 0}), std::runtime_error);
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

  using block_t = embedding::imp_block_t;

  auto filename              = "ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5";
  auto [target_density, obe] = one_body_elements_from_dft_converter(filename);
  auto E                     = make_embedding(obe.C_space);

  auto sigma_embed_decomp = std::vector<long>{};
  for (auto &x : obe.C_space.atoms_block_decomposition()(r_all, 0)) {
    for (auto &y : x) sigma_embed_decomp.emplace_back(y);
  }

  nda::array<block_t, 2> psi(sigma_embed_decomp.size(), obe.C_space.n_sigma());
  // clang-format off
  psi(0, 0) = block_t{0, 0, 0};    psi(0, 1) = block_t{0, 0, 1};
  psi(1, 0) = block_t{0, 1, 0};    psi(1, 1) = block_t{0, 1, 1};
  psi(2, 0) = block_t{0, 2, 0};    psi(2, 1) = block_t{0, 2, 1};
  psi(3, 0) = block_t{1, 0, 0};    psi(3, 1) = block_t{1, 0, 1};
  psi(4, 0) = block_t{1, 1, 0};    psi(4, 1) = block_t{1, 1, 1};
  psi(5, 0) = block_t{1, 0, 0};    psi(5, 1) = block_t{1, 0, 1};
  psi(6, 0) = block_t{1, 1, 0};    psi(6, 1) = block_t{1, 1, 1};
  psi(7, 0) = block_t{1, 0, 0};    psi(7, 1) = block_t{1, 0, 1};
  psi(8, 0) = block_t{1, 1, 0};    psi(8, 1) = block_t{1, 1, 1};
  // clang-format on

  auto E_ref = embedding(sigma_embed_decomp, std::vector<std::vector<long>>{{1, 2, 2}, {3, 2}}, psi, {"up", "down"});
  EXPECT_EQ(E, E_ref);

  auto E1   = E.split_imp(0, {2});
  auto psi1 = psi;
  // clang-format off
  psi1(0, 0) = block_t{1, 0, 0}; psi1(0, 1) = block_t{1, 0, 1};
  psi1(1, 0) = block_t{1, 1, 0}; psi1(1, 1) = block_t{1, 1, 1};
  psi1(2,0)   = block_t{0, 0, 0};  psi1(2,1) = block_t{0, 0, 1};
  psi1(3, 0) = block_t{2, 0, 0};    psi1(3, 1) = block_t{2, 0, 1};
  psi1(4, 0) = block_t{2, 1, 0};    psi1(4, 1) = block_t{2, 1, 1};
  psi1(5, 0) = block_t{2, 0, 0};    psi1(5, 1) = block_t{2, 0, 1};
  psi1(6, 0) = block_t{2, 1, 0};    psi1(6, 1) = block_t{2, 1, 1};
  psi1(7, 0) = block_t{2, 0, 0};    psi1(7, 1) = block_t{2, 0, 1};
  psi1(8, 0) = block_t{2, 1, 0};    psi1(8, 1) = block_t{2, 1, 1};
  // clang-format on

  auto E1_ref = embedding(sigma_embed_decomp, std::vector<std::vector<long>>{{2}, {1, 2}, {3, 2}}, psi1, {"up", "down"});
  EXPECT_EQ(E1, E1_ref);

  auto E2   = E1.drop_imp(0);
  auto psi2 = psi1;
  // clang-format off
  psi2(0, 0) = block_t{0, 0, 0};  psi2(0, 1) = block_t{0, 0, 1};
  psi2(1, 0) = block_t{0, 1, 0};  psi2(1, 1) = block_t{0, 1, 1};
  psi2(2, 0) = block_t{-1, 0, 0}; psi2(2, 1) = block_t{-1, 0, 0};
  psi2(3, 0) = block_t{1, 0, 0};  psi2(3, 1) = block_t{1, 0, 1};
  psi2(4, 0) = block_t{1, 1, 0};  psi2(4, 1) = block_t{1, 1, 1};
  psi2(5, 0) = block_t{1, 0, 0};  psi2(5, 1) = block_t{1, 0, 1};
  psi2(6, 0) = block_t{1, 1, 0};  psi2(6, 1) = block_t{1, 1, 1};
  psi2(7, 0) = block_t{1, 0, 0};  psi2(7, 1) = block_t{1, 0, 1};
  psi2(8, 0) = block_t{1, 1, 0};  psi2(8, 1) = block_t{1, 1, 1};
  // clang-format on
  auto E2_ref = embedding(sigma_embed_decomp, std::vector<std::vector<long>>{{1, 2}, {3, 2}}, psi2, {"up", "down"});
  EXPECT_EQ(E2, E2_ref);

  auto E3   = E2.swap_sigma(0);
  auto psi3 = psi2;
  // clang-format off
  psi3(0, 0) = block_t{0, 0, 1};  psi3(0, 1) = block_t{0, 0, 0};
  // clang-format on
  auto E3_ref = embedding(sigma_embed_decomp, std::vector<std::vector<long>>{{1, 2}, {3, 2}}, psi3, {"up", "down"});
  EXPECT_EQ(E3, E3_ref);

  auto E4   = E.split_imp(0, {2}).split_imp(1, {1});
  auto psi4 = psi;
  // clang-format off
  psi4(0, 0) = block_t{2, 0, 0};    psi4(0, 1) = block_t{2, 0, 1};
  psi4(1, 0) = block_t{1, 0, 0};    psi4(1, 1) = block_t{1, 0, 1};
  psi4(2, 0) = block_t{0, 0, 0};    psi4(2, 1) = block_t{0, 0, 1};
  psi4(3, 0) = block_t{3, 0, 0};    psi4(3, 1) = block_t{3, 0, 1};
  psi4(4, 0) = block_t{3, 1, 0};    psi4(4, 1) = block_t{3, 1, 1};
  psi4(5, 0) = block_t{3, 0, 0};    psi4(5, 1) = block_t{3, 0, 1};
  psi4(6, 0) = block_t{3, 1, 0};    psi4(6, 1) = block_t{3, 1, 1};
  psi4(7, 0) = block_t{3, 0, 0};    psi4(7, 1) = block_t{3, 0, 1};
  psi4(8, 0) = block_t{3, 1, 0};    psi4(8, 1) = block_t{3, 1, 1};
  // clang-format on
  auto E4_ref = embedding(sigma_embed_decomp, std::vector<std::vector<long>>{{2}, {2}, {1}, {3, 2}}, psi4, {"up", "down"});
  EXPECT_EQ(E4, E4_ref);
}

//---------------------------------------------------------------
// Embedding API tests:
//---------------------------------------------------------------
TEST(embed_desc_tests, api2) {
  using block_t = embedding::imp_block_t;

  std::string filename       = "ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5";
  auto [target_density, obe] = one_body_elements_from_dft_converter(filename);
  auto E                     = make_embedding(obe.C_space);
  auto Enew                  = E.split_imp(0, {2}).drop_imp(0).split_imp(1, {1}).drop_imp(1);

  auto sigma_embed_decomp = std::vector<long>{};
  for (auto &x : obe.C_space.atoms_block_decomposition()(r_all, 0)) {
    for (auto &y : x) sigma_embed_decomp.emplace_back(y);
  }

  nda::array<block_t, 2> psi(sigma_embed_decomp.size(), obe.C_space.n_sigma());
  // clang-format off
  psi(0, 0) = block_t{0, 0, 0};  psi(0, 1) = block_t{0, 0, 1};
  psi(1, 0) = block_t{0, 1, 0};  psi(1, 1) = block_t{0, 1, 1};
  psi(2, 0) = block_t{-1, 0, 0}; psi(2, 1) = block_t{-1, 0, 0};
  psi(3, 0) = block_t{1, 0, 0};  psi(3, 1) = block_t{1, 0, 1};
  psi(4, 0) = block_t{-1, 0, 0}; psi(4, 1) = block_t{-1, 0, 0};
  psi(5, 0) = block_t{1, 0, 0};  psi(5, 1) = block_t{1, 0, 1};
  psi(6, 0) = block_t{-1, 0, 0}; psi(6, 1) = block_t{-1, 0, 0};
  psi(7, 0) = block_t{1, 0, 0};  psi(7, 1) = block_t{1, 0, 1};
  psi(8, 0) = block_t{-1, 0, 0}; psi(8, 1) = block_t{-1, 0, 0};
  // clang-format on
  auto E_ref = embedding(sigma_embed_decomp, std::vector<std::vector<long>>{{1, 2}, {3}}, psi, {"up", "down"});
  EXPECT_EQ(Enew, E_ref);

  auto mesh      = triqs::mesh::imfreq(40., triqs::mesh::Fermion);
  auto gf_solver = make_vec_block_gf(mesh, E.imp_block_structure());
  auto gf_embed  = make_block2_gf(mesh, E.embed_block_structure());

  // randomize and check extract embed are inverse of each other
  for (auto &bg : gf_solver)
    for (auto &g : bg) fill_random(g.data());
  auto gf_embed_from_solver = E.embed(gf_solver);
  auto gf_solver_from_embed = E.extract(gf_embed_from_solver);
  for (auto &&[g1, g2] : zip(gf_solver_from_embed, gf_solver)) EXPECT_BLOCK_GF_NEAR(g1, g2, 1.e-15);
}

#endif

MPI_TEST_MAIN;
