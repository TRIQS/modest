#include "./common.hpp" // IWYU pragma: keep
#include "triqs_modest/loaders.hpp"
#include "triqs_modest/transport.hpp"

using namespace triqs;
using namespace triqs::modest;

namespace {
  // Shared input: the SVO transport archive carries the t2g projectors + dispersion, the band velocities
  // and cell volume (dft_transp_input), and — under ref_data — the reference transport distribution.
  constexpr auto svo_ref = "ref_data/svo_transport.ref.h5";

  one_body_elements_on_grid load_svo_transport_obe() {
    return one_body_elements_from_dft_converter(svo_ref, 1e-5, false, /*read_velocities=*/true).second;
  }
} // namespace

// Loading the transport input attaches the band velocities and cell volume, shaped consistently with H(k).
TEST(transport_tests, load_velocities_svo) { // NOLINT
  auto obe = load_svo_transport_obe();

  ASSERT_TRUE(obe.velocities.has_value());
  ASSERT_TRUE(obe.cell_volume.has_value());

  auto const &vel = *obe.velocities;
  ASSERT_EQ(vel.n_k(), obe.H.n_k());
  ASSERT_EQ(vel.n_directions(), 3);
  ASSERT_EQ(vel.v_k.extent(0), obe.H.n_k());
}

// Transport distribution Γ_xx(ω,Ω) at Σ = 0, checked against the reference in the archive's ref_data group.
// That reference is the triqs_dft_tools transport_distribution converted to ModEST units (eV·Å velocities,
// Å³ volume); the two codes were validated to agree to ~1e-17 (see doc/userguide/transport). DFTTools reports
// Γ only on [ew0-Ω, ew1+Ω] and zero-pads a max(Ω) margin, so the reference (and this comparison) covers the
// inner window [10, 390] of the [-4.2, 4.2] / 401-point mesh used to generate it.
TEST(transport_tests, transport_distribution_svo) { // NOLINT
  auto obe       = load_svo_transport_obe();
  auto root      = h5::proxy{svo_ref, 'r'};
  auto Gamma_ref = as<nda::array<double, 2>>(root["ref_data"]["Gamma_xx"]); // (n_Om, n_overlap)

  // Σ = 0 on the uniform real-frequency mesh the reference was generated on.
  auto E       = make_embedding(obe.C_space);
  auto mesh    = triqs::mesh::refreq{-4.2, 4.2, 401};
  auto Sigma_w = E.embed(triqs::make_vec_block_gf(mesh, E.imp_block_structure()));

  auto Om_mesh                        = nda::array<double, 1>{0.0, 0.105, 0.21}; // multiples of dω (exact grid shift)
  std::vector<std::string> directions = {"xx"};

  auto td = transport_distribution(obe, /*mu=*/0.0, Sigma_w, Om_mesh, directions, /*broadening=*/0.05);

  ASSERT_EQ(td.Gamma.extent(0), 1);   // one direction
  ASSERT_EQ(td.Gamma.extent(1), 3);   // three Ω
  ASSERT_EQ(td.Gamma.extent(2), 401); // n_ω

  auto win = nda::range(10, 401 - 10); // inner window covered by the reference
  for (long iq = 0; iq < td.Gamma.extent(1); ++iq)
    EXPECT_ARRAY_NEAR(nda::array<double, 1>{td.Gamma(0, iq, win)}, Gamma_ref(iq, r_all), 1e-10);
}
