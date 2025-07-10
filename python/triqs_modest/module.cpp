#include "triqs_modest/triqs_modest.hpp"

#include <c2py/c2py.hpp>

using namespace std::string_literals;
using triqs_modest::toto;
template <> struct c2py::arithmetic<toto, c2py::OpName::Add> : std::tuple<triplet<toto, toto, toto>> {};

namespace c2py_module {

  auto documentation = "Sample documentation for triqs_modest module";
  auto match_names   = "triqs_modest::";
  auto package_name  = "triqs_modest";

} // namespace c2py_module
