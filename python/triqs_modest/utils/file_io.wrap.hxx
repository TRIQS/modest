#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_file_io_GUARDS
#define C2PY_HXX_DECLARATION_file_io_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::modest::iteration_data>                            = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::iteration_data>                        = "triqs_modest.utils.file_io.IterationData";
template <> constexpr bool c2py::is_wrapped<triqs::modest::checkpoint<triqs::modest::iteration_data>> = true;
template <>
inline constexpr auto c2py::tp_name<triqs::modest::checkpoint<triqs::modest::iteration_data>> = "triqs_modest.utils.file_io.CheckpointBase";
#endif