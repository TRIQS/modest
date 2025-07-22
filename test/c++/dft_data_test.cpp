#include "./common.hpp" // IWYU pragma: keep
#include "triqs_modest/loaders.hpp"

using namespace triqs;
using itertools::zip;

auto test_load(std::string filename) {
  std::cout << "Loading dft_data from data in HDF5 " << filename << std::endl;
  auto [density, data] = one_body_elements_from_dft_converter(filename);
  std::cout << data << std::endl;
  // auto root  = h5::proxy{filename, 'r'};
  // auto g_dft = root["dft_input"];
  // auto target_density = double(g_dft["density_required"]) - double(g_dft["charge_below"]);
  // EXPECT_DOUBLE_EQ(density, target_density);
  // EXPECT_EQ(long(g_dft["n_corr_shells"]), data.atomic_shells().size());

  // FIXME : OP : CAN NOT TEST THIS ANYMORe
  //EXPECT_ARRAY_EQ(std::get<triqs::one_body_elements::on_grid>(data.dispersion).H_k, hopping);
  //EXPECT_ARRAY_EQ(std::get<triqs::one_body_elements::on_grid>(data.dispersion).n_bands_per_k,
  //                as<nda::matrix<int>>(g_dft["n_orbitals"]));
  // auto P_k = as<nda::array<dcomplex, 5>>(g_dft["proj_mat"]);
  // for (auto const &alpha : range(data.n_alpha()))
  //   EXPECT_ARRAY_EQ(std::get<triqs::one_body_elements::on_grid>(data.dispersion).P_k[alpha],
  //                   P_k(r_all, r_all, alpha, r_all, r_all));
};

auto test_load_data_with_symm_ops(std::string filename) {
  std::cout << "Loading dft_data from data in HDF5 " << filename << std::endl;
  auto [_, data] = one_body_elements_from_dft_converter(filename);
  std::cout << data << std::endl;
  auto root  = h5::proxy{filename, 'r'};
  auto perms = to_vector<std::vector<long>>(sort_keys_as_int(root["ref_data"]["permutations"]));
  if (data.ibz_symm_ops) {
    auto ops = data.ibz_symm_ops->ops;
    ASSERT_EQ(ops.size(), perms.size());
    for (auto &&[sym, sym_ref] : zip(ops, perms)) { ASSERT_EQ(sym.permutation, sym_ref); }
  }
}

TEST(dft_data_tests, load_dft_from_h5_w90_fixed_grid) { // NOLINT
  auto h5_files = {"dft_data/la327-1313-p30-w90-fixedgrid.ref.h5", "dft_data/la327-2222-strain-w90-fixedgrid.ref.h5"};
  for (auto &&file : h5_files) test_load(file);
}
TEST(dft_data_tests, load_dft_from_h5_wien2k_fixed_grid) { // NOLINT
  auto h5_files = {"dft_data/liv2o4-r-3m-strained-wien2k.ref.h5", "dft_data/svo-wien2k.ref.h5"};
  for (auto &&file : h5_files) test_load(file);
}
TEST(dft_data_tests, load_dft_from_h5_vasp_fixed_grid) { // NOLINT
  auto h5_files = {"dft_data/prnio3-vasp.ref.h5", "dft_data/lunio3-vasp.ref.h5", "dft_data/nio.ref.h5"};
  for (auto &&file : h5_files) test_load(file);
}
TEST(dft_data_tests, load_dft_from_h5_wien2k_soc_fixed_grid) { // NOLINT
  auto h5_files = {"dft_data/sriro3-wien2k-soc.ref.h5", "dft_data/sr2mgoso6-wien2k-soc.ref.h5"};
  for (auto &&file : h5_files) test_load(file);
}

TEST(dft_data_tests, load_dft_data_with_symm_ops) {
  auto h5_files = {"dft_data/liv2o4-r-3m-strained-wien2k.ref.h5", "dft_data/sriro3-wien2k-soc.ref.h5",
                   "dft_data/sr2mgoso6-wien2k-soc.ref.h5"};
  for (auto &&file : h5_files) test_load_data_with_symm_ops(file);
}

TEST(dft_data_tests, obe_high_symm_path) {
  auto [_, obe] = one_body_elements_from_dft_converter("dft_data/SrVO3-cubic-t2g.ref.h5");
  std::cout << obe << std::endl;
  auto obe_bands = one_body_elements_on_high_symmetry_path("dft_data/SrVO3-cubic-t2g.ref.h5", obe);
  std::cout << obe_bands << std::endl;
}

TEST(dft_data_tests, obe_parproj) {
  auto [_, obe] = one_body_elements_from_dft_converter("dft_data/SrVO3-cubic-t2g.ref.h5");
  std::cout << obe << std::endl;
  auto obe_dos = one_body_elements_with_theta_projectors("dft_data/SrVO3-cubic-t2g.ref.h5", obe);
  std::cout << obe_dos << std::endl;
}
