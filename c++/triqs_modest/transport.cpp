// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "./transport.hpp"
#include "./lattice_gf_helpers.hpp"
#include "utils/defs.hpp"
#include <h5/h5.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

namespace triqs::modest {

  namespace {

    // Map a Cartesian direction character to an index.
    long dir_char_to_int(char c) {
      switch (c) {
        case 'x': return 0;
        case 'y': return 1;
        case 'z': return 2;
        default: throw std::runtime_error{fmt::format("transport: invalid direction character '{}'", c)};
      }
    }

    // Parse direction labels like "xy" into (alpha, beta) index pairs.
    std::vector<std::array<long, 2>> parse_directions(std::vector<std::string> const &directions) {
      std::vector<std::array<long, 2>> pairs;
      pairs.reserve(directions.size());
      for (auto const &d : directions) {
        if (d.size() != 2) throw std::runtime_error{fmt::format("transport: direction '{}' must be two characters (e.g. \"xy\")", d)};
        pairs.push_back({dir_char_to_int(d[0]), dir_char_to_int(d[1])});
      }
      return pairs;
    }

    // HL: Can we make this simpler?
    // Nearest-grid-point shift maps for ω+Ω
    //   jmap(iq, n)  = index of the ω grid point nearest to ω_n + Om_mesh(iq)
    //   valid(iq, n) = 1 if ω_n + Om is within [ω_0, ω_{N-1}], else 0
    std::pair<nda::array<long, 2>, nda::array<long, 2>> build_shift_maps(std::vector<double> const &omega, nda::array<double, 1> const &Om) {
      long n  = long(omega.size());
      long nq = Om.extent(0);
      nda::array<long, 2> jmap(nq, n);
      nda::array<long, 2> valid(nq, n);
      double lo = omega.front();
      double hi = omega.back();
      for (long iq = 0; iq < nq; ++iq) {
        double O = Om(iq);
        for (long i = 0; i < n; ++i) {
          double t = omega[i] + O;
          if (t < lo || t > hi) {
            valid(iq, i) = 0;
            jmap(iq, i)  = 0;
            continue;
          }
          valid(iq, i) = 1;
          auto it      = std::lower_bound(omega.begin(), omega.end(), t);
          long r       = long(std::distance(omega.begin(), it));
          if (r <= 0) {
            jmap(iq, i) = 0;
          } else if (r >= n) {
            jmap(iq, i) = n - 1;
          } else {
            long l      = r - 1;
            jmap(iq, i) = (std::abs(omega[r] - t) < std::abs(omega[l] - t)) ? r : l;
          }
        }
      }
      return {jmap, valid};
    }

    // Build the full band-basis Green's function G(ω) = [ω + iδ + μ − H(k) − P†Σ(ω)P]⁻¹ for a fixed (k, σ),
    // returned as (n_ω, N_ν, N_ν). Uses the rank-reduced Woodbury identity when H(k) is diagonal
    // (`matrix_valued == false`), and a direct inversion otherwise.
    nda::array<dcomplex, 3> build_G_band(one_body_elements_on_grid const &obe, detail::active_subspace_t const &active,
                                         nda::array<dcomplex, 3> const &Sa_n, std::vector<dcomplex> const &omegas, double mu, long k_idx,
                                         long sigma) {
      long N_nu = obe.H.N_nu(sigma, k_idx);
      long n_w  = long(omegas.size());
      auto Hk   = obe.H.H(sigma, k_idx);
      auto G    = nda::zeros<dcomplex>(n_w, N_nu, N_nu);

      // Transport is only defined here for a diagonal (band-basis) dispersion, which is always the case for a
      // grid OBE from the DFT converter. A matrix-valued H(k) would need a per-ω eigenbasis and is not supported.
      if (obe.H.matrix_valued)
        throw std::runtime_error{"transport: matrix-valued H(k) is not supported (expected a diagonal band-basis dispersion)."};

      // Diagonal H(k): rank-reduced Woodbury.  G = D⁻¹ + D⁻¹Q† K Q D⁻¹,
      //   D(ν) = ω + iδ + μ − ε_ν,   Q = active rows of P (rank × N_ν),
      //   K = (Σ_a⁻¹ − Q D⁻¹ Q†)⁻¹  applied via detail::apply_K (no inversion of Σ).
      long rank = active.rank;
      auto P    = obe.P.P(sigma, k_idx); // (M × N_ν)
      auto Q    = nda::matrix<dcomplex>(rank, N_nu);
      for (long c = 0; c < rank; ++c)
        for (long nu = 0; nu < N_nu; ++nu) Q(c, nu) = P(active.c_indices[c], nu);

      for (long n = 0; n < n_w; ++n) {
        auto Dinv = nda::vector<dcomplex>(N_nu);
        for (long nu = 0; nu < N_nu; ++nu) {
          Dinv(nu)     = 1.0 / (omegas[n] + mu - Hk(nu, nu));
          G(n, nu, nu) = Dinv(nu);
        }
        if (rank == 0) continue;

        // Rmat = Q · D⁻¹  (rank × N_ν);   Yaa = Q D⁻¹ Q† = Rmat · Q†
        auto Rmat = nda::matrix<dcomplex>(rank, N_nu);
        for (long c = 0; c < rank; ++c)
          for (long nu = 0; nu < N_nu; ++nu) Rmat(c, nu) = Q(c, nu) * Dinv(nu);
        auto Yaa = nda::matrix<dcomplex>{Rmat * dagger(Q)};
        auto Sa  = nda::matrix<dcomplex>{Sa_n(n, r_all, r_all)};
        auto KR  = detail::apply_K(Sa, Yaa, Rmat); // K · (Q D⁻¹): (rank × N_ν)

        // Lmat = D⁻¹ Q†  (N_ν × rank);   correction = Lmat · KR  (≠ Rmat† for complex D).
        auto Lmat = nda::matrix<dcomplex>(N_nu, rank);
        for (long nu = 0; nu < N_nu; ++nu)
          for (long c = 0; c < rank; ++c) Lmat(nu, c) = Dinv(nu) * conj(Q(c, nu));
        G(n, r_all, r_all) += nda::matrix<dcomplex>{Lmat * KR};
      }
      return G;
    }

    // The Cartesian symmetry operations to average over (fall back to the identity if none are provided).
    std::vector<nda::matrix<double>> symmetry_ops(band_velocities const &vel) {
      if (vel.rot_symmetries.empty()) return {nda::eye<double>(3)};
      return vel.rot_symmetries;
    }

  } // namespace

  transport_distribution_t transport_distribution(one_body_elements_on_grid const &obe, double mu,
                                                  block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, nda::array<double, 1> const &Om_mesh,
                                                  std::vector<std::string> const &directions, double broadening) {
    if (!obe.velocities) throw std::runtime_error{"transport_distribution: obe.velocities is empty. Load with read_velocities=true."};

    auto const &vel = *obe.velocities;
    double V        = *obe.cell_volume;
    auto spin_kind  = obe.C_space.spin_kind();
    long n_spin     = (spin_kind == spin_kind_e::Polarized) ? 2 : 1;
    long spin_pol   = (spin_kind == spin_kind_e::NonPolarized) ? 0 : 1;

    auto const &mesh = Sigma_w(0, 0).mesh();
    long n_w         = mesh.size();
    auto im          = dcomplex(0, 1.0);
    auto delta       = im * broadening;

    auto omega_vals = mesh | stdv::transform([](auto w) { return dcomplex(w).real(); }) | tl::to<std::vector>();
    auto omegas     = mesh | stdv::transform([&](auto w) { return dcomplex(w) + delta; }) | tl::to<std::vector>();
    auto omega_arr  = nda::array<double, 1>(n_w);
    for (auto &&[i, om] : enumerate(omega_vals)) omega_arr(i) = om;

    auto dir_pairs     = parse_directions(directions);
    long n_dir         = long(dir_pairs.size());
    long n_Om          = Om_mesh.extent(0);
    auto [jmap, valid] = build_shift_maps(omega_vals, Om_mesh);

    // Cartesian directions actually used by the requested pairs (only these get a batched v·A GEMM below).
    std::array<bool, 3> need_dir{false, false, false};
    for (auto const &p : dir_pairs) {
      need_dir[p[0]] = true;
      need_dir[p[1]] = true;
    }

    // Active subspace and Σ_active per spin (independent of k).
    auto decomp       = get_struct(Sigma_w).dims(r_all, 0) | tl::to<std::vector>();
    auto active       = detail::detect_active_subspace(Sigma_w, decomp);
    auto Sa_per_sigma = range(n_spin) | stdv::transform([&](long s) { return detail::compute_sigma_active(Sigma_w, active, s); }) //
       | tl::to<std::vector>();

    auto syms       = symmetry_ops(vel);
    double sym_norm = 1.0 / double(syms.size());

    auto Gamma       = nda::zeros<double>(n_dir, n_Om, n_w);
    auto Gamma_intra = nda::zeros<double>(n_dir, n_Om, n_w);

    mpi::communicator comm = {};
    long n_k               = obe.H.n_k();

    for (auto k_idx : mpi::chunk(range(n_k), comm)) {
      double w_k = obe.H.k_weights(k_idx);
      for (long sigma = 0; sigma < n_spin; ++sigma) {
        long sp = sigma_to_data_idx(spin_kind, sigma);

        // Precomputed intersection between the dispersion/A window and the velocity window (see band_velocities).
        long a_off = vel.joint_window(sp, k_idx, 0);
        long v_off = vel.joint_window(sp, k_idx, 1);
        long n_ov  = vel.joint_window(sp, k_idx, 2);
        if (n_ov <= 0) continue;
        auto A_slice = nda::range(a_off, a_off + n_ov);
        auto v_slice = nda::range(v_off, v_off + n_ov);

        // Full band-basis G, then the block spectral function A = i(G − G†)/(2π) on the intersection.
        auto G_band = build_G_band(obe, active, Sa_per_sigma[sigma], omegas, mu, k_idx, sigma);
        // Spectral function A = i(G − G†)/(2π) on the intersection, stacked over ω as block-columns:
        //   A2(j, n·n_ov + l) = A_k(ω_n)_{jl}   (n_ov, n_ω·n_ov).
        // Stacking turns each v·A (a matrix product for every ω) into a single GEMM over all ω at once.
        auto A2 = nda::matrix<dcomplex>(n_ov, n_w * n_ov);
        for (long n = 0; n < n_w; ++n) {
          auto Gsub                                       = nda::matrix<dcomplex>{G_band(n, A_slice, A_slice)};
          A2(r_all, nda::range(n * n_ov, (n + 1) * n_ov)) = nda::matrix<dcomplex>{im * (Gsub - dagger(Gsub)) / (2.0 * M_PI)};
        }

        auto vfull = vel.v(sigma, k_idx); // (3, N_ν_v, N_ν_v)
        // Velocity block matrices per Cartesian direction.
        std::array<nda::matrix<dcomplex>, 3> v_block;
        for (long d = 0; d < 3; ++d) v_block[d] = nda::matrix<dcomplex>{vfull(d, v_slice, v_slice)};

        for (auto const &R : syms) {
          // Rotated velocities vR[d] = Σ_c R(d,c) v_block[c].
          std::array<nda::matrix<dcomplex>, 3> vR;
          for (long d = 0; d < 3; ++d) {
            vR[d] = nda::zeros<dcomplex>(n_ov, n_ov);
            for (long c = 0; c < 3; ++c) vR[d] += R(d, c) * v_block[c];
          }

          // Batched v·A: one GEMM per needed direction, P[d](n) = vR[d]·A(ω_n), stored as (n_ω, n_ov, n_ov).
          // Computed once per symmetry and shared across all direction pairs below.
          std::array<nda::array<dcomplex, 3>, 3> P;
          for (long d = 0; d < 3; ++d) {
            if (!need_dir[d]) continue;
            auto Pd2 = nda::matrix<dcomplex>{vR[d] * A2}; // (n_ov, n_ω·n_ov) — single GEMM over all ω
            P[d]     = nda::array<dcomplex, 3>(n_w, n_ov, n_ov);
            for (long n = 0; n < n_w; ++n) P[d](n, r_all, r_all) = Pd2(r_all, nda::range(n * n_ov, (n + 1) * n_ov));
          }

          for (long ip = 0; ip < n_dir; ++ip) {
            long a = dir_pairs[ip][0], b = dir_pairs[ip][1];
            auto const &Pa = P[a];
            auto const &Pb = P[b];
            for (long n = 0; n < n_w; ++n) {
              for (long iq = 0; iq < n_Om; ++iq) {
                if (valid(iq, n) == 0) continue;
                long j        = jmap(iq, n);
                dcomplex tot  = 0;
                dcomplex intr = 0;
                // total = Tr[Pa(ω+Ω) Pb(ω)] = Σ_ij Pa(j)_{ij} Pb(n)_{ji}; intra = Σ_i Pa(j)_{ii} Pb(n)_{ii}.
                for (long i = 0; i < n_ov; ++i) {
                  intr += Pa(j, i, i) * Pb(n, i, i);
                  for (long jj = 0; jj < n_ov; ++jj) tot += Pa(j, i, jj) * Pb(n, jj, i);
                }
                double f = w_k * sym_norm;
                Gamma(ip, iq, n) += f * tot.real();
                Gamma_intra(ip, iq, n) += f * intr.real();
              }
            }
          }
        }
      }
    }

    Gamma            = mpi::all_reduce(Gamma);
    Gamma_intra      = mpi::all_reduce(Gamma_intra);
    auto Gamma_inter = nda::array<double, 3>{Gamma - Gamma_intra};

    Gamma /= V;
    Gamma_intra /= V;
    Gamma_inter /= V;

    return {.Gamma             = std::move(Gamma),
            .Gamma_intra       = std::move(Gamma_intra),
            .Gamma_inter       = std::move(Gamma_inter),
            .omega_mesh        = std::move(omega_arr),
            .Om_mesh           = Om_mesh,
            .directions        = directions,
            .spin_polarization = spin_pol};
  }

  transport_function_t transport_function(one_body_elements_on_grid const &obe, nda::array<double, 1> const &omega,
                                          std::vector<std::string> const &directions, double broadening) {
    if (!obe.velocities) throw std::runtime_error{"transport_function: obe.velocities is empty. Load with read_velocities=true."};
    if (!obe.cell_volume) throw std::runtime_error{"transport_function: obe.cell_volume is empty. Load with read_velocities=true."};

    auto const &vel = *obe.velocities;
    double V        = *obe.cell_volume;
    auto spin_kind  = obe.C_space.spin_kind();
    long n_spin     = (spin_kind == spin_kind_e::Polarized) ? 2 : 1;

    auto dir_pairs = parse_directions(directions);
    long n_dir     = long(dir_pairs.size());
    long n_w       = omega.extent(0);
    double eta2    = broadening * broadening;
    double norm    = broadening / M_PI;

    auto syms       = symmetry_ops(vel);
    double sym_norm = 1.0 / double(syms.size());

    auto Phi = nda::zeros<double>(n_dir, n_w);

    mpi::communicator comm = {};
    long n_k               = obe.H.n_k();

    for (auto k_idx : mpi::chunk(range(n_k), comm)) {
      double w_k = obe.H.k_weights(k_idx);
      for (long sigma = 0; sigma < n_spin; ++sigma) {
        long sp = sigma_to_data_idx(spin_kind, sigma);

        // Precomputed intersection between the dispersion/A window and the velocity window (see band_velocities).
        long a_off = vel.joint_window(sp, k_idx, 0); // offset of the intersection into the H/band array
        long v_off = vel.joint_window(sp, k_idx, 1);
        long n_ov  = vel.joint_window(sp, k_idx, 2);
        if (n_ov <= 0) continue;
        auto v_slice = nda::range(v_off, v_off + n_ov);

        auto Hk    = obe.H.H(sigma, k_idx); // diagonal band energies (assumes band basis)
        auto vfull = vel.v(sigma, k_idx);
        std::array<nda::matrix<dcomplex>, 3> v_block;
        for (long d = 0; d < 3; ++d) v_block[d] = nda::matrix<dcomplex>{vfull(d, v_slice, v_slice)};

        for (auto const &R : syms) {
          std::array<nda::matrix<dcomplex>, 3> vR;
          for (long d = 0; d < 3; ++d) {
            vR[d] = nda::zeros<dcomplex>(n_ov, n_ov);
            for (long c = 0; c < 3; ++c) vR[d] += R(d, c) * v_block[c];
          }
          for (long ip = 0; ip < n_dir; ++ip) {
            long a = dir_pairs[ip][0], b = dir_pairs[ip][1];
            for (long ib = 0; ib < n_ov; ++ib) {
              double eps  = (Hk(a_off + ib, a_off + ib)).real(); // ε_n(k), measured from the same reference as omega
              double vv   = (vR[a](ib, ib) * vR[b](ib, ib)).real();
              double pref = w_k * sym_norm * vv;
              for (long iw = 0; iw < n_w; ++iw) {
                double d2 = omega(iw) - eps;
                Phi(ip, iw) += pref * norm / (d2 * d2 + eta2);
              }
            }
          }
        }
      }
    }

    Phi = mpi::all_reduce(Phi);
    Phi /= V;

    return {.Phi = std::move(Phi), .omega_mesh = omega, .directions = directions};
  }

  // ------------------------------------------------------------------ h5 + printing

  void h5_read(h5::group g, std::string const &name, transport_distribution_t &x) {
    auto sg = g.open_group(name);
    assert_hdf5_format(g, x);
    h5_read(sg, "Gamma", x.Gamma);
    h5_read(sg, "Gamma_intra", x.Gamma_intra);
    h5_read(sg, "Gamma_inter", x.Gamma_inter);
    h5_read(sg, "omega_mesh", x.omega_mesh);
    h5_read(sg, "Om_mesh", x.Om_mesh);
    h5_read(sg, "directions", x.directions);
    h5_read(sg, "spin_polarization", x.spin_polarization);
  }
  void h5_write(h5::group g, std::string const &name, transport_distribution_t const &x) {
    auto sg = g.create_group(name);
    write_hdf5_format(g, x);
    h5_write(sg, "Gamma", x.Gamma);
    h5_write(sg, "Gamma_intra", x.Gamma_intra);
    h5_write(sg, "Gamma_inter", x.Gamma_inter);
    h5_write(sg, "omega_mesh", x.omega_mesh);
    h5_write(sg, "Om_mesh", x.Om_mesh);
    h5_write(sg, "directions", x.directions);
    h5_write(sg, "spin_polarization", x.spin_polarization);
  }

  void h5_read(h5::group g, std::string const &name, transport_function_t &x) {
    auto sg = g.open_group(name);
    assert_hdf5_format(g, x);
    h5_read(sg, "Phi", x.Phi);
    h5_read(sg, "omega_mesh", x.omega_mesh);
    h5_read(sg, "directions", x.directions);
  }
  void h5_write(h5::group g, std::string const &name, transport_function_t const &x) {
    auto sg = g.create_group(name);
    write_hdf5_format(g, x);
    h5_write(sg, "Phi", x.Phi);
    h5_write(sg, "omega_mesh", x.omega_mesh);
    h5_write(sg, "directions", x.directions);
  }

  std::ostream &operator<<(std::ostream &out, transport_distribution_t const &x) {
    out << "Transport distribution Γ_αβ(ω,Ω):\n";
    out << fmt::format("  directions        = {}\n", fmt::join(x.directions, ", "));
    out << fmt::format("  Gamma shape       = {}\n", x.Gamma.shape());
    out << fmt::format("  n_omega, n_Om     = {}, {}\n", x.omega_mesh.extent(0), x.Om_mesh.extent(0));
    out << fmt::format("  spin_polarization = {}\n", x.spin_polarization);
    return out;
  }

  std::ostream &operator<<(std::ostream &out, transport_function_t const &x) {
    out << "Transport function Φ_αβ(ω):\n";
    out << fmt::format("  directions = {}\n", fmt::join(x.directions, ", "));
    out << fmt::format("  Phi shape  = {}\n", x.Phi.shape());
    return out;
  }

} // namespace triqs::modest
