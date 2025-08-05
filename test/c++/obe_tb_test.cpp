#include <gtest/gtest.h>
#include <nda/basic_functions.hpp>
#include <nda/gtest_tools.hpp>
#include <nda/matrix_functions.hpp>
#include <triqs/arrays.hpp>
#include <triqs/mesh/imfreq.hpp>
#include "triqs_modest/density.hpp"
#include "triqs_modest/gloc_fixed_grid.hpp"
#include "triqs_modest/obe_tb.hpp"
#include "triqs_modest/utils/h5_proxy.hpp"
#include "triqs_modest/utils/to_vector.hpp"

using namespace triqs::modest;
using namespace triqs::gfs;
using namespace triqs;

TEST(obe_tb, lco_wannier90) { // NOLINT

  // set up atomic shells
  std::vector<atomic_shell_t> atomic_shells;
  long dim  = 1;
  long l    = 2;
  long type = 0;
  long atom = 0;
  atomic_shells.emplace_back(dim, l, type, atom);

  // load in a obe from a Wannier tb model
  one_body_elements_tb obe_tb = one_body_elements_from_wannier90("./ref_data/lco", spin_kind_e::NonPolarized, atomic_shells);
  double beta                 = 10.;
  auto iw_mesh                = mesh::imfreq{beta, triqs::mesh::Fermion, 51};

  // make a zero self energy
  long n_sigma      = 2;
  auto Sigma        = gfs::gf<mesh::imfreq>{iw_mesh, {dim, dim}};
  auto Sigma_block2 = make_block2_gf(1, n_sigma, Sigma); // block, nsigma, fill with empty sigma
  nda::array<nda::matrix<dcomplex>, 2> Sigma_hartree(1, n_sigma);
  // spin up and down both zero for now
  for (auto shell : atomic_shells) {
    for (auto sigma : nda::range(n_sigma)) { Sigma_hartree(0, sigma) = nda::matrix<dcomplex>::zeros(shell.dim, shell.dim); }
  }

  // read in some reference data
  // FIXME: put all the args so I can set verbotisty to false on find mu?
  auto [dft_density, obe_dft] = one_body_elements_from_dft_converter("./ref_data/lco_wannier.ref.h5");
  double mu_dft               = find_chemical_potential(dft_density, obe_dft, beta);
  auto gloc_dft               = gloc(iw_mesh, obe_dft, mu_dft);

  // run gloc, forcing gloc to use PTR on the same grid as the DFT calculation was done
  // REFACTOR : is there a better option we want to force a standard ptr run?
  triqs::lattice::bz_int_options opt = {.k_grid_dims = {7, 7, 7}, .n_k_max = 8, .run_adaptive = false};
  auto gloc_tb                       = gloc(obe_tb, mu_dft, Sigma_block2, Sigma_hartree, opt);

  // Check equivalence
  for (auto iw : iw_mesh) { EXPECT_COMPLEX_NEAR(gloc_tb(0, 0)[iw](0, 0), gloc_dft(0, 0)[iw](0, 0), 1e-5); }

  // density calculation for one spin channel
  double n = density(obe_tb, mu_dft, Sigma_block2, Sigma_hartree, opt);
  EXPECT_NEAR(n, 1.0, 1e-4);

  // check mu finding
  double mu_calc = find_chemical_potential(n, obe_tb, Sigma_block2, Sigma_hartree, opt);
  EXPECT_NEAR(mu_calc, mu_dft, 1e-4);

  // check spin up and down channels
  for (auto sigma : nda::range(n_sigma)) { EXPECT_ARRAY_NEAR(impurity_levels(obe_dft)(0, sigma), impurity_levels(obe_tb)(0, sigma), 1e-8); }
}

// ----------------------------- SVO with t2g only test -----------------------------------------

TEST(obe_tb, svo_t2g_wannier90) { // NOLINT

  // set up atomic shells
  std::vector<atomic_shell_t> atomic_shells;
  // set up SVO shells, trying to keep the same order as the wannier hamiltonian does
  // try to make a shell with 3 d orbitals on the V atom
  long dim  = 3;
  long l    = 2;
  long type = 0;
  long atom = 0;
  atomic_shells.emplace_back(dim, l, type, atom);

  // load in a obe from a Wannier tb model
  one_body_elements_tb obe_tb = one_body_elements_from_wannier90("../../../test/c++/ref_data_lfs/svo_t2g", spin_kind_e::NonPolarized, atomic_shells);
  double beta                 = 10.;
  auto iw_mesh                = mesh::imfreq{beta, triqs::mesh::Fermion, 51};

  // make a zero self energy
  long n_blocks     = 1;
  long n_sigma      = 2;
  auto Sigma        = gfs::gf<mesh::imfreq>{iw_mesh, {dim, dim}};
  auto Sigma_block2 = make_block2_gf(n_blocks, n_sigma, Sigma); // block, nsigma, fill with empty sigma
  nda::array<nda::matrix<dcomplex>, 2> Sigma_hartree(n_blocks, n_sigma);
  // spin up and down both zero for now, 3 orbitals in the single block
  for (auto shell : atomic_shells) {
    for (auto sigma : nda::range(n_sigma)) { Sigma_hartree(0, sigma) = nda::matrix<dcomplex>::zeros(shell.dim, shell.dim); }
  }

  // read in some reference data
  auto [dft_density, obe_dft] = one_body_elements_from_dft_converter("../../../test/c++/ref_data_lfs/svo_t2g.h5");
  double mu_dft               = find_chemical_potential(dft_density, obe_dft, beta);
  auto gloc_dft               = gloc(iw_mesh, obe_dft, mu_dft);

  // run gloc, forcing gloc to use PTR on the same grid as the DFT calculation was done
  triqs::lattice::bz_int_options opt = {.k_grid_dims = {5, 5, 5}, .n_k_max = 6, .run_adaptive = false};
  auto gloc_tb                       = gloc(obe_tb, mu_dft, Sigma_block2, Sigma_hartree, opt);

  // Check equivalence
  for (auto iw : iw_mesh) { EXPECT_COMPLEX_NEAR(gloc_tb(0, 0)[iw](0, 0), gloc_dft(0, 0)[iw](0, 0), 1e-5); }

  // density calculation for one spin channel
  double n = density(obe_tb, mu_dft, Sigma_block2, Sigma_hartree, opt);
  // calculation does not give exactly 1 for density because of inaccurate int paramters
  EXPECT_NEAR(n, 0.99802109742386458, 1e-4);

  // check mu finding
  double mu_calc = find_chemical_potential(n, obe_tb, Sigma_block2, Sigma_hartree, opt);
  EXPECT_NEAR(mu_calc, mu_dft, 1e-4);

  // check spin up and down channels
  for (auto [is, shell] : enumerate(atomic_shells)) {
    for (auto sigma : nda::range(n_sigma)) { EXPECT_ARRAY_NEAR(impurity_levels(obe_dft)(is, sigma), impurity_levels(obe_tb)(is, sigma), 1e-8); }
  }
}

TEST(obe_tb, svo_t2g_eg_wannier90) { // NOLINT

  // set up atomic shells
  std::vector<atomic_shell_t> atomic_shells;

  // a shell with 3 d orbitals on the V atom
  long dim  = 2;
  long l    = 2;
  long type = 0;
  long atom = 0;
  atomic_shells.emplace_back(dim, l, type, atom);
  // set up SVO shells, trying to keep the same order as the wannier hamiltonian does
  // a shell with 3 d orbitals on the V atom
  dim  = 3;
  l    = 2;
  type = 0;
  atom = 0;
  atomic_shells.emplace_back(dim, l, type, atom);

  // load in a obe from a Wannier tb model
  one_body_elements_tb obe_tb =
     one_body_elements_from_wannier90("../../../test/c++/ref_data_lfs/svo_t2g_eg", spin_kind_e::NonPolarized, atomic_shells);
  double beta  = 10.;
  auto iw_mesh = mesh::imfreq{beta, triqs::mesh::Fermion, 51};

  // make a zero self energy
  long n_blocks     = 1;
  long n_sigma      = 2;
  auto Sigma        = gfs::gf<mesh::imfreq>{iw_mesh, {dim, dim}};
  auto Sigma_block2 = make_block2_gf(n_blocks, n_sigma, Sigma); // block, nsigma, fill with empty sigma
  nda::array<nda::matrix<dcomplex>, 2> Sigma_hartree(n_blocks, n_sigma);
  // spin up and down both zero for now, 3 orbitals in the single block
  for (auto shell : atomic_shells) {
    for (auto sigma : nda::range(n_sigma)) { Sigma_hartree(0, sigma) = nda::matrix<dcomplex>::zeros(shell.dim, shell.dim); }
  }

  // read in some reference data
  auto [dft_density, obe_dft] = one_body_elements_from_dft_converter("../../../test/c++/ref_data_lfs/svo_t2g_eg.h5");
  double mu_dft               = find_chemical_potential(dft_density, obe_dft, beta);
  auto gloc_dft               = gloc(iw_mesh, obe_dft, mu_dft);

  // run gloc, forcing gloc to use PTR on the same grid as the DFT calculation was done
  triqs::lattice::bz_int_options opt = {.k_grid_dims = {5, 5, 5}, .n_k_max = 6, .run_adaptive = false};
  auto gloc_tb                       = gloc(obe_tb, mu_dft, Sigma_block2, Sigma_hartree, opt);

  // Check equivalence
  for (auto iw : iw_mesh) { EXPECT_COMPLEX_NEAR(gloc_tb(0, 0)[iw](0, 0), gloc_dft(0, 0)[iw](0, 0), 1e-5); }

  // density calculation for one spin channel
  double n = density(obe_tb, mu_dft, Sigma_block2, Sigma_hartree, opt);
  // calculation does not give exactly 1 for density because of inaccurate int paramters
  EXPECT_NEAR(n, 1.0063915056934261, 1e-4);

  // check mu finding
  double mu_calc = find_chemical_potential(n, obe_tb, Sigma_block2, Sigma_hartree, opt);
  EXPECT_NEAR(mu_calc, mu_dft, 1e-4);

  //for (auto sh : obe_dft.C_space.atomic_shells()) { std::cout << sh.dim << std::endl; }

  // check spin up and down channels
  //for (auto [is, shell] : enumerate(atomic_shells)) {
  //for (auto sigma : nda::range(n_sigma)) { EXPECT_ARRAY_NEAR(impurity_levels(obe_dft)(is, sigma), impurity_levels(obe_tb)(is, sigma), 1e-8); }
  //std::cout << impurity_levels(obe_dft) << std::endl;
  //std::cout << impurity_levels(obe_tb) << std::endl;
  //}
}