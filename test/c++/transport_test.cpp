#include "./common.hpp" // IWYU pragma: keep
#include "triqs_modest/loaders.hpp"
#include "triqs_modest/transport.hpp"
#include "triqs_modest/lattice_gf_helpers.hpp"
#include <cmath>

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

// The Σ = 0 checks above leave the Woodbury *correction* term untouched: with Σ = 0 the band-basis G is
// just the diagonal bare propagator D⁻¹, so `build_G_band` never exercises the rank-reduced upfolding
// (Q, K, apply_K) that is the whole point of the port. This test drives it with a NON-ZERO self-energy
// and compares Γ against an independent reference in which G is assembled by an explicit N_ν × N_ν
// inversion of (ω + iδ + μ) − H(k) − P†Σ(ω)P (the upfolding uses the shared `upfold_self_energy_all_freq`
// helper; only the inversion path differs from the Woodbury kernel). Agreement to ~1e-9 pins the port.
TEST(transport_tests, woodbury_nonzero_sigma) { // NOLINT
  auto obe = load_svo_transport_obe();

  auto E    = make_embedding(obe.C_space);
  auto mesh = triqs::mesh::refreq{-2.0, 2.0, 121};

  // Non-zero, orbital-/block-dependent constant self-energy on the impurity block(s), then embed.
  auto Sigma_imp = triqs::make_vec_block_gf(mesh, E.imp_block_structure());
  long n_w       = mesh.size();
  for (long iv = 0; iv < long(Sigma_imp.size()); ++iv) {
    auto &bg = Sigma_imp[iv];
    for (long b = 0; b < bg.size(); ++b) {
      long dim = bg[b].target_shape()[0];
      for (long n = 0; n < n_w; ++n)
        for (long i = 0; i < dim; ++i) bg[b].data()(n, i, i) = dcomplex(0.10 + 0.02 * b, -0.15 - 0.05 * i);
    }
  }
  auto Sigma_w = E.embed(Sigma_imp);

  double const mu         = 0.3;
  double const broadening = 0.02;
  auto Om_mesh            = nda::array<double, 1>{0.0};
  std::vector<std::string> directions = {"xx", "xy"};

  auto td = transport_distribution(obe, mu, Sigma_w, Om_mesh, directions, broadening);

  // ---- independent full-inversion reference (single rank; the ctest runs serially) ----
  auto const &vel = *obe.velocities;
  double V        = *obe.cell_volume;
  auto spin_kind  = obe.C_space.spin_kind();
  long n_spin     = (spin_kind == spin_kind_e::Polarized) ? 2 : 1;
  auto syms       = vel.rot_symmetries.empty() ? std::vector<nda::matrix<double>>{nda::eye<double>(3)} : vel.rot_symmetries;
  double sym_norm = 1.0 / double(syms.size());
  auto im         = dcomplex(0, 1.0);

  auto omega_arr = nda::array<double, 1>(n_w);
  for (auto &&[i, w] : enumerate(mesh)) omega_arr(i) = dcomplex(w).real();

  std::vector<std::array<long, 2>> pairs = {{0, 0}, {0, 1}}; // xx, xy
  long n_dir                             = long(pairs.size());
  auto Gamma_ref                         = nda::zeros<double>(n_dir, 1, n_w);

  using nda::linalg::inv;
  for (long k = 0; k < obe.H.n_k(); ++k) {
    double wk = obe.H.k_weights(k);
    for (long s = 0; s < n_spin; ++s) {
      long sp    = sigma_to_data_idx(spin_kind, s);
      long a_off = vel.joint_window(sp, k, 0);
      long v_off = vel.joint_window(sp, k, 1);
      long n_ov  = vel.joint_window(sp, k, 2);
      if (n_ov <= 0) continue;
      long N_nu = obe.H.N_nu(s, k);
      nda::matrix<dcomplex> Hk = obe.H.H(s, k);
      auto PSP = triqs::modest::detail::upfold_self_energy_all_freq(obe, obe.P, Sigma_w, k, s); // (n_w, N_nu, N_nu)

      auto A_sl = nda::range(a_off, a_off + n_ov);
      auto v_sl = nda::range(v_off, v_off + n_ov);
      std::vector<nda::matrix<dcomplex>> A(n_w);
      for (long n = 0; n < n_w; ++n) {
        nda::matrix<dcomplex> Ginv = -Hk;
        Ginv -= nda::matrix<dcomplex>{PSP(n, r_all, r_all)};
        for (long i = 0; i < N_nu; ++i) Ginv(i, i) += omega_arr(n) + im * broadening + mu;
        nda::matrix<dcomplex> G  = inv(Ginv);
        nda::matrix<dcomplex> Gs = G(A_sl, A_sl);
        nda::matrix<dcomplex> Ad = im * (Gs - dagger(Gs)) / (2.0 * M_PI);
        A[n]                     = Ad;
      }

      auto vfull = vel.v(s, k); // (3, N_nu_v, N_nu_v)
      std::array<nda::matrix<dcomplex>, 3> vb;
      for (long d = 0; d < 3; ++d) vb[d] = nda::matrix<dcomplex>{vfull(d, v_sl, v_sl)};

      for (auto const &R : syms) {
        std::array<nda::matrix<dcomplex>, 3> vR;
        for (long d = 0; d < 3; ++d) {
          vR[d] = nda::zeros<dcomplex>(n_ov, n_ov);
          for (long c = 0; c < 3; ++c) vR[d] += R(d, c) * vb[c];
        }
        for (long ip = 0; ip < n_dir; ++ip) {
          for (long n = 0; n < n_w; ++n) {
            nda::matrix<dcomplex> Ma = vR[pairs[ip][0]] * A[n];
            nda::matrix<dcomplex> Mb = vR[pairs[ip][1]] * A[n];
            dcomplex tot{0, 0};
            for (long i = 0; i < n_ov; ++i)
              for (long j = 0; j < n_ov; ++j) tot += Ma(i, j) * Mb(j, i);
            Gamma_ref(ip, 0, n) += wk * sym_norm * tot.real();
          }
        }
      }
    }
  }
  Gamma_ref /= V;

  ASSERT_EQ(td.Gamma.extent(0), n_dir);
  ASSERT_EQ(td.Gamma.extent(1), 1);
  ASSERT_EQ(td.Gamma.extent(2), n_w);
  // Reference exercises the rank-reduced Woodbury against a dense inversion — expect agreement to ~1e-9.
  EXPECT_GT(nda::sum(nda::abs(Gamma_ref)), 1e-6); // guard: the self-energy actually moved Γ
  EXPECT_ARRAY_NEAR(nda::array<double, 3>{td.Gamma}, Gamma_ref, 1e-9);
}
