#include <c2py/c2py.hpp>

#include "triqs_modest/downfolding.hpp"
#include "triqs_modest/loaders.hpp"
#include "triqs_modest/embedding.hpp"
#include "triqs_modest/hamiltonians.hpp"
#include "triqs_modest/root_finder.hpp"
#include "triqs_modest/double_counting.hpp"
#include "triqs_modest/gloc_fixed_grid.hpp"
#include "triqs_modest/density.hpp"
#include "triqs_modest/postprocess.hpp"
#include "triqs_modest/checkpoint.hpp"
#include "triqs_modest/degenerate_blocks.hpp"
#include "triqs_modest/obe_tb.hpp"
#include "triqs/lattice/superlattice.hpp"

namespace c2py_module {
  using DMFTCheckpoint = triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>;
} // namespace c2py_module

#include "module.wrap.cxx"
