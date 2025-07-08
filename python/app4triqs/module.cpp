#include <c2py/c2py.hpp>

#include "app4triqs/app4triqs.hpp"

using namespace std::string_literals;
using app4triqs::toto;
template <> struct c2py::arithmetic<toto, c2py::OpName::Add> : std::tuple<triplet<toto, toto, toto>> {};

#include "module.wrap.cxx"
