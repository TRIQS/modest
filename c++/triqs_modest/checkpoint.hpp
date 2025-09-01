// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "downfolding.hpp"
#include "embedding.hpp"
#include <filesystem>
#include <string>
#include <string_view>

namespace triqs::modest {

  namespace fs = std::filesystem;

  /**
   * @ingroup checkpoint
   * @brief A checkpoint manager for logging data after each DMFT iteration
   *
   * @tparam InitialData the initial data of the calculatoin (one_body_elements, embedding)
   * @tparam IterationData the data after each DMFT iteration
   */
  template <typename InitialData, typename IterationData> class checkpoint {

    fs::path _dirname;
    long _n_iter = 0;
    struct h5path {
      std::string file;       // absolute path to the h5 file
      std::string group_name; // group name in the h5 file
    };

    h5path path_initial_data        = h5path{_dirname.string() + "/initial_data.h5", "initial_data"};
    std::string iteration_file_path = _dirname.string() + "/iterations.h5";

    [[nodiscard]] h5path path_iteration(long n_iter) const { return {iteration_file_path, std::to_string(n_iter)}; }

    void write(h5path p, auto const &x) const { h5::write(h5::file(p.file, 'a'), p.group_name, x); }

    template <typename T> [[nodiscard]] T read(h5path p) const { return h5::read<T>(h5::file(p.file, 'r'), p.group_name); }

    public:
    /**
     * @brief Open checkpoint from an existing file (rw).
     * @param dirname Name of the directory containing the checkpoint files.
     */
    checkpoint(std::string dirname) : _dirname{dirname} {
      if (!fs::exists(dirname)) throw std::runtime_error{fmt::format("Checkpoint: dirname '{}' does not exist", dirname)};
      if (!fs::is_directory(dirname)) throw std::runtime_error{fmt::format("Checkpoint: dirname '{}' exists but is not a directory", dirname)};

      // Read n_iter from file : length of the iteration data in the h5 file
      auto file = h5::file(iteration_file_path, 'r');
      _n_iter   = long(h5::group{file}.get_all_subgroup_dataset_names().size());
    }

    /**
     * @brief Create a new checkpoint from initial_data.
     * @param dirname Name of the directory containing the checkpoint files. Must not exist.
     * @param initial_data Initial data to store in the checkpoint.
     */
    checkpoint(std::string dirname, InitialData const &initial_data) : _dirname{dirname} {
      if (fs::exists(dirname)) throw std::runtime_error{fmt::format("Checkpoint: dirname '{}' already exists", dirname)};
      fs::create_directory(dirname);
      auto f    = h5::file(path_initial_data.file, 'w');
      auto root = h5::group{f};
      h5::write(root, path_initial_data.group_name, initial_data);
      // Subgroup for logging data
      auto g = root.create_group("logging");
      // FIXME : Writout some metadata
    }

    /// Directory name containing the checkpoint files.
    [[nodiscard]] std::string dirname() const { return _dirname.string(); }

    /// Initial data.
    [[nodiscard]] InitialData initial_data() const { return read<InitialData>(path_initial_data); }

    void append(IterationData const &x) {
      write(path_iteration(_n_iter), x);
      _n_iter++;
    }

    [[nodiscard]] IterationData operator[](long i) const {
      if (i < 0) i += size();
      if (i < 0 || i >= size()) throw std::out_of_range{fmt::format("Checkpoint: index {} out of range [0, {}]", i, size())};
      return read<IterationData>(path_iteration(i));
    }

    [[nodiscard]] long size() const { return _n_iter; }
  };

  // ===========================================================================
  //          Checkpoint data structures for DMFT loop
  // ===========================================================================

  /// Initial data used for checkpointing.
  struct initial_data {
    /// One-body elements.
    one_body_elements_on_grid obe;

    /// Embedding object.
    embedding embed;

    /// Impurity model
    //  impurity_model imp_model;
  };

  // HDF5 read and write for initial_data
  void h5_write(h5::group g, std::string const &name, initial_data const &data) {
    auto g2 = g.create_group(name);
    h5::write(g2, "one_body_elements_on_grid", data.obe);
    h5::write(g2, "embedding", data.embed);
    // h5::write(g, "impurity_model", data.imp_model); // Uncomment if impurity_model is added
  }

  void h5_read(h5::group g, std::string const &name, initial_data &data) {
    auto g2 = g.create_group(name);
    h5::read(g2, "one_body_elements_on_grid", data.obe);
    h5::read(g2, "embedding", data.embed);
    // h5::read(g, "impurity_model", data.imp_model); // Uncomment if impurity_model is added
  }

  // ------------------------------------------------------
  using block_gf_imfreq_t = block_gf<imfreq, matrix_valued>;
  using block_gf_imtime_t = block_gf<imtime, matrix_valued>;
  using block_mat_t       = std::vector<nda::matrix<dcomplex>>;

  /// Iteration data used for checkpointing.
  struct iteration_data {

    /// Chemical potential.
    double mu;

    /// Impurities self-energies.
    std::vector<block_gf_imfreq_t> Sigma_imp_list;

    /// Sigma Hartree.
    block_mat_t Sigma_hartree_list;

    /// Impurities Green's functions.
    std::vector<block_gf_imfreq_t> Gimp_freq_list;

    /// Impurities Green's functions.
    std::vector<block_gf_imtime_t> Gimp_time_list;

    /// Impurities self-energies.
    std::vector<block_gf_imfreq_t> Sigma_dc_list;
  };

  // HDF5 read and write for iteration_data
  void h5_write(h5::group g, std::string const &name, iteration_data const &data) {
    auto g2 = g.create_group(name);
    h5::write(g2, "mu", data.mu);

    h5::write(g2, "Sigma_imp_list", data.Sigma_imp_list);
    h5::write(g2, "Sigma_hartree_list", data.Sigma_hartree_list);
    h5::write(g2, "Sigma_dc_list", data.Sigma_dc_list);

    h5::write(g2, "Gimp_freq_list", data.Gimp_freq_list);
    h5::write(g2, "Gimp_time_list", data.Gimp_time_list);
  }

  void h5_read(h5::group g, std::string const &name, iteration_data &data) {
    auto g2 = g.open_group(name);
    h5::read(g2, "mu", data.mu);

    h5::read(g2, "Sigma_imp_list", data.Sigma_imp_list);
    h5::read(g2, "Sigma_hartree_list", data.Sigma_hartree_list);
    h5::read(g2, "Sigma_dc_list", data.Sigma_dc_list);

    h5::read(g2, "Gimp_freq_list", data.Gimp_freq_list);
    h5::read(g2, "Gimp_time_list", data.Gimp_time_list);
  }
  // ===========================================================

  // Template specialization for the checkpoint class
  // For c2py
  /// A checkpoint manager for logging data after each DMFT iteration.
  template class checkpoint<initial_data, iteration_data>;

} // namespace triqs::modest