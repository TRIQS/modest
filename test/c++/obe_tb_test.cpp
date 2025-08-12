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

// REFACTOR This is partially duplicate with density_test.cpp
double random_constant() {
  std::mt19937 gen(std::random_device{}());
  std::uniform_real_distribution<double> dist(0, 1);
  return dist(gen);
}

///-----------------------------------------
auto eps   = 1.e-12;
auto w_max = 10.0;

auto make_random_self(long M, double beta) {
  auto iw_mesh     = mesh::imfreq{beta, triqs::mesh::Fermion, 51};
  auto g           = gfs::gf<mesh::imfreq>{iw_mesh, {M, M}};
  auto const &mesh = g.mesh();
  auto A           = random_constant();
  auto B           = random_constant();
  for (auto &&[n, iw] : itertools::enumerate(mesh)) { g.data()(n, r_all, r_all) = A + B / (iw + 10); }
  return g;
}

TEST(obe_tb, lco_wannier90) { // NOLINT

  // set up atomic shells
  std::vector<atomic_orbs> atomic_shells;
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
  nda::array<nda::matrix<dcomplex>, 2> Sigma_static(1, n_sigma);
  // spin up and down both zero for now
  for (auto shell : atomic_shells) {
    for (auto sigma : nda::range(n_sigma)) { Sigma_static(0, sigma) = nda::matrix<dcomplex>::zeros(shell.dim, shell.dim); }
  }

  // read in some reference data
  // REFACTOR: put all the args so I can set verbotisty to false on find mu?
  auto [dft_density, obe_dft] = one_body_elements_from_dft_converter("./ref_data/lco_wannier.ref.h5");
  double mu_dft               = find_chemical_potential(dft_density, obe_dft, beta);
  auto gloc_dft               = gloc(iw_mesh, obe_dft, mu_dft);

  // run gloc, forcing gloc to use PTR on the same grid as the DFT calculation was done
  triqs::lattice::bz_int_options opt = {.k_grid = {7, 7, 7}, .k_grid_max = {7, 7, 7}, .run_adaptive = false};
  auto gloc_tb                       = gloc(obe_tb, mu_dft, Sigma_block2, Sigma_static, opt);

  // Check equivalence
  for (auto iw : iw_mesh) { EXPECT_COMPLEX_NEAR(gloc_tb(0, 0)[iw](0, 0), gloc_dft(0, 0)[iw](0, 0), 1e-5); }

  // density calculation for one spin channel
  double n = density(obe_tb, mu_dft, Sigma_block2, Sigma_static, opt);
  EXPECT_NEAR(n, 1.0, 1e-4);

  // check mu finding
  double mu_calc = find_chemical_potential(n, obe_tb, Sigma_block2, Sigma_static, opt);
  EXPECT_NEAR(mu_calc, mu_dft, 1e-4);

  // check spin up and down channels
  for (auto sigma : nda::range(n_sigma)) { EXPECT_ARRAY_NEAR(impurity_levels(obe_dft)(0, sigma), impurity_levels(obe_tb)(0, sigma), 1e-8); }
}

// ----------------------------- SVO with t2g only test -----------------------------------------

#if LFS
TEST(obe_tb, svo_t2g_wannier90_multiorbtial) { // NOLINT

  // set up atomic shells
  std::vector<atomic_orbs> atomic_shells;
  // set up SVO shells, trying to keep the same order as the wannier hamiltonian does
  // try to make a shell with 3 d orbitals on the V atom
  long dim  = 3;
  long l    = 2;
  long type = 0;
  long atom = 0;
  atomic_shells.emplace_back(dim, l, type, atom);

  // load in a obe from a Wannier tb model
  one_body_elements_tb obe_tb = one_body_elements_from_wannier90("./ref_data_lfs/svo_t2g", spin_kind_e::NonPolarized, atomic_shells);
  double beta                 = 10.;
  auto iw_mesh                = mesh::imfreq{beta, triqs::mesh::Fermion, 51};
  long n_sigma                = 2;
  long n_atoms                = 1;

  // TODO replace this with function calls that don't take self energy
  // make a zero self energy
  auto Sigma        = gfs::gf<mesh::imfreq>{iw_mesh, {dim, dim}};
  auto Sigma_block2 = make_block2_gf(n_atoms, n_sigma, Sigma); // block, nsigma, fill with empty sigma
  nda::array<nda::matrix<dcomplex>, 2> Sigma_static(n_atoms, n_sigma);
  // spin up and down both zero for now, 3 orbitals in the single block
  for (auto sigma : nda::range(n_sigma)) { Sigma_static(0, sigma) = nda::diag(nda::vector<dcomplex>{0., 0., 0.}); }

  // read in some reference data
  auto [dft_density, obe_dft] = one_body_elements_from_dft_converter("./ref_data_lfs/svo_t2g_w90_fixedgrid.ref.h5");
  double mu_dft               = find_chemical_potential(dft_density, obe_dft, beta);
  auto gloc_dft               = gloc(iw_mesh, obe_dft, mu_dft);

  // run gloc, forcing gloc to use PTR on the same grid as the DFT calculation was done
  triqs::lattice::bz_int_options opt = {.k_grid = {5, 5, 5}, .k_grid_max = {5, 5, 5}, .run_adaptive = false};
  auto gloc_tb                       = gloc(iw_mesh, obe_tb, mu_dft, opt);

  // FIXME : something about this seems potentially wrong
  // Check equivalence -- note, this is a very coarse DFT grid, it seems Wannierization has a tiny variation that
  // results in a small but meaningful (second decimal) difference in gloc for the frequency closest to zero.
  // therefore we set a lower tolerance for two points near zero.
  for (auto iw : iw_mesh) { //
    if (std::abs(dcomplex(iw.value())) < 1.7) {
      EXPECT_COMPLEX_NEAR(gloc_tb(0, 0)[iw](0, 0), gloc_dft(0, 0)[iw](0, 0), 1e-2);
    } else {
      EXPECT_COMPLEX_NEAR(gloc_tb(0, 0)[iw](0, 0), gloc_dft(0, 0)[iw](0, 0), 1e-4);
    }
  }

  // density calculation for one spin channel
  double n = density(obe_tb, mu_dft, Sigma_block2, Sigma_static, opt);
  // calculation does not give exactly 1 for density because of inaccurate int paramters
  EXPECT_NEAR(n, 0.99802109742386458, 1e-4);

  // check mu finding
  double mu_calc = find_chemical_potential(n, obe_tb, Sigma_block2, Sigma_static, opt);
  EXPECT_NEAR(mu_calc, mu_dft, 1e-4);

  // check spin up and down channels
  for (auto [is, shell] : enumerate(atomic_shells)) {
    for (auto sigma : nda::range(n_sigma)) { EXPECT_ARRAY_NEAR(impurity_levels(obe_dft)(is, sigma), impurity_levels(obe_tb)(is, sigma), 1e-8); }
  }

  // make a self energy, call self energy interfaces --------------------
  auto Sigma_single = make_random_self(obe_tb.H[0].n_orbitals(), beta);
  auto Sigma_dyn    = make_block2_gf(n_atoms, n_sigma, Sigma_single);
  // spin up/down, 3 orbitals in the single block
  for (auto sigma : nda::range(n_sigma)) { Sigma_static(0, sigma) = nda::diag(nda::vector<dcomplex>{0.6, 0.6, 0.8}); }

  // try with a self energy
  // TODO why does this not return exactly the same type as the earlier function call?
  auto gloc_tb_SE  = gloc(obe_tb, mu_dft, Sigma_dyn, Sigma_static, opt);
  auto gloc_dft_SE = gloc(obe_dft, mu_dft, Sigma_dyn, Sigma_static);

  for (auto iw : iw_mesh) { //
    if (std::abs(dcomplex(iw.value())) < 1.7) {
      EXPECT_COMPLEX_NEAR(gloc_tb_SE(0, 0)[iw](0, 0), gloc_dft_SE(0, 0)[iw](0, 0), 1e-2);
    } else {
      EXPECT_COMPLEX_NEAR(gloc_tb_SE(0, 0)[iw](0, 0), gloc_dft_SE(0, 0)[iw](0, 0), 1e-4);
    }
  }
}
/*
TEST(obe_tb, sr2moo4_wannier90) { // NOLINT

  // set up atomic shells
  std::vector<atomic_orbs> atomic_shells;

  // a shell with 3 d orbitals on the V atom
  long dim  = 2;
  long l    = 2;
  long type = 0;
  long atom = 0;
  atomic_shells.emplace_back(dim, l, type, atom);

  // load in a obe from a Wannier tb model
  one_body_elements_tb obe_tb = one_body_elements_from_wannier90("./ref_data_lfs/sr2moo4", spin_kind_e::NonPolarized, atomic_shells);
  double beta                 = 10.;
  auto iw_mesh                = mesh::imfreq{beta, triqs::mesh::Fermion, 51};

  // make a zero self energy
  long n_sigma = 2;
  long n_atoms = 1;

  // make a self energy, call self energy interfaces --------------------
  auto Sigma_single = make_random_self(obe_tb.H[0].n_orbitals(), beta);
  auto Sigma_dyn    = make_block2_gf(n_atoms, n_sigma, Sigma_single);
  nda::array<nda::matrix<dcomplex>, 2> Sigma_static(1, n_sigma);
  // spin up/down, 3 orbitals in the single block, nonsense inf omega SE
  for (auto sigma : nda::range(n_sigma)) { Sigma_static(0, sigma) = nda::diag(nda::vector<dcomplex>{0.6, 0.5, 0.9}); }

  // read in some reference data
  // REFACTOR: put all the args so I can set verbotisty to false on find mu?
  auto [dft_density, obe_dft] = one_body_elements_from_dft_converter("./ref_data_lfs/sr2moo4_noshift.h5");
  double mu_dft               = 0;
  {
    //scoped_timer timer;
    //mu_dft = find_chemical_potential(dft_density, obe_dft, Sigma_dyn, Sigma_static);
    //auto gloc_dft               = gloc(obe_dft, mu_dft, Sigma_dyn, Sigma_static);
  }
  // run gloc, forcing gloc to use PTR on the same grid as the DFT calculation was done
  triqs::lattice::bz_int_options opt = {.run_ptr = false}; //{.k_grid = {40, 40, 40}, .k_grid_max = {40, 40, 40}, .run_adaptive = false};
  //auto gloc_tb                       = gloc(obe_tb, mu_dft, Sigma_dyn, Sigma_static, opt);

  // check mu finding
  double mu_tb = 0;
  {
    scoped_timer timer;
    mu_tb = find_chemical_potential(2.0, obe_tb, Sigma_dyn, Sigma_static, opt);
  }
  //EXPECT_NEAR(mu_tb, mu_dft, 1e-4);

  // density calculation
  //double n = density(obe_tb, mu_tb, Sigma_dyn, Sigma_static, opt);
  //EXPECT_NEAR(n, 2.0, 1e-4);

  // check gloc is the same
  //for (auto iw : iw_mesh) { EXPECT_COMPLEX_NEAR(gloc_tb(0, 0)[iw](0, 0), gloc_dft(0, 0)[iw](0, 0), 1e-5); }

  // check impurity levels are the same
  for (auto sigma : nda::range(n_sigma)) { EXPECT_ARRAY_NEAR(impurity_levels(obe_dft)(0, sigma), impurity_levels(obe_tb)(0, sigma), 1e-8); }
} */
#endif

MPI_TEST_MAIN
