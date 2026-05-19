// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "embedding.hpp"
#include "utils/gf_supp.hpp"
#include "utils/defs.hpp"
#include "utils/nda_pretty_printer.hpp"
#include <triqs/utility/report_stream.hpp>
#include <ranges>
#include <set>
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

  embedding embedding_builder(std::vector<std::string> const &spin_names, nda::array<std::vector<long>, 2> const &block_decomposition,
                              std::vector<long> const &atom_to_imp) {

    long n_alpha = stdr::fold_left(block_decomposition(r_all, 0) | stdv::transform([](auto const &x) { return long(x.size()); }), 0, std::plus<>());
    long n_sigma = spin_names.size();

    auto embed_bl_sizes     = std::vector<long>{};
    auto imp_decompositions = std::vector<std::vector<long>>{};
    auto psi                = nda::array<embedding::imp_block_t, 2>(n_alpha, n_sigma);

    std::set<long> seen_impurities;

    auto n_atoms = block_decomposition.extent(0);
    for (auto atom : range(n_atoms)) {
      for (auto &&[idx, bl_size] : enumerate(block_decomposition(atom, 0))) {
        for (auto sigma : range(n_sigma)) { psi(embed_bl_sizes.size(), sigma) = {atom_to_imp[atom], idx, sigma}; }
        embed_bl_sizes.emplace_back(bl_size);
      }
      auto build_impurity = !seen_impurities.contains(atom_to_imp[atom]);
      if (build_impurity) {
        imp_decompositions.push_back(std::move(block_decomposition(atom, 0)));
        seen_impurities.insert(atom_to_imp[atom]);
      }
    }
    return embedding{std::move(embed_bl_sizes), std::move(imp_decompositions), std::move(psi), std::move(spin_names)};
  }

  embedding embedding_builder(std::vector<std::string> const &spin_names, std::vector<std::vector<long>> const &block_decomposition,
                              std::vector<long> const &atom_to_imp) {
    auto n_decomps        = block_decomposition.size();
    auto n_sigma          = spin_names.size();
    auto block_decomp_mat = nda::array<std::vector<long>, 2>(n_decomps, n_sigma);
    for (auto d : range(n_decomps))
      for (auto s : range(n_sigma)) block_decomp_mat(d, s) = block_decomposition[d];
    return embedding_builder(spin_names, block_decomp_mat, atom_to_imp);
  }

  // -------------------------------------------------------------------------------------------
  embedding make_embedding_with_equivalences(local_space const &C_space, bool use_atom_decomp) {
    std::unordered_map<long, long> atom_to_cls, cls_to_imp;
    long imp_idx = 0;
    for (auto const &[ish, sh] : enumerate(C_space.atomic_shells())) {
      atom_to_cls[ish] = sh.cls_idx;
      if (not cls_to_imp.contains(sh.cls_idx)) cls_to_imp[sh.cls_idx] = imp_idx++;
    }
    auto atom_to_imp_idx = range(C_space.atomic_shells().size()) | stdv::transform([&](auto const &atom) { return cls_to_imp[atom_to_cls[atom]]; })
       | tl::to<std::vector>();

    nda::array<std::vector<long>, 2> decomp(C_space.n_atoms(), C_space.n_sigma());
    if (use_atom_decomp) {
      auto atom_decomp = C_space.atomic_decomposition();
      for (auto &&[iatom, atom] : enumerate(atom_decomp)) {
        for (auto sigma : range(C_space.n_sigma())) { decomp(atom, sigma).emplace_back(atom); }
      }
    } else {
      decomp = C_space.atoms_block_decomposition();
    }
    return embedding_builder(C_space.sigma_names(), decomp, atom_to_imp_idx);
  };

  // -------------------------------------------------------------------------------------------
  embedding make_embedding_with_no_equivalences(local_space const &C_space, bool use_atom_decomp) {
    nda::array<std::vector<long>, 2> decomp(C_space.n_atoms(), C_space.n_sigma());
    if (use_atom_decomp) {
      auto atom_decomp = C_space.atomic_decomposition();
      for (auto &&[iatom, atom] : enumerate(atom_decomp)) {
        for (auto sigma : range(C_space.n_sigma())) { decomp(atom, sigma).emplace_back(atom); }
      }
    } else {
      decomp = C_space.atoms_block_decomposition();
    }

    auto atom_to_imp_idx = range(C_space.atomic_shells().size()) | tl::to<std::vector>();
    return embedding_builder(C_space.sigma_names(), decomp, atom_to_imp_idx);
  }

  // -------------------------------------------------------------------------------------------
  embedding make_embedding(local_space const &C_space, bool use_atom_equivalences, bool use_atom_decomp) {
    return (use_atom_equivalences) ? make_embedding_with_equivalences(C_space, use_atom_decomp) :
                                     make_embedding_with_no_equivalences(C_space, use_atom_decomp);
  }

  // ------------------------------ PRINTING -------------------------------------------------------------

  auto format_as(embedding::imp_block_t const &p) {
    //return fmt::format("({},{},{})", p.n_imp, p.gamma, p.tau);
    return fmt::format("(imp_idx = {}, γ = {}, τ = {})", p.imp_idx, p.gamma, p.tau);
  }

  // -------------------------------------------------------------------------------------------------------

  std::string embedding::description(bool verbosity) const {
    auto sigma_embed_shape     = this->sigma_embed_block_shape();
    auto impurities_shape_list = this->imp_block_shape();

    std::ostringstream out;
    auto out1 = triqs::utility::indented_ostream(out, 2); // same stream, but shifted by 2 spaces
    auto out2 = triqs::utility::indented_ostream(out, 4);
    auto out3 = triqs::utility::indented_ostream(out, 6);

    if (!verbosity) {
      out << "Embedding: ";
      out << fmt::format("{} impurities\n", this->n_impurities());
      out1 << "Σ_embed block decomposition:\n";
      auto pr_vec = [](auto const &V) {
        return fmt::format("{}\n", fmt::join(V | stdv::transform([](auto x) { return fmt::format("{:>3}", x); }), " "));
      };
      out2 << "dim_α: " << pr_vec(this->sigma_embed_decomp);
      out2 << "    α: " << pr_vec(range(this->sigma_embed_decomp.size()));
      out1 << "\nImpurities\n";
      out2 << "Block dimensions, dim_γ for all γ:\n";
      for (auto &&[n, dec] : enumerate(this->imp_decomps)) {
        auto head = fmt::format("[n_imp = {}]", n);
        out3 << fmt::format("{} dim_γ = {}", head, pr_vec(dec));
        out3 << fmt::format("{:>{}}     γ = {}", " ", head.size(), pr_vec(range(dec.size())));
      }
      return out.str();
    }

    out << "Embedding:\n";
    out1 << fmt::format("Spin index (σ/τ) names: {}\n\n", this->sigma_names());
    out1 << "Σ_embed block decomposition:\n";
    auto pr_vec = [](auto const &V) {
      return fmt::format("{}\n", fmt::join(V | stdv::transform([](auto x) { return fmt::format("{:>3}", x); }), " "));
    };
    out2 << "dim_α: " << pr_vec(this->sigma_embed_decomp);
    out2 << "    α: " << pr_vec(range(this->sigma_embed_decomp.size()));
    //out << fmt::format("  {}\n", enumerate(E.sigma_embed_decomp));
    out1 << "\nImpurities\n";
    out2 << "Block dimensions, dim_γ for all γ:\n";
    for (auto &&[n, dec] : enumerate(this->imp_decomps)) {
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
    auto row_labels = range(this->n_alpha()) | stdv::transform([](auto x) { return fmt::format("α = {}", x); }) | tl::to<std::vector>();
    auto col_labels = range(this->n_sigma()) | stdv::transform([&](auto i) { return fmt::format("σ = {} / {}", i, this->sigma_names()[i]); })
       | tl::to<std::vector>();
    nda::format_as_table(out3, this->psi, row_labels, col_labels);

    return out.str();
  }

  std::ostream &operator<<(std::ostream &out, embedding const &E) {
    out << E.description(false);
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
    if (alpha >= n_alpha()) throw std::runtime_error(fmt::format("Out of bounds {} >= {}", alpha, n_alpha()));
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

  // ----------------------- Embedding and Extracting --------------------------

  // T = Block Matrix

  // ----------------------------------------------------------------------
  block2_matrix_t embedding::embed(std::vector<block_matrix_t> const &Sigma_imp_static_vec) const {
    auto Sigma_static_embed = nda::array<nda::matrix<dcomplex>, 2>(n_alpha(), n_sigma());
    for (auto &&[alpha, sigma] : psi.indices()) {
      auto bl_size                     = sigma_embed_decomp[alpha];
      Sigma_static_embed(alpha, sigma) = nda::zeros<dcomplex>(bl_size, bl_size);
    }
    for (auto &&[S, m] : zip(Sigma_static_embed, psi)) {
      if (m.imp_idx == -1) continue;
      S = Sigma_imp_static_vec[m.imp_idx][m.gamma + n_gamma(m.imp_idx) * m.tau];
    }
    return Sigma_static_embed;
  }

  // TODO: helper functions to scatter and gather to avoid code duplication:
  // A common pattern is to scatter/ gather from/to a large matrix to/from smaller matrices
  block_matrix_t embedding::embed_ij(std::vector<block_matrix_t> const &Sigma_imp_static_vec) const {
    auto Sigma_static_embed = this->embed(Sigma_imp_static_vec);
    auto dim_C              = stdr::fold_left(sigma_embed_decomp, 0, std::plus<>());
    block_matrix_t Sigma_static_embed_ij;
    for (auto const &sigma : range(n_sigma())) {
      auto mat = nda::zeros<dcomplex>(dim_C, dim_C);
      for (auto &&[alpha, r_alpha] : enumerated_sub_slices(sigma_embed_decomp)) { mat(r_alpha, r_alpha) = Sigma_static_embed(alpha, sigma); }
      Sigma_static_embed_ij.push_back(mat);
    }
    return Sigma_static_embed_ij;
  }

  // ----------------------------------------------------------------------
  std::vector<block_matrix_t> embedding::extract(block2_matrix_t const &matrix_C) const {

    auto imp_gf_stru_list = imp_block_shape();

    auto matrix_E = nda::matrix<nda::matrix<dcomplex>>(n_alpha(), n_sigma());
    for (auto [alpha, r_alpha] : enumerated_sub_slices(sigma_embed_decomp)) {
      for (auto sigma : range(n_sigma())) { matrix_E(alpha, sigma) = matrix_C(0, sigma)(r_alpha, r_alpha); }
    }

    auto extract_one_imp = [&](long n_imp) {
      auto matrix_imp = std::vector<nda::matrix<dcomplex>>{};
      for (auto [bl, bl_size] : imp_gf_stru_list[n_imp]) { matrix_imp.emplace_back(bl_size, bl_size); }
      auto const &rpsi = reverse_psi[n_imp];
      for (auto [gamma, tau] : rpsi.indices()) {
        auto [alpha, sigma]                      = rpsi(gamma, tau)[0];
        matrix_imp[gamma + n_gamma(n_imp) * tau] = matrix_E(alpha, sigma);
      }
      return matrix_imp;
    };

    return range(n_impurities()) | stdv::transform(extract_one_imp) | tl::to<std::vector>();
  }

  // T = Tensors

  // ----------------------------------------------------------------------
  std::vector<nda::array<dcomplex, 4>> embedding::extract_ijkl(nda::array<dcomplex, 4> const &U_tensor) const {

    auto imp_gf_stru_list = imp_block_shape();

    auto tensor_E = std::vector<nda::array<dcomplex, 4>>(n_alpha());
    for (auto [alpha, r_alpha] : enumerated_sub_slices(sigma_embed_decomp)) { tensor_E[alpha] = U_tensor(r_alpha, r_alpha, r_alpha, r_alpha); }

    auto extract_one_imp = [&](long n_imp) {
      auto bl_size    = imp_decomposition(n_imp)[0];
      auto tensor_imp = nda::zeros<dcomplex>(bl_size, bl_size, bl_size, bl_size);
      auto alpha      = reverse_psi[n_imp](0, 0)[0][0];
      return tensor_E[alpha];
    };
    return range(n_impurities()) | stdv::transform(extract_one_imp) | tl::to<std::vector>();
  }

  // --------------------------------------------------------------------------------------------
  nda::array<dcomplex, 4> embedding::embed_ijkl(std::vector<nda::array<dcomplex, 4>> const &U_tensor_vec) const {
    auto Utensor_E = std::vector<nda::array<dcomplex, 4>>(n_alpha());

    for (auto alpha : range(n_alpha())) {
      auto bl_size     = sigma_embed_decomp[alpha];
      Utensor_E[alpha] = nda::zeros<dcomplex>(bl_size, bl_size, bl_size, bl_size);
    }

    for (auto &&[U, a] : zip(Utensor_E, range(n_alpha()))) {
      if (psi(a, 0).imp_idx == -1) continue;
      U = U_tensor_vec[psi(a, 0).imp_idx];
    }

    auto dim_C      = stdr::fold_left(sigma_embed_decomp, 0, std::plus<>());
    auto U_tensor_C = nda::array<dcomplex, 4>(dim_C, dim_C, dim_C, dim_C);
    for (auto &&[index, sli] : enumerated_sub_slices(sigma_embed_decomp)) { U_tensor_C(sli, sli, sli, sli) = Utensor_E[index]; }
    return U_tensor_C;
  }

  // T = Block Green's functions
  // --------------------------------------------------------------------------------------------
  std::vector<std::vector<nda::array<dcomplex, 3>>> embedding::extract_wij(nda::array<dcomplex, 4> const &g_loc) const {

    auto imp_gf_stru_list = imp_block_shape();
    auto n_w              = g_loc.extent(0);

    auto gloc_E = nda::array<nda::array<dcomplex, 3>, 2>(n_alpha(), n_sigma());
    for (auto [alpha, r_alpha] : enumerated_sub_slices(sigma_embed_decomp)) {
      for (auto sigma : range(n_sigma())) { gloc_E(alpha, sigma) = g_loc(r_all, sigma, r_alpha, r_alpha); }
    }
    auto extract_one_imp = [&](long n_imp) {
      auto g_imp = std::vector<nda::array<dcomplex, 3>>{};
      for (auto [bl, bl_size] : imp_gf_stru_list[n_imp]) { g_imp.emplace_back(n_w, bl_size, bl_size); }
      auto const &rpsi = reverse_psi[n_imp];
      for (auto [gamma, tau] : rpsi.indices()) {
        auto [alpha, sigma]                 = rpsi(gamma, tau)[0];
        g_imp[gamma + n_gamma(n_imp) * tau] = gloc_E(alpha, sigma);
      }
      return g_imp;
    };

    return range(n_impurities()) | stdv::transform(extract_one_imp) | tl::to<std::vector>();
  }

  // --------------------------------------------------------------------------------------------
  std::vector<nda::array<dcomplex, 3>> embedding::embed_wij(std::vector<std::vector<nda::array<dcomplex, 3>>> const &Sigma_imp_vec) const {

    auto Sigma_embed = nda::array<nda::array<dcomplex, 3>, 2>(n_alpha(), n_sigma());
    auto n_w         = Sigma_imp_vec[0][0].extent(0);

    for (auto &&[alpha, sigma] : psi.indices()) {
      auto bl_size              = sigma_embed_decomp[alpha];
      Sigma_embed(alpha, sigma) = nda::zeros<dcomplex>(n_w, bl_size, bl_size);
    }

    for (auto &&[S, m] : zip(Sigma_embed, psi)) {
      if (m.imp_idx == -1) continue;
      S = Sigma_imp_vec[m.imp_idx][m.gamma + n_gamma(m.imp_idx) * m.tau];
    }

    auto dim_C = stdr::fold_left(sigma_embed_decomp, 0, std::plus<>());
    auto Sigma_C =
       range(n_sigma()) | stdv::transform([n_w, dim_C](auto) { return nda::array<dcomplex, 3>(n_w, dim_C, dim_C); }) | tl::to<std::vector>();

    for (auto sigma : range(n_sigma())) {
      for (auto &&[index, sli] : enumerated_sub_slices(sigma_embed_decomp)) { Sigma_C[sigma](r_all, sli, sli) = Sigma_embed(index, sigma); }
    }

    return Sigma_C;
  }

  // --------------------------------------------------------------------------------------------
  //FIXME : protect this function as it will only work with a Pi_embed_decomp
  std::vector<nda::array<dcomplex, 5>> embedding::extract_wijkl(nda::array<dcomplex, 5> const &pi_loc) const {

    auto Pi_E = std::vector<nda::array<dcomplex, 5>>{};
    for (auto [alpha, r_alpha] : enumerated_sub_slices(sigma_embed_decomp)) { Pi_E.emplace_back(pi_loc(r_all, r_alpha, r_alpha, r_alpha, r_alpha)); }

    auto extract_one_imp = [&](long n_imp) {
      auto const &rpsi    = reverse_psi[n_imp];
      auto [alpha, sigma] = rpsi(0, 0)[0];
      return Pi_E[alpha];
    };

    return range(n_impurities()) | stdv::transform(extract_one_imp) | tl::to<std::vector>();
  }
  // --------------------------------------------------------------------------------------------

  //FIXME : protect this function as it will only work with a Pi_embed_decomp
  nda::array<dcomplex, 5> embedding::embed_wijkl(std::vector<nda::array<dcomplex, 5>> const &pi_imp_vec) const {

    auto n_w = pi_imp_vec[0].extent(0);

    auto Pi_embed = std::vector<nda::array<dcomplex, 5>>(n_alpha());
    for (auto alpha : range(n_alpha())) {
      auto bl_size    = sigma_embed_decomp[alpha];
      Pi_embed[alpha] = nda::zeros<dcomplex>(n_w, bl_size, bl_size, bl_size, bl_size);
    }

    for (auto alpha : range(n_alpha())) {
      auto const &m = psi(alpha, 0);
      if (m.imp_idx == -1) continue; // check
      Pi_embed[alpha] = pi_imp_vec[m.imp_idx];
    }

    auto dim_C = stdr::fold_left(sigma_embed_decomp, 0, std::plus<>());
    auto Pi_C  = nda::array<dcomplex, 5>(n_w, dim_C, dim_C, dim_C, dim_C);
    for (auto &&[alpha, sli] : enumerated_sub_slices(sigma_embed_decomp)) { Pi_C(r_all, sli, sli, sli, sli) = Pi_embed[alpha]; }

    return Pi_C;
  }

  //-------------------------------------------------------------------------
  std::vector<nda::array<dcomplex, 3>> rotate_embedded_self_energy(std::vector<nda::array<dcomplex, 3>> const &Sigma_embed_dynamic_loc,
                                                                   nda::matrix<dcomplex> const &U) {
    auto n_sigma                     = Sigma_embed_dynamic_loc.size();
    auto n_w                         = Sigma_embed_dynamic_loc[0].extent(0);
    auto Sigma_embed_dynamic_rotated = range(Sigma_embed_dynamic_loc.size())
       | stdv::transform([&](auto) { return nda::zeros<dcomplex>(Sigma_embed_dynamic_loc[0].shape()); }) | tl::to<std::vector>();
    for (auto const &sigma : range(n_sigma))
      for (auto const &w : range(n_w))
        Sigma_embed_dynamic_rotated[sigma](w, r_all, r_all) = U * nda::matrix<dcomplex>{Sigma_embed_dynamic_loc[sigma](w, r_all, r_all)} * dagger(U);
    return Sigma_embed_dynamic_rotated;
  }

  //-------------------------------------------------------------------------
  std::vector<nda::array<dcomplex, 3>> rotate_embedded_self_energy(std::vector<nda::array<dcomplex, 3>> const &Sigma_embed_dynamic_loc,
                                                                   one_body_elements_on_grid const &obe) {
    auto const &U = obe.C_space.rotation_from_dft_to_local_basis()(r_all, 0) | tl::to<std::vector<nda::matrix<dcomplex>>>();
    return rotate_embedded_self_energy(Sigma_embed_dynamic_loc, nda::block_diag(U));
  }

  //-------------------------------------------------------------------------
  std::vector<nda::matrix<dcomplex>> rotate_embedded_self_energy(std::vector<nda::matrix<dcomplex>> const &Sigma_embed_static_loc,
                                                                 nda::matrix<dcomplex> const &U) {
    return range(Sigma_embed_static_loc.size()) | stdv::transform([&](auto sigma) { return U * Sigma_embed_static_loc[sigma] * dagger(U); })
       | tl::to<std::vector>();
  }

  //-------------------------------------------------------------------------
  std::vector<nda::matrix<dcomplex>> rotate_embedded_self_energy(std::vector<nda::matrix<dcomplex>> const &Sigma_embed_static_loc,
                                                                 one_body_elements_on_grid const &obe) {
    auto const &U = obe.C_space.rotation_from_dft_to_local_basis()(r_all, 0) | tl::to<std::vector<nda::matrix<dcomplex>>>();
    return rotate_embedded_self_energy(Sigma_embed_static_loc, nda::block_diag(U));
  }

} // namespace triqs::modest