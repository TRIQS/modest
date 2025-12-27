#include <nda/gtest_tools.hpp>
#include <string>
#include "./common.hpp"
#include "triqs_modest/hamiltonians.hpp"
#include "triqs_modest/utils/h5_proxy.hpp"
#include "triqs_modest/dft_tools/utils.hpp"
#include "triqs_modest/dft_tools/spherical_rotation.hpp"

using namespace triqs::operators;

TEST(hamiltonian_tests, kanamori) { // NOLINT
  auto spin_names    = std::vector<std::string>{"up", "down"};
  auto n_orb         = 2;
  auto U             = 3.0;
  auto J             = 1.0;
  auto Up            = U - 2 * J;
  auto [Umat, Upmat] = U_matrix_kanamori(n_orb, U, Up, J);
  auto h_int         = h_int_kanamori(Umat, Upmat, J, n_orb, spin_names);

  auto h_ref = 3 * n("up", 0) * n("down", 0) + 3 * n("up", 1) * n("down", 1) + 3 * n("up", 2) * n("down", 2);

  // TODO: refactor reference data to explicit construction
  EXPECT_PRINT(
     "1*c_dag('down',0)*c_dag('up',0)*c('up',1)*c('down',1) + 3*c_dag('down',0)*c_dag('up',0)*c('up',0)*c('down',0) + 1*c_dag('down',0)*c_dag('up',1)*c('up',1)*c('down',0) + 1*c_dag('down',0)*c_dag('up',1)*c('up',0)*c('down',1) + 1*c_dag('down',1)*c_dag('up',0)*c('up',1)*c('down',0) + 1*c_dag('down',1)*c_dag('up',0)*c('up',0)*c('down',1) + 3*c_dag('down',1)*c_dag('up',1)*c('up',1)*c('down',1) + 1*c_dag('down',1)*c_dag('up',1)*c('up',0)*c('down',0)",
     h_int);
}

TEST(hamiltonian_tests, kanamori_with_rename) { // NOLINT
  auto spin_names    = std::vector<std::string>{"up", "down"};
  auto n_orb         = 2;
  auto U             = 3.0;
  auto J             = 1.0;
  auto Up            = U - 2 * J;
  auto [Umat, Upmat] = U_matrix_kanamori(n_orb, U, Up, J);

  std::map<op_name, op_name> op_map;
  for (auto const &s : spin_names) {
    for (auto o : range(n_orb)) { op_map[{s, o}] = {fmt::format("{}_{}", s, o), 0}; }
  }

  // TODO: refactor reference data to explicit construction
  auto h_int = rename_op(h_int_kanamori(Umat, Upmat, J, n_orb, spin_names), op_map);
  EXPECT_PRINT(
     "1*c_dag('down_0',0)*c_dag('up_0',0)*c('up_1',0)*c('down_1',0) + 3*c_dag('down_0',0)*c_dag('up_0',0)*c('up_0',0)*c('down_0',0) + 1*c_dag('down_0',0)*c_dag('up_1',0)*c('up_1',0)*c('down_0',0) + 1*c_dag('down_0',0)*c_dag('up_1',0)*c('up_0',0)*c('down_1',0) + 1*c_dag('down_1',0)*c_dag('up_0',0)*c('up_1',0)*c('down_0',0) + 1*c_dag('down_1',0)*c_dag('up_0',0)*c('up_0',0)*c('down_1',0) + 3*c_dag('down_1',0)*c_dag('up_1',0)*c('up_1',0)*c('down_1',0) + 1*c_dag('down_1',0)*c_dag('up_1',0)*c('up_0',0)*c('down_0',0)",
     h_int);
}

TEST(hamiltonian_test, kanamori_from_embedding) {

  auto spin_names    = std::vector<std::string>{"up", "down"};
  auto n_orb         = 3;
  auto U             = 3.0;
  auto J             = 0.5;
  auto Up            = U - 2 * J;
  auto [Umat, Upmat] = U_matrix_kanamori(n_orb, U, Up, J);

  std::map<op_name, op_name> op_map;
  for (auto const &s : spin_names) {
    for (auto o : range(n_orb)) { op_map[{s, o}] = {fmt::format("{}_{}", s, o), 0}; }
  }
  auto h_int1 = rename_op(h_int_kanamori(Umat, Upmat, J, n_orb, spin_names), op_map);

  std::string filename = "ref_data/svo-wien2k.ref.h5";
  auto [_, obe]        = one_body_elements_from_dft_converter(filename, 1.e-3);
  auto E               = make_embedding(obe.C_space);
  auto h_int2          = make_kanamori(E.sigma_names(), E.imp_decomposition(0), 3.0, 2.0, 0.5);

  EXPECT_TRUE((h_int1 - h_int2).is_zero());
}

TEST(hamiltonian_tests, spherical_umatrix_slater_construction) {
  // auto spherical_to_cubic_conventions = {"wien2k", "qe", "vasp", "wannier90"};
  auto root   = h5::proxy{"ref_data/u_matrix_slater.ref.h5", 'r'};
  auto U_int  = as<double>(root["U_int"]);
  auto J_hund = as<double>(root["J_hund"]);
  auto ls     = std::vector<long>{1, 2, 3};
  for (auto l : ls) {
    auto Umat_ref = as<nda::array<double, 4>>(root[std::to_string(l)]["spherical"]);
    auto Umat     = U_matrix_slater_spherical(l, U_int, J_hund);
    EXPECT_ARRAY_NEAR(Umat, Umat_ref, 1e-14);
  }
}

TEST(hamiltonian_tests, wien2k_umatrix_slater_construction) {
  // auto spherical_to_cubic_conventions = {"wien2k", "qe", "vasp", "wannier90"};
  auto root   = h5::proxy{"ref_data/u_matrix_slater.ref.h5", 'r'};
  auto U_int  = as<double>(root["U_int"]);
  auto J_hund = as<double>(root["J_hund"]);
  auto ls     = std::vector<long>{1, 2};
  for (auto l : ls) {
    auto Umat_ref = as<nda::array<dcomplex, 4>>(root[std::to_string(l)]["wien2k"]);
    auto T        = dft_tools::wien2k::get_spherical_to_dft_rotation(l);
    auto Umat     = U_matrix_slater_local(l, T, U_int, J_hund);
    EXPECT_ARRAY_NEAR(Umat, Umat_ref, 1e-14);
  }
}
