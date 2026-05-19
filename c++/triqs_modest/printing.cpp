// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "./downfolding.hpp"
#include "./embedding.hpp"
#include <algorithm>
#include <fmt/base.h>
#include <triqs/utility/streams.hpp>
#include "utils/nda_pretty_printer.hpp"
#include <fmt/ranges.h>
#include <ranges>
namespace triqs::modest {

  // -----------------------------  PRINT FUNCTIONS ---------------------------------------------

  std::ostream &operator<<(std::ostream &out, band_dispersion const &x) {
    out << "Band dispersion ε^σ(k) on a grid [band_dispersion]:\n";
    auto out1 = indented_ostream(out, 2);
    out1 << fmt::format("Number of bands (max): {}\n", x.H_k.extent(3));
    out1 << fmt::format("Represented on a fixed grid of {} points.\n", x.n_k());
    out1 << fmt::format("Shape of H[k_idx, σ, ν, ν'] = {}\n", x.H_k.shape());
    out1 << fmt::format("ε^σ(k) is matrix valued? = {}\n", x.matrix_valued);
    return out;
  }

  // ---------------------------------------------------------------------------------------------

  std::ostream &operator<<(std::ostream &out, downfolding_projector const &x) {
    out << "Downfolding projector P^σ_mν(k) on a grid [downfolding_projector]:\n";
    auto out1 = indented_ostream(out, 2);
    out1 << fmt::format("Shape of P[k_idx, σ, m, ν]  = {}\n", x.P_k.shape());
    return out;
  }

  // ---------------------------------------------------------------------------------------------

  std::ostream &operator<<(std::ostream &out, local_space const &x) {
    out << "Local space [orbital_set]:\n";
    auto out1 = indented_ostream(out, 2); // same stream, but shifted by 2 spaces
    auto out2 = indented_ostream(out, 4);
    auto out3 = indented_ostream(out, 6);
    out1 << fmt::format("Total dimension [M]: {}\n", x.dim());
    out1 << fmt::format("Number of correlated atoms: {}\n", x.atomic_shells().size());
    // auto ineq = 0;
    // for (auto iatom : range(x.atomic_shells().size())) {
    //   if (x.first_shell_of_its_equiv_cls(iatom) == iatom) ineq++;
    // }
    auto ineq = stdr::count_if(range(x.atomic_shells().size()), //
                               [&](auto i) { return x.first_shell_of_its_equiv_cls(i) == i; });
    out1 << fmt::format("Number of inequivalent correlated atoms = {}\n", ineq);

    out1 << "Atomic decomposition:\n";
    auto pr_vec = [](auto const &V) {
      return fmt::format("{}\n", fmt::join(V | stdv::transform([](auto x) { return fmt::format("{:>3}", x); }), " "));
    };
    auto dec = x.atomic_decomposition();
    out2 << " dim_a: " << pr_vec(dec);
    out2 << "     a: " << pr_vec(range(dec.size()));
    auto sub = x.atoms_block_decomposition()(r_all, 0) | stdv::transform([](auto x) { return fmt::format("{}", x); });
    out2 << "irreps: " << fmt::format("{}\n", fmt::join(sub, " "));
    return out;
  }

  // ---------------------------------------------------------------------------------------------
  std::ostream &operator<<(std::ostream &out, ibz_symmetry_ops const &ibz) {
    out << "[I]rreduicible [B]rillouin [Z]one symmetry operations from the DFT code [ibz_symmetry_ops]\n";
    auto out1 = indented_ostream(out, 2);
    out1 << fmt::format("Number of symmetry ops: {}\n", ibz.ops.size());
    out1 << fmt::format("Number of rotations per op: {}\n", ibz.ops[0].mats.size());
    return out;
  }

  // ---------------------------------------------------------------------------------------------

  std::ostream &operator<<(std::ostream &out, one_body_elements_on_grid const &obe) {
    auto out1 = indented_ostream(out, 2); // same stream, but shifted by 2 spaces
    auto out2 = indented_ostream(out, 4);
    out << "One body elements representing a downfolding from (restricted) Bloch 𝓑 to Correlated space 𝓒 from DFT code [one_body_elements_on_grid]\n";
    out1 << "H:\n";
    out2 << obe.H;
    out1 << "C_space:\n";
    out2 << obe.C_space;
    out1 << "P:\n";
    out2 << obe.P;
    out1 << fmt::format("IBZ = {}\n", bool(obe.ibz_symm_ops));
    if (obe.ibz_symm_ops) { out2 << obe.ibz_symm_ops.value(); };
    return out;
  }

  // ------------------------------ PRINTING -------------------------------------------------------------

  auto format_as(embedding::imp_block_t const &p) {
    //return fmt::format("({},{},{})", p.n_imp, p.gamma, p.tau);
    return fmt::format("(imp_idx = {}, γ = {}, τ = {})", p.imp_idx, p.gamma, p.tau);
  }

  // -------------------------------------------------------------------------------------------------------

  std::string embedding::description(bool verbosity) const {
    auto sigma_embed_shape     = this->embed_block_structure();
    auto impurities_shape_list = this->imp_block_structure();

    std::ostringstream out;
    auto out1 = indented_ostream(out, 2); // same stream, but shifted by 2 spaces
    auto out2 = indented_ostream(out, 4);
    auto out3 = indented_ostream(out, 6);

    // Detect impurities not connected to any alpha block.
    auto unconnected_imps = std::vector<long>{};
    auto is_unconnected   = std::vector<bool>(n_impurities(), true);
    for (long n = 0; n < n_impurities(); ++n) {
      is_unconnected[n] = stdr::all_of(reverse_psi[n], [](auto const &v) { return v.empty(); });
      if (is_unconnected[n]) unconnected_imps.push_back(n);
    }

    auto fmt_vec = [](auto const &V) { return fmt::join(V | stdv::transform([](auto x) { return fmt::format("{:>3}", x); }), " "); };
    auto pr_vec  = [&](auto const &V) { return fmt::format("{}\n", fmt_vec(V)); };

    if (!verbosity) {
      out << "Embedding: ";
      out << fmt::format("{} impurities\n", this->n_impurities());
      out1 << "Σ_embed block decomposition:\n";
      out2 << "dim_α: " << pr_vec(this->sigma_embed_decomp);
      out2 << "    α: " << pr_vec(range(this->sigma_embed_decomp.size()));
      out1 << "\nImpurities\n";
      out2 << "Block dimensions, dim_γ for all γ:\n";
      for (auto &&[n, dec] : enumerate(this->imp_decomps)) {
        auto head = fmt::format("[n_imp = {}]", n);
        auto tag  = is_unconnected[n] ? "   \033[31m(not connected)\033[0m" : "";
        out3 << fmt::format("{} dim_γ = {}{}\n", head, fmt_vec(dec), tag);
        out3 << fmt::format("{:>{}}     γ = {}", " ", head.size(), pr_vec(range(dec.size())));
      }
      if (!unconnected_imps.empty()) out << fmt::format("\nImpurities not connected to any alpha block: {}\n", unconnected_imps);
      return out.str();
    }

    out << "Embedding:\n";
    out1 << fmt::format("Spin index (σ/τ) names: {}\n\n", this->sigma_names());
    out1 << "Σ_embed block decomposition:\n";
    out2 << "dim_α: " << pr_vec(this->sigma_embed_decomp);
    out2 << "    α: " << pr_vec(range(this->sigma_embed_decomp.size()));
    out1 << "\nImpurities\n";
    out2 << "Block dimensions, dim_γ for all γ:\n";
    for (auto &&[n, dec] : enumerate(this->imp_decomps)) {
      auto head = fmt::format("[n_imp = {}]", n);
      auto tag  = is_unconnected[n] ? "   \033[31m(not connected)\033[0m" : "";
      out3 << fmt::format("{} dim_γ = {}{}\n", head, fmt_vec(dec), tag);
      out3 << fmt::format("{:>{}}     γ = {}", " ", head.size(), pr_vec(range(dec.size())));
    }
    out2 << "Gf Block structures for solvers as names, [dim]:\n";
    for (auto &&[n, ish] : enumerate(impurities_shape_list)) {
      auto formatted_vec = ish | stdv::transform([](auto &&p) { return fmt::format("{} [{}]", p.first, p.second); }) | tl::to<std::vector>();
      out3 << fmt::format("[imp_idx = {}] {}\n", n, fmt::join(formatted_vec, ", "));
    }
    out1 << "\nMapping ψ(α,σ) = (imp_idx, γ, τ) \n";
    auto row_labels = range(this->n_alpha()) | stdv::transform([](auto x) { return fmt::format("α = {}", x); }) | tl::to<std::vector>();
    auto col_labels = range(this->n_sigma()) | stdv::transform([&](auto i) { return fmt::format("σ = {} / {}", i, this->sigma_names()[i]); })
       | tl::to<std::vector>();
    nda::format_as_table(out3, this->psi, row_labels, col_labels);
    if (!unconnected_imps.empty()) out << fmt::format("\nImpurities not connected to any alpha block: {}\n", unconnected_imps);

    return out.str();
  }

  std::ostream &operator<<(std::ostream &out, embedding const &E) {
    out << E.description(false);
    return out;
  }

} // namespace triqs::modest