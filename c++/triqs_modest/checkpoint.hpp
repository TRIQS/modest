// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "downfolding.hpp"
#include "embedding.hpp"
#include <filesystem>
#include <string>

namespace triqs::modest {

  namespace fs = std::filesystem;

  // ===========================================================================
  //          Checkpoint data structures for DMFT loop
  // ===========================================================================

  using block_gf_imfreq_t = block_gf<imfreq, matrix_valued>;
  using block_gf_imtime_t = block_gf<imtime, matrix_valued>;
  using block_mat_t       = std::vector<nda::matrix<dcomplex>>;

  /**
   * @brief Minimal iteration data required to restart a DMFT loop.
   */
  struct iteration_data {
    double mu;
    std::vector<block_gf_imfreq_t> Sigma_imp_list;
    std::vector<block_mat_t> Sigma_hartree_list;

    C2PY_IGNORE friend void mpi_broadcast(iteration_data &x, mpi::communicator c = {}, int root = 0) {
      mpi::broadcast(x.mu, c, root);
      mpi::broadcast(x.Sigma_imp_list, c, root);
      mpi::broadcast(x.Sigma_hartree_list, c, root);
    }
  };

  // HDF5 serialization for iteration_data
  inline void h5_write(h5::group g, std::string const &name, iteration_data const &data) {
    auto g2 = g.create_group(name);
    h5::write(g2, "mu", data.mu);
    h5::write(g2, "Sigma_imp_list", data.Sigma_imp_list);
    h5::write(g2, "Sigma_hartree_list", data.Sigma_hartree_list);
  }

  inline void h5_read(h5::group g, std::string const &name, iteration_data &data) {
    auto g2 = g.open_group(name);
    h5::read(g2, "mu", data.mu);
    h5::read(g2, "Sigma_imp_list", data.Sigma_imp_list);
    h5::read(g2, "Sigma_hartree_list", data.Sigma_hartree_list);
  }

  // ===========================================================================
  //          checkpoint
  // ===========================================================================

  template <typename IterationData> class checkpoint {

    fs::path _dirname;
    long _n_iter = 0;
    std::string _iter_file;

    struct h5path {
      std::string file;
      std::string group_name;
    };

    [[nodiscard]] h5path path_iteration(long n) const { return {_iter_file, std::to_string(n)}; }

    template <typename T> [[nodiscard]] T read(h5path p) const { return h5::read<T>(h5::file(p.file, 'r'), p.group_name); }

    public:
    /// Open existing or create new checkpoint directory.
    checkpoint(std::string dirname) : _dirname{dirname}, _iter_file{dirname + "/iterations.h5"} {
      if (fs::exists(dirname)) {
        if (!fs::is_directory(dirname))
          throw std::runtime_error{fmt::format("Checkpoint: '{}' exists but is not a directory", dirname)};
        auto file = h5::file(_iter_file, 'r');
        _n_iter   = long(h5::group{file}.get_all_subgroup_dataset_names().size());
      } else {
        fs::create_directory(dirname);
        h5::file(_iter_file, 'w');
      }
    }

    [[nodiscard]] std::string dirname() const { return _dirname.string(); }
    [[nodiscard]] long size() const { return _n_iter; }
    [[nodiscard]] bool empty() const { return _n_iter == 0; }

    [[nodiscard]] IterationData last() const {
      if (empty()) throw std::runtime_error{"Checkpoint: no iterations stored"};
      return (*this)[-1];
    }

    void append(IterationData const &x) {
      auto p    = path_iteration(_n_iter);
      auto f    = h5::file(p.file, 'a');
      auto root = h5::group{f};
      h5::write(root, p.group_name, x);
      _n_iter++;
    }

    [[nodiscard]] IterationData operator[](long i) const {
      if (i < 0) i += size();
      if (i < 0 || i >= size()) throw std::out_of_range{fmt::format("Checkpoint: index {} out of range [0, {})", i, size())};
      return read<IterationData>(path_iteration(i));
    }

    // Range-based for loop support
    class const_iterator {
      checkpoint const *_cp;
      long _idx;

      public:
      const_iterator(checkpoint const *cp, long idx) : _cp{cp}, _idx{idx} {}
      IterationData operator*() const { return (*_cp)[_idx]; }
      const_iterator &operator++() {
        ++_idx;
        return *this;
      }
      bool operator!=(const_iterator const &other) const { return _idx != other._idx; }
    };

    [[nodiscard]] const_iterator begin() const { return {this, 0}; }
    [[nodiscard]] const_iterator end() const { return {this, _n_iter}; }
  };

  // Explicit instantiation for c2py
  template class checkpoint<iteration_data>;

} // namespace triqs::modest
