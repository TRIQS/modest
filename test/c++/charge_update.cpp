#include "./common.hpp"
#include "triqs_modest/chargedensity.hpp"

// TODO: Test against dft_tools.sum_kdft.calc_density_correction
auto run_charge_update_case(std::string filename) {

  auto [target_density, obe] = one_body_elements_from_dft_converter(filename);
  auto E                     = make_embedding(obe.C_space);

  auto Sigma_imps        = E.make_zero_imp_self_energies(mesh::imfreq{40.0, statistic_enum::Fermion, 1025});
  auto Sigma_imp_dynamic = Sigma_imps | stdv::keys | tl::to<std::vector>();
  auto Sigma_imp_static  = Sigma_imps | stdv::values | tl::to<std::vector>();

  auto [Sigma_embed, Sigma_H_embed] = E.embed(Sigma_imp_dynamic, Sigma_imp_static);
  double mu                         = -0.04445945328245646;
  auto N_nu_nup_k                   = charge_density_correction(obe, mu, Sigma_embed, Sigma_H_embed);
  return 0;
}

TEST(charge_update, charge_wien2k_svo3) { // NOLINT
  run_charge_update_case("ref_data/SrVO3-cubic-t2g.ref.h5");
}

// charge_density_correction on a dlr_imfreq Sigma must match the same Sigma expanded onto a dense imfreq mesh.
TEST(charge_update, charge_dlr_matches_imfreq_svo3) { // NOLINT
  auto [target_density, obe] = one_body_elements_from_dft_converter("ref_data/SrVO3-cubic-t2g.ref.h5");
  auto E                     = make_embedding(obe.C_space);

  auto Sigma_imps        = E.make_zero_imp_self_energies(mesh::dlr_imfreq{40.0, statistic_enum::Fermion, 80.0, 1e-10});
  auto Sigma_imp_dynamic = Sigma_imps | stdv::keys | tl::to<std::vector>();
  auto Sigma_imp_static  = Sigma_imps | stdv::values | tl::to<std::vector>();

  // single pole Sigma = V^2 / (iw - eb)
  double const V = 0.8, eb = 1.3;
  nda::clef::placeholder<0> iw_;
  for (auto &g : Sigma_imp_dynamic[0]) { g(iw_) << V * V / (iw_ - eb); }
  auto [Sigma_dlr, Sigma_H] = E.embed(Sigma_imp_dynamic, Sigma_imp_static);

  double mu   = -0.04445945328245646;
  auto N_dlr  = charge_density_correction(obe, mu, Sigma_dlr, Sigma_H);
  auto N_full = charge_density_correction(obe, mu, make_gf_imfreq<>(Sigma_dlr, 1025), Sigma_H);

  EXPECT_ARRAY_NEAR(N_full, N_dlr, 1e-4);
}
