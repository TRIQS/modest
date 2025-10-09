// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <complex>
#include <nda/nda.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>

// WARNING no namespace here, it is in fmt::

// HARD TO FIND. Need to disable the range formatter for the matrix, array
#ifdef FMT_RANGES_H_
template <typename T, typename Char> struct fmt::is_range<nda::matrix<T>, Char> : std::false_type {};
template <typename T, typename Char> struct fmt::is_range<nda::array<T, 2>, Char> : std::false_type {};
#endif

//-------------------------------------------------------

template <typename U> struct fmt::formatter<nda::matrix<U>> {
  static_assert(fmt::is_formattable<U>::value, "Cannot format nda::matrix<U>: U must be formattable");

  //
  //constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
  // Format specifier for elements of type U
  fmt::formatter<U> value_formatter;

  // Parse the format specifier for U
  constexpr auto parse(format_parse_context &ctx) {
    // Let the value formatter handle the parsing
    return value_formatter.parse(ctx);
  }

  //
  auto format(auto const &mat, format_context &ctx) const {
    auto out      = ctx.out();
    int max_width = std::ranges::max(mat | std::views::transform([](auto &&x) { return fmt::format("{}", x).length(); }));
    for (long i = 0; i < mat.extent(0); ++i) {
      if (i == 0)
        fmt::format_to(out, "\n[[");
      else
        fmt::format_to(out, " [");
      for (long j = 0; j < mat.extent(1); ++j) fmt::format_to(out, "{:>{}}", fmt::format("{}", mat(i, j)), max_width);
      if (i != mat.extent(0) - 1)
        fmt::format_to(out, "]\n");
      else
        fmt::format_to(out, "]]\n");
    }
    return out;
  }
};

// -----------------------------------------

template <typename U> struct fmt::formatter<nda::array<U, 2>> : fmt::formatter<nda::matrix<U>> {};

/// FIXME : do I need this still ??
// Specialization of fmt::formatter for nda::matrix<std::complex<T>>
template <typename T> struct fmt::formatter<nda::matrix<std::complex<T>>> : fmt::formatter<double> {
  int precision = 4; // Default precision

  // Parse format specifiers
  constexpr auto parse(format_parse_context &ctx) {
    auto it = ctx.begin();
    if (it != ctx.end()) {
      if (*it >= '0' && *it <= '9') {
        precision = *it - '0'; // Extract precision
        ++it;
      }
    }
    return it;
  }

  // Format function
  template <typename FormatContext> auto format(const nda::matrix<std::complex<T>> &mat, FormatContext &ctx) const {
    auto out = ctx.out();

    int max_width_r = std::ranges::max(mat | std::views::transform([&](auto &&x) { return fmt::format("{:.{}f}", x.real(), precision).length(); }));
    int max_width_i = std::ranges::max(mat | std::views::transform([&](auto &&x) { return fmt::format("{:.{}f}", x.imag(), precision).length(); }));
    int max_width   = std::max(max_width_r, max_width_i);

    for (size_t i = 0; i < mat.extent(0); ++i) {
      if (i == 0)
        fmt::format_to(out, "\n[[");
      else
        fmt::format_to(out, " [");
      for (size_t j = 0; j < mat.extent(1); ++j) {
        const auto &c = mat(i, j);
        fmt::format_to(out, "{2:{0}.{1}f}{3:+{0}.{1}f}i ", max_width, precision, c.real(), c.imag(), precision);
      }
      if (i != mat.extent(0) - 1)
        fmt::format_to(out, "]\n");
      else
        fmt::format_to(out, "]]\n");
    }
    return out;
  }
};

namespace nda {

  /**
 * @brief Format the matrix mat as a table, with row/col_labels.
 *
 * @param out output stream
 * @param mat The matrix
 * @param row_labels Range of string: name of the rows
 * @param col_labels Range of string: name of the cols
 */
  // FIXME : use a matrix<std::string>  then a map() in the code
  void format_as_table(std::ostream &out, nda::Matrix auto const &mat, auto const &row_labels, auto const &col_labels) {
    namespace stdv    = std::views;
    namespace stdr    = std::ranges;
    auto [rows, cols] = mat.shape();

    // Width of first column (row labels)
    long first_col_width = stdr::max(row_labels | stdv::transform([](auto &&s) { return long(s.size()); }));

    // Widths of each column: max(label, all formatted elements)
    std::vector<long> col_widths(cols, 0);
    for (long j : range(cols)) {
      auto formatted_column = stdv::iota(0L, rows) | stdv::transform([&](long i) { return long(fmt::format("{}", mat(i, j)).size()); });
      col_widths[j]         = std::max(long(col_labels[j].size()), stdr::max(formatted_column));
    }

    // Header row (centered labels)
    out << fmt::format("{:<{}} | ", "", first_col_width);
    for (long j : range(cols)) out << fmt::format("{:^{}}  ", col_labels[j], col_widths[j]);
    out << '\n';

    // Separator line
    out << fmt::format("{:-<{}}-+-", "", first_col_width);
    for (long j : range(cols)) out << fmt::format("{:-<{}}-+", "", col_widths[j]);
    out << '\n';

    // Data rows (right-aligned)
    for (long i : range(rows)) {
      if (mat(i, 0).imp_idx == -1) continue; // HL: skip alpha which do not map to an impurity (OP review)
      out << fmt::format("{:<{}} | ", row_labels[i], first_col_width);
      for (long j : range(cols)) { out << fmt::format("{:>{}}  ", fmt::format("{}", mat(i, j)), col_widths[j]); };
      out << '\n';
    }
  }
} // namespace nda
