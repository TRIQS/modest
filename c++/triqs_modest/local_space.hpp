#pragma once
#include <triqs/gfs.hpp>
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
  /// Info on an atomic shell
  struct atomic_shell_t {
    long dim     = 0; // dimension of orbital space
    long l       = 0; // angular quantum number
    long cls_idx = 0; // Equivalent atoms will have the same sort index (sort)
    long dft_idx = 0; // idx of the atom in the dft code if any, or -1
    // int n_irrep;     // are the orbitals reducible into irreps (irep) ( seach "irep" in TRIQS/dft_tools; never used in DMFT routines
  };

  // ==========================================================
  /**
    * @ingroup local_space
    * @brief Describe the set of local orbitals of a downfolded 𝓒 space.
    *
    * It contains: 
    *   * the spin kind (NonPolarized, Polarized, NonColinear)
    *   * the shell structure.
    *   * the decomposition of the 𝓒 space into atomic and irreps decomposition.
    *   * the transformation matrices to the DFT-basis and to the spherical basis. FIXME : Cf note : EXPLAIN  
    * 
    */
  class local_space {

    spin_kind_e _spin_kind = spin_kind_e::Polarized;

    /// List of all atomic shells spanning the 𝓒 space
    std::vector<atomic_shell_t> _atomic_shells;

    /// For each atom, a list of the dimensions of the irreps decomposition of the atomic shell
    nda::array<std::vector<long>, 2> _irreps_decomp_per_atom;

    /// rotation matrices that rotate BACK to the basis of the DFT code basis.
    // FIXME : CHECK vs INVERSE, EXPLAIN, WRITE NOTE.
    nda::array<nda::matrix<dcomplex>, 2> _rotation_from_dft_to_local_basis;

    /// rotation matrix from the Ylm basis to the dft code
    nda::array<nda::matrix<dcomplex>, 1> _rotation_from_spherical_to_dft_basis;

    /// dimension of the correlated space
    long _dim_C = 0;

    /// atom names
    std::vector<std::string> _atom_names = {};

    /// h5 read/write
    friend void h5_read(h5::group g, std::string const &name, local_space &ls);
    friend void h5_write(h5::group g, std::string const &name, local_space const &ls);

    public:
    /**
     * @brief Constructs an orbital_set object with the specified parameters.
     *
     * @param spin_kind The spin kind 
     * @param atomic_shells The atomic shells.
     * @param irreps_decomp_per_atom  irreps_decomp_per_atom[a, σ] = a list of dimension of the irreps decomposition the atom a
     * @param rotation_from_dft_to_local_basis  rotation_to_dft_basis[a, σ]: unitary matrix of the rotation to the DFT code basis. FIXME : CHECK 
     * @param rotation_from_spherical_to_dft_basis  rotation_to_dft_basis[a, σ]: unitary matrix of the rotation to the DFT code basis. FIXME : CHECK 
     */
    local_space(spin_kind_e spin_kind, std::vector<atomic_shell_t> atomic_shells,
                nda::array<std::vector<long>, 2> irreps_decomp_per_atom,
                nda::array<nda::matrix<dcomplex>, 2> rotation_from_dft_to_local_basis,
                nda::array<nda::matrix<dcomplex>, 1> rotation_from_spherical_to_dft_basis);

    /// default constructor for h5
    local_space() = default;

    // ---------------- sigma indices and co. ----------------

    /// Accessor for the spin kind
    [[nodiscard]] spin_kind_e spin_kind() const { return _spin_kind; };

    /// Dimension of the σ index
    [[nodiscard]] long n_sigma() const { return _spin_kind == spin_kind_e::NonColinear ? 1 : 2; }

    /// names of spin indices for naming blocks in block_gf
    [[nodiscard]] std::vector<std::string> sigma_names() const {
      return (_spin_kind == spin_kind_e::NonColinear ? std::vector{"ud"s} : std::vector{"up"s, "down"s});
    }

    // --------  Atomic shells and related functions -----------

    /// Dimension of the correlated space
    [[nodiscard]] long dim() const { return _dim_C; }

    /// List of all atomic shells spanning the 𝓒 space
    [[nodiscard]] std::vector<atomic_shell_t> const &atomic_shells() const { return _atomic_shells; }

    /// The number of atoms
    [[nodiscard]] long n_atoms() const { return long(_atomic_shells.size()); }

    /// List of all blocks spanning 𝓒 space -> atoms_block_decomposition
    // OP : explain the 2 indices : spin ?? [ a, σ] ?
    [[nodiscard]] nda::array<std::vector<long>, 2> const &atoms_block_decomposition() const {
      return _irreps_decomp_per_atom;
    }

    /// List of all (a, sigma) local rotation matices that rotate the data
    [[nodiscard]] nda::array<nda::matrix<dcomplex>, 2> const &rotation_from_dft_to_local_basis() const {
      return _rotation_from_dft_to_local_basis;
    }

    /// List of rotation matrices from spherical harmonics to dft specific orbital basis
    [[nodiscard]] nda::array<nda::matrix<dcomplex>, 1> const &rotation_from_spherical_to_dft_basis() const {
      return _rotation_from_spherical_to_dft_basis;
    }

    /// Given the index idx of an atomic shell, return the index of the first atomic shell of its equivalence class
    [[nodiscard]] long first_shell_of_its_equiv_cls(long idx) const;

    /// Names of the atoms in the orbital set.
    [[nodiscard]] std::vector<std::string> atom_names() const { return _atom_names; }

    /// Generates [dimension of the atomic shell].
    [[nodiscard]] auto atomic_decomposition() const {
      return atomic_shells() | stdv::transform([](auto const &s) { return s.dim; });
    }

    // --------  Atomic "view" methods for Green functions and matrices -----------

    /**
     * @brief Creates an atomic view of a block2_gf object by extracting specific slices
     *        based on the atomic decomposition of the current object.
     *
     * @tparam Mesh The type of the mesh 
     * @param G_C The input block2_gf object from which the atomic view is created.
     * @return A new block2_gf object representing the atomic view.
     *
     */
    // FIXME : IT IS NOT A VIEW, IT IS A COPY
    // Where do we use this ? do we want a view or a copy ?
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
     * @brief Creates an atomic view of block matrices  by extracting specific slices
     *        based on the atomic decomposition of the current object.
     *
     * @param matrix_C The input block matrices object from which the atomic view is created.
     * @return new block matrices in the atomic view.
     *
     */
    nda::array<nda::matrix<dcomplex>, 2> atomic_view(nda::array<nda::matrix<dcomplex>, 2> const &matrix_C);

    // --------  gf_structs for different block shapes -----------

    /// Shape of the Green function in the correlated space, without block decomposition
    [[nodiscard]] C2PY_IGNORE gf_struct2_t Gc_block_shape() const;

    /// Shape of the Green function in the correlated space, decomposed by atomic shells
    // OP : [a, \sigma] : what is a, sigma ?
    [[nodiscard]] C2PY_IGNORE gf_struct2_t Gatom_block_shape() const {
      auto res = nda::zeros<long>(_atomic_shells.size(), n_sigma());
      for (auto const &[alpha, shell] : enumerate(_atomic_shells)) res(alpha, r_all) = shell.dim;
      return {.names = {atom_names(), sigma_names()}, .dims = std::move(res)};
    }
  };

  // --------  stream -----------

  std::ostream &operator<<(std::ostream &out, local_space const &bd);

  // --------  instantiations --------------

  /** @cond DOXYGEN_SKIP_THIS */
  template block2_gf<imfreq> local_space::atomic_view(block2_gf<imfreq> const &G_C);
  /** @endcond */

} // namespace triqs::modest
