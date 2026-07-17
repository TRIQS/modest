// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once

#include <nda/nda.hpp>
#include <triqs/gfs.hpp>
#include <string>
#include <vector>
#include "./downfolding.hpp"

namespace triqs::modest {

  /**
   * @ingroup bubble
   * @brief Kubo transport distribution \f$ \Gamma_{\alpha\beta}(\omega,\Omega) \f$.
   *
   * @details
   * \f[
   *   \Gamma_{\alpha\beta}(\omega,\Omega) = \frac{1}{V} \sum_{\mathbf{k}}
   *     \mathrm{Tr}\left[ v_{\mathbf{k},\alpha} A_{\mathbf{k}}(\omega+\Omega)
   *                        v_{\mathbf{k},\beta}  A_{\mathbf{k}}(\omega) \right],
   * \f]
   * with the band-basis spectral function \f$ A = -(G-G^\dagger)/(2\pi i) \f$. The result is decomposed into
   * total / intraband / interband contributions (band-diagonal vs off-diagonal in the dispersion basis).
   */
  struct transport_distribution_t {
    nda::array<double, 3> Gamma;         ///< Total \f$ \Gamma_{\alpha\beta}(\omega,\Omega) \f$: (n_dir, n_Om, n_omega).
    nda::array<double, 3> Gamma_intra;   ///< Intraband (band-diagonal) contribution: (n_dir, n_Om, n_omega).
    nda::array<double, 3> Gamma_inter;   ///< Interband contribution = total − intra: (n_dir, n_Om, n_omega).
    nda::array<double, 1> omega_mesh;    ///< Internal frequency grid \f$ \omega \f$ (from Sigma_w's mesh): (n_omega,).
    nda::array<double, 1> Om_mesh;       ///< External frequency grid \f$ \Omega \f$: (n_Om,).
    std::vector<std::string> directions; ///< Direction labels (e.g. "xx", "xy"), aligned with the leading axis.
    long spin_polarization;              ///< 0 if a spin-degeneracy factor of 2 should be applied downstream, else 1.

    static std::string hdf5_format() { return "TransportDistribution"; }
    friend std::ostream &operator<<(std::ostream &out, transport_distribution_t const &x);
  };

  /**
   * @ingroup bubble
   * @brief Transport function \f$ \Phi_{\alpha\beta}(\omega) = \sum_{\mathbf{k}} \sum_n
   *        v^{nn}_{\alpha} v^{nn}_{\beta}\, \delta(\omega - \varepsilon_n(\mathbf{k})) / V \f$.
   *
   * @details The intraband (Drude) building block, evaluated with a Lorentzian-broadened delta and no
   * self-energy (bare DFT bands). Assumes a diagonal (band-basis) dispersion.
   */
  struct transport_function_t {
    nda::array<double, 2> Phi;           ///< \f$ \Phi_{\alpha\beta}(\omega) \f$: (n_dir, n_omega).
    nda::array<double, 1> omega_mesh;    ///< Frequency grid \f$ \omega \f$: (n_omega,).
    std::vector<std::string> directions; ///< Direction labels, aligned with the leading axis.

    static std::string hdf5_format() { return "TransportFunction"; }
    friend std::ostream &operator<<(std::ostream &out, transport_function_t const &x);
  };

  /// h5 read/write
  void h5_read(h5::group g, std::string const &name, transport_distribution_t &x);
  void h5_write(h5::group g, std::string const &name, transport_distribution_t const &x);
  void h5_read(h5::group g, std::string const &name, transport_function_t &x);
  void h5_write(h5::group g, std::string const &name, transport_function_t const &x);

  /**
   * @ingroup bubble
   * @brief Compute the Kubo transport distribution \f$ \Gamma_{\alpha\beta}(\omega,\Omega) \f$.
   *
   * The band-basis spectral function \f$ A_{\mathbf{k}}(\omega) \f$ is built with the rank-reduced Woodbury
   * identity (diagonal H(k) + rank-M self-energy); a matrix-valued H(k) is not supported. Requires
   * `obe.velocities` and `obe.cell_volume` (see
   * `one_body_elements_from_dft_converter(..., read_velocities=true)`). MPI-parallel over k-points.
   *
   * @param obe One-body elements on grid, carrying the optional band velocities and cell volume.
   * @param mu Chemical potential.
   * @param Sigma_w Self-energy in real frequencies; its mesh defines the internal \f$ \omega \f$ grid.
   * @param Om_mesh External frequency mesh \f$ \Omega \f$ (must include 0.0 for DC transport).
   * @param directions Direction labels, e.g. {"xx","yy","zz","xy"}.
   * @param broadening Additional Lorentzian broadening η added to the frequency argument.
   * @return The transport distribution (total/intra/inter) with its frequency meshes.
   */
  transport_distribution_t transport_distribution(one_body_elements_on_grid const &obe, double mu,
                                                  block2_gf<mesh::refreq, matrix_valued> const &Sigma_w, nda::array<double, 1> const &Om_mesh,
                                                  std::vector<std::string> const &directions, double broadening = 0.01);

  /**
   * @ingroup bubble
   * @brief Compute the transport function \f$ \Phi_{\alpha\beta}(\omega) \f$ (bare DFT bands, no self-energy).
   *
   * @param obe One-body elements on grid, carrying the optional band velocities and cell volume.
   * @param omega Frequency grid on which \f$ \Phi \f$ is evaluated.
   * @param directions Direction labels, e.g. {"xx","yy","zz"}.
   * @param broadening Lorentzian broadening of the delta function.
   * @return The transport function with its frequency mesh.
   */
  transport_function_t transport_function(one_body_elements_on_grid const &obe, nda::array<double, 1> const &omega,
                                          std::vector<std::string> const &directions, double broadening = 0.01);

} // namespace triqs::modest
