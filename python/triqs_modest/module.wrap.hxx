#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_module_GUARDS
#define C2PY_HXX_DECLARATION_module_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::modest::atomic_shell_t>                                                         = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::local_space>                                                            = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::band_dispersion>                                                        = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::downfolding_projector>                                                  = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::one_body_elements_on_grid>                                              = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::embedding>                                                              = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::embedding::imp_block_t>                                                 = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::dc_solver>                                                              = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::spectral_function_kw>                                                   = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::spectral_function_w>                                                    = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::initial_data>                                                           = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::iteration_data>                                                         = true;
template <> constexpr bool c2py::is_wrapped<triqs::lattice::superlattice>                                                          = true;
template <> constexpr bool c2py::is_wrapped<triqs::tb_hamiltonian>                                                                 = true;
template <> constexpr bool c2py::is_wrapped<triqs::lattice::bz_int_options>                                                        = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::one_body_elements>                                                      = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>> = true;
#endif