// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <triqs/lattice/tb_hamiltonian.hpp>
#include <stdexcept>
#include <triqs/mesh/imfreq.hpp>
#include "loaders.hpp"
#include "triqs/lattice/bz_integrators.hpp"
#include "triqs/lattice/gloc.hpp"
#include "triqs_modest/utils/gf_supp.hpp"
#include <nda/nda.hpp>
#include <triqs/gfs/functions/density.hpp>
#include "./root_finder.hpp"
#include "triqs/lattice/superlattice.hpp"

namespace triqs::modest {

  using triqs::lattice::superlattice;

  struct one_body_elements_tb {
    local_space C_space;
    std::vector<tb_hamiltonian> H; // positibility to have two spin channels for spin pol
    //downfolding_projector P;
    //C2PY_IGNORE std::optional<ibz_symmetry_ops> ibz_symm_ops = {}; //< IBZ symmetrizer after a k-sum
  };

  /** 
   * @brief Construct a obe_tb from Wannier90 in the case of a single spin index. 
   * 
   * @param wannier_file_path string to Wannier90 files, including the prefix, 
   *       as in "path/to/file/seedname" to specify a Wannier files named in the format "seedname_tb.dat"
   * @param spin_kind spin type for this calculation 
   * @param atomic_shells list of atomic shells input by the user 
   * 
   * @return One_body_elements object containing the Wannier90 tight binding Hamiltonian 
   */
  one_body_elements_tb one_body_elements_from_wannier90(std::string const &wannier_file_path, spin_kind_e spin_kind,
                                                        std::vector<atomic_shell_t> atomic_shells);

  /** 
   * @brief Construct a obe_tb from Wannier90 in the case with separate spin up/spin down channels.
   * 
   * @param wannier_file_path_up string to Wannier90 files, including the prefix, for the up spin channel, 
   *     as in "path/to/file/seedname" to specify a Wannier files named in the format "seedname_tb.dat"
   * @param wannier_file_path_dn string to Wannier90 files, including the prefix, for the down spin channel  
   *     as in "path/to/file/seedname" to specify a Wannier files named in the format "seedname_tb.dat"
   * @param spin_kind spin type for this calculation 
   * @param atomic_shells list of atomic shells input by the user 
   *
   * @return One_body_elements object containing the Wannier90 tight binding Hamiltonian 
   */
  one_body_elements_tb one_body_elements_from_wannier90(std::string const &wannier_file_path_up, std::string const &wannier_file_path_dn,
                                                        spin_kind_e spin_kind, std::vector<atomic_shell_t> atomic_shells);

  /// Helper to contruct and return an OBE_tb object given a list of tb_Hamiltonians of length n_sigma
  C2PY_IGNORE one_body_elements_tb make_obe_from_tb(std::vector<tb_hamiltonian> const tb_H_sigma, spin_kind_e spin_kind,
                                                    std::vector<atomic_shell_t> atomic_shells);

  /** @brief Compute Hloc = H(R=0) given n_sigma tight_binding Hamiltonians 
   * 
   * @param H_sigma a list of TB Hamiltonians of length n_sigma 
   * @param atomic_shells a list of atomic shells corresponding to the orbitals contained in the TB Hamiltonian
   * @return Hloc, formated with dimensions [alphsa,sigma] each containing (n_orbitals_atom, n_orbitals_atom)
   */
  C2PY_IGNORE nda::array<nda::matrix<dcomplex>, 2> Hloc(std::vector<tb_hamiltonian> const &H_sigma, std::vector<atomic_shell_t> const &atomic_shells);

  /**
   * @ingroup hybridization
   * @brief Compute the atomic (impurity) levels from an obe.
   * 
   * @param obe One-body elements
   * @return Atomic impurity levels 
   */
  nda::array<nda::matrix<dcomplex>, 2> impurity_levels(one_body_elements_tb const &obe);

  /** 
    * @brief Convert a tight binding Hamiltonian to its superlattice equivalent.
    * 
    * @param sl The superlattice object containing its lattice vectors and locations of cluster points.
    * @param obe A one_body_elements object containing the tb_hamiltonian.
    * @return one_body_elements object based on the superlattice tight binding Hamiltonian. 
    */
  one_body_elements_tb fold(superlattice const &sl, one_body_elements_tb const &obe);

  //  -----------------------------------------------------------------------

  /** 
    * @ingroup gloc
    * @brief Compute the local Green's function without a self-energy.
    * 
    * @details See gloc for more details.
    * 
    * @tparam Mesh
    * @param obe A one_body_elements object containing the tb_hamiltonian
    * @param mu Chemical potential 
    * @param Sigma_dynamic The dynamic part of the embedded self-energy.
    * @param Sigma_static The static part of the embedded self-energy.
    * @param opt Container for options related integration of the BZ
    * @return gloc[alpha][sigma], the local Green's function
    */
  template <typename Mesh>
  block2_gf<Mesh, matrix_valued> gloc(one_body_elements_tb const &obe, double mu, block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                                      nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static, triqs::lattice::bz_int_options const &opt) {

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
    * @details See gloc for more details.
    * 
    * @tparam Mesh 
    * @param mesh The mesh on which Gloc will be computed 
    * @param obe A one_body_elements object containing the tb_hamiltonian
    * @param mu Chemical potential 
    * @param opt Container for options related integration of the BZ
    * @return gloc[alpha][sigma], the local Green's function
    */
  template <typename Mesh>
  block2_gf<Mesh, matrix_valued> gloc(Mesh const &mesh, one_body_elements_tb const &obe, double mu, triqs::lattice::bz_int_options const &opt) {
    auto result = make_block2_gf(mesh, obe.C_space.Gc_block_shape());
    for (auto sigma : range(obe.C_space.n_sigma())) { result(0, sigma) = gloc(mesh, obe.H[sigma], mu, opt); }
    return result;
  }

  //  -----------------------------------------------------------------------

  /**
   * @ingroup mu
   * @brief Compute the density of the lattice Green's function with a self-energy.
   * 
   * @tparam Mesh 
   * @param obe The one-body elements.
   * @param mu The chemical potential.
   * @param Sigma_dynamic The dynamic part of the embedded self-energy.
   * @param Sigma_static The static part of the embedded self-energy.
   * @param opt Container for options related integration of the BZ
   * @return density 
   */
  template <typename Mesh>
  double density(one_body_elements_tb const &obe, double mu, block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static, triqs::lattice::bz_int_options const &opt) {

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
    * @tparam Mesh
    * @param target_density The total electron density.
    * @param obe The one-body elements
    * @param Sigma_dynamic The dynamic part of the embedded self-energy.
    * @param Sigma_static The static part of the embedded self-energy.
    * @param opt Container for options related integration of the BZ
    * @param method The root finding method to use (default = dichotomy).
    * @param precision The precision to end search (default = 1e-5).
    * @param verbosity Printing of the root finder's progress (default = true).
    * @return Chemical potential  
    */
  template <typename Mesh>
  double find_chemical_potential(double const target_density, one_body_elements_tb const &obe, block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static, triqs::lattice::bz_int_options const &opt,
                                 std::string method = "dichotomy", double precision = 1.e-5, bool verbosity = true) {
    std::function<double(double)> f = [&obe, &Sigma_dynamic, &Sigma_static, &opt](double x) {
      return density(obe, x, Sigma_dynamic, Sigma_static, opt);
    };
    return std::get<0>(triqs::root_finder(method, f, 0.0, target_density, precision, 0.5, 1000, "Chemical Potential", "Total Density", verbosity));
  }

  /**
  * @ingroup mu
  * @brief Find the chemical potenital from the local Green's function and self-energy given a target density.
  *      
  * @tparam Mesh
  * @param target_density The total electron density.
  * @param obe The one-body elements
  * @param mesh The mesh on which Gloc will be computed 
  * @param opt Container for options related integration of the BZ
  * @param method The root finding method to use (default = dichotomy).
  * @param precision The precision to end search (default = 1e-5).
  * @param verbosity Printing of the root finder's progress (default = true).
  * @return Chemical potential  
  */
  template <typename Mesh>
  double find_chemical_potential(double const target_density, one_body_elements_tb const &obe, Mesh const &mesh,
                                 triqs::lattice::bz_int_options const &opt, std::string method = "dichotomy", double precision = 1.e-5,
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
                                                       triqs::lattice::bz_int_options const &opt);

  template block2_gf<mesh::imfreq, matrix_valued> gloc(mesh::imfreq const &mesh, one_body_elements_tb const &obe, double mu,
                                                       triqs::lattice::bz_int_options const &opt);

  template double density(one_body_elements_tb const &obe, double mu, block2_gf<mesh::imfreq, matrix_valued> const &Sigma_dynamic,
                          nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static, triqs::lattice::bz_int_options const &opt);

  template double find_chemical_potential(double const target_density, one_body_elements_tb const &obe,
                                          block2_gf<mesh::imfreq, matrix_valued> const &Sigma_dynamic,
                                          nda::array<nda::matrix<dcomplex>, 2> const &Sigma_static, triqs::lattice::bz_int_options const &opt,
                                          std::string method, double precision, bool verbosity);

  template double find_chemical_potential(double const target_density, one_body_elements_tb const &obe, mesh::imfreq const &mesh,
                                          triqs::lattice::bz_int_options const &opt, std::string method, double precision, bool verbosity);

  /** @endcond */

} // namespace triqs::modest
