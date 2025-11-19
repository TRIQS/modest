
// C.f. https://numpy.org/doc/1.21/reference/c-api/array.html#importing-the-api
#define PY_ARRAY_UNIQUE_SYMBOL _cpp2py_ARRAY_API
#ifndef CLAIR_C2PY_WRAP_GEN
#ifdef __clang__
// #pragma clang diagnostic ignored "-W#warnings"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wcast-function-type"
#pragma GCC diagnostic ignored "-Wcpp"
#endif

#define C2PY_VERSION_MAJOR 0
#define C2PY_VERSION_MINOR 1

#include <c2py/c2py.hpp>

using c2py::operator""_a;

// ==================== Wrapped classes =====================

// ==================== enums =====================

// ==================== module classes =====================

// ==================== module functions ====================

// U_matrix_in_spherical_basis
static auto const fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](long l, double U_int, double J_hund) { return triqs::U_matrix_in_spherical_basis(l, U_int, J_hund); }, "l", "U_int", "J_hund")};

// make_density_density
static auto const fun_1 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const std::vector<std::string> &tau_names, const std::vector<long> &dim_gamma, double U_int, double U_prime,
                                         double J_hund) { return triqs::make_density_density(tau_names, dim_gamma, U_int, U_prime, J_hund); },
                                      "tau_names", "dim_gamma", "U_int", "U_prime", "J_hund")};

// make_kanamori
static auto const fun_2 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const std::vector<std::string> &tau_names, const std::vector<long> &dim_gamma, double U_int, double U_prime, double J_hund, bool spin_flip,
      bool pair_hopping) { return triqs::make_kanamori(tau_names, dim_gamma, U_int, U_prime, J_hund, spin_flip, pair_hopping); },
   "tau_names", "dim_gamma", "U_int", "U_prime", "J_hund", "spin_flip"_a = true, "pair_hopping"_a = true)};

// make_slater
static auto const fun_3 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const std::vector<std::string> &tau_names, const std::vector<long> &dim_gamma, double U_int, double J_hund,
      const nda::matrix<triqs::dcomplex> &spherical_to_dft, const std::optional<nda::matrix<triqs::dcomplex>> &dft_to_local) {
     return triqs::make_slater(tau_names, dim_gamma, U_int, J_hund, spherical_to_dft, dft_to_local);
   },
   "tau_names", "dim_gamma", "U_int", "J_hund", "spherical_to_dft", "dft_to_local")};

static const auto doc_d_0 = fun_0.doc(R"DOC(
Construct a four-index Coulomb tensor in the basis of spherical harmonics.

We typically construct the four-index Coulomb tensor in the basis of spherical harmonics,

.. math::

   U_{m_{1}m_{2}m_{3}m_{4}}^{\mathrm{spherical}} = \sum_{k=0}^{2l} F_{k} \alpha (l, k, m_{1}, m_{2}, m_{3}, m_{4}),

where :math:`F_{k}` are radial Slater integrals and :math:`\alpha(l, k, m_{1}, m_{2}, m_{3}, m_{4})` denote angular
Racah-Wigner numbers for a spherically symmetric interaction tensor.

Parameters
----------
l : {par_0}
   Angular quantum number.
U_int : {par_1}
   Screened Hubbard interaction.
J_hund : {par_2}
   Hund's coupling.

Returns
-------
{ret_0}
   Coulomb tensor.
)DOC",
                                      {{c2py::python_typename<long>()}, {c2py::python_typename<double>()}, {c2py::python_typename<double>()}},
                                      {c2py::python_typename<nda::array<double, 4>>()});
static const auto doc_d_1 = fun_1.doc(R"DOC(
Construct a density-density interaction Hamiltonian.

Create a density-density Hamiltonian

.. math::

   H_{\mathrm{int}} = \frac{1}{2} \sum_{(i\sigma)\neq(j\sigma^{\prime})} U_{ij}^{\sigma\sigma^{\prime}}n_{i\sigma}
   n_{j\sigma^{\prime}}.

Parameters
----------
tau_names : {par_0}
   Names of tau indices ['up', 'down'].
dim_gamma : {par_1}
   Dimension of the blocks :math:`\gamma`.
U_int : {par_2}
   Hubbard :math:`U`.
U_prime : {par_3}
   :math:`U'` (typically :math:`U' = U - 2J`).
J_hund : {par_4}
   Kanamori :math:`J`.

Returns
-------
{ret_0}
   Many-body operator representing the Hamiltonian.
)DOC",
                                      {{c2py::python_typename<const std::vector<std::string> &>()},
                                       {c2py::python_typename<const std::vector<long> &>()},
                                       {c2py::python_typename<double>()},
                                       {c2py::python_typename<double>()},
                                       {c2py::python_typename<double>()}},
                                      {c2py::python_typename<triqs::operators::many_body_operator>()});
static const auto doc_d_2 = fun_2.doc(R"DOC(
Construct a Hubbard-Kanamori Hamiltonian.

Create a Hubbard-Kanamori Hamiltonian using the density-density, spin-flip, and pair-hopping interactions,

.. math::

   H_{\mathrm{int}} = \frac{1}{2} \sum_{(i\sigma)\neq(j\sigma^{\prime})} U_{ij}^{\sigma\sigma^{\prime}}n_{i\sigma}
   n_{j\sigma^{\prime}} - \sum_{i\neq j}Jc_{i\uparrow}^{\dagger}c_{i\downarrow}c_{j\downarrow}^{\dagger}
   c_{j\uparrow} + \sum_{i\neq j} J c_{i\uparrow}^{\dagger}c_{i\downarrow}^{\dagger}c_{j\downarrow}c_{j\uparrow}.

Parameters
----------
tau_names : {par_0}
   Names of tau indices ['up', 'down'].
dim_gamma : {par_1}
   Dimension of the blocks :math:`\gamma`.
U_int : {par_2}
   Hubbard :math:`U`.
U_prime : {par_3}
   :math:`U'` (typically :math:`U' = U - 2J`).
J_hund : {par_4}
   Kanamori :math:`J`.
spin_flip : {par_5}
   Spin flip term.
pair_hopping : {par_6}
   Pair-hopping term.

Returns
-------
{ret_0}
   Many-body operator representing the Hamiltonian.
)DOC",
                                      {{c2py::python_typename<const std::vector<std::string> &>()},
                                       {c2py::python_typename<const std::vector<long> &>()},
                                       {c2py::python_typename<double>()},
                                       {c2py::python_typename<double>()},
                                       {c2py::python_typename<double>()},
                                       {c2py::python_typename<bool>()},
                                       {c2py::python_typename<bool>()}},
                                      {c2py::python_typename<triqs::operators::many_body_operator>()});
static const auto doc_d_3 = fun_3.doc(R"DOC(
Construct a Slater Hamiltonian.

Create a Slater Hamiltonian using fully rotationally-invariant four-index interactions:

.. math::

   H_{\mathrm{int}} = \frac{1}{2} \sum_{ijkl, \sigma\sigma^{\prime}} U_{ijkl}c^{\dagger}_{i\sigma}
   c^{\dagger}_{j\sigma^{\prime}}c_{l\sigma^{\prime}}c_{k\sigma}.

Parameters
----------
tau_names : {par_0}
   Names of tau indices ['up', 'down'].
dim_gamma : {par_1}
   Dimension of the blocks :math:`\gamma`.
U_int : {par_2}
   Hubbard :math:`U`.
J_hund : {par_3}
   Hund's :math:`J`.
spherical_to_dft : {par_4}
   Rotation matrices from spherical :math:`Y_l^m` basis to DFT orbital basis.
dft_to_local : {par_5}
   Rotation matrices from DFT basis to the local impurity basis.
)DOC",
                                      {{c2py::python_typename<const std::vector<std::string> &>()},
                                       {c2py::python_typename<const std::vector<long> &>()},
                                       {c2py::python_typename<double>()},
                                       {c2py::python_typename<double>()},
                                       {c2py::python_typename<const nda::matrix<triqs::dcomplex> &>()},
                                       {c2py::python_typename<const std::optional<nda::matrix<triqs::dcomplex>> &>()}});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"U_matrix_in_spherical_basis", (PyCFunction)c2py::pyfkw<fun_0>, METH_VARARGS | METH_KEYWORDS, doc_d_0.c_str()},
   {"make_density_density", (PyCFunction)c2py::pyfkw<fun_1>, METH_VARARGS | METH_KEYWORDS, doc_d_1.c_str()},
   {"make_kanamori", (PyCFunction)c2py::pyfkw<fun_2>, METH_VARARGS | METH_KEYWORDS, doc_d_2.c_str()},
   {"make_slater", (PyCFunction)c2py::pyfkw<fun_3>, METH_VARARGS | METH_KEYWORDS, doc_d_3.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "hamiltonians",                                                                                                  /* name of module */
   R"RAWDOC(Functions and utilities to construct interaction Hamiltonians using TRIQS many-body operators.)RAWDOC", /* module documentation, may be NULL */
   -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
   module_methods,
   NULL,
   NULL,
   NULL,
   NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_hamiltonians() {

  if (not c2py::check_python_version("hamiltonians")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;

  return m;
}
#endif
// CLAIR_WRAP_GEN
