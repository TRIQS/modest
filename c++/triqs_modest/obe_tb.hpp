// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <mpi/mpi.hpp>
#include <nda/nda.hpp>

#include <triqs/experimental.hpp>
#include <triqs/mesh/imfreq.hpp>
#include <triqs/gfs.hpp>
#include <triqs/utility/root_finder.hpp>
#include <triqs/utility/macros.hpp>

#include "loaders.hpp"
#include "triqs_modest/utils/gf_supp.hpp"

namespace triqs::modest {

  using namespace triqs::experimental;

  /**
   * @ingroup one_body_elements
   * @brief A one-body elements using a tight-binding Hamiltonian.
   */
  struct one_body_elements_tb {
    local_space C_space;           ///< Local \f$ \mathcal{C} \f$ space.
    std::vector<tb_hk> H; ///< List of TB Hamiltonians.
    //downfolding_projector P;
    //C2PY_IGNORE std::optional<ibz_symmetry_ops> ibz_symm_ops = {}; //< IBZ symmetrizer after a k-sum

    /// Equality comparison operator.
    bool operator==(one_body_elements_tb const &) const = default;

    /// MPI broadcast
    C2PY_IGNORE friend void mpi_broadcast(one_body_elements_tb &x, mpi::communicator c = {}, int root = 0) {
      mpi::broadcast(x.C_space, c, root);
      mpi::broadcast(x.H, c, root);
    }
  };

  one_body_elements_tb one_body_elements_from_model(std::vector<std::array<long, 3>> const &Rs, std::vector<nda::array<dcomplex, 2>> const &HR,
                                                    spin_kind_e spin_kind, std::vector<atomic_orbs> atomic_shells);

  one_body_elements_tb one_body_elements_from_model(std::vector<std::array<long, 3>> const &Rs, std::vector<nda::array<dcomplex, 2>> const &HR_up,
                                                    std::vector<nda::array<dcomplex, 2>> const &HR_dn, spin_kind_e spin_kind,
                                                    std::vector<atomic_orbs> atomic_shells);

  /** @name OBE factories using a TB Hamiltonian
 *  Factory functions to create one_body_elements_on_tb
 *  @{
 */

  /**
   * @ingroup one_body_elements
   * @brief Construct a one-body elements TB object from Wannier90 in the case of a single spin index.
   *
   * @param wannier_file_path String to Wannier90 files, including the prefix, as in "path/to/file/seedname" to specify
   * a Wannier files named in the format "seedname_tb.dat".
   * @param spin_kind Spin kind for this calculation.
   * @param atomic_shells List of atomic shells input by the user.
   * @return One-body elements containing the Wannier90 tight binding Hamiltonian.
   */
  one_body_elements_tb one_body_elements_from_wannier90(std::string const &wannier_file_path, spin_kind_e spin_kind,
                                                        std::vector<atomic_orbs> atomic_shells);

  /**
   * @ingroup one_body_elements
   * @brief Construct a one-body elements TB object from Wannier90 in the case with separate spin up/spin down channels.
   *
   * @param wannier_file_path_up String to Wannier90 files, including the prefix, for the up spin channel, as in
   * "path/to/file/seedname" to specify a Wannier files named in the format "seedname_tb.dat".
   * @param wannier_file_path_dn String to Wannier90 files, including the prefix, for the down spin channel as in
   * "path/to/file/seedname" to specify a Wannier files named in the format "seedname_tb.dat"
   * @param spin_kind Spin kind for this calculation.
   * @param atomic_shells List of atomic shells input by the user.
   * @return One-body elements containing the Wannier90 tight binding Hamiltonian.
   */
  one_body_elements_tb one_body_elements_from_wannier90(std::string const &wannier_file_path_up, std::string const &wannier_file_path_dn,
                                                        spin_kind_e spin_kind, std::vector<atomic_orbs> atomic_shells);

  /** @} */ // tb  factories

  /// Helper to contruct and return an OBE_tb object given a list of tb_hks of length n_sigma
  C2PY_IGNORE one_body_elements_tb make_obe_from_tb(std::vector<tb_hk> const tb_H_sigma, spin_kind_e spin_kind,
                                                    std::vector<atomic_orbs> atomic_shells);

  /**
   * @brief Compute \f$ H_{\text{loc}} = H(R=0) \f$ given \f$ n_\sigma \f$ tight binding Hamiltonians.
   *
   * @param H_sigma A list of TB Hamiltonians of length \f$ n_\sigma \f$.
   * @param atomic_shells A list of atomic shells corresponding to the orbitals contained in the TB Hamiltonians.
   * @return \f$ H_{\text{loc}} \f$, formatted with dimensions \f$ [\alpha,\sigma] \f$ each containing (n_orbitals_atom,
   * n_orbitals_atom).
   */
  nda::array<nda::matrix<dcomplex>, 2> Hloc(std::vector<tb_hk> const &H_sigma, std::vector<atomic_orbs> const &atomic_shells);

  /**
   * @ingroup hybridization
   * @brief Compute the atomic (impurity) levels from an obe.
   *
   * @param obe One-body elements.
   * @return Impurity levels stored in the format [n_atoms, n_sigma].
   */
  nda::array<nda::matrix<dcomplex>, 2> impurity_levels(one_body_elements_tb const &obe);

  /**
   * @brief Convert a tight binding Hamiltonian to its superlattice equivalent.
   *
   * @param sl The superlattice object containing its lattice vectors and locations of cluster points.
   * @param obe A one-body elements containing the TB Hamiltonian.
   * @return One-body elements based on the superlattice tight binding Hamiltonian.
   */
  one_body_elements_tb fold(superlattice const &sl, one_body_elements_tb const &obe);

  /**
 * @brief Rotate a tight-binding Hamiltonian by a unitary matrix \f$ U \f$.
 *
 * @details The rotation is performed as \f$ H' = U H U^\dagger \f$.
 *
 * @param obe One-body elements containing the TB Hamiltonian.
 * @param U Unitary matrix used for the rotation.
 * @return One-body elements containing the rotated TB Hamiltonian.
 */
  one_body_elements_tb rotate(one_body_elements_tb const &obe, nda::matrix<dcomplex> const &U);

  one_body_elements_tb extend_to_spin(one_body_elements_tb const &obe);
  one_body_elements_tb add_local_term(one_body_elements_tb const &obe, nda::matrix<dcomplex> const &local_term);

  //  -----------------------------------------------------------------------

  /** @name Local Green's function using a TB Hamiltonian
  */
  ///@{

  /**
   * @ingroup gloc
   * @brief Compute the local Green's function without a self-energy.
   *
   * @details See other overloads (gloc) for more details.
   *
   * @tparam Mesh The mesh type.
   * @param obe One-body elements containing the TB Hamiltonian.
   * @param mu Chemical potential \f$ \mu \f$.
   * @param Sigma_dynamic The dynamic part of the embedded self-energy in the embedded view,
   * \f$ \Sigma_{\text{dynamic}}[\alpha, \sigma] \f$.
   * @param Sigma_static The static part of the embedded self-energy in the embedded view,
   * \f$ \Sigma_{\text{static}}[\alpha,\sigma] \f$.
   * @param opt Container for options related to integration of the BZ.
   * @return \f$ G_{\mathrm{loc}}^{\sigma} \f$, the local Green's function.
   */
  template <typename Mesh>
  block2_gf<Mesh, matrix_valued> gloc(one_body_elements_tb const &obe, double mu, block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                                      nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static, bz_int_options const &opt) {

    auto &mesh       = Sigma_dynamic(0, 0).mesh();
    auto n_sigma     = obe.C_space.n_sigma();
    auto gloc_result = make_block2_gf(mesh, obe.C_space.Gc_block_shape());
    // TODO also check safety of orbital space sizes...
    if (n_sigma != Sigma_static.shape(1)) { throw std::runtime_error("Mismatch between the spin channels in Sigma_Static and Sigma_Dynamic"); }
    if (n_sigma != obe.H.size()) { throw std::runtime_error("Mismatch between the spin channels in Sigma and spin channels in Hamiltonian."); }

    // Embedding decomposition from structure of Sigma -- provides a list of block names
    auto embedding_decomp = get_struct(Sigma_dynamic).dims(r_all, 0) | tl::to<std::vector>();

    for (auto sigma : range(n_sigma)) {

      // spin index
      auto Sigma_full_space = gfs::gf(mesh, {obe.H[sigma].n_orbitals(), obe.H[sigma].n_orbitals()}); //
      for (auto &&[block, R] : enumerated_sub_slices(embedding_decomp)) {
        for (auto [n, w] : enumerate(mesh)) {
          Sigma_full_space.data()(n, R, R) = Sigma_dynamic(block, sigma).data()(n, r_all, r_all) + Sigma_static(block, sigma);
        }
      }
      // Call the TRIQS version of this function
      gloc_result(0, sigma) = gloc(obe.H[sigma], mu, Sigma_full_space, opt);
    }
    return gloc_result;
  }

  /**
    * @ingroup gloc
    * @brief Compute the local Green's function without a self-energy.
    *
    * @details See other overloads (gloc) for more details.
    *
    * @tparam Mesh The mesh type.
    * @param mesh The mesh on which \f$ G_{\mathrm{loc}} \f$ will be computed.
    * @param obe One-body elements containing the TB Hamiltonian.
    * @param mu Chemical potential \f$ \mu \f$.
    * @param opt Container for options related to integration of the BZ.
    * @return \f$ G_{\mathrm{loc}}^{\sigma} \f$, the local Green's function.
    */
  template <typename Mesh>
  block2_gf<Mesh, matrix_valued> gloc(Mesh const &mesh, one_body_elements_tb const &obe, double mu, bz_int_options const &opt) {
    auto result = make_block2_gf(mesh, obe.C_space.Gc_block_shape());
    for (auto sigma : range(obe.C_space.n_sigma())) { result(0, sigma) = gloc(mesh, obe.H[sigma], mu, opt); }
    return result;
  }

  ///@}

  //  -----------------------------------------------------------------------

  /**
   * @ingroup mu
   * @brief Compute the density of the lattice Green's function with a self-energy.
   *
   * @tparam Mesh The mesh type.
   * @param obe One-body elements.
   * @param mu Chemical potential.
   * @param Sigma_dynamic Dynamic part of the embedded self-energy.
   * @param Sigma_static Static part of the embedded self-energy.
   * @param opt Container for options related to the integration of the BZ.
   * @return Electron density of the lattice Green's function.
   */
  template <typename Mesh>
  double density(one_body_elements_tb const &obe, double mu, block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static, bz_int_options const &opt) {

    //auto n_blocks = Sigma_dynamic.size1();
    auto n_sigma = obe.C_space.n_sigma();

    double n  = 0;
    auto Gloc = gloc(obe, mu, Sigma_dynamic, Sigma_static, opt); // returns block2gf (1, nsigma, {norb, norb})
    // return type of Gloc is a B2GF with dimensions (1, nspin, {norb, norb})
    for (auto sigma : range(n_sigma)) { // spin index
      n += real(nda::trace(density(Gloc(0, sigma))));
    }
    return n;
  }

  //  -----------------------------------------------------------------------
  /**
   * @ingroup mu
   * @brief Find the chemical potenital from the local Green's function and self-energy given a target density.
   *
   * @tparam Mesh The mesh type.
   * @param target_density Total electron density.
   * @param obe One-body elements.
   * @param Sigma_dynamic Dynamic part of the embedded self-energy.
   * @param Sigma_static Static part of the embedded self-energy.
   * @param opt Container for options related to integration of the BZ.
   * @param method Root finding method to use (default = `dichotomy`).
   * @param precision Precision to end search (default = 1e-5).
   * @param verbosity Printing of the root finder's progress (default = true).
   * @return Chemical potential corresponding to target density.
   */
  template <typename Mesh>
  double find_chemical_potential(double const target_density, one_body_elements_tb const &obe, block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static, bz_int_options const &opt,
                                 std::string method = "dichotomy", double precision = 1.e-5, bool verbosity = true) {
    std::function<double(double)> f = [&obe, &Sigma_dynamic, &Sigma_static, &opt](double x) {
      return density(obe, x, Sigma_dynamic, Sigma_static, opt);
    };
    return std::get<0>(
       triqs::utility::root_finder(method, f, 0.0, target_density, precision, 0.5, 1000, "Chemical Potential", "Total Density", verbosity));
  }

  /**
   * @ingroup mu
   * @brief Find the chemical potenital from the local Green's function and self-energy given a target density.
   *
   * @tparam Mesh The mesh type.
   * @param target_density Total electron density.
   * @param obe One-body elements.
   * @param mesh Mesh on which local GF will be computed.
   * @param opt Container for options related to integration of the BZ.
   * @param method Root finding method to use (default = `dichotomy`).
   * @param precision Precision to end search (default = 1e-5).
   * @param verbosity Printing of the root finder's progress (default = true).
   * @return Chemical potential corresponding to target density.
   */
  template <typename Mesh>
  double find_chemical_potential(double const target_density, one_body_elements_tb const &obe, Mesh const &mesh,
                                 bz_int_options const &opt, std::string method = "dichotomy", double precision = 1.e-5,
                                 bool verbosity = true) {

    auto Sigma_dynamic = make_block2_gf(mesh, obe.C_space.Gc_block_shape());
    auto Sigma_static  = nda::array<nda::matrix<dcomplex>, 2>(1, obe.C_space.n_sigma());
    for (auto [i, j] : Sigma_static.indices()) { Sigma_static(i, j) = nda::zeros<dcomplex>(obe.C_space.dim(), obe.C_space.dim()); }
    return find_chemical_potential(target_density, obe, Sigma_dynamic, Sigma_static, opt, method, precision, verbosity);
  }

  // --------  instantiations --------------

  /** @cond DOXYGEN_SKIP_THIS */
  template block2_gf<mesh::imfreq, matrix_valued> gloc(one_body_elements_tb const &obe, double mu,
                                                       block2_gf<mesh::imfreq, matrix_valued> const &Sigma_dynamic,
                                                       nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static,
                                                       bz_int_options const &opt);

  template block2_gf<mesh::imfreq, matrix_valued> gloc(mesh::imfreq const &mesh, one_body_elements_tb const &obe, double mu,
                                                       bz_int_options const &opt);

  template double density(one_body_elements_tb const &obe, double mu, block2_gf<mesh::imfreq, matrix_valued> const &Sigma_dynamic,
                          nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static, bz_int_options const &opt);

  template double find_chemical_potential(double const target_density, one_body_elements_tb const &obe,
                                          block2_gf<mesh::imfreq, matrix_valued> const &Sigma_dynamic,
                                          nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static, bz_int_options const &opt,
                                          std::string method, double precision, bool verbosity);

  template double find_chemical_potential(double const target_density, one_body_elements_tb const &obe, mesh::imfreq const &mesh,
                                          bz_int_options const &opt, std::string method, double precision, bool verbosity);

  /** @endcond */

} // namespace triqs::modest
