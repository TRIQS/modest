#include <gtest/gtest.h>
#include "triqs_modest/local_space.hpp"
#include "triqs_modest/obe_tb.hpp"

using namespace triqs::modest;

TEST(obe_tb, simple_construct) { // NOLINT

  // set up atomic shells
  std::vector<atomic_shell_t> atomic_shells;
  // set up SVO shells, trying to keep the same order as the wannier hamiltonian does
  // try to make a shell with 3 d orbitals on the V atom
  long dim  = 3;
  long l    = 2;
  long type = 0;
  long atom = 0;
  atomic_shells.emplace_back(dim, l, type, atom);

  one_body_elements_tb obe_tb = one_body_elements_from_wannier90("./dft_data/svo", spin_kind_e::NonPolarized, atomic_shells);
}