#include "./common.hpp"
#include "triqs_modest/chargedensity.hpp"

// TODO: Test against dft_tools.sum_kdft.calc_density_correction
auto run_charge_update_case(std::string filename) {

  auto [target_density, obe] = one_body_elements_from_dft_converter(filename);
  auto E                     = make_embedding(obe.C_space);

  auto const &mesh = mesh::imfreq{40.0, statistic_enum::Fermion, 1025};

  auto Sigma_imps        = E.make_zero_imp_self_energies(mesh);
  auto Sigma_imp_dynamic = Sigma_imps | stdv::transform([](auto &x) { return x.first; }) | tl::to<std::vector>();
  auto Sigma_imp_static  = Sigma_imps | stdv::transform([](auto &x) { return x.second; }) | tl::to<std::vector>();

  auto [Sigma_embed, Sigma_H_embed] = E.embed(Sigma_imp_dynamic, Sigma_imp_static);
  double mu                         = -0.04445945328245646;
  auto N_nu_nup_k                   = charge_density_correction(obe, mu, Sigma_embed, Sigma_H_embed);
  return 0;
}

TEST(charge_update, charge_wien2k_svo3) { // NOLINT
  run_charge_update_case("ref_data/SrVO3-cubic-t2g.ref.h5");
}

// The dlr_imfreq instantiation must give the same N_nu_nup_k as the imfreq one, for a NON-ZERO
// Sigma (single pole V^2/(iw - eb)) built independently on both meshes.
TEST(charge_update, charge_dlr_matches_imfreq_svo3) { // NOLINT
  auto [target_density, obe] = one_body_elements_from_dft_converter("ref_data/SrVO3-cubic-t2g.ref.h5");
  auto E                     = make_embedding(obe.C_space);

  double const beta = 40.0;
  double const V = 0.8, eb = 1.3;

  auto const &mesh_full = mesh::imfreq{beta, statistic_enum::Fermion, 1025};
  auto const &mesh_dlr  = mesh::dlr_imfreq{beta, statistic_enum::Fermion, 80.0, 1e-10};

  auto make_pole_sigma = [&](auto const &mesh) {
    auto Sigma_imps = E.make_zero_imp_self_energies(mesh);
    for (auto &[dyn, inf] : Sigma_imps) {
      for (auto &g : dyn) {
        for (auto const &iw : g.mesh()) { g[iw] = (V * V / (dcomplex(iw) - eb)) * nda::eye<dcomplex>(g.target_shape()[0]); }
      }
    }
    auto Sigma_imp_dynamic = Sigma_imps | stdv::transform([](auto &x) { return x.first; }) | tl::to<std::vector>();
    auto Sigma_imp_static  = Sigma_imps | stdv::transform([](auto &x) { return x.second; }) | tl::to<std::vector>();
    return E.embed(Sigma_imp_dynamic, Sigma_imp_static);
  };

  auto [Sigma_full, Sigma_H_full] = make_pole_sigma(mesh_full);
  auto [Sigma_dlr, Sigma_H_dlr]   = make_pole_sigma(mesh_dlr);

  double mu   = -0.04445945328245646;
  auto N_full = charge_density_correction(obe, mu, Sigma_full, Sigma_H_full);
  auto N_dlr  = charge_density_correction(obe, mu, Sigma_dlr, Sigma_H_dlr);

  EXPECT_ARRAY_NEAR(N_full, N_dlr, 1e-4);
}
