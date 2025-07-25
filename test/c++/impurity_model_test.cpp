#include <nda/gtest_tools.hpp>
#include <string>
#include "./common.hpp"
#include "triqs_modest/impurity_model.hpp"
#include "triqs_modest/utils/h5_proxy.hpp"
#include "triqs_modest/dft_code_specific.hpp"

using namespace triqs;

TEST(impurity_model_tests, imp_model_kanamori) { // NOLINT
  auto spin_names = std::vector<std::string>{"up", "down"};
  auto dim_gamma  = std::vector<long>{3};
  auto model      = make_kanamori(spin_names, dim_gamma, 3.0, 1.0, 1.0);
}

TEST(impurity_model_tests, imp_model_kanamori2) { // NOLINT
  auto spin_names = std::vector<std::string>{"up", "down"};
  auto dim_gamma  = std::vector<long>{1, 1, 1};
  auto model      = make_kanamori(spin_names, dim_gamma, 3.0, 1.0, 1.0);
}

TEST(impurity_model_tests, imp_model_from_embedding) {
  std::string filename = "ref_data/svo-wien2k.ref.h5";
  for (auto threshold : std::vector<double>{1.e-5, 1.e-3}) {
    auto [_, obe] = one_body_elements_from_dft_converter(filename, threshold);
    auto E        = make_embedding_with_equivalences(obe.C_space);
    auto model    = make_kanamori(E.sigma_names(), E.imp_decomposition(0), 3.0, 2.0, 0.5);
  }
}

TEST(impurity_model_tests, slater_model_from_embedding) {
  auto [_, obe] = one_body_elements_from_dft_converter("ref_data/la5ni3o11-wien2k.ref.h5", 1e-2);
  auto E        = make_embedding_with_equivalences(obe.C_space);
  // FIXME: is this the cleanest way to map from atom_idx to imp_idx?
  for (auto imp_idx = 0; auto [atom, shell] : enumerate(obe.C_space.atomic_shells())) {
    if (obe.C_space.first_shell_of_its_equiv_cls(atom) == imp_idx) {
      auto model = make_slater(E.sigma_names(), E.imp_decomposition(imp_idx), 10.0, 1.0, obe.C_space.rotation_from_spherical_to_dft_basis()(imp_idx),
                               obe.C_space.rotation_from_dft_to_local_basis()(imp_idx, 0));
      imp_idx++;
    }
  }
}

TEST(impurity_model_tests, spherical_umatrix_slater_construction) {
  // auto spherical_to_cubic_conventions = {"wien2k", "qe", "vasp", "wannier90"};
  auto root   = h5::proxy{"ref_data/u_matrix_slater.ref.h5", 'r'};
  auto U_int  = as<double>(root["U_int"]);
  auto J_hund = as<double>(root["J_hund"]);
  auto ls     = std::vector<long>{1, 2, 3};
  for (auto l : ls) {
    auto Umat_ref = as<nda::array<double, 4>>(root[std::to_string(l)]["spherical"]);
    auto Umat     = U_matrix_in_spherical_basis(l, U_int, J_hund);
    EXPECT_ARRAY_NEAR(Umat, Umat_ref, 1e-14);
  }
}

TEST(impurity_model_tests, wien2k_umatrix_slater_construction) {
  // auto spherical_to_cubic_conventions = {"wien2k", "qe", "vasp", "wannier90"};
  auto root   = h5::proxy{"ref_data/u_matrix_slater.ref.h5", 'r'};
  auto U_int  = as<double>(root["U_int"]);
  auto J_hund = as<double>(root["J_hund"]);
  auto ls     = std::vector<long>{1, 2};
  for (auto l : ls) {
    auto Umat_ref = as<nda::array<dcomplex, 4>>(root[std::to_string(l)]["wien2k"]);
    auto T        = dft_code::get_spherical_to_dft_rotation_Wien2k(l);
    auto Umat     = U_matrix_in_local_basis(l, T, U_int, J_hund);
    EXPECT_ARRAY_NEAR(Umat, Umat_ref, 1e-14);
  }
}
