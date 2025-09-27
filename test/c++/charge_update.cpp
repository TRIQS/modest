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
