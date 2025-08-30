#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_embedding_GUARDS
#define C2PY_HXX_DECLARATION_embedding_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::modest::embedding>                  = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::embedding>              = "triqs_modest.embedding.Embedding";
template <> constexpr bool c2py::is_wrapped<triqs::modest::embedding::imp_block_t>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::embedding::imp_block_t> = "triqs_modest.embedding.ImpBlockT";
#endif