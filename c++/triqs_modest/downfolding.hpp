// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <triqs/gfs.hpp>
#include <triqs/utility/macros.hpp>
#include <type_traits>
#include "./local_space.hpp"
#include "./ibz_symmetry_ops.hpp"
#include "utils/nda_supp.hpp"

namespace triqs::modest {

  /**
   * @brief Map a spin index to a data index.
   *
   * @details The mapping depends on the spin kind:
   *
   * * `Polarized`: \f$ \sigma \to \sigma \f$ (an object can have different values for different \f$ \sigma \f$).
   * * `NonPolarized`: \f$ \sigma \to 0 \f$ (an object is the same for both \f$ \sigma \f$, so only one copy is stored).
   * * `NonCollinear`: \f$ \sigma \to 0 \f$ (\f$ \sigma \f$ is always 0).
   */
  [[nodiscard]] inline long sigma_to_data_idx(spin_kind_e spin_kind, long sigma) { return (spin_kind == spin_kind_e::Polarized) ? sigma : 0; };

  /**
   * @ingroup one_body_elements
   * @brief The one-body dispersion as a function of momentum.
   *
   * @details The band dispersion typically corresponds to the solution of a (Kohn-Sham) Hamiltonian which has been
   * diagonalized in momentum space and formulated in a basis of Bloch states \f$ | \phi_{\nu\mathbf{k}} \rangle \f$ with
   * corresponding eigenvalues (\f$\varepsilon_{\nu\mathbf{k}}^{\sigma}\f$).
   *
   * A band dispersion object contains the DFT band structure \f$\varepsilon_{\nu\mathbf{k}}^{\sigma}\f$, weights in the
   * Brillouin zone, and the spin kind used in the DFT calculation.
   */
  struct band_dispersion {
    spin_kind_e spin_kind;             ///< Spin kind of the one-body data.
    nda::array<dcomplex, 4> H_k;       ///< Hamiltonian \f$ H^{\sigma}_{\nu\nu'}(\mathbf{k}) \f$.
    nda::array<long, 2> n_bands_per_k; ///< Number of bands for each k-point and \f$ \sigma \f$.
    nda::array<double, 1> k_weights;   ///< Weight in the BZ for each k-point.
    bool matrix_valued;                ///< Is the dispersion matrix-valued?

    /// Equality comparison operator.
    bool operator==(band_dispersion const &) const = default;

    /// MPI broadcast
    C2PY_IGNORE friend void mpi_broadcast(band_dispersion &x, mpi::communicator c = {}, int root = 0) {
      mpi::broadcast(x.spin_kind, c, root);
      mpi::broadcast(x.H_k, c, root);
      mpi::broadcast(x.n_bands_per_k, c, root);
      mpi::broadcast(x.k_weights, c, root);
      mpi::broadcast(x.matrix_valued, c, root);
    }

    // public:
    // /// Constructor
    // band_dispersion(spin_kind_e spin_kind, nda::array<dcomplex, 4> H_k, nda::array<long, 2> n_bands_per_k,
    //                 nda::array<double, 1> k_weights)
    //    : spin_kind(spin_kind),
    //      H_k(std::move(H_k)),
    //      n_bands_per_k(std::move(n_bands_per_k)),
    //      _k_weights(std::move(k_weights)) {
    //   // TODO add some checks on the data ?
    // }

    /**
     * @brief Get \f$ H^{\sigma}_{\nu\nu'}(\mathbf{k}) \f$ for a given \f$ \mathbf{k} \f$ and \f$ \sigma \f$.
     *
     * @param sigma Spin index \f$ \sigma \f$.
     * @param k_idx Index of the k-point in the grid.
     * @return Matrix view of \f$ H^{\sigma}_{\nu\nu'}(\mathbf{k}) \f$ in \f$ (\nu, \nu') \f$ for a given
     * \f$ \mathbf{k} \f$ and \f$ \sigma \f$.
     */
    [[nodiscard]] nda::matrix_const_view<dcomplex> H(long sigma, long k_idx) const {
      auto sigma_p = sigma_to_data_idx(spin_kind, sigma);
      auto R_nu    = nda::range(n_bands_per_k(k_idx, sigma_p));
      return H_k(k_idx, sigma_p, R_nu, R_nu);
    }

    /// Number of bands for a given k-point and spin \f$ \sigma \f$.
    [[nodiscard]] long N_nu(long sigma, long k_idx) const { return n_bands_per_k(k_idx, sigma_to_data_idx(spin_kind, sigma)); }

    /// Number of k-points in the grid.
    [[nodiscard]] long n_k() const { return H_k.extent(0); }

    // /// Weight of k point (typically from DFT code).
    // [[nodiscard]] double k_weights(long k_idx) const { return _k_weights(k_idx); }

    /// Print information about a band_dispersion object.
    friend std::ostream &operator<<(std::ostream &out, band_dispersion const &bd);
  };

  /**
   * @ingroup one_body_elements
   * @brief The projector that downfolds the energy bands onto a set of localized atomic-like orbitals.
   *
   * A downfolding projector contains the projector, the kind of spin used in the projection, and the number of bands
   * per k-point for cases when a band goes outside of the projection window.
   *
   * The projectors \f$P_{m\nu}^{\sigma}(\mathbf{k})\f$ connect the Bloch space \f${\cal B}\f$ to \f${\cal C}\f$. The
   * projectors are obtained from DFT codes or Wannier90. They are defined by
   * \f[
   *   P_{(a,m_{a})\nu}^{\sigma}(\mathbf{k})\equiv e^{-i \mathbf{k} R_a}
   *   \langle \chi_{m_{a}}^{R_a \sigma} | \psi_{\nu}^{\sigma}(\mathbf{k}) \rangle,
   * \f]
   * where \f$ | \chi_{m_{a}}^{R_a \sigma} \rangle \f$ is a Wannier function localized at atom \f$ a \f$ with index
   * \f$m_a\f$ at position \f$R_a\f$ and \f$ | \psi_{\nu}^{\sigma}(\mathbf{k}) \rangle \f$ is the Kohn-Sham wavefunction.
   * The relation between the Wannier and  Bloch function is therefore
   * \f[
   *   | \chi_{m_{a}}^{R_a \sigma} \rangle = \sum_{\mathbf{k} \nu} e^{-i \mathbf{k} R_a} \bigl(P^\sigma_{(a,m_{a})\nu}
   *   (\mathbf{k})\bigr)^* | \psi_{\nu}^{\sigma}(\mathbf{k}) \rangle.
   * \f]
   *
   * Some properties:
   *
   * * Basis change in \f$\cal C\f$ space: They are given by a unitary matrix \f$U\f$, the projector transforms as
   *   \f$ P^{'\sigma}_{m\nu}(\mathbf{k}) = U^{\dagger}_{m, m'} P^{\sigma}_{m'\nu}(\mathbf{k}).\f$
   * * Partial unitarity property: In general \f$P\f$ is not unitary as \f$N_\nu^{\mathbf{k}} > M\f$. However, if the
   *   Wannier functions are reorthonormalized with respect to the truncated band basis, we have
   *   \f$ \sum_{ \nu} P^{\sigma}_{m\nu}(\mathbf{k}) P^{\dagger\sigma}_{\nu m'}(\mathbf{k}) = \delta_{mm'} \f$.
   */
  struct downfolding_projector {
    spin_kind_e spin_kind;             ///< Spin kind of the one-body data.
    nda::array<dcomplex, 4> P_k;       ///< Projector \f$ P_{m\nu}^{\sigma}(\mathbf{k}) \f$.
    nda::array<long, 2> n_bands_per_k; ///< Number of bands for each k-point and \f$ \sigma \f$.

    /// Equality comparison operator.
    bool operator==(downfolding_projector const &) const = default;

    /// MPI broadcast
    C2PY_IGNORE friend void mpi_broadcast(downfolding_projector &x, mpi::communicator c = {}, int root = 0) {
      mpi::broadcast(x.spin_kind, c, root);
      mpi::broadcast(x.P_k, c, root);
      mpi::broadcast(x.n_bands_per_k, c, root);
    }

    // public:
    // /// Constructor
    // downfolding_projector(spin_kind_e spin_kind, nda::array<dcomplex, 4> P_k, nda::array<long, 2> n_bands_per_k)
    //    : spin_kind(spin_kind), P_k(std::move(P_k)), n_bands_per_k(std::move(n_bands_per_k)) {}

    // ----------------- accessors --------------------
    // FIXME : SWAP k and sigma in data

    /**
     * @brief Get \f$ P_{m\nu}^{\sigma}(\mathbf{k}) \f$ for a given \f$ \mathbf{k} \f$ and \f$ \sigma \f$.
     *
     * @param sigma Spin index \f$ \sigma \f$.
     * @param k_idx Index of the k-point in the grid.
     * @return Matrix view of \f$ P_{m\nu}^{\sigma}(\mathbf{k}) \f$ in \f$ (m, \nu) \f$ for the given \f$ \mathbf{k} \f$
     * and \f$ \sigma \f$.
     */
    [[nodiscard]] nda::matrix_const_view<dcomplex> P(long sigma, long k_idx) const {
      auto sigma_p = sigma_to_data_idx(spin_kind, sigma);
      auto R_nu    = nda::range(n_bands_per_k(k_idx, sigma_p));
      return P_k(k_idx, sigma_p, r_all, R_nu);
    }

    /// Rotates the local basis of the downfolding projector.
    downfolding_projector rotate_local_basis(nda::array<nda::matrix<dcomplex>, 2> const &U) const;

    /// Print information about a downfolding_projector object.
    friend std::ostream &operator<<(std::ostream &out, downfolding_projector const &proj);
  };

  struct downfolding_projector_ext : downfolding_projector {
    nda::array<long, 3> band_window;
    nda::array<double, 2> kpts;

    bool operator==(downfolding_projector_ext const &other) const = default;

    friend void mpi_broadcast(downfolding_projector_ext &x, mpi::communicator c = {}, int root = 0) {
      mpi::broadcast(static_cast<downfolding_projector &>(x), c, root);
      mpi::broadcast(x.band_window, c, root);
      mpi::broadcast(x.kpts, c, root);
    }
  };

  // ------------------------------------------------------------
  /**
   * @ingroup one_body_elements
   * @brief A one-body elements struct where all of the underlying data exists on a fixed momentum grid.
   */
  struct one_body_elements_on_grid {
    band_dispersion H;                                 ///< Band dispersion.
    local_space C_space;                               ///< Local \f$ \mathcal{C} \f$ space.
    downfolding_projector P;                           ///< Downfolding projector \f$ P \f$.
    std::optional<ibz_symmetry_ops> ibz_symm_ops = {}; ///< IBZ symmetrizer after a k-sum

    /// Equality comparison operator.
    bool operator==(one_body_elements_on_grid const &) const = default;

    /// MPI broadcast
    C2PY_IGNORE friend void mpi_broadcast(one_body_elements_on_grid &x, mpi::communicator c = {}, int root = 0) {
      mpi::broadcast(x.H, c, root);
      mpi::broadcast(x.C_space, c, root);
      mpi::broadcast(x.P, c, root);
      mpi::broadcast(x.ibz_symm_ops, c, root);
    }
  };

  // ------------------------------------------------------------

  /**
   * @ingroup one_body_elements
   * @brief A one-body elements struct for GW (CoQui) calculations.
   */
  struct one_body_elements_gw {
    local_space C_space;
    downfolding_projector_ext P;

    /// Equality comparison operator.
    bool operator==(one_body_elements_gw const &) const = default;

    /// MPI broadcast
    friend void mpi_broadcast(one_body_elements_gw &x, mpi::communicator c = {}, int root = 0) {
      mpi::broadcast(x.C_space, c, root);
      mpi::broadcast(x.P, c, root);
    }
  };

  //-------------------------------------------------------------
  std::ostream &operator<<(std::ostream &out, one_body_elements_on_grid const &);
  void h5_read(h5::group g, std::string const &name, one_body_elements_on_grid &x);
  void h5_write(h5::group g, std::string const &name, one_body_elements_on_grid const &x);

  //-------------------------------------------------------------
  one_body_elements_on_grid permute_local_space(std::vector<std::vector<long>> const &atom_partition, one_body_elements_on_grid const &x);

  // -------------------------------------------------------------
  /// Rotates the local basis of the downfolding projector
  one_body_elements_on_grid rotate_local_basis(nda::array<nda::matrix<dcomplex>, 2> const &U, one_body_elements_on_grid const &x);
  // -------------------------------------------------------------

  /// Compute the local impurity levels from the single-particle dispersion.

  /**
   * @ingroup hybridization
   * @brief Compute the atomic (impurity) levels from an obe.
   *
   * @param obe One-body elements.
   * @return Impurity levels stored in the format [n_atoms, n_sigma].
   */
  nda::array<nda::matrix<dcomplex>, 2> impurity_levels(one_body_elements_on_grid const &obe);
  // -------------------------------------------------------------

  // These functions are not part of the public API: not documented, not wrapped.
  namespace detail {
    /// The projected bare function on the C space, cf notes.
    nda::array<dcomplex, 3> G0_C_k_sigma(one_body_elements_on_grid const &obe, double mu, long k_idx, long sigma, std::vector<dcomplex> const &omegas,
                                         bool mu_derivative = false);
  } // namespace detail

} // namespace triqs::modest
