// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "./downfolding.hpp"
#include "loaders.hpp"
#include "utils/defs.hpp"
#include "utils/gf_supp.hpp"
#include "utils/nda_supp.hpp"
#include "utils/range_supp.hpp"
#include <fmt/ranges.h>

namespace triqs {
  using block_matrix_t  = std::vector<nda::matrix<dcomplex>>;
  using block2_matrix_t = nda::array<nda::matrix<dcomplex>, 2>;
} // namespace triqs

namespace triqs::modest {

  /**
   * @ingroup embedding
   * @brief The embedding class.
   *
   * @details The embedding class encodes the mapping between impurities and the correlated space \f$ \mathcal{C} \f$
   * (cf. ModEST implementation notes). In particular, it maps the individual "blocks" within an impurity self-energy to
   * a block and spin of the embedded self-energy function, which can then be upfolded (downfolded) to obtain the
   * lattice self-energy and the local Green's function.
   */
  class embedding {

    // decomposition of C used by Sigma
    std::vector<long> sigma_embed_decomp;

    // decomposition of each impurity
    std::vector<std::vector<long>> imp_decomps;

    // names of the sigma (and tau) indices
    std::vector<std::string> _sigma_names;

    // names of the alpha indices
    std::vector<std::string> alpha_names;

    // stream
    friend std::ostream &operator<<(std::ostream &out, embedding const &E);

    /// h5 read/write
    friend void h5_write(h5::group g, std::string const &name, embedding const &x);
    friend void h5_read(h5::group g, std::string const &name, embedding &x);

    public:
    /** @cond DOXYGEN_SKIP_THIS */

    /// Struct representing an impurity block.
    struct imp_block_t {

      long imp_idx = 0; ///< Index of the impurity.
      long gamma   = 0; ///< First index \f$ \gamma \f$ of the block in the \f$ \Sigma_{\text{imp}} \f$.
      long tau     = 0; ///< Second index \f$ \tau \f$ of the block in the \f$ \Sigma_{\text{imp}} \f$.

      imp_block_t() = default;
      imp_block_t(long n_imp, long gamma, long tau) : imp_idx(n_imp), gamma(gamma), tau(tau) {}
      friend bool operator==(imp_block_t const &, imp_block_t const &) = default;

      // FIXME : merge with format_as
      friend std::ostream &operator<<(std::ostream &out, imp_block_t const &x) {
        return out << fmt::format("(imp_idx = {}, γ = {}, τ = {})", x.imp_idx, x.gamma, x.tau);
      }
    };

    /** @endcond */

    private:
    // For each imp, for all (γ, τ), a list of all (α, σ) such that ψ[α, σ] ==(n_imp, γ, τ)
    nda::array<imp_block_t, 2> psi; // ψ [α, σ] --> (n_imp, γ, τ)

    // reverse ψ [n_imp] -> (γ, τ) -> [(α, σ)]
    std::vector<nda::array<std::vector<std::array<long, 2>>, 2>> reverse_psi;

    public:
    /**
     * @brief Construct an embedding object.
     *
     * @param sigma_embed_decomposition  Decomposition (list of block sizes) for \f$ \Sigma_{\text{embed}} \f$:
     * \f$[ \alpha ] = \f$ block size. Note that the decompositions are given for \f$ \alpha \f$, \f$ \gamma \f$ index,
     * and are independent of \f$ \sigma \f$.
     * @param imp_decompositions List of decomposition of solvers: [n_imp][\f$ \gamma \f$] = block size.
     * @param psi The mapping \f$ \psi[\alpha,\sigma]\f$ -> n_imp, \f$ \gamma \f$, \f$ \tau \f$.
     * @param sigma_names Names for the values of the \f$ \sigma \f$ index (e.g., "up", "down").
     */
    embedding(std::vector<long> sigma_embed_decomposition, std::vector<std::vector<long>> imp_decompositions, nda::array<imp_block_t, 2> psi,
              std::vector<std::string> sigma_names);

    /** @cond DOXYGEN_SKIP_THIS */

    /// Default constructor for reading from HDF5.
    embedding() = default; // for h5_read

    /** @endcond */

    /** @name Accessors */
    ///@{

    /**
     * @brief Bracket accessor: \f$ \psi[\alpha,\sigma] \f$.
     * @return The corresponding impurity block (n_imp, \f$ \gamma \f$, \f$ \tau \f$).
     */
    imp_block_t operator[](long alpha, long sigma) const { return psi(alpha, sigma); }

    // HL: get_psi -> psi to match other classes?
    /// The mapping table \f$ \psi \f$.
    [[nodiscard]] nda::array<imp_block_t, 2> get_psi() const { return psi; }

    /// Number of blocks in \f$ \alpha \f$ for the \f$ \Sigma_{\text{embed}} \f$.
    [[nodiscard]] long n_alpha() const { return psi.extent(0); }

    /// Number of blocks in \f$ \sigma \f$ for the \f$ \Sigma_{\text{embed}} \f$.
    [[nodiscard]] long n_sigma() const { return psi.extent(1); }

    /// Number of blocks in \f$ \gamma \f$ for the \f$ \Sigma_{\text{imp}} \f$ [imp_idx].
    [[nodiscard]] long n_gamma(long imp_idx) const { return long(imp_decomps[imp_idx].size()); }

    /// Number of impurities.
    [[nodiscard]] long n_impurities() const { return long(imp_decomps.size()); }

    /// The names of the sigma indices.
    [[nodiscard]] std::vector<std::string> sigma_names() const { return _sigma_names; };

    /// The impurity decomposition.
    [[nodiscard]] std::vector<long> imp_decomposition(long imp_idx) const { return imp_decomps[imp_idx]; };

    /// Gf block structure for \f$ \Sigma_{\text{embed}} \f$.
    C2PY_IGNORE gf_struct2_t sigma_embed_block_shape() const;

    /// Gf block structure for the impurity solvers.
    std::vector<gf_struct_t> imp_block_shape() const;

    /// Summarize the embedding object.
    std::string description(bool verbosity = false) const;
    ///@}

    // ****************** Operation methods to change the embedding **********************

    /**
     * @name Unary Ops
     * Embedding methods which operate on itself and return a new Embedding object.
     * @{
     */

    // --------------------------------------------------------
    bool operator==(embedding const &other) const = default;
    // --------------------------------------------------------

    /**
     * @brief Remove an impurity from the embedding table \f$ \psi \f$.
     *
     * @details Remove the impurity `imp_idx`, map the corresponding \f$ \alpha \f$ blocks to -1 (i.e. no longer
     * connected to an impurity) and relabel the other impurities with a number > `imp_idx`.
     *
     * @param imp_idx Index of the impurity to remove from the table \f$ \psi \f$.
     * @return New embedding with the updated \f$ \psi \f$ map.
     */
    embedding drop(long imp_idx) const;

    /**
     * @brief Replaces one impurity in the embedding table \f$ \psi \f$.
     *
     * @details Replaces the impurity solver corresponding to `imp_idx_to_remove` with the impurity solver at
     * `imp_idx_to_replace_with` and updates the underlying table \f$ \psi \f$.
     *
     * @param imp_idx_to_remove The index of the impurity solver to be replaced.
     * @param imp_idx_to_replace_with The index of the impurity solver that will replace `imp_idx_to_remove`.
     * @return A new embedding with the updated \f$ \psi \f$ map.
     */
    embedding replace(long imp_idx_to_remove, long imp_idx_to_replace_with) const;

    /**
     * @brief Flip the spins (\f$ \sigma \f$) for block \f$ \alpha \f$.
     *
     * @details For block \f$ \alpha \f$ transforms \f$ (\sigma,\sigma') \to (\tau, \tau') \f$ to
     * \f$ (\sigma, \sigma') \to (\tau', \tau)\f$.
     *
     * @param alpha The index of the alpha block to flip the spins.
     * @return New embedding with the updated \f$ \psi \f$ map.
     */
    embedding flip_spin(long alpha) const;

    /**
     * @brief Flip the spins (\f$ \sigma \f$) for all \f$ [\alpha] \f$.
     *
     * @details For block \f$ \alpha \f$ transforms \f$ (\sigma,\sigma') \to (\tau, \tau') \f$ to
     * \f$ (\sigma, \sigma') \to (\tau', \tau)\f$.
     *
     * @param alphas A list of alpha blocks to flip the spins.
     * @return New embedding with the updated \f$ \psi \f$ map.
     */
    embedding flip_spin(std::vector<long> alphas) const;

    /**
  * @brief Convert the embedding to a spinless embedding.
  *
  * @details This method converts a spinful embedding (with two spin channels, e.g., "up" and "down") into a spinless colloquially referred to as "ud".
  * This caseful is usefuly when working with sytems with spin-orbit coupling or working with spin-averaged quantities, like the
  * density-density susceptibility.
  * 
  * @return embedding 
  */
    embedding make_spinless() const;

    /**
     * @brief Split impurity `imp_idx`.
     * @details Predicate p (long block_idx) -> 0 or 1.
     */
    embedding split(long imp_idx, std::function<bool(long)> p) const;

    /**
     * @brief Split impurity `imp_idx` based on `block_list`.
     *
     * @details For impurity `imp_idx`, split part of its blocks into a new impurity.
     * The blocks that were split off remain mapped to their original alpha blocks with the updated `imp_idx`.
     *
     * @param imp_idx Impurity number.
     * @param block_list A list of blocks of `imp_idx` to split off.
     * @return New embedding with the updated \f$ \psi \f$ map.
     */
    embedding split(long imp_idx, std::vector<long> const &block_list) const;

    embedding split(long imp_idx, std::initializer_list<const char *> x) = delete;

    /**
  * @brief Split the block (gamma) of an impurity (imp_idx) into multiple blocks with dimensions given by `new_dims`.
  *
  * @details This method splits a specific block (gamma) of an impurity (imp_idx) into multiple blocks with dimensions
  * specified by `new_dims`. The sum of `new_dims` must equal the original dimension of the block. For example, if the
  * original block has a dimension of 5, it could be split into blocks of dimensions 2 (eg) and 3 (t2g).
  *
  * @param imp_idx The index of the impurity to split.
  * @param gamma The block index of the impurity to split.
  * @param new_dims The new dimensions of the split blocks. The sum of `new_dims` must equal the original dimension of the block.
  * @return New embedding with the impurity structure and \f$ \psi \f$ mapping.
  */
    embedding split_block(long imp_idx, long gamma, std::vector<long> const &new_dims) const;

    ///@}

    //------------------------------------------------------------------------------------

    /// Zero initialize impurity self-energies.
    template <typename Mesh> std::vector<std::pair<block_gf<Mesh, matrix_valued>, block_matrix_t>> make_zero_imp_self_energies(Mesh const &mesh) {

      auto make_block_matrix = [](auto const &gf_struct) {
        return gf_struct | stdv::transform([](auto &x) {
                 auto bl_size = x.second;
                 return nda::zeros<dcomplex>(bl_size, bl_size);
               })
           | tl::to<std::vector<nda::matrix<dcomplex>>>();
      };

      auto make_self_energy = [&](auto const &gf_struct) {
        auto Sigma_static  = make_block_matrix(gf_struct);
        auto Sigma_dynamic = block_gf<Mesh, matrix_valued>{mesh, gf_struct};
        return std::make_pair(Sigma_dynamic, Sigma_static);
      };

      return imp_block_shape() | stdv::transform(make_self_energy) | tl::to<std::vector>();
    }
    //------------------------------------------------------------------------------------

    // ****************************************************
    //     Embedding Extract/Embed methods
    // ****************************************************
    /**
     * @name Complex Ops
     * Embedding methods which operate on a given object X where X ∈ (block2gf, block_gf, block_matrix, etc.)
     * @{
     */

    //--------------------- Embed -----------------------------------------

    /**
     * @brief Embed single-particle quantities (TRIQS/ModEST).
     *
     * @details Embed impurity solver self-energies into an embedded self-energy.
     *
     * * \f$ \Sigma_{\text{embed}}^{ασ} = \Sigma_{\text{imp}}^{ψ(α,σ)} \f$,
     * * \f$ \psi(\alpha,\sigma) \to \f$ (n_imp, \f$ \gamma \f$, \f$ \tau \f$).
     *
     * @param Sigma_imp_vec A list of impurity self-energies.
     * @return \f$ \Sigma_{\text{embed}}[\alpha, \sigma] \f$ in \f$ \mathcal{C} \f$ space using the embed decomposition.
     */
    template <typename Mesh> block2_gf<Mesh, matrix_valued> embed(std::vector<block_gf<Mesh, matrix_valued>> const &Sigma_imp_vec) const {
      // Check that all meshes are the same for Sigma_imp_vec
      if (not all_equal(Sigma_imp_vec | stdv::transform([](auto &&x) -> decltype(auto) { return x[0].mesh(); })))
        throw std::runtime_error{"[embedding_desc::embed]: meshes of solvers are not all equal"};

      auto const &mesh = Sigma_imp_vec[0][0].mesh();

      // build the result
      auto Sigma_embed = make_block2_gf(mesh, this->sigma_embed_block_shape());
      for (auto &&[S, m] : zip(Sigma_embed, psi)) {
        if (m.imp_idx == -1) continue;
        S() = Sigma_imp_vec[m.imp_idx][m.gamma + n_gamma(m.imp_idx) * m.tau];
      }
      return Sigma_embed;
    }

    /**
     * @brief Embed single-particle quantities (TRIQS/ModEST).
     *
     * @details Embed the impurity self-energy decomposed as a list of dynamic and a list of static parts.
     *
     * @tparam Mesh The type of mesh.
     * @param Sigma_imp_vec A list of dynamic impurity self-energies.
     * @param Sigma_imp_static_vec A list of static impurity self-energies.
     * @return The self-energy in the \f$ \mathcal{C} \f$ space as a pair (dynamic, static).
     */
    template <typename Mesh>
    std::pair<block2_gf<Mesh, matrix_valued>, block2_matrix_t> embed(std::vector<block_gf<Mesh, matrix_valued>> const &Sigma_imp_vec,
                                                                     std::vector<block_matrix_t> const &Sigma_imp_static_vec) const {
      if (Sigma_imp_vec.size() != Sigma_imp_static_vec.size()) {
        throw std::runtime_error(fmt::format("The lists of self-energies are not equal {} != {}", Sigma_imp_vec.size(), Sigma_imp_static_vec.size()));
      }
      return {this->embed(Sigma_imp_vec), this->embed(Sigma_imp_static_vec)};
    }

    /// Embed block matrices (TRIQS/ModEST).
    block2_matrix_t embed(std::vector<block_matrix_t> const &Sigma_imp_static_vec) const;

    block_matrix_t embed_ij(std::vector<block_matrix_t> const &Sigma_imp_static_vec) const;

    //--------------------- Extract ---------------------------------------

    /**
     * @brief Extract single-particle quantities (TRIQS/ModEST).
     *
     * @tparam Mesh The mesh type triqs::mesh::{dlr_imfreq, imfreq}.
     * @param g_loc Block2Gf of gloc in \f$ M \times M \f$ space.
     * @return Local impurity Green's function.
     */
    template <typename Mesh> std::vector<block_gf<Mesh, matrix_valued>> extract(block2_gf<Mesh, matrix_valued> const &g_loc) const {

      if (auto decomp = get_struct(g_loc).dims(r_all, 0) | tl::to<std::vector>(); decomp != this->sigma_embed_decomp) {
        if (decomp.size() != 1) throw std::runtime_error{"extract: g should have decomp = sigma_embedding_decomp or [1]"};
        return extract(decomposition_view(g_loc, this->sigma_embed_block_shape()));
      }

      // FIXME : check all meshes are the same
      auto imp_gf_stru_list = imp_block_shape();
      auto extract_one_imp  = [&](long n_imp) {
        auto gimp        = block_gf{g_loc(0, 0).mesh(), imp_gf_stru_list[n_imp]};
        auto const &rpsi = reverse_psi[n_imp];
        for (auto [gamma, tau] : rpsi.indices()) {
          auto [alpha, sigma]                       = rpsi(gamma, tau)[0];
          gimp[gamma + n_gamma(n_imp) * tau].data() = g_loc(alpha, sigma).data();
        }
        return gimp;
      };
      return range(n_impurities()) | stdv::transform(extract_one_imp) | tl::to<std::vector>();
    }

    /// Extract matrices (TRIQS/ModEST).
    std::vector<block_matrix_t> extract(block2_matrix_t const &matrix_C) const;

    ///@}

    /**
     * @name CoQui Ops
     * Embedding methods which operate on a given object X where X ∈ (nda::array<dcomplex, 4>, nda::array<dcomplex, 5>, etc.)
     * These methods are used to extract and embed quantities in the CoQui format.
     * @{
     */

    //--------------------- Embed -----------------------------------------

    /// Embed single-particle quantities (CoQui).
    std::vector<nda::array<dcomplex, 3>> embed_wij(std::vector<std::vector<nda::array<dcomplex, 3>>> const &Sigma_imp_vec) const;

    /// Embed two-particle quantities (CoQui).
    nda::array<dcomplex, 5> embed_wijkl(std::vector<nda::array<dcomplex, 5>> const &pi_imp_vec) const;

    /// Embed tensors (CoQui).
    nda::array<dcomplex, 4> embed_ijkl(std::vector<nda::array<dcomplex, 4>> const &U_tensor_vec) const;

    //--------------------- Extract ---------------------------------------

    /// Extract single-particle quantities (CoQui).
    std::vector<std::vector<nda::array<dcomplex, 3>>> extract_wij(nda::array<dcomplex, 4> const &g_loc) const;

    /// Extract two-particle quantities (CoQui).
    std::vector<nda::array<dcomplex, 5>> extract_wijkl(nda::array<dcomplex, 5> const &Pi_loc) const;

    /// Extract tensors (CoQui).
    std::vector<nda::array<dcomplex, 4>> extract_ijkl(nda::array<dcomplex, 4> const &U_tensor) const;

    ///@}
  };

  // ***************************** Free functions/factories *******************************************

  /**
   * @name Embedding factories functions
   * Factory functions to create the embedding class for different embedding scenarios. Typically, one will create the embedding
   * from the local space.
   * @{
   */

  /** @cond DOXYGEN_SKIP_THIS */
  /**
   * @brief Construct the embedding class from the local space, a description of the block decomposition, and an
   * equivalence mapping between atom sites.
   *
   * @param spin_names The names of the spin indices.
   * @param block_decomposition The decomposition of atomic orbitals into their irreducible representations.
   * @param atom_to_imp [optional] A mapping between equivalent atom sites.
   * @return Embedding object.
   */
  embedding embedding_builder(std::vector<std::string> const &spin_names, nda::array<std::vector<long>, 2> const &block_decomposition,
                              std::vector<long> const &atom_to_imp);
  /** @endcond */

  /**
   * @brief Construct the embedding class from the local space, a description of the block decomposition, and an
   * equivalence mapping between atom sites.
   *
   * @param spin_names The names of the spin indices.
   * @param block_decomposition The decomposition of atomic orbitals into their irreducible representations.
   * @param atom_to_imp [optional] A mapping between equivalent atom sites.
   * @return Embedding object.
   */
  embedding embedding_builder(std::vector<std::string> const &spin_names, std::vector<std::vector<long>> const &block_decomposition,
                              std::vector<long> const &atom_to_imp);

  /**
   * @ingroup embedding
   * @brief Make an embedding from the local space.
   *
   * @details This function creates an embedding object from the local space, which is typically used in ModEST for
   * embedding calculations. The default behavior is to use the equivalences between different atoms when constructing
   * the embedding and to use the irrep decomposition of the atomic orbitals. Instead of the irrep decomposition, one
   * can use the atomic decomposition.
   *
   * @param C_space The local space from a one-body elements (on grid/tight-binding).
   * @param use_atom_equivalences Use the equivalences between different atoms when constructing the embedding.
   * @param use_atom_decomp Use the atomic decomposition instead of the atomic orbital decomposition.
   * @return Embedding object.
   */
  embedding make_embedding(local_space const &C_space, bool use_atom_equivalences = true, bool use_atom_decomp = false);

  // -----------------------------------------------------------------------
  /**
   * @ingroup embedding
   * @brief Make an embedding for clusters of atoms.
   *
   * @details This function creates an embedding object from the one-body elements on grid (obe) and a partition of the
   * atoms into clusters. The clusters are defined by the `atom_partition`, which is a vector of vectors, where each
   * inner vector contains the indices of the atoms in the cluster.
   * The resulting one-body elements on grid will have the atoms permuted according to the `atom_partition`.
   *
   * @param obe One-body elements on grid (obe) to permute.
   * @param atom_partition New partition of the atoms into clusters, where each inner vector contains the indices of the
   * atoms in the cluster.
   * @return A new one-body elements on grid and the corresponding embedding object.
   */
  inline std::pair<one_body_elements_on_grid, embedding> make_embedding_with_clusters(one_body_elements_on_grid obe,
                                                                                      std::vector<std::vector<long>> const &atom_partition) {
    auto new_obe = permute_local_space(atom_partition, obe);
    auto E       = make_embedding(new_obe.C_space, false);
    return {new_obe, E};
  }

  inline std::pair<downfolding_projector, embedding> make_embedding_from_h5(std::string const &filename) {
    auto [target_density, obe] = one_body_elements_from_dft_converter(filename);
    auto E                     = make_embedding(obe.C_space, false, true);
    return {obe.P, E};
  }

  /** @} Embedding factories functions */

#define INSTANTIATE(Mesh)                                                                                                                            \
  template block2_gf<Mesh, matrix_valued> embedding::embed(std::vector<block_gf<Mesh, matrix_valued>> const &) const;                                \
  template std::pair<block2_gf<Mesh, matrix_valued>, block2_matrix_t> embedding::embed(std::vector<block_gf<Mesh, matrix_valued>> const &,           \
                                                                                       std::vector<block_matrix_t> const &) const;                   \
  template std::vector<block_gf<Mesh, matrix_valued>> embedding::extract(block2_gf<Mesh, matrix_valued> const &) const;                              \
  template std::vector<std::pair<block_gf<Mesh, matrix_valued>, block_matrix_t>> embedding::make_zero_imp_self_energies(Mesh const &);
  INSTANTIATE(imfreq);
  INSTANTIATE(refreq);
  INSTANTIATE(dlr_imfreq);
#undef INSTANTIATE

} // namespace triqs::modest
