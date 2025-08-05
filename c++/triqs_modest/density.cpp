// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "./density.hpp"

namespace triqs::modest {

  double density_nk_matrix_valued_impl(one_body_elements_on_grid const &obe, double mu, double beta) {

    auto Fermi     = [](double x) { return (x > 0 ? exp(-x) / (1 + exp(-x)) : 1 / (1 + exp(x))); };
    double KS_term = 0;

    // MPI parallel only on this loop; eigenvalues inside the loop may thread via blas
    mpi::communicator comm = {}; // for now using default comm in MPI
    for (auto k_idx : mpi::chunk(nda::range(obe.H.n_k()), comm)) {
      for (auto sigma : nda::range(obe.C_space.n_sigma())) {
        double KS_term_acc = 0;
        auto eps           = nda::linalg::eigenvalues(nda::matrix<dcomplex>{obe.H.H(sigma, k_idx)});
        for (auto nu : range(obe.H.N_nu(sigma, k_idx))) { KS_term_acc += Fermi(beta * (eps(nu) - mu)); }
        KS_term += obe.H.k_weights(k_idx) * KS_term_acc;
      }
    }
    KS_term = mpi::all_reduce(KS_term);
    return KS_term;
  }

  double density_nk(one_body_elements_on_grid const &obe, double mu, double beta) {
    // if band dispersion is matrix valued need to diagonalize H(k) at every k.
    if (obe.H.matrix_valued) return density_nk_matrix_valued_impl(obe, mu, beta);

    auto Fermi     = [](double x) { return (x > 0 ? exp(-x) / (1 + exp(-x)) : 1 / (1 + exp(x))); };
    double KS_term = 0;

    mpi::communicator comm = {}; // for now using default comm in MPI
#pragma omp parallel for collapse(2) reduction(+ : KS_term) default(none) shared(obe, beta, Fermi, comm, mu)
    for (auto k_idx : mpi::chunk(nda::range(obe.H.n_k()), comm)) {
      for (auto sigma : nda::range(obe.C_space.n_sigma())) {
        double KS_term_acc = 0;
        auto eps           = obe.H.H(sigma, k_idx);
        for (auto nu : range(obe.H.N_nu(sigma, k_idx))) { KS_term_acc += Fermi(beta * real(eps(nu, nu) - mu)); }
        KS_term += obe.H.k_weights(k_idx) * KS_term_acc;
      }
    }
    KS_term = mpi::all_reduce(KS_term);
    return KS_term;
  }
} // namespace triqs::modest