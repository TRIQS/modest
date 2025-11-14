#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_misc_GUARDS
#define C2PY_HXX_DECLARATION_misc_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::modest::atomic_orbs>         = true;
template <> inline constexpr auto c2py::tp_name<triqs::modest::atomic_orbs>     = "triqs_modest.misc.AtomicOrbs";
template <> constexpr bool c2py::is_wrapped<triqs::tb::superlattice>            = true;
template <> inline constexpr auto c2py::tp_name<triqs::tb::superlattice>        = "triqs_modest.misc.Superlattice";
template <> constexpr bool c2py::is_wrapped<triqs::tb::tb_hamiltonian>          = true;
template <> inline constexpr auto c2py::tp_name<triqs::tb::tb_hamiltonian>      = "triqs_modest.misc.TbHamiltonian";
template <> constexpr bool c2py::is_wrapped<triqs::lattice::bz_int_options>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::lattice::bz_int_options> = "triqs_modest.misc.BzIntOptions";
#endif