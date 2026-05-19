// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "embedding.hpp"
#include "utils/gf_supp.hpp"
#include "utils/defs.hpp"
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

  embedding make_embedding(std::vector<std::string> const &spin_names, nda::array<std::vector<long>, 2> const &block_decomposition,
                           std::vector<long> const &atom_to_imp) {

    // Validate inputs
    if (spin_names.empty()) throw std::runtime_error("[make_embedding] spin_names must not be empty");

    auto n_atoms = block_decomposition.extent(0);
    if (long(atom_to_imp.size()) != n_atoms)
      throw std::runtime_error(
         fmt::format("[make_embedding] block_decomposition has {} atoms but atom_to_imp has {} entries", n_atoms, atom_to_imp.size()));

    for (long atom = 0; atom < n_atoms; ++atom) {
      if (atom_to_imp[atom] < 0)
        throw std::runtime_error(fmt::format("[make_embedding] atom_to_imp[{}] = {} is negative", atom, atom_to_imp[atom]));
    }

    // Check that equivalent atoms have identical block decompositions
    std::unordered_map<long, long> imp_to_first_atom;
    for (long atom = 0; atom < n_atoms; ++atom) {
      auto imp = atom_to_imp[atom];
      if (auto it = imp_to_first_atom.find(imp); it != imp_to_first_atom.end()) {
        auto first_atom = it->second;
        if (block_decomposition(atom, 0) != block_decomposition(first_atom, 0))
          throw std::runtime_error(fmt::format(
             "[make_embedding] Atoms {} and {} are mapped to the same impurity {} but have different block decompositions", first_atom, atom, imp));
      } else {
        imp_to_first_atom[imp] = atom;
      }
    }

    long n_alpha = stdr::fold_left(block_decomposition(r_all, 0) | stdv::transform([](auto const &x) { return long(x.size()); }), 0, std::plus<>());
    long n_sigma = spin_names.size();

    auto embed_bl_sizes     = std::vector<long>{};
    auto imp_decompositions = std::vector<std::vector<long>>{};
    auto psi                = nda::array<embedding::imp_block_t, 2>(n_alpha, n_sigma);

    std::set<long> seen_impurities;

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

  embedding make_embedding(std::vector<std::string> const &spin_names, std::vector<std::vector<long>> const &block_decomposition,
                           std::vector<long> const &atom_to_imp) {
    auto n_decomps        = block_decomposition.size();
    auto n_sigma          = spin_names.size();
    auto block_decomp_mat = nda::array<std::vector<long>, 2>(n_decomps, n_sigma);
    for (auto d : range(n_decomps))
      for (auto s : range(n_sigma)) block_decomp_mat(d, s) = block_decomposition[d];
    return make_embedding(spin_names, block_decomp_mat, atom_to_imp);
  }

  // -------------------------------------------------------------------------------------------
  embedding make_embedding_with_equivalences(local_space const &C_space, bool use_atom_decomp) {
    std::unordered_map<long, long> atom_to_cls, cls_to_imp;
    long imp_idx = 0;
    for (auto const &[ish, sh] : enumerate(C_space.atomic_shells())) {
      atom_to_cls[ish] = sh.cls_idx;
      if (not cls_to_imp.contains(sh.cls_idx)) cls_to_imp[sh.cls_idx] = imp_idx++;
    }

    nda::array<std::vector<long>, 2> decomp(C_space.n_atoms(), C_space.n_sigma());
    if (use_atom_decomp) {
      auto atom_decomp = C_space.atomic_decomposition() | tl::to<std::vector>();
      for (auto &&[iatom, atom] : enumerate(atom_decomp)) {
        for (auto sigma : range(C_space.n_sigma())) { decomp(iatom, sigma).emplace_back(atom); }
      }
    } else {
      decomp = C_space.atoms_block_decomposition();
    }

    auto n_atoms_in_decomp = decomp.extent(0);
    auto atom_to_imp_idx   = range(n_atoms_in_decomp) | stdv::transform([&](auto const &atom) { return cls_to_imp[atom_to_cls[atom]]; })
       | tl::to<std::vector>();
    return make_embedding(C_space.sigma_names(), decomp, atom_to_imp_idx);
  };

  // -------------------------------------------------------------------------------------------
  embedding make_embedding_with_no_equivalences(local_space const &C_space, bool use_atom_decomp) {
    nda::array<std::vector<long>, 2> decomp(C_space.n_atoms(), C_space.n_sigma());
    if (use_atom_decomp) {
      auto atom_decomp = C_space.atomic_decomposition() | tl::to<std::vector>();
      for (auto &&[iatom, atom] : enumerate(atom_decomp)) {
        for (auto sigma : range(C_space.n_sigma())) { decomp(iatom, sigma).emplace_back(atom); }
      }
    } else {
      decomp = C_space.atoms_block_decomposition();
    }

    auto n_atoms_in_decomp = decomp.extent(0);
    auto atom_to_imp_idx   = range(n_atoms_in_decomp) | tl::to<std::vector>();
    return make_embedding(C_space.sigma_names(), decomp, atom_to_imp_idx);
  }

  // -------------------------------------------------------------------------------------------
  embedding make_embedding(local_space const &C_space, bool use_atom_equivalences, bool use_atom_decomp) {
    return (use_atom_equivalences) ? make_embedding_with_equivalences(C_space, use_atom_decomp) :
                                     make_embedding_with_no_equivalences(C_space, use_atom_decomp);
  }

  //-----------------------------------------------------------------

  gf_struct2_t embedding::embed_block_structure() const {
    auto res = nda::zeros<long>(n_alpha(), n_sigma());
    for (long alpha : range(n_alpha())) res(alpha, r_all) = sigma_embed_decomp[alpha];
    return {.names = {alpha_names, _sigma_names}, .dims = std::move(res)};
  }

  //-----------------------------------------------------------------

  std::vector<gf_struct_t> embedding::imp_block_structure() const {
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
  embedding embedding::swap_sigma(long alpha) const {
    if (n_sigma() != 2) throw std::runtime_error(fmt::format("Can not swap sigma when {} != 2", n_sigma()));
    if (alpha >= n_alpha()) throw std::runtime_error(fmt::format("Out of bounds {} >= {}", alpha, n_alpha()));
    auto new_psi = this->psi;
    for (auto sigma : range(n_sigma())) new_psi(alpha, sigma).tau = 1 - new_psi(alpha, sigma).tau;
    return {this->sigma_embed_decomp, this->imp_decomps, new_psi, this->_sigma_names};
  }

  //-----------------------------------------------------------------
  embedding embedding::swap_sigma(std::vector<long> alphas) const {
    auto res = *this;
    for (auto alpha : alphas) { res = res.swap_sigma(alpha); }
    return res;
  }

  //------------------------------------------------------------------

  embedding embedding::slice_sigma() const {
    if (n_sigma() != 2) throw std::runtime_error(fmt::format("Can not slice sigma when {} != 2", n_sigma()));
    auto new_psi         = nda::array<imp_block_t, 2>(n_alpha(), 1);
    auto new_sigma_names = std::vector<std::string>{"ud"};
    new_psi(r_all, 0)    = this->psi(r_all, 0);
    return {this->sigma_embed_decomp, this->imp_decomps, std::move(new_psi), new_sigma_names};
  }

  //-----------------------------------------------------------------

  embedding embedding::merge_embed_block_by_imp() const {

    // Group consecutive α blocks that belong to the same atom.
    // Within one atom the γ values are strictly increasing (0, 1, 2, …).
    // When γ resets or imp_idx changes we start a new group.
    // Disconnected blocks (imp_idx == -1) are never merged.

    struct group_t {
      std::vector<long> alphas; // α indices in this group
      long imp_idx;             // impurity (or -1)
    };

    std::vector<group_t> groups;

    for (long alpha = 0; alpha < n_alpha(); ++alpha) {
      auto const &m = psi(alpha, 0); // reference spin channel for grouping

      bool start_new_group = true;
      if (m.imp_idx >= 0 && !groups.empty()) {
        auto &prev         = groups.back();
        auto const &prev_m = psi(prev.alphas.back(), 0);
        if (prev.imp_idx == m.imp_idx && m.gamma > prev_m.gamma) { start_new_group = false; }
      }

      if (start_new_group) {
        groups.push_back({{alpha}, m.imp_idx});
      } else {
        groups.back().alphas.push_back(alpha);
      }
    }

    // Build new sigma_embed_decomp: one entry per group, sized as the sum of members.
    auto new_sigma_embed_decomp = std::vector<long>{};
    for (auto const &g : groups) {
      long total = 0;
      for (auto a : g.alphas) total += sigma_embed_decomp[a];
      new_sigma_embed_decomp.push_back(total);
    }

    // Build new imp_decomps: each impurity gets a single merged block.
    // Equivalent atoms share the same solver, so only the first group
    // for each impurity defines the block size.
    auto new_imp_decomps = std::vector<std::vector<long>>(n_impurities());
    auto imp_seen        = std::vector<bool>(n_impurities(), false);
    for (long g_idx = 0; g_idx < long(groups.size()); ++g_idx) {
      auto const &g = groups[g_idx];
      if (g.imp_idx < 0) continue;
      if (!imp_seen[g.imp_idx]) {
        new_imp_decomps[g.imp_idx].push_back(new_sigma_embed_decomp[g_idx]);
        imp_seen[g.imp_idx] = true;
      }
    }

    // Build new psi: one row per group.
    // All groups for the same impurity map to γ=0 (single block per impurity).
    auto new_psi = nda::array<imp_block_t, 2>(long(groups.size()), n_sigma());

    for (long g_idx = 0; g_idx < long(groups.size()); ++g_idx) {
      auto const &g = groups[g_idx];
      if (g.imp_idx < 0) {
        for (auto sigma : range(n_sigma())) new_psi(g_idx, sigma) = {-1, 0, 0};
      } else {
        for (auto sigma : range(n_sigma())) {
          auto tau              = psi(g.alphas[0], sigma).tau;
          new_psi(g_idx, sigma) = {g.imp_idx, 0, tau};
        }
      }
    }

    return {std::move(new_sigma_embed_decomp), std::move(new_imp_decomps), std::move(new_psi), this->_sigma_names};
  }

  //-----------------------------------------------------------------

  embedding embedding::drop_imp(long imp_idx) const {
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
  embedding embedding::replace_imp(long imp_idx_old, long imp_idx_new) const {
    auto new_psi = this->psi;
    if (imp_block_structure()[imp_idx_old] != imp_block_structure()[imp_idx_new])
      throw std::runtime_error(fmt::format("Block structures of imp_idx_old={} and imp_idx_new={} do not match; cannot redirect.",
                                           imp_idx_old, imp_idx_new));
    new_psi = nda::map([&](imp_block_t const &b) -> imp_block_t {
      if (b.imp_idx == imp_idx_old) return {imp_idx_new, b.gamma, b.tau};
      return b;
    })(new_psi);
    return {this->sigma_embed_decomp, this->imp_decomps, new_psi, this->_sigma_names};
  }

  //-----------------------------------------------------------------

  embedding embedding::split_imp(long imp_idx, std::function<bool(long)> p) const {
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

  embedding embedding::split_imp(long imp_idx, std::vector<long> const &block_list) const {
    if (not stdr::all_of(block_list, [&](auto i) { return (i >= 0) and (i < n_gamma(imp_idx)); }))
      throw std::runtime_error(fmt::format("[embedding::split_imp] The list of block indices {} is incorrect. Indices i should all be 0<= i < N_γ = {}",
                                           block_list, n_gamma(imp_idx)));
    return split_imp(imp_idx, [&](long idx) { return stdr::find(block_list, idx) != block_list.end(); });
  }

  // ----------------------------------------------------------------------

  embedding embedding::split_imp_block(long imp_idx, long gamma, std::vector<long> const &new_dims) const {
    // Validation:
    // Impurity index
    if (imp_idx < 0 or imp_idx >= n_impurities())
      throw std::runtime_error(fmt::format("Invalid impurity index {} (must be 0 <= imp_idx < {})", imp_idx, n_impurities()));
    // Gamma index for impurity
    if (gamma < 0 or gamma >= n_gamma(imp_idx))
      throw std::runtime_error(fmt::format("Invalid gamma {} for impurity {} (must be 0 <= gamma < {})", gamma, imp_idx, n_gamma(imp_idx)));
    // Dimensionality check
    auto old_dim = imp_decomps[imp_idx][gamma];
    if (stdr::fold_left(new_dims, 0, std::plus<>()) != old_dim)
      throw std::runtime_error(fmt::format("New dimensions sum {} != old dimension {}", stdr::fold_left(new_dims, 0, std::plus<>()), old_dim));

    // Update impurity decomposition
    auto new_imp_decomps = imp_decomps;
    new_imp_decomps[imp_idx].erase(begin(new_imp_decomps[imp_idx]) + gamma);
    new_imp_decomps[imp_idx].insert(begin(new_imp_decomps[imp_idx]) + gamma, begin(new_dims), end(new_dims));

    // Find all (alpha, sigma) pairs that map to (imp_idx, gamma, *) and update sigma_embed_decomposition
    std::set<long> alphas_to_split;
    auto const &alpha_list = reverse_psi[imp_idx](gamma, 0);
    for (auto const &[alpha, sigma] : alpha_list) { alphas_to_split.insert(alpha); }

    if (alphas_to_split.empty())
      throw std::runtime_error(fmt::format("No embedding block maps to impurity {}, gamma {}. Cannot update!", imp_idx, gamma));

    std::vector<long> sorted_alphas(alphas_to_split.begin(), alphas_to_split.end());
    stdr::sort(sorted_alphas, std::greater<>());

    auto new_sigma_embed_decomp = sigma_embed_decomp;
    long n_new_blocks           = long(new_dims.size());
    for (auto alpha : sorted_alphas) {
      new_sigma_embed_decomp.erase(begin(new_sigma_embed_decomp) + alpha);
      new_sigma_embed_decomp.insert(begin(new_sigma_embed_decomp) + alpha, begin(new_dims), end(new_dims));
    }

    // Build new psi mapping
    long n_new_alphas = long(new_sigma_embed_decomp.size());
    auto new_psi      = nda::array<imp_block_t, 2>(n_new_alphas, n_sigma());

    long new_alpha_idx = 0;
    for (long old_alpha = 0; old_alpha < n_alpha(); ++old_alpha) {
      auto old_map = psi(old_alpha, 0); // check first sigma as reference

      // Check if this alpha maps to the block we're splitting
      bool is_split_alpha = (old_map.imp_idx == imp_idx && old_map.gamma == gamma);

      if (is_split_alpha) {
        // insert n_new_blocks number of new alphas
        for (long split_idx = 0; split_idx < n_new_blocks; ++split_idx) {
          for (auto sigma : range(n_sigma())) {
            auto m                        = psi(old_alpha, sigma);
            new_psi(new_alpha_idx, sigma) = {m.imp_idx, gamma + split_idx, m.tau};
          }
          new_alpha_idx++;
        }
      } else {
        // copy with potential gamma adjustment
        for (auto sigma : range(n_sigma())) {
          auto m = psi(old_alpha, sigma);
          if (m.imp_idx == imp_idx && m.gamma > gamma) {
            new_psi(new_alpha_idx, sigma) = {m.imp_idx, m.gamma + n_new_blocks - 1, m.tau};
          } else {
            new_psi(new_alpha_idx, sigma) = m;
          }
        }
        new_alpha_idx++;
      }
    }
    return {std::move(new_sigma_embed_decomp), std::move(new_imp_decomps), std::move(new_psi), this->_sigma_names};
  }

  // ----------------------- Embedding and Extracting --------------------------
  block2_matrix_t embedding::embed(std::vector<block_matrix_t> const &Sigma_imp_static_vec) const {
    if (long(Sigma_imp_static_vec.size()) != n_impurities())
      throw std::runtime_error(fmt::format("[embedding::embed] Wrong number of impurity static self-energies: got {} but embedding has {} impurities",
                                           Sigma_imp_static_vec.size(), n_impurities()));
    return detail::data_array_to_block2_array(this->embed(detail::matrix_to_array(Sigma_imp_static_vec)), sigma_embed_decomp);
  }

  // ----------------------------------------------------------------------
  std::vector<block_matrix_t> embedding::extract(block2_matrix_t const &matrix_C) const {
    if (matrix_C.extent(1) != n_sigma())
      throw std::runtime_error(
         fmt::format("[embedding::extract] Wrong number of spin channels in matrix: got {} but embedding has n_sigma = {}", matrix_C.extent(1), n_sigma()));
    if (matrix_C.extent(0) != 1) return extract(detail::gather_blocks_to_data_view(matrix_C));
    auto data = range(n_sigma()) | stdv::transform([&](auto sigma) { return nda::array<dcomplex, 2>{matrix_C(0, sigma)}; }) | tl::to<std::vector>();
    return detail::array_to_matrix(this->extract(data));
  }
  // ----------------------------------------------------------------------

} // namespace triqs::modest