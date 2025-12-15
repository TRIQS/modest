#include "./common.hpp" // IWYU pragma: keep
#include "triqs_modest/loaders.hpp"

using namespace triqs;
using itertools::zip;

auto test_load(std::string filename) {
  std::cout << "Loading ref_data from data in HDF5 " << filename << std::endl;
  auto [density, data] = one_body_elements_from_dft_converter(filename);
  std::cout << data << std::endl;
  auto root          = h5::proxy{filename, 'r'};
  auto dft_code      = as<std::string>(root["dft_input"]["dft_code"]);
  auto matrix_valued = (dft_code == "w90") ? true : false;
  ASSERT_EQ(data.H.matrix_valued, matrix_valued);
};

auto test_load_data_with_symm_ops(std::string filename) {
  std::cout << "Loading ref_data from data in HDF5 " << filename << std::endl;
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

TEST(load_data_tests, load_dft_from_h5_wien2k_fixed_grid) { // NOLINT
  test_load("ref_data/svo-wien2k.ref.h5");
}

TEST(load_data_tests, load_dft_from_h5_vasp_fixed_grid) { // NOLINT
  auto h5_files = {"ref_data/prnio3-vasp.ref.h5", "ref_data/lunio3-vasp.ref.h5", "ref_data/nio.ref.h5"};
  for (auto &&file : h5_files) test_load(file);
}

TEST(load_data_tests, load_dft_from_w90_qe_fixed_grid) { // NOLINT
  auto [target_density, obe] = one_body_elements_from_dft_converter("ref_data/lco_qe_dp_w90_grid.ref.h5");
  std::cout << obe << std::endl;
  ASSERT_TRUE(obe.H.matrix_valued);
}

TEST(load_data_tests, obe_high_symm_path) {
  auto [_, obe] = one_body_elements_from_dft_converter("ref_data/SrVO3-cubic-t2g.ref.h5");
  std::cout << obe << std::endl;
  auto obe_bands = one_body_elements_on_high_symmetry_path("ref_data/SrVO3-cubic-t2g.ref.h5", obe);
  std::cout << obe_bands << std::endl;
}

TEST(load_data_tests, obe_parproj) {
  auto [_, obe] = one_body_elements_from_dft_converter("ref_data/SrVO3-cubic-t2g.ref.h5");
  std::cout << obe << std::endl;
  auto obe_dos = one_body_elements_with_theta_projectors("ref_data/SrVO3-cubic-t2g.ref.h5", obe);
  std::cout << obe_dos << std::endl;
}

TEST(load_data_tests, obe_gw) { auto obe_gw = make_one_body_elements_gw("ref_data/nd112.mlwf.h5"); }

#if LFS
TEST(load_data_tests, load_dft_from_h5_wien2k_soc_fixed_grid) { // NOLINT
  auto h5_files = {"ref_data_lfs/sriro3-wien2k-soc.ref.h5", "ref_data_lfs/sr2mgoso6-wien2k-soc.ref.h5"};
  for (auto &&file : h5_files) test_load(file);
}

TEST(load_data_tests, load_dft_data_with_symm_ops) {
  auto h5_files = {"ref_data_lfs/liv2o4-r-3m-strained-wien2k.ref.h5", "ref_data_lfs/sriro3-wien2k-soc.ref.h5",
                   "ref_data_lfs/sr2mgoso6-wien2k-soc.ref.h5"};
  for (auto &&file : h5_files) test_load_data_with_symm_ops(file);
}
#endif