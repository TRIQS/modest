#include "./common.hpp"
#include "triqs_modest/loaders.hpp"

using namespace triqs;

enum class self_energy_kind_e { ZeroSigma, ConstSigma, Sigma };

template <typename X, typename Y> void assert_block2_gfs_and_vec_gf_are_close(X const &x, Y const &y, double precision) {

  if (x.size1() != y.size()) TRIQS_RUNTIME_ERROR << "Block2 GFs have different number of blocks";
  for (int i = 0; i < x.size1(); ++i) {
    if (x.size2() != y[i].size()) TRIQS_RUNTIME_ERROR << "Block2 GFs have different number of blocks";
    for (int j = 0; j < x.size2(); ++j) {
      if (x.block_names()[1][j] != y[i].block_names()[j])
        TRIQS_RUNTIME_ERROR << "Block2 GFs have different block name " << x.block_names()[i][j] << " != " << y[i].block_names()[j];
      assert_gfs_are_close(x(i, j), y[i][j], precision);
    }
  }
}

// // -------------------------------------------
// // for TEST only for now ?
// gf_struct2_t Gatom_block_struct(one_body_elements_on_grid const &obe) {
//   gf_struct2_t out(obe.atomic_shells().size(), obe.n_sigma());
//   for (auto [alpha, m_alpha] : enumerate(obe.atomic_decomposition()))
//     for (auto sigma : range(obe.n_sigma())) out(alpha, sigma) = std::pair{obe.sigma_name(sigma), m_alpha};
//   return out;
// }

// -------------------------------------------
template <typename Mesh> auto run_gloc_test_case(std::string filename, double const threshold, self_energy_kind_e const self_energy_kind) {
  auto compute_gloc_wrapper = [&]() {
    auto load                     = one_body_elements_from_dft_converter(filename, threshold);
    one_body_elements_on_grid obe = load.second;
    auto U                        = obe.C_space.rotation_from_dft_to_local_basis();
    auto E                        = make_embedding_with_equivalences(obe.C_space);
    auto root                     = h5::proxy{filename, 'r'};
    std::vector<block_gf<Mesh, matrix_valued>> gloc_ref, Sigma_imp_solver;
    if (self_energy_kind == self_energy_kind_e::ZeroSigma) {
      gloc_ref         = to_vector<block_gf<Mesh, matrix_valued>>(sort_keys_as_int(root["ref_data"]["Gloc_DFT"]));
      auto const &mesh = gloc_ref[0][0].mesh();
      Sigma_imp_solver = make_vec_block_gf(mesh, E.imp_block_shape());
    } else if (self_energy_kind == self_energy_kind_e::ConstSigma) {
      gloc_ref         = to_vector<block_gf<Mesh, matrix_valued>>(sort_keys_as_int(root["ref_data"]["Gloc_constSigma"]));
      Sigma_imp_solver = to_vector<block_gf<Mesh, matrix_valued>>(sort_keys_as_int(root["ref_data"]["const_Sigma_input"]));
    } else if (self_energy_kind == self_energy_kind_e::Sigma) {
      gloc_ref         = to_vector<block_gf<Mesh, matrix_valued>>(sort_keys_as_int(root["ref_data"]["Gloc_Sigma"]));
      Sigma_imp_solver = to_vector<block_gf<Mesh, matrix_valued>>(sort_keys_as_int(root["ref_data"]["Sigma_input"]));
    }
    //auto [Sigma_hartee, Sigma_dynamic] = split_self_energy(Sigma_imp_solver);
    auto Sigma_static  = Sigma_imp_solver | stdv::transform([](auto &x) { return std::get<0>(split_self_energy(x)); }) | tl::to<std::vector>();
    auto Sigma_dynamic = Sigma_imp_solver | stdv::transform([](auto &x) { return std::get<1>(split_self_energy(x)); }) | tl::to<std::vector>();
    auto Sigma_embed   = E.embed(Sigma_dynamic);
    auto Sigma_H_embed = E.embed(Sigma_static);
    auto G             = decomposition_view(gloc(obe, 0.0, Sigma_embed, Sigma_H_embed), obe.C_space.Gatom_block_shape());
    for (auto &&[g, u] : zip(G, U)) { U_X_Udag_in_place(u, g.data()); }
    return std::pair{G, gloc_ref};
  };

  auto [Gloc, Gloc_ref] = compute_gloc_wrapper();
  assert_block2_gfs_and_vec_gf_are_close(Gloc, Gloc_ref, 1.e-12);
}

auto gloc_two_ways(std::string filename) {
  auto [target_density, obe] = one_body_elements_from_dft_converter(filename);
  auto E                     = make_embedding_with_equivalences(obe.C_space);
  auto mesh                  = mesh::imfreq{10.0, triqs::mesh::Fermion, 251};
  auto Sigma_imp_solver      = make_vec_block_gf(mesh, E.imp_block_shape());
  auto Sigma_static          = Sigma_imp_solver | stdv::transform([](auto &x) { return std::get<0>(split_self_energy(x)); }) | tl::to<std::vector>();
  auto Sigma_dynamic         = Sigma_imp_solver | stdv::transform([](auto &x) { return std::get<1>(split_self_energy(x)); }) | tl::to<std::vector>();
  auto Sigma_embed           = E.embed(Sigma_dynamic);
  auto Sigma_H_embed         = E.embed(Sigma_static);
  auto Gloc1                 = gloc(mesh, obe, 0.0);
  auto Gloc2                 = gloc(obe, 0.0, Sigma_embed, Sigma_H_embed);
  assert_block2_gfs_are_close(Gloc1, Gloc2, 1.e-12);
}

// Compute Gloc with Sigma = 0, Const, Const + Const/iwn

//--------------------------------
// PrNiO3 (VASP)
//--------------------------------
TEST(gloc_tests, gloc_2ways_prnio3) { gloc_two_ways("ref_data/prnio3-vasp.ref.h5"); }
TEST(gloc_tests, gloc_vasp_zero_sigma_prnio3) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/prnio3-vasp.ref.h5", 1.e-6, self_energy_kind_e::ZeroSigma);
}
TEST(gloc_tests, gloc_vasp_const_sigma_prnio3) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/prnio3-vasp.ref.h5", 1.e-6, self_energy_kind_e::ConstSigma);
}
TEST(gloc_tests, gloc_vasp_sigma_prnio3) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/prnio3-vasp.ref.h5", 1.e-6, self_energy_kind_e::Sigma);
}
//--------------------------------

//--------------------------------
// LuNiO3 (VASP)
//--------------------------------
TEST(gloc_tests, gloc_2ways_lunio3) { gloc_two_ways("ref_data/lunio3-vasp.ref.h5"); }
TEST(gloc_tests, gloc_vasp_zero_sigma_lunio3) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/lunio3-vasp.ref.h5", 1.e-6, self_energy_kind_e::ZeroSigma);
}
TEST(gloc_tests, gloc_vasp_const_sigma_lunio3) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/lunio3-vasp.ref.h5", 1.e-6, self_energy_kind_e::ConstSigma);
}
TEST(gloc_tests, gloc_vasp_sigma_lunio3) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/lunio3-vasp.ref.h5", 1.e-6, self_energy_kind_e::Sigma);
}
//--------------------------------

//--------------------------------
// NiO (d-p) (VASP)
//--------------------------------
TEST(gloc_tests, gloc_2ways_nio) { gloc_two_ways("ref_data/nio.ref.h5"); }
TEST(gloc_tests, gloc_vasp_zero_sigma_nio) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/nio.ref.h5", 1.e-5, self_energy_kind_e::ZeroSigma);
}
TEST(gloc_tests, gloc_vasp_const_sigma_nio) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/nio.ref.h5", 1.e-5, self_energy_kind_e::ConstSigma);
}
TEST(gloc_tests, gloc_vasp_sigma_nio) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/nio.ref.h5", 1.e-5, self_energy_kind_e::Sigma);
}
//--------------------------------

//--------------------------------
// SrVO3 (Wien2k)
//--------------------------------
TEST(gloc_tests, gloc_2ways_svo3) { gloc_two_ways("ref_data/svo-wien2k.ref.h5"); }
TEST(gloc_tests, gloc_wien2k_zero_sigma_svo3) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/svo-wien2k.ref.h5", 1.e-6, self_energy_kind_e::ZeroSigma);
}
TEST(gloc_tests, gloc_wien2k_const_sigma_svo3) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/svo-wien2k.ref.h5", 1.e-6, self_energy_kind_e::ConstSigma);
}
TEST(gloc_tests, gloc_wien2k_sigma_svo3) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/svo-wien2k.ref.h5", 1.e-6, self_energy_kind_e::Sigma);
}
//--------------------------------

//--------------------------------
// LaVO3 (Wien2k)
//--------------------------------
TEST(gloc_tests, gloc_2ways_lvo3) { gloc_two_ways("ref_data/LaVO3-Pnma-t2g.ref.h5"); }
TEST(gloc_tests, gloc_wien2k_zero_sigma_lvo3) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/LaVO3-Pnma-t2g.ref.h5", 1.e-4, self_energy_kind_e::ZeroSigma);
}
TEST(gloc_tests, gloc_wien2k_const_sigma_lvo3) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/LaVO3-Pnma-t2g.ref.h5", 1.e-4, self_energy_kind_e::ConstSigma);
}
TEST(gloc_tests, gloc_wien2k_sigma_lvo3) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/LaVO3-Pnma-t2g.ref.h5", 1.e-4, self_energy_kind_e::Sigma);
}
//--------------------------------

//--------------------------------
// La5Ni3O11 (Wien2k) (large nu)
//--------------------------------
TEST(gloc_tests, gloc_2ways_la5ni3o11) { gloc_two_ways("ref_data/la5ni3o11-wien2k.ref.h5"); }
TEST(gloc_tests, gloc_wien2k_zero_sigma_large_nu) { //NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/la5ni3o11-wien2k.ref.h5", 0.00001, self_energy_kind_e::ZeroSigma);
}
TEST(gloc_tests, gloc_wien2k_const_sigma_large_nu) { //NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/la5ni3o11-wien2k.ref.h5", 0.00001, self_energy_kind_e::ConstSigma);
}
TEST(gloc_tests, gloc_wien2k_sigma_large_nu) { //NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/la5ni3o11-wien2k.ref.h5", 0.00001, self_energy_kind_e::Sigma);
}
//--------------------------------

//--------------------------------
// La2CuO4 (Fixed grid W90)
//--------------------------------
TEST(gloc_tests, gloc_2ways_lco) { gloc_two_ways("ref_data/lco_qe_dp_w90_grid.ref.h5"); }
TEST(gloc_tests, gloc_lco_zero_sigma) { //NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/lco_qe_dp_w90_grid.ref.h5", 0.00001, self_energy_kind_e::ZeroSigma);
}
TEST(gloc_tests, gloc_lco_const_sigma) { //NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/lco_qe_dp_w90_grid.ref.h5", 0.00001, self_energy_kind_e::ConstSigma);
}
TEST(gloc_tests, gloc_lco_sigma) { //NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data/lco_qe_dp_w90_grid.ref.h5", 0.00001, self_energy_kind_e::Sigma);
}
//--------------------------------

#if LFS
//--------------------------------
// La5Ni3O11 (Wien2k) (dense freq mesh)
//--------------------------------
TEST(gloc_tests, gloc_wien2k_sigma_large_nu_large_mesh) { // I am a test that is long. Comment me out for speed up!
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data_lfs/la5ni3o11-wien2k-large-mesh.ref.h5", 0.00001, self_energy_kind_e::Sigma);
}

//--------------------------------
// LiV2O4 (Wien2k)
//--------------------------------
TEST(gloc_tests, gloc_2ways_liv2o4) { gloc_two_ways("ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5"); }
TEST(gloc_tests, gloc_wien2k_zero_sigma_liv2o4) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5", 1.e-6, self_energy_kind_e::ZeroSigma);
}
TEST(gloc_tests, gloc_wien2k_const_sigma_liv2o4) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5", 1.e-6, self_energy_kind_e::ConstSigma);
}
TEST(gloc_tests, gloc_wien2k_sigma_liv2o4) { // NOLINT
  run_gloc_test_case<triqs::mesh::imfreq>("ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5", 1.e-6, self_energy_kind_e::Sigma);
}
//--------------------------------
//--------------------------------

//------------------------------------------------------------
// SOC Tests NB: failing because of merged rotations
//------------------------------------------------------------
// TEST(gloc_tests, gloc_wien2k_zero_sigma_sriro3_soc) { // NOLINT
//   run_gloc_test_case<triqs::mesh::imfreq>("ref_data_lfs/sriro3-wien2k-soc.ref.h5", 0.00001, self_energy_kind_e::ZeroSigma);
// }

// TEST(gloc_tests, gloc_wien2k_const_sigma_sriro3_soc) { // NOLINT
//   run_gloc_test_case<triqs::mesh::imfreq>("ref_data_lfs/sriro3-wien2k-soc.ref.h5", 0.00001, self_energy_kind_e::ConstSigma);
// }

// TEST(gloc_tests, gloc_wien2k_sigma_sriro3_soc) { // NOLINT
//   run_gloc_test_case<triqs::mesh::imfreq>("ref_data_lfs/sriro3-wien2k-soc.ref.h5", 0.00001, self_energy_kind_e::Sigma);
// }

// TEST(gloc_tests, gloc_wien2k_zero_sigma_sr2mgoso6_soc) { // NOLINT
//   run_gloc_test_case<triqs::mesh::imfreq>("ref_data_lfs/sr2mgoso6-wien2k-soc.ref.h5", 0.00001, self_energy_kind_e::ZeroSigma);
// }

// TEST(gloc_tests, gloc_wien2k_const_sigma_sr2mgoso6_soc) { // NOLINT
//   run_gloc_test_case<triqs::mesh::imfreq>("ref_data_lfs/sr2mgoso6-wien2k-soc.ref.h5", 0.00001, self_energy_kind_e::ConstSigma);
// }

// TEST(gloc_tests, gloc_wien2k_sigma_sr2mgoso6_soc) { // NOLINT
//   run_gloc_test_case<triqs::mesh::imfreq>("ref_data_lfs/sr2mgoso6-wien2k-soc.ref.h5", 0.00001, self_energy_kind_e::Sigma);
// }
//------------------------------------------------------------
// W90 Tests NB: failing because of Woodbury Formula
//------------------------------------------------------------
// TEST(gloc_tests, gloc_w90_fixed_grid_zero_sigma_la327_2222) { // NOLINT
//   run_gloc_test_case<triqs::mesh::imfreq>("ref_data_lfs/la327-2222-strain-w90-fixedgrid.ref.h5", 1.e-6,
//                                           self_energy_kind_e::ZeroSigma);
// }
// TEST(gloc_tests, gloc_w90_fixed_grid_const_sigma_la327_2222) { // NOLINT
//   run_gloc_test_case<triqs::mesh::imfreq>("ref_data_lfs/la327-2222-strain-w90-fixedgrid.ref.h5", 1.e-6,
//                                           self_energy_kind_e::ConstSigma);
// }
// TEST(gloc_tests, gloc_w90_fixed_grid_sigma_la327_2222) { // NOLINT
//   run_gloc_test_case<triqs::mesh::imfreq>("ref_data_lfs/la327-2222-strain-w90-fixedgrid.ref.h5", 1.e-6, self_energy_kind_e::Sigma);
// }
#endif

MPI_TEST_MAIN
