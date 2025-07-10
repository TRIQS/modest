#include "app4triqs/app4triqs.hpp"

#include <c2py/c2py.hpp>

using namespace std::string_literals;
using app4triqs::toto;
template <> struct c2py::arithmetic<toto, c2py::OpName::Add> : std::tuple<triplet<toto, toto, toto>> {};

namespace c2py_module {

  auto documentation = "Sample documentation for app4triqs module";
  auto match_names   = "app4triqs::";
  auto package_name  = "app4triqs";

} // namespace c2py_module
