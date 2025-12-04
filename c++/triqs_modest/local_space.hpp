// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <triqs/gfs.hpp>
#include <mpi/mpi.hpp>
#include "./utils/defs.hpp"
#include "./utils/gf_supp.hpp"

namespace triqs::modest {

  using namespace triqs::gfs;

  // -----------------------------------------------
  /// Kind of σ index
  enum class spin_kind_e {
    Polarized,    // σ = 0,1, the object can have different value for different σ
    NonPolarized, // σ = 0,1, but the object is the same for both σ. Can store only one copy e.g.
    NonColinear   // σ = 0. There is no index, the spin index is grouped with other non-diagonal indices. e.g. Nambu, spin-orbit
  };

  // --------------------------------------------------------------------
  /// Info on an atomic shell.
  struct atomic_orbs {
    long dim     = 0; ///< Dimension of the orbital space.
    long l       = 0; ///< Angular quantum number.
    long cls_idx = 0; ///< Equivalent atoms will have the same sort index (sort).
    long dft_idx = 0; ///< Index of the atom in the dft code if any, or \f$ -1 \f$.
    // int n_irrep;     // are the orbitals reducible into irreps (irep) ( seach "irep" in TRIQS/dft_tools; never used in DMFT routines
    void serialize(auto &ar) const { ar & dim & l & cls_idx & dft_idx; };
    void deserialize(auto &ar) { ar & dim & l & cls_idx & dft_idx; };

    /// Equality comparison operator.
    bool operator==(atomic_orbs const &) const = default;

    /// MPI broadcast
    friend void mpi_broadcast(atomic_orbs &x, mpi::communicator c = {}, int root = 0) {
      mpi::broadcast(x.dim, c, root);
      mpi::broadcast(x.l, c, root);
      mpi::broadcast(x.cls_idx, c, root);
      mpi::broadcast(x.dft_idx, c, root);
    }
  };

  // ==========================================================
  /**
   * @ingroup one_body_elements
   * @brief Describe the atomic orbitals within downfolded \f$\mathcal{C}\f$ space.
   *
   * @details The local space \f$ \mathcal{C} \f$ defines the correlated subspace which defines impurities to be solved
   * within DMFT.
   *
   * The DFT + DMFT equations involve three different spaces, following (mostly) the notations of [S. Beck et al. 2022]:
   *
   * * The (reduced) Bloch space \f${\cal B}\f$ contains bands of dispersion \f$\varepsilon_{\nu}^{\sigma}(\mathbf{k})\f$,
   *   in some window of energy. Here, \f$\nu\f$ is the band index (\f$0 \leq \nu < N_\nu^{\mathbf{k}}\f$) and
   *   \f$\mathbf{k}\f$ is a point in the Brillouin zone. We define
   *   \f$N_\nu \equiv \mathrm{max}_{\mathbf{k}} N_\nu^{\mathbf{k}}\f$.
   * * The Wannier space \f${\cal W}\f$ is spanned from Wannier functions constructed from \f${\cal B}\f$.
   * * The correlated space \f${\cal C} \subseteq {\cal W}\f$ containing \f$M\f$ Wannier orbitals, is a subspace of the
   *   Wannier space, in which the self-energy is approximated by the embedding. \f${\cal C}\f$ is spanned by Wannier
   *   functions at several atoms/sites with index \f$a\f$ at position \f$R_a\f$ and orbital/Wannier index \f$m_{a}\f$.
   *   \f${\cal C}\f$ is indexed by a composite index \f$m = (a, m_{a})\f$, with \f$0\leq m \leq M-1\f$ and
   *   \f$M=\sum_{a}\mathrm{max}(m_{a}\f$). We will write the main equations with the composite \f$m\f$ index, as the
   *   \f$m = (a, m_{a})\f$ decomposition of \f$m\f$ is not, in general, appropriate for embeddings.
   *
   * The \f$\sigma\f$ index is a general block diagonal index. In simple cases, it is the spin index, but not always.
   *
   * * In "spin (non-)polarized" computations, \f$\sigma\f$ is the spin index.
   * * In spin-orbit or Nambu computations, the spin index is merged with \f$m\f$ and \f$\nu\f$, so \f$\sigma =0\f$
   *   (i.e. one value of the index, equivalent to no index at all).
   */
  class local_space {

    spin_kind_e _spin_kind = spin_kind_e::Polarized;

    // List of all atomic shells spanning the 𝓒 space
    std::vector<atomic_orbs> _atomic_shells;

    // For each atom, a list of the dimensions of the irreps decomposition of the atomic shell
    nda::array<std::vector<long>, 2> _irreps_decomp_per_atom;

    // rotation matrices that rotate BACK to the basis of the DFT code basis.
    // FIXME : CHECK vs INVERSE, EXPLAIN, WRITE NOTE.
    nda::array<nda::matrix<dcomplex>, 2> _rotation_from_dft_to_local_basis;

    // rotation matrix from the Ylm basis to the dft code
    nda::array<nda::matrix<dcomplex>, 1> _rotation_from_spherical_to_dft_basis;

    // dimension of the correlated space
    long _dim_C = 0;

    // atom names
    std::vector<std::string> _atom_names = {};

    /// h5 read/write
    friend void h5_read(h5::group g, std::string const &name, local_space &ls);
    friend void h5_write(h5::group g, std::string const &name, local_space const &ls);

    /// MPI broadcast
    friend void mpi_broadcast(local_space &x, mpi::communicator c = {}, int root = 0) {
      mpi::broadcast(x._spin_kind, c, root);
      mpi::broadcast(x._atomic_shells, c, root);
      mpi::broadcast(x._irreps_decomp_per_atom, c, root);
      mpi::broadcast(x._rotation_from_dft_to_local_basis, c, root);
      mpi::broadcast(x._rotation_from_spherical_to_dft_basis, c, root);
      mpi::broadcast(x._dim_C, c, root);
      mpi::broadcast(x._atom_names, c, root);
    }

    public:
    /// Equality comparison operator.
    bool operator==(local_space const &) const = default;

    /**
     * @brief Construct a new local space object.
     *
     * @param spin_kind Kind of \f$ \sigma \f$ index.
     * @param atomic_shells List of all atomic orbitals.
     * @param irreps_decomp_per_atom List of all blocks spanning \f$ \mathcal{C} \f$ space -> atoms_block_decomposition.
     * @param rotation_from_dft_to_local_basis Rotation matices from DFT to local basis.
     * @param rotation_from_spherical_to_dft_basis Rotation matrices from spherical to DFT basis.
     */
    local_space(spin_kind_e spin_kind, std::vector<atomic_orbs> atomic_shells, nda::array<std::vector<long>, 2> irreps_decomp_per_atom,
                nda::array<nda::matrix<dcomplex>, 2> rotation_from_dft_to_local_basis,
                nda::array<nda::matrix<dcomplex>, 1> rotation_from_spherical_to_dft_basis);

    /** @cond DOXYGEN_SKIP_THIS */
    /// default constructor for h5
    local_space() = default;
    /** @endcond */

    // ---------------- sigma indices and co. ----------------
    /** @name Accessors */
    ///@{

    /// Spin kind of \f$ \sigma \f$ index.
    [[nodiscard]] spin_kind_e spin_kind() const { return _spin_kind; };

    /// Dimension of the \f$ \sigma \f$ index.
    [[nodiscard]] long n_sigma() const { return _spin_kind == spin_kind_e::NonColinear ? 1 : 2; }

    /// Names of spin indices for naming blocks in block GFs.
    [[nodiscard]] std::vector<std::string> sigma_names() const {
      return (_spin_kind == spin_kind_e::NonColinear ? std::vector{"ud"s} : std::vector{"up"s, "down"s});
    }

    // --------  Atomic shells and related functions -----------
    /// Dimension of the correlated space.
    [[nodiscard]] long dim() const { return _dim_C; }

    /// List of all atomic shells spanning the \f$ \mathcal{C} \f$ space.
    [[nodiscard]] std::vector<atomic_orbs> const &atomic_shells() const { return _atomic_shells; }

    /// The number of atoms.
    [[nodiscard]] long n_atoms() const { return long(_atomic_shells.size()); }

    // OP : explain the 2 indices : spin ?? [ a, σ] ?
    /// 2-dim array of all blocks spanning \f$ \mathcal{C} \f$ space -> atoms_block_decomposition.
    [[nodiscard]] nda::array<std::vector<long>, 2> const &atoms_block_decomposition() const { return _irreps_decomp_per_atom; }

    /// 2-dim array of all \f$ (a, \sigma) \f$ local rotation matices that rotate the data.
    [[nodiscard]] nda::array<nda::matrix<dcomplex>, 2> const &rotation_from_dft_to_local_basis() const { return _rotation_from_dft_to_local_basis; }

    /// Array of rotation matrices from spherical harmonics to dft specific orbital basis.
    [[nodiscard]] nda::array<nda::matrix<dcomplex>, 1> const &rotation_from_spherical_to_dft_basis() const {
      return _rotation_from_spherical_to_dft_basis;
    }

    /// Given the index of an atomic shell, return the index of the first atomic shell of its equivalence class.
    [[nodiscard]] long first_shell_of_its_equiv_cls(long idx) const;

    /// Names of the atoms in the orbital set.
    [[nodiscard]] std::vector<std::string> atom_names() const { return _atom_names; }

    /// Transformed view containing the dimension of each atomic shell.
    [[nodiscard]] auto atomic_decomposition() const {
      return atomic_shells() | stdv::transform([](auto const &s) { return s.dim; });
    }

    ///@}

    // --------  Atomic "view" methods for Green functions and matrices -----------

    // FIXME : IT IS NOT A VIEW, IT IS A COPY
    // Where do we use this ? do we want a view or a copy ?
    /** @name Views and shapes */
    ///@{
    /**
     * @brief Views a 2-dim block GF according to the atomic decomposition.
     *
     * @tparam Mesh The Green's function meesh
     * @param G_C A 2-dim block GF spanning the entire \f$ \mathcal{C} \f$ space [0, sigma](M, M).
     * @return A 2-dim block GF in the atomic decomposition view [atom, sigma][m_orb, m_orb].
     */
    template <typename Mesh> block2_gf<Mesh> atomic_view(block2_gf<Mesh> const &G_C) {
      // FIXME : ASSERT check the dimension of G_C m x m , one a block, n_sigma ...
      auto Gout    = make_block2_gf(G_C(0, 0).mesh(), this->Gatom_block_shape());
      auto n_sigma = G_C.size2();
      for (auto sigma : range(n_sigma))
        for (auto const &[atom, r_atom] : enumerated_sub_slices(this->atomic_decomposition())) {
          Gout(atom, sigma).data() = G_C(0, sigma).data()(r_all, r_atom, r_atom);
        }
      return Gout;
    }

    /**
     * @brief Creates an atomic view of block matrices by extracting specific slices based on the atomic decomposition
     * of the current object.
     *
     * @param matrix_C The input block matrices object from which the atomic view is created.
     * @return 2-dim array containing the new block matrices in the atomic view.
     */
    nda::array<nda::matrix<dcomplex>, 2> atomic_view(nda::array<nda::matrix<dcomplex>, 2> const &matrix_C);

    // --------  gf_structs for different block shapes -----------

    /// Shape of the Green function in the correlated space, without block decomposition.
    [[nodiscard]] C2PY_IGNORE gf_struct2_t Gc_block_shape() const;

    // OP : [a, \sigma] : what is a, sigma ?
    /// Shape of the Green function in the correlated space, decomposed by atomic shells.
    [[nodiscard]] C2PY_IGNORE gf_struct2_t Gatom_block_shape() const {
      auto res = nda::zeros<long>(_atomic_shells.size(), n_sigma());
      for (auto const &[alpha, shell] : enumerate(_atomic_shells)) res(alpha, r_all) = shell.dim;
      return {.names = {atom_names(), sigma_names()}, .dims = std::move(res)};
    }

    ///@}
  };

  // --------  stream -----------

  std::ostream &operator<<(std::ostream &out, local_space const &bd);

  // --------  instantiations --------------

  /** @cond DOXYGEN_SKIP_THIS */
  template block2_gf<imfreq> local_space::atomic_view(block2_gf<imfreq> const &G_C);
  /** @endcond */

} // namespace triqs::modest
