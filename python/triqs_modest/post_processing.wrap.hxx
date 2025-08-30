#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_post_processing_GUARDS
#define C2PY_HXX_DECLARATION_post_processing_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::modest::spectral_function_kw>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::spectral_function_kw> = "triqs_modest.post_processing.SpectralFunctionKw";
template <> constexpr bool c2py::is_wrapped<triqs::modest::spectral_function_w>      = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::spectral_function_w>  = "triqs_modest.post_processing.SpectralFunctionW";
#endif