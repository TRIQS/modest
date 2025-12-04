#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_triqs_temp_GUARDS
#define C2PY_HXX_DECLARATION_triqs_temp_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::tb::tb_hamiltonian>          = true;
template <> inline constexpr auto c2py::tp_name<triqs::tb::tb_hamiltonian>      = "triqs.triqs_temp.TbHamiltonian";
template <> constexpr bool c2py::is_wrapped<triqs::lattice::bz_int_options>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::lattice::bz_int_options> = "triqs.triqs_temp.BzIntOptions";
#endif