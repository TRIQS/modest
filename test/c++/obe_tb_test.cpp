#include <gtest/gtest.h>
#include <nda/basic_functions.hpp>
#include <nda/gtest_tools.hpp>
#include <nda/matrix_functions.hpp>
#include <triqs/arrays.hpp>
#include <triqs/mesh/imfreq.hpp>
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
  one_body_elements obe_tb = one_body_elements_from_wannier90("./ref_data/lco", spin_kind_e::NonPolarized, atomic_shells);
  double beta              = 10.;
  auto iw_mesh             = mesh::imfreq{beta, triqs::mesh::Fermion, 51};

  // make a zero self energy
  auto Sigma        = gfs::gf<mesh::imfreq>{iw_mesh, {dim, dim}};
  auto Sigma_block2 = make_block2_gf(1, 1, Sigma); // block, nsigma, fill with empty sigma
  nda::array<nda::matrix<dcomplex>, 2> Sigma_hartree(1, 1);
  Sigma_hartree(0,0) = nda::matrix<dcomplex>::zeros(1,1); // 1 orbital 

  // read in some reference data
  auto [dft_density, obe_dft] = one_body_elements_from_dft_converter("./ref_data/lco_wannier.ref.h5");
  double mu                   = find_chemical_potential(dft_density, obe_dft, beta);
  auto gloc_dft               = gloc(iw_mesh, obe_dft, mu);

  // run gloc, forcing gloc to use PTR on the same grid as the DFT calculation was done
  triqs::lattice::bz_int_options opt = {.k_grid_dims = {7, 7, 7}, .n_k_max = 8, .run_adaptive = false};
  auto gloc_tb                      = gloc(obe_tb, mu, Sigma_block2, Sigma_hartree, opt);

  // Check equivalence
  for (auto iw : iw_mesh) { EXPECT_COMPLEX_NEAR(gloc_tb(0, 0)[iw](0, 0), gloc_dft(0, 0)[iw](0, 0), 1e-5); }

  // density calculation
  //std::cout << density_obe(obe_tb, mu, Sigma_block2, Sigma_hartree) << std::endl;
  //std::cout << density(gloc_tb(0, 0)) << std::endl;
}