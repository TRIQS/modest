#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_obe_GUARDS
#define C2PY_HXX_DECLARATION_obe_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::modest::local_space>                   = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::local_space>               = "triqs_modest.obe.LocalSpace";
template <> constexpr bool c2py::is_wrapped<triqs::modest::band_dispersion>               = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::band_dispersion>           = "triqs_modest.obe.BandDispersion";
template <> constexpr bool c2py::is_wrapped<triqs::modest::downfolding_projector>         = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::downfolding_projector>     = "triqs_modest.obe.DownfoldingProjector";
template <> constexpr bool c2py::is_wrapped<triqs::modest::one_body_elements_on_grid>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::one_body_elements_on_grid> = "triqs_modest.obe.OneBodyElementsOnGrid";
template <> constexpr bool c2py::is_wrapped<triqs::modest::one_body_elements_tb>          = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::one_body_elements_tb>      = "triqs_modest.obe.OneBodyElementsTb";
#endif