#pragma once
#include "./downfolding.hpp"
#include "utils/defs.hpp"
#include "utils/gf_supp.hpp"
#include "utils/range_supp.hpp"
#include <fmt/ranges.h>

namespace triqs::modest {

  /**
 * @ingroup embedding
 * @brief The embedding class.
 * 
 * @details The embeddding class encodes the mapping between impurities and the correlated space 𝓒 (cf. ModEST implementation notes). 
 * In particular, it maps the individual "blocks" within an impurity self-energy to a block and spin of the embedded self-energy function,
 * which can then be upfolded (downfolded) to obtain the lattice self-energy and the local Green's function. 
 *
 */
  class embedding {

    /// decomposition of C used by Sigma
    std::vector<long> sigma_embed_decomp;

    /// decomposition of each impurity
    std::vector<std::vector<long>> imp_decomps;

    /// names of the sigma (and tau) indices
    std::vector<std::string> _sigma_names;

    /// names of the alpha indices
    std::vector<std::string> alpha_names;

    /// stream
    friend std::ostream &operator<<(std::ostream &out, embedding const &E);

    /// h5 read/write
    friend void h5_write(h5::group g, std::string const &name, embedding const &x);
    friend void h5_read(h5::group g, std::string const &name, embedding &x);

    public:
    /// struct of tuples imp_idx, γ, τ
    struct imp_block_t {

      long imp_idx = 0; // index of the impurity
      long gamma   = 0; // first index γ of the block in the Σ_imp
      long tau     = 0; // second index τ of the block in the Σ_imp

      imp_block_t() = default;
      imp_block_t(long n_imp, long gamma, long tau) : imp_idx(n_imp), gamma(gamma), tau(tau) {}
      friend bool operator==(imp_block_t const &, imp_block_t const &) = default;

      // FIXME : merge with format_as
      friend std::ostream &operator<<(std::ostream &out, imp_block_t const &x) {
        return out << fmt::format("(imp_idx = {}, γ = {}, τ = {})", x.imp_idx, x.gamma, x.tau);
      }
    };

    private:
    // For each imp, for all (γ, τ), a list of all (α, σ) such that ψ[α, σ] ==(n_imp, γ, τ)
    nda::array<imp_block_t, 2> psi; // ψ [α, σ] --> (n_imp, γ, τ)

    // reverse ψ [n_imp] -> (γ, τ) -> [(α, σ)]
    std::vector<nda::array<std::vector<std::array<long, 2>>, 2>> reverse_psi;

    public:
    /// bracket accessor: [α, σ] -> the corresponding impurity block (n_imp, γ, τ)
    imp_block_t operator[](long alpha, long sigma) const { return psi(alpha, sigma); }

    // HL: get_psi -> psi to match other classes?
    /// the mapping table ψ
    [[nodiscard]] nda::array<imp_block_t, 2> get_psi() const { return psi; }

    /// Number of blocks in α for the Σ_embed
    [[nodiscard]] long n_alpha() const { return psi.extent(0); }

    /// Number of blocks in σ for the Σ_embed
    [[nodiscard]] long n_sigma() const { return psi.extent(1); }

    /// Number of blocks in γ for the Σ_imp[imp_idx]
    [[nodiscard]] long n_gamma(long imp_idx) const { return long(imp_decomps[imp_idx].size()); }

    /// Number of impurities
    [[nodiscard]] long n_impurities() const { return long(imp_decomps.size()); }

    /// The names of the sigma indices
    [[nodiscard]] std::vector<std::string> sigma_names() const { return _sigma_names; };

    /// The impurity decomposition
    [[nodiscard]] std::vector<long> imp_decomposition(long imp_idx) const { return imp_decomps[imp_idx]; };

    // --------------------------------------------------------

    /// Default constructor
    embedding() = default; // for h5_read

    /**
     * @ingroup embedding_factories 
     * @brief Constructor 
     * 
     * @param sigma_embed_decomposition  Decomposition (list of block sizes) for Σ_embed: [α] = block size
     * @note The decompositions are given for α, γ index, and are independent of σ
     * @param imp_decompositions         List of decomposition of solvers: [n_imp][γ] = block size
     * @param psi                        The mapping ψ[α,σ] -> n_imp, γ, τ
     * @param sigma_names                Names for the values of the σ index (e.g., "up", "down")
     */
    embedding(std::vector<long> sigma_embed_decomposition, std::vector<std::vector<long>> imp_decompositions, nda::array<imp_block_t, 2> psi,
              std::vector<std::string> sigma_names);

    public:
    // --------------------------------------------------------
    bool operator==(embedding const &other) const = default;

    // --------------------------------------------------------

    /// Gf block structure for Σ_embed
    C2PY_IGNORE gf_struct2_t sigma_embed_block_shape() const;

    /// Gf block structure for the impurity solvers
    std::vector<gf_struct_t> imp_block_shape() const;

    // ****************** Operation methods to change the embedding **********************

    /**
    * @ingroup embedding_ops
    * @brief Remove an impurity from the embedding table ψ
    *
    * @details Remove the impurity imp_idx, maps the corresponding α blocks to -1 (i.e. no longer connected to an impurity)
    *          and relabel the other impurities with number > imp_idx.
    * 
    * @param imp_idx Index of the impurity to remove from the table ψ
    * @return New embedding with the updated ψ map.
    */
    embedding drop(long imp_idx) const;

    /**
    * @ingroup embedding_ops
    * @brief Replaces one impurity in the embedding table ψ.
    *
    * @details Replaces the impurity solver corresponding to imp_idx_to with the impurity solver at imp_idx_from 
    * and updates the underlying table ψ.
    * 
    * @param imp_idx_to the index of the impurity solver to be replaced.
    * @param imp_idx_from the index of the impurity solver that will replace imp_idx_to.
    * @return a new embedding with the updated ψ map.
    */
    embedding replace(long imp_idx_to_remove, long imp_idx_to_replace_with) const;

    /**
    * @ingroup embedding_ops
    * @brief Flip the spins (σ) for block α.  
    *
    * @details For block α transforms (σ,σ') -> (τ, τ') to (σ, σ') -> (τ', τ).
    * 
    * @param alpha the index of the alpha block to flip the spins.
    * @return a new embedding with the updated ψ map.
    */
    embedding flip_spin(long alpha) const;

    /**
    * @ingroup embedding_ops
    * @brief Flip the spins (σ) for all [α].
    *
    * @details For block α transforms (σ,σ') -> (τ, τ') to (σ, σ') -> (τ', τ).
    * 
    * @param alphas a list of alpha blocks to flip the spins.
    * @return New embedding with the updated ψ map.
    */
    embedding flip_spin(std::vector<long> alphas) const;

    /// Predicate p (long block_idx) -> 0 or 1.
    embedding split(long imp_idx, std::function<bool(long)> p) const;

    /**
    * @ingroup embedding_ops
    * @brief Split impurity imp_idx based on block_list
    *
    * @details For impurity imp_idx, split part of its blocks into a new impurity.
    * The blocks that were split off remain mapped to their original alpha blocks with the updated imp_idx.
    * 
    * @param imp_idx Impurity number
    * @param block_list A list of blocks of imp_idx to split off.
    * @return New embedding with the updated ψ map.
    */
    embedding split(long imp_idx, std::vector<long> const &block_list) const;

    embedding split(long imp_idx, std::initializer_list<const char *> x) = delete;

    //------------------------------------------------------------------------------------

    // ****************************************************
    //     Embedding Extract/Embed methods
    // ****************************************************

    //--------------------- Embed -----------------------------------------

    /// embed single-particle quantities
    template <typename Mesh> block2_gf<Mesh, matrix_valued> embed(std::vector<block_gf<Mesh, matrix_valued>> const &Sigma_imp_vec) const;

    /// embed two-particle quantities
    template <typename Mesh> block2_gf<Mesh, tensor_valued<4>> embed(std::vector<block_gf<Mesh, tensor_valued<4>>> const &Pi_imp_vec) const;

    /// embed block matrices
    nda::array<nda::matrix<dcomplex>, 2> embed(std::vector<std::vector<nda::matrix<dcomplex>>> const &Sigma_imp_static_vec) const;

    /// embed tensors
    // nda::array<dcomplex, 4> embed(std::vector<nda::array<dcomplex, 4>> const &U_tensor_vec) const;

    //--------------------- Extract ---------------------------------------

    /// extract single-particle quantities (ModEST)
    template <typename Mesh> std::vector<block_gf<Mesh, matrix_valued>> extract(block2_gf<Mesh, matrix_valued> const &g_loc) const;

    /// extract single-particle quantities (CoQui)
    template <typename Mesh> std::vector<block_gf<Mesh, matrix_valued>> extract(block_gf<Mesh, matrix_valued> const &g_loc) const;

    /// extract two-particle quantities (CoQui)
    template <typename Mesh> std::vector<block_gf<Mesh, tensor_valued<4>>> extract(block_gf<Mesh, tensor_valued<4>> const &Pi_loc) const;

    /// extract matrices
    std::vector<std::vector<nda::matrix<dcomplex>>> extract(nda::array<nda::matrix<dcomplex>, 2> const &matrix_C) const;

    /// embed tensors
    // std::vector<nda::array<dcomplex, 4>> extract(nda::array<dcomplex, 4> const &U_tensor) const;
  };

  // ***************************** Free functions/factories *******************************************

  /**
 * @ingroup embedding_factories 
 * @brief Construct the embedding class from the local space, a description of the block decomposition, and an equivalence 
 *        mapping between atom sites.
 * 
 * @param C_space the local space from one_body_elements
 * @param block_decomposition the decomposition of atomic orbitals into their irreducible representations
 * @param atom_to_imp [optional] a mapping between equivalent atom sites.
 * @return embedding 
 */
  embedding make_embedding(local_space const &C_space, nda::array<std::vector<long>, 2> const &block_decomposition,
                           std::optional<std::vector<long>> const &atom_to_imp = std::nullopt);

  /**
 * @ingroup embedding_factories 
 * @brief Construct the embedding class from the local space using equivalences between atoms.
 * 
 * @param C_space the local space from one_body_elements
 * @return embedding 
 */
  embedding make_embedding_with_equivalences(local_space const &C_space);

  /**
 * @ingroup embedding_factories 
 * @brief Construct the embedding class from the local space without using equivalences between atoms.
 * 
 * @param C_space the local space from one_body_elements.
 * @return embedding 
 */
  embedding make_embedding_with_no_equivalences(local_space const &C_space);

  // *****************************  Method implementation ***************************************

  /**
    * @ingroup embedding_methods
    * @brief Embed impurity solver self-energies into an embedded self-energy. 
    *
    *    \Sigma_{embed}^{ασ} = \Sigma_{imp}^{ψ(α,σ)} 
    *     ψ(α,σ) -> (n_imp, γ, τ) 
    * @param Sigma_imp_vec 
    * @return Σ_embed in C space
    */
  template <typename Mesh> block2_gf<Mesh, matrix_valued> embedding::embed(std::vector<block_gf<Mesh, matrix_valued>> const &Sigma_imp_vec) const {
    // Check that all meshes are the same for Sigma_imp_vec
    if (not all_equal(Sigma_imp_vec | stdv::transform([](auto &&x) -> decltype(auto) { return x[0].mesh(); })))
      throw std::runtime_error{"[embedding_desc::embed]: meshes of solvers are not all equal"};
    //
    auto const &mesh = Sigma_imp_vec[0][0].mesh();
    // build the result
    auto Sigma_embed = make_block2_gf(mesh, this->sigma_embed_block_shape());
    // auto Sigma_embed= nda::array<gf<Mesh, matrix_valued>,2>(this->psi.shape());
    for (auto &&[S, m] : zip(Sigma_embed, psi)) {
      if (m.imp_idx == -1) continue;
      S() = Sigma_imp_vec[m.imp_idx][m.gamma + n_gamma(m.imp_idx) * m.tau];
    }
    return Sigma_embed;
  }

  /**
    * @ingroup embedding_methods
    * @brief Embed impurity solver self-energies into an embedded self-energy. 
    *
    *    \Pi_{embed}^{ασ} = \Pi_{imp}^{ψ(α,σ)} 
    *     ψ(α,σ) -> (n_imp, γ, τ) 
    * @param Pi_imp_vec 
    * @return Π_embed in C space
    */
  template <typename Mesh> block2_gf<Mesh, tensor_valued<4>> embedding::embed(std::vector<block_gf<Mesh, tensor_valued<4>>> const &Pi_imp_vec) const {
    // Check that all meshes are the same for Sigma_imp_vec
    if (not all_equal(Pi_imp_vec | stdv::transform([](auto &&x) -> decltype(auto) { return x[0].mesh(); })))
      throw std::runtime_error{"[embedding_desc::embed]: meshes of solvers are not all equal"};

    auto const &mesh = Pi_imp_vec[0][0].mesh();
    // build the result
    auto Pi_embed = make_block2_gf(mesh, this->sigma_embed_block_shape());

    for (auto &&[Pi, m] : zip(Pi_embed, psi)) {
      if (m.imp_idx == -1) continue;
      Pi() = Pi_imp_vec[m.imp_idx][m.gamma + n_gamma(m.imp_idx) * m.tau];
    }
    return Pi_embed;
  }

  // ---------------------------------------------------------------

  /**
 * @ingroup embedding_methods
 * @brief  Extract the impurity Green's function
 * 
 * @tparam Mesh  triqs::mesh::{dlr_imfreq, imfreq}
 * @param g_loc  Block2Gf of gloc in MxM space
 * @return local impurity Green's function std::vector<block_gf<Mesh, matrix_valued>> 
 */
  template <typename Mesh> std::vector<block_gf<Mesh, matrix_valued>> embedding::extract(block2_gf<Mesh, matrix_valued> const &g) const {

    if (auto decomp = get_struct(g).dims(r_all, 0) | tl::to<std::vector>(); decomp != this->sigma_embed_decomp) {
      if (decomp.size() != 1) throw std::runtime_error{"extract: g should have decomp = sigma_embedding_decomp or [1]"};
      return extract(decomposition_view(g, this->sigma_embed_block_shape()));
    }

    // FIXME : check all meshes are the same
    auto imp_gf_stru_list = imp_block_shape();
    auto extract_one_imp  = [&](long n_imp) {
      auto gimp        = block_gf{g(0, 0).mesh(), imp_gf_stru_list[n_imp]};
      auto const &rpsi = reverse_psi[n_imp];
      for (auto [gamma, tau] : rpsi.indices()) {
        auto [alpha, sigma]                       = rpsi(gamma, tau)[0];
        gimp[gamma + n_gamma(n_imp) * tau].data() = g(alpha, sigma).data();
      }
      return gimp;
    };
    return range(n_impurities()) | stdv::transform(extract_one_imp) | tl::to<std::vector>();
  }

  /**
 * @ingroup embedding_methods
 * @brief  Extract the impurity Green's function
 * 
 * @tparam Mesh  triqs::mesh::{dlr_imfreq, imfreq}
 * @param g_loc  BlockGf of gloc in MxM space
 * @return local impurity Green's function std::vector<block_gf<Mesh, matrix_valued>> 
 */
  template <typename Mesh> std::vector<block_gf<Mesh, matrix_valued>> embedding::extract(block_gf<Mesh, matrix_valued> const &g) const {

    if (auto decomp = get_struct(g) | tl::to<std::vector>(); decomp != this->sigma_embed_decomp) {
      if (decomp.size() != 1) throw std::runtime_error{"extract: g should have decomp = sigma_embedding_decomp or [1]"};
      return extract(decomposition_view(g, this->sigma_embed_block_shape()));
    }
    // FIXME : check all meshes are the same
    auto imp_gf_stru_list = imp_block_shape();
    auto extract_one_imp  = [&](long n_imp) {
      auto gimp        = block_gf{g(0, 0).mesh(), imp_gf_stru_list[n_imp]};
      auto const &rpsi = reverse_psi[n_imp];
      for (auto [gamma, tau] : rpsi.indices()) {
        auto [alpha, sigma]                       = rpsi(gamma, tau)[0];
        gimp[gamma + n_gamma(n_imp) * tau].data() = g(alpha, sigma).data();
      }
      return gimp;
    };
    return range(n_impurities()) | stdv::transform(extract_one_imp) | tl::to<std::vector>();
  }

  /**
 * @ingroup embedding_methods
 * @brief  Extract the impurity Green's function
 * 
 * @tparam Mesh  triqs::mesh::{dlr_imfreq, imfreq}
 * @param g_loc  BlockGf of gloc in MxM space
 * @return local impurity Green's function std::vector<block_gf<Mesh, matrix_valued>> 
 */
  template <typename Mesh> std::vector<block_gf<Mesh, tensor_valued<4>>> embedding::extract(block_gf<Mesh, tensor_valued<4>> const &pi_loc) const {

    if (auto decomp = get_struct(pi_loc) | tl::to<std::vector>(); decomp != this->sigma_embed_decomp) {
      if (decomp.size() != 1) throw std::runtime_error{"extract: g should have decomp = sigma_embedding_decomp or [1]"};
      return extract(decomposition_view(pi_loc, this->sigma_embed_block_shape()));
    }
    // FIXME : check all meshes are the same
    auto imp_gf_stru_list = imp_block_shape();
    auto extract_one_imp  = [&](long n_imp) {
      auto pi_imp      = block_gf{pi_loc(0, 0).mesh(), imp_gf_stru_list[n_imp]};
      auto const &rpsi = reverse_psi[n_imp];
      for (auto [gamma, tau] : rpsi.indices()) {
        auto [alpha, sigma]                         = rpsi(gamma, tau)[0];
        pi_imp[gamma + n_gamma(n_imp) * tau].data() = pi_loc(alpha, sigma).data();
      }
      return pi_imp;
    };
    return range(n_impurities()) | stdv::transform(extract_one_imp) | tl::to<std::vector>();
  }

  // -----------------------------------------------------------------------
  inline std::pair<one_body_elements_on_grid, embedding> make_embedding_with_clusters(one_body_elements_on_grid obe,
                                                                                      std::vector<std::vector<long>> const &atom_partition) {
    auto new_obe = permute_local_space(atom_partition, obe);
    auto E       = make_embedding_with_no_equivalences(new_obe.C_space);
    return {new_obe, E};
  }

// ------------------------------------------------------------------------------

/** @cond DOXYGEN_SKIP_THIS */
#define INSTANTIATE(Mesh)                                                                                                                            \
  template block2_gf<Mesh, matrix_valued> embedding::embed(std::vector<block_gf<Mesh, matrix_valued>> const &) const;                                \
  template std::vector<block_gf<Mesh, matrix_valued>> embedding::extract(block2_gf<Mesh, matrix_valued> const &) const;
  INSTANTIATE(imfreq);
  INSTANTIATE(refreq);
  INSTANTIATE(dlr_imfreq);
#undef INSTANTIATE
  /** @endcond */

} // namespace triqs::modest
