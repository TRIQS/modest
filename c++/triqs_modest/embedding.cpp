// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "embedding.hpp"
#include "utils/gf_supp.hpp"
#include "utils/defs.hpp"
#include "utils/nda_pretty_printer.hpp"
#include <triqs/utility/streams.hpp>
#include <ranges>
//#include <fmt/ranges.h>
//#include <sstream>

namespace triqs::modest {
  embedding::embedding(std::vector<long> sigma_embed_decomposition, std::vector<std::vector<long>> imp_decompositions, nda::array<imp_block_t, 2> psi,
                       std::vector<std::string> sigma_names)
     : sigma_embed_decomp{std::move(sigma_embed_decomposition)},
       imp_decomps{std::move(imp_decompositions)},
       _sigma_names{std::move(sigma_names)},
       psi{std::move(psi)} {

    alpha_names = range(n_alpha()) | //
       stdv::transform([](auto i) { return std::to_string(i); }) | tl::to<std::vector>();

    // Compute reverse_psi
    reverse_psi = imp_decomps | stdv::transform([&](auto &v) { return nda::array<std::vector<std::array<long, 2>>, 2>(v.size(), n_sigma()); })
       | tl::to<std::vector>();

    for (auto [alpha, sigma] : this->psi.indices()) { // beware the psi (argument) has been moved. use 2 names ?
      auto const &y = this->psi(alpha, sigma);
      if (y.imp_idx == -1) continue;
      reverse_psi[y.imp_idx](y.gamma, y.tau).push_back(std::array{alpha, sigma});
    }

    // FIXME : MAKE ALL NECESSARY CHECKS.... since
  }

  embedding make_embedding(local_space const &C_space, nda::array<std::vector<long>, 2> const &block_decomposition,
                           std::optional<std::vector<long>> const &atom_to_imp) {

    long n_alpha = stdr::fold_left(block_decomposition(r_all, 0) | stdv::transform([](auto const &x) { return long(x.size()); }), 0, std::plus<>());

    auto embed_bl_sizes     = std::vector<long>{};
    auto imp_decompositions = std::vector<std::vector<long>>{};
    auto psi                = nda::array<embedding::imp_block_t, 2>(n_alpha, C_space.n_sigma());

    auto n_atoms = C_space.atoms_block_decomposition().shape()[0];

    auto const &atom_imp_map = (atom_to_imp) ? *atom_to_imp : range(n_atoms) | tl::to<std::vector>();

    for (auto atom : range(n_atoms)) {
      for (auto &&[idx, bl_size] : enumerate(block_decomposition(atom, 0))) {
        for (auto sigma : range(C_space.n_sigma())) { psi(embed_bl_sizes.size(), sigma) = {atom_imp_map[atom], idx, sigma}; }
        embed_bl_sizes.emplace_back(bl_size);
      }
      auto build_impurity = C_space.first_shell_of_its_equiv_cls(atom) == atom;
      if (build_impurity || !atom_to_imp) imp_decompositions.push_back(std::move(block_decomposition(atom, 0)));
    }
    return embedding{std::move(embed_bl_sizes), std::move(imp_decompositions), std::move(psi), C_space.sigma_names()};
  }

  // -------------------------------------------------------------------------------------------
  embedding make_embedding_with_equivalences(local_space const &C_space) {

    // cls_to_imp[alpha_to_cls[alpha]]
    std::unordered_map<long, long> atom_to_cls, cls_to_imp;
    long imp_idx = 0;
    for (auto const &[ish, sh] : enumerate(C_space.atomic_shells())) {
      atom_to_cls[ish] = sh.cls_idx;
      if (not cls_to_imp.contains(sh.cls_idx)) cls_to_imp[sh.cls_idx] = imp_idx++;
    }
    auto atom_to_imp_idx = range(C_space.atomic_shells().size()) | stdv::transform([&](auto const &atom) { return cls_to_imp[atom_to_cls[atom]]; })
       | tl::to<std::vector>();

    auto decomp = C_space.atoms_block_decomposition();

    return make_embedding(C_space, decomp, atom_to_imp_idx);
  };

  embedding make_embedding_with_no_equivalences(local_space const &C_space) { return make_embedding(C_space, C_space.atoms_block_decomposition()); }

  // ------------------------------ PRINTING -------------------------------------------------------------

  auto format_as(embedding::imp_block_t const &p) {
    //return fmt::format("({},{},{})", p.n_imp, p.gamma, p.tau);
    return fmt::format("(imp_idx = {}, γ = {}, τ = {})", p.imp_idx, p.gamma, p.tau);
  }

  // ---------------------------

  std::ostream &operator<<(std::ostream &out, embedding const &E) {
    auto sigma_embed_shape     = E.sigma_embed_block_shape();
    auto impurities_shape_list = E.imp_block_shape();

    out << "Embedding:\n";
    auto out1 = indented_ostream(out, 2); // same stream, but shifted by 2 spaces
    auto out2 = indented_ostream(out, 4);
    auto out3 = indented_ostream(out, 6);

    out1 << fmt::format("Spin index (σ/τ) names: {}\n\n", E.sigma_names());

    out1 << "Σ_embed block decomposition:\n";
    auto pr_vec = [](auto const &V) {
      return fmt::format("{}\n", fmt::join(V | stdv::transform([](auto x) { return fmt::format("{:>3}", x); }), " "));
    };
    out2 << "dim_α: " << pr_vec(E.sigma_embed_decomp);
    out2 << "    α: " << pr_vec(range(E.sigma_embed_decomp.size()));
    //out << fmt::format("  {}\n", enumerate(E.sigma_embed_decomp));

    out1 << "\nImpurities\n";
    out2 << "Block dimensions, dim_γ for all γ:\n";
    for (auto &&[n, dec] : enumerate(E.imp_decomps)) {
      auto head = fmt::format("[n_imp = {}]", n);
      out3 << fmt::format("{} dim_γ = {}", head, pr_vec(dec));
      out3 << fmt::format("{:>{}}     γ = {}", " ", head.size(), pr_vec(range(dec.size())));
    }
    out2 << "Gf Block structures for solvers as names, [dim]:\n";
    for (auto &&[n, ish] : enumerate(impurities_shape_list)) {
      auto formatted_vec = ish | stdv::transform([](auto &&p) { return fmt::format("{} [{}]", p.first, p.second); }) | tl::to<std::vector>();
      out3 << fmt::format("[imp_idx = {}] {}\n", n, fmt::join(formatted_vec, ", "));
    }

    out1 << "\nMapping ψ(α,σ) = (imp_idx, γ, τ) \n";

    //out2 << fmt::format("{}", E.psi);
    auto row_labels = range(E.n_alpha()) | stdv::transform([](auto x) { return fmt::format("α = {}", x); }) | tl::to<std::vector>();
    auto col_labels =
       range(E.n_sigma()) | stdv::transform([&](auto i) { return fmt::format("σ = {} / {}", i, E.sigma_names()[i]); }) | tl::to<std::vector>();
    nda::format_as_table(out3, E.psi, row_labels, col_labels);

    return out;
  }

  //-----------------------------------------------------------------

  gf_struct2_t embedding::sigma_embed_block_shape() const {
    auto res = nda::zeros<long>(n_alpha(), n_sigma());
    for (long alpha : range(n_alpha())) res(alpha, r_all) = sigma_embed_decomp[alpha];
    return {.names = {alpha_names, _sigma_names}, .dims = std::move(res)};
  }

  //-----------------------------------------------------------------

  std::vector<gf_struct_t> embedding::imp_block_shape() const {
    auto l = [this](std::vector<long> const &decomp) -> gf_struct_t {
      gf_struct_t res;
      for (auto sigma : range(n_sigma())) {
        for (auto const &[idx, bl_size] : enumerate(decomp)) res.emplace_back(fmt::format("{}_{}", _sigma_names[sigma], idx), bl_size);
      }
      return res;
    };
    return imp_decomps | stdv::transform(l) | tl::to<std::vector>();
  }

  //-----------------------------------------------------------------
  embedding embedding::flip_spin(long alpha) const {
    if (n_sigma() != 2) throw std::runtime_error(fmt::format("Can not flip spin when {} != 2", n_sigma()));
    auto new_psi = this->psi;
    for (auto sigma : range(n_sigma())) new_psi(alpha, sigma).tau = 1 - new_psi(alpha, sigma).tau;
    return {this->sigma_embed_decomp, this->imp_decomps, new_psi, this->_sigma_names};
  }

  //-----------------------------------------------------------------
  embedding embedding::flip_spin(std::vector<long> alphas) const {
    auto res = *this;
    for (auto alpha : alphas) { res = res.flip_spin(alpha); }
    return res;
  }

  //-----------------------------------------------------------------

  embedding embedding::drop(long imp_idx) const {
    auto new_imp_decomps = this->imp_decomps;
    auto new_psi         = this->psi;
    new_imp_decomps.erase(begin(new_imp_decomps) + imp_idx);
    new_psi = nda::map([&](imp_block_t const &b) -> imp_block_t {
      if (b.imp_idx < imp_idx) return {b.imp_idx, b.gamma, b.tau};
      if (b.imp_idx > imp_idx) return {b.imp_idx - 1, b.gamma, b.tau};
      return {-1, 0, 0}; // imp_idx == b.n_imp : we connect to nothing now
    })(new_psi);
    return {this->sigma_embed_decomp, new_imp_decomps, new_psi, this->_sigma_names};
  }

  //-----------------------------------------------------------------
  // HL: re-implementation of replace. Please review.
  embedding embedding::replace(long imp_idx_to_remove, long imp_idx_to_replace_with) const {
    auto new_psi = this->psi;
    if (imp_block_shape()[imp_idx_to_remove] != imp_block_shape()[imp_idx_to_replace_with])
      throw std::runtime_error(fmt::format("The number of blocks that imp_to_remove= {} and imp_to_replace_with={} connect to do not match!",
                                           imp_idx_to_remove, imp_idx_to_replace_with));
    new_psi = nda::map([&](imp_block_t const &b) -> imp_block_t {
      if (b.imp_idx == imp_idx_to_remove) return {imp_idx_to_replace_with, b.gamma, b.tau};
      return b;
    })(new_psi);
    return embedding(this->sigma_embed_decomp, this->imp_decomps, new_psi, this->_sigma_names).drop(imp_idx_to_remove);
  }

  //-----------------------------------------------------------------

  embedding embedding::split(long imp_idx, std::function<bool(long)> p) const {
    // auto res        = *this; // copy. We will modify the impurities_gf_struct and the table
    auto new_psi         = this->psi;
    auto new_imp_decomps = this->imp_decomps;
    auto const &igs      = new_imp_decomps[imp_idx];
    auto indices         = nda::range(long(igs.size())) | tl::to<std::vector>(); // {0,1,2, ... n_gamma}

    // separate the block indices according to p. We put the indices in regard to the blocks and partition them
    auto mid = std::stable_partition(begin(indices), end(indices), p);

    // make 2 new gf_struct from the indices partition
    auto get_igs = [&](int i) { return igs[i]; };
    auto stru1   = stdr::subrange{begin(indices), mid} | stdv::transform(get_igs) | tl::to<std::vector>();
    auto stru2   = stdr::subrange{mid, end(indices)} | stdv::transform(get_igs) | tl::to<std::vector>();
    long L1      = long(stru1.size());

    // Store the new gf_struct in result at the position of the old impurity model
    new_imp_decomps[imp_idx] = std::move(stru1);
    new_imp_decomps.insert(begin(new_imp_decomps) + imp_idx + 1, std::move(stru2));

    // compute the inverse table of indices
    auto indices_inv = indices;
    for (auto i : range(indices.size())) indices_inv[indices[i]] = i;

    new_psi = nda::map([&](imp_block_t const &b) -> imp_block_t {
      if (b.imp_idx < imp_idx) return b;
      if (b.imp_idx > imp_idx) return {b.imp_idx + 1, b.gamma, b.tau};
      long new_gamma = indices_inv[b.gamma];
      return (new_gamma < L1 ? imp_block_t{b.imp_idx, new_gamma, b.tau} : imp_block_t{b.imp_idx + 1, new_gamma - L1, b.tau});
    })(new_psi);
    return {this->sigma_embed_decomp, new_imp_decomps, new_psi, this->_sigma_names};
  }

  // ----------------------------------------------------------------------

  embedding embedding::split(long imp_idx, std::vector<long> const &block_list) const {
    if (not stdr::all_of(block_list, [&](auto i) { return (i >= 0) and (i < n_gamma(imp_idx)); }))
      throw std::runtime_error(fmt::format("[embedding::split] The list of block indices {} is incorrect. Indices i should all be 0<= i < N_γ = {}",
                                           block_list, n_gamma(imp_idx)));
    return split(imp_idx, [&](long idx) { return stdr::find(block_list, idx) != block_list.end(); });
  }

  // ----------------------------------------------------------------------

  nda::array<nda::matrix<dcomplex>, 2> embedding::embed(std::vector<std::vector<nda::matrix<dcomplex>>> const &Sigma_imp_static_vec) const {
    auto Sigma_static_embed = nda::array<nda::matrix<dcomplex>, 2>(n_alpha(), n_sigma());
    for (auto &&[S, m] : zip(Sigma_static_embed, psi)) {
      if (m.imp_idx == -1) continue;
      S = Sigma_imp_static_vec[m.imp_idx][m.gamma + n_gamma(m.imp_idx) * m.tau];
    }
    return Sigma_static_embed;
  }

  // ----------------------------------------------------------------------

  std::vector<std::vector<nda::matrix<dcomplex>>> embedding::extract(nda::array<nda::matrix<dcomplex>, 2> const &matrix_C) const {
    auto imp_gf_stru_list = imp_block_shape();

    auto [_, n_sigma] = matrix_C.shape();
    auto matrix_E     = nda::array<nda::matrix<dcomplex>, 2>{sigma_embed_decomp.size(), n_sigma};
    for (auto [alpha, Ralpha] : enumerated_sub_slices(sigma_embed_decomp)) {
      for (auto sigma : range(n_sigma)) { matrix_E(alpha, sigma) = matrix_C(0, sigma)(Ralpha, Ralpha); }
    }
    auto extract_one_imp = [&](long n_imp) {
      auto h_imp = std::vector<nda::matrix<dcomplex>>{};
      for (auto [bl, bl_size] : imp_gf_stru_list[n_imp]) { h_imp.emplace_back(bl_size, bl_size); }
      auto const &rpsi = reverse_psi[n_imp];
      for (auto [gamma, tau] : rpsi.indices()) {
        auto [alpha, sigma]                 = rpsi(gamma, tau)[0];
        h_imp[gamma + n_gamma(n_imp) * tau] = matrix_E(alpha, sigma);
      }
      return h_imp;
    };
    return range(n_impurities()) | stdv::transform(extract_one_imp) | tl::to<std::vector>();
  }

  // std::pair<one_body_elements_on_grid, embedding> make_embedding_with_clusters(one_body_elements_on_grid obe,
  //                                                                              std::vector<std::vector<long>> const &atom_partition) {
  //   // build permutation based on atom_partition
  // }

} // namespace triqs::modest
