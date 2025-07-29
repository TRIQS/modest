#include <gtest/gtest.h>
#include <triqs/mesh/imfreq.hpp>
#include "triqs_modest/gloc_fixed_grid.hpp"
//#include "triqs_modest/local_space.hpp"
#include "triqs_modest/obe_tb.hpp"
#include "triqs_modest/utils/h5_proxy.hpp"
#include "triqs_modest/utils/to_vector.hpp"

using namespace triqs::modest;
using namespace triqs::gfs;
using namespace triqs;

TEST(obe_tb, svo_wannier90) { // NOLINT

  // set up atomic shells
  std::vector<atomic_shell_t> atomic_shells;
  // set up SVO shells, trying to keep the same order as the wannier hamiltonian does
  // try to make a shell with 3 d orbitals on the V atom
  long dim  = 3;
  long l    = 2;
  long type = 0;
  long atom = 0;
  atomic_shells.emplace_back(dim, l, type, atom);

  one_body_elements obe_tb = one_body_elements_from_wannier90("../../../test/c++/ref_data/svo", spin_kind_e::NonPolarized, atomic_shells);

  //double target_density = 1.5;
  double beta  = 10.;
  auto iw_mesh = mesh::imfreq{beta, triqs::mesh::Fermion, 11};

  //auto root                     = h5::proxy{"ref_data/SrVO3-cubic-t2g.ref.h5", 'r'};
  //std::vector<block_gf<mesh::imfreq, matrix_valued>> gloc_ref = to_vector<block_gf<mesh::imfreq, matrix_valued>>(sort_keys_as_int(root["ref_data"]["Gloc_DFT"]));
  //auto iw_mesh = gloc_ref[0].mesh();

  // full kgrid info is not written to reference files, it appears this one was ~12x12x12?
  triqs::bz_int::bz_int_options opt = {.k_grid_dims = {24, 24, 24}};

  // make a zero self energy
  auto Sigma = gfs::gf<mesh::imfreq>{iw_mesh, {dim, dim}};
  // TODO what is this function signature supposed to be? constructor just calls these n, p, gf?
  auto Sigma_block2 = make_block2_gf(1, 1, Sigma); // block, nsigma, fill with empty sigma

  // call gloc -- mu currently the one from the DFT calculation of the tb file directly
  auto gloc_tb = gloc(obe_tb, 12.6263, Sigma_block2, opt);

  // read in some reference data
  auto [target_density, obe_dft] = one_body_elements_from_dft_converter("ref_data/SrVO3-cubic-t2g.ref.h5");
  // FIXME: changing target density does not alter calculated value of mu...
  double mu     = find_chemical_potential(target_density, obe_dft, beta);
  auto gloc_dft = gloc(iw_mesh, obe_dft, mu);
  std::cout << mu << " " << target_density << std::endl;

  for (auto iw : iw_mesh) { std::cout << gloc_tb(0, 0)[iw](0, 0) << " " << gloc_dft(0, 0)[iw](0, 0) << std::endl; }

  // need to add final comparison with EXPECT once this is settled
}

// need to put SVO file in lfs

// Atoms here are indexed from their positions in the DFT files... that's a bit different than
// how things will be used in wannier90

// root finder has no tests?  --> test/c++/density_test.cpp ?? does it not check anything?

// add OMP/MPI to density.cpp

// could take from the file ref_data Gloc_DFT instead, use the mesh from that file
// density taken from ref_data/SrVO3-cubic-t2g.ref.h5/ref_data/total_density_zero_mu .. .need to go backwards?
//   auto n_ref                    = as<double>(root["ref_data"]["total_density_zero_mu"]);
//double mu = 15.208752510374; // ? is this mu or a density???

// fix duplication of inverse in nda::sup, which in triqs right now is nda::temp
// fix the same issue with duplicate gf_supp.hpp here, namespace in triqs is block2gf_temp

// Wannier converter: I need to try an SOC / spin pol version
// https://wannier90.readthedocs.io/en/latest/tutorials/tutorial_8/