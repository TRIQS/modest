// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "defs.hpp"
#include "./enumerate_slice.hpp"
#include <triqs/mesh/dlr_imfreq.hpp>
#include <triqs/mesh/utils.hpp>

#include <fmt/ostream.h>

// Detect if a formatter already exists for T
// template <typename T, typename Char = char>
// concept HasFmtFormatter = requires { fmt::formatter<T, Char>{}; };
template <typename T>
concept OstreamPrintable = requires(std::ostream &os, T const &t) {
  { os << t } -> std::same_as<std::ostream &>;
};
template <typename M>
  requires(triqs::gfs::Mesh<M> && OstreamPrintable<M>)
struct fmt::formatter<M> : fmt::ostream_formatter {};

namespace triqs::gfs {

  // We need formatters for meshes

  // Mainly notes for myself here.
  // - use array<gf, 2> of indices alpha, sigma
  // - have a new gf_struct_t --> gf_struct = array<pair<string, dim>, 2>
  // then make_array_gf(mesh m, array<pair<string, dim>, 2> S)
  //            = map( [&m](auto && p){ return g(m, p); })(S);

  struct gf_struct2_t {
    std::vector<std::vector<std::string>> names;
    nda::array<long, 2> dims;
  };

  // FIXME : to be simplified when block2_gf gets simplified ...
  template <typename Mesh> block2_gf<Mesh, matrix_valued> make_block2_gf(Mesh const &mesh, gf_struct2_t const &gf_s) {
    using g_t = gf<Mesh, matrix_valued>;
    auto g    = std::vector<std::vector<g_t>>(gf_s.names[0].size(), std::vector<g_t>(gf_s.names[1].size()));
    for (auto [alpha, sigma] : gf_s.dims().indices()) {
      auto dim        = gf_s.dims()(alpha, sigma);
      g[alpha][sigma] = g_t{mesh, {dim, dim}};
    }
    return {gf_s.names, std::move(g)};
  }

  //
  template <typename Mesh> gf_struct2_t get_struct(block2_gf<Mesh, matrix_valued> const &g) {
    auto res = nda::zeros<long>(g.size1(), g.size2());
    for (auto [alpha, sigma] : res.indices()) res(alpha, sigma) = g(alpha, sigma).target_shape()[0];
    return {g.block_names(), std::move(res)};
  }

  template <typename Mesh> block2_gf<Mesh> decomposition_view(block2_gf<Mesh> const &g, gf_struct2_t const &stru) {
    if (g.size1() != 1) throw std::runtime_error{"decomposition_view: not implemented if g.size1 !=1"};
    auto Gout    = make_block2_gf(g(0, 0).mesh(), stru);
    auto decomp  = stru.dims(r_all, 0);
    auto n_sigma = g.size2();
    for (auto [alpha, Ralpha] : enumerated_sub_slices(decomp)) {
      for (auto sigma : range(n_sigma)) { Gout(alpha, sigma).data() = g(0, sigma).data()(r_all, Ralpha, Ralpha); }
    }
    return Gout;
  }

  template <typename Mesh> block2_gf<Mesh> decomposition_view(block_gf<Mesh> const &g, gf_struct2_t const &stru) {
    auto Gout    = make_block2_gf(g[0].mesh(), stru);
    auto decomp  = stru.dims(r_all, 0);
    auto n_sigma = g.size();
    for (auto [alpha, Ralpha] : enumerated_sub_slices(decomp)) {
      for (auto sigma : range(n_sigma)) { Gout(alpha, sigma).data() = g(0, sigma).data()(r_all, Ralpha, Ralpha); }
    }
    return Gout;
  }

  inline block2_gf<mesh::dlr_imfreq, matrix_valued> make_block2_dlr_gf(block2_gf<mesh::imfreq, matrix_valued> const &g, double w_max, double eps) {
    auto &in_mesh = g(0, 0).mesh();
    auto beta     = in_mesh.beta();
    auto dlr_mesh = mesh::dlr_imfreq(beta, mesh::statistic_enum::Fermion, w_max, eps);
    auto gdlr     = make_block2_gf(dlr_mesh, get_struct(g));
    auto n_alpha  = g.size1();
    auto n_sigma  = g.size2();
    for (auto a : range(n_alpha))
      for (auto s : range(n_sigma))
        for (auto [iw, wdlr] : enumerate(dlr_mesh)) {
          if (wdlr.n > g(0, 0).mesh().last_index())
            throw std::runtime_error{
               fmt::format("make_block2_dlr_gf: the imfreq mesh \n{}\n is too short to extract the dlr_mesh \n{}\n", in_mesh, dlr_mesh)};
          // FIXME : fmt
          gdlr(a, s).data()(iw, r_all, r_all) = g(a, s)(wdlr);
        }
    return gdlr;
  }

} // namespace triqs::gfs
