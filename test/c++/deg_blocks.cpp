#include "./common.hpp"
#include "triqs_modest/degenerate_blocks.hpp"

auto get_deg_blocks(std::string filename) {
  auto [_, obe] = one_body_elements_from_dft_converter(filename);
  auto E        = make_embedding_with_equivalences(obe.C_space);
  auto mesh     = mesh::imfreq{40.0, triqs::mesh::Fermion, 1025};
  return E.extract(gloc(mesh, obe, 0.0)) | stdv::transform([](auto &G) { return analyze_degenerate_blocks(G); })
     | tl::to<std::vector>();
}

TEST(deg_blocks, SrVO3) {
  auto deg_blocks = get_deg_blocks("ref_data/SrVO3-cubic-t2g.ref.h5");
  auto ref        = std::vector<std::vector<std::vector<long>>>{{{0, 1, 2, 3, 4, 5}}};
  EXPECT_EQ(deg_blocks, ref);
}

TEST(deg_blocks, LiV2O4) {
  auto deg_blocks = get_deg_blocks("ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5");
  auto ref        = std::vector<std::vector<std::vector<long>>>{
     {{0, 3}, {1, 2, 4, 5}},
     {{0, 2}, {1, 3}},
  };
  EXPECT_EQ(deg_blocks, ref);
}

TEST(deg_blocks, SrVO3_symmetrize) {
  auto [_, obe] = one_body_elements_from_dft_converter("ref_data/SrVO3-cubic-t2g.ref.h5");
  auto E        = make_embedding_with_equivalences(obe.C_space);
  auto mesh     = mesh::imfreq{40.0, triqs::mesh::Fermion, 1025};
  auto Gimp     = E.extract(gloc(mesh, obe, 0.0));
  auto deg_blocks =
     Gimp | stdv::transform([](auto &G) { return analyze_degenerate_blocks(G); }) | tl::to<std::vector>();
  auto Gsymm = Gimp
     | stdv::transform([deg_blocks, idx = 0](auto const &Gin) mutable { return symmetrize_gf(Gin, deg_blocks[idx++]); })
     | tl::to<std::vector>();
}

TEST(deg_blocks, LiV2O4_symmetrize) {
  auto [_, obe] = one_body_elements_from_dft_converter("ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5");
  auto E        = make_embedding_with_equivalences(obe.C_space);
  auto mesh     = mesh::imfreq{40.0, triqs::mesh::Fermion, 1025};
  auto Gimp     = E.extract(gloc(mesh, obe, 0.0));
  auto deg_blocks =
     Gimp | stdv::transform([](auto &G) { return analyze_degenerate_blocks(G); }) | tl::to<std::vector>();
  auto Gsymm = Gimp
     | stdv::transform([deg_blocks, idx = 0](auto const &Gin) mutable { return symmetrize_gf(Gin, deg_blocks[idx++]); })
     | tl::to<std::vector>();
}
