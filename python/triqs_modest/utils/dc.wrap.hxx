#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_dc_GUARDS
#define C2PY_HXX_DECLARATION_dc_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::modest::dc_solver>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::dc_solver> = "triqs_modest.utils.dc.DcSolver";
template <> constexpr bool c2py::is_wrapped<triqs::modest::spin_kind_e>   = true;
template <>
const std::map<triqs::modest::spin_kind_e, str_t> c2py::enum_to_string<triqs::modest::spin_kind_e> = {
   {triqs::modest::spin_kind_e::Polarized, "Polarized"},
   {triqs::modest::spin_kind_e::NonPolarized, "NonPolarized"},
   {triqs::modest::spin_kind_e::NonColinear, "NonColinear"}};
#endif