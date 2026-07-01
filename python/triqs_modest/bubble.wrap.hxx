#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_bubble_GUARDS
#define C2PY_HXX_DECLARATION_bubble_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::modest::transport_distribution_t>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::transport_distribution_t> = "triqs_modest.bubble.TransportDistributionT";
template <> constexpr bool c2py::is_wrapped<triqs::modest::transport_function_t>         = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::transport_function_t>     = "triqs_modest.bubble.TransportFunctionT";
#endif