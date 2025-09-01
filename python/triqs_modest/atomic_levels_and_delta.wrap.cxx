
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

// hybridization
static auto const fun_0 = c2py::dispatcher_f_kw_t{c2py::cfun(
                                                     [](const std::vector<nda::matrix<triqs::dcomplex>> &epsilon_levels,
                                                        const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Gloc,
                                                        const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Sigma_dynamic,
                                                        const std::vector<nda::matrix<triqs::dcomplex>> &Sigma_static) {
                                                       return triqs::modest::hybridization(epsilon_levels, Gloc, Sigma_dynamic, Sigma_static);
                                                     },
                                                     "epsilon_levels", "Gloc", "Sigma_dynamic", "Sigma_static"),
                                                  c2py::cfun(
                                                     [](const std::vector<nda::matrix<triqs::dcomplex>> &epsilon_levels,
                                                        const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &Gloc,
                                                        const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &Sigma_dynamic,
                                                        const std::vector<nda::matrix<triqs::dcomplex>> &Sigma_static) {
                                                       return triqs::modest::hybridization(epsilon_levels, Gloc, Sigma_dynamic, Sigma_static);
                                                     },
                                                     "epsilon_levels", "Gloc", "Sigma_dynamic", "Sigma_static"),
                                                  c2py::cfun(
                                                     [](const std::vector<nda::matrix<triqs::dcomplex>> &epsilon_levels,
                                                        const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Gloc) {
                                                       return triqs::modest::hybridization(epsilon_levels, Gloc);
                                                     },
                                                     "epsilon_levels", "Gloc"),
                                                  c2py::cfun(
                                                     [](const std::vector<nda::matrix<triqs::dcomplex>> &epsilon_levels,
                                                        const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &Gloc) {
                                                       return triqs::modest::hybridization(epsilon_levels, Gloc);
                                                     },
                                                     "epsilon_levels", "Gloc")};

// impurity_levels
static auto const fun_1 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const triqs::modest::one_body_elements_on_grid &obe) { return triqs::modest::impurity_levels(obe); }, "obe"),
                           c2py::cfun([](const triqs::modest::one_body_elements_tb &obe) { return triqs::modest::impurity_levels(obe); }, "obe")};

static const auto doc_d_0 = fun_0.doc(
   R"DOC(
[1, 2] Compute the hybridization function from the effective impurity levels, the local Green's function, and the impurity self-energy.

------

[3, 4] Compute the hybridization function from the effective impurity levels and the local Green's function.

------

Parameters
----------
epsilon_levels : {par_0}
   The effective impurity levels.
Gloc : {par_1}
   The local Green's function of the impurity.
Sigma_dynamic : {par_2}
   The frequency dependent part of the impurity self-energy.
Sigma_static : {par_3}
   The static term of the impurity self-energy.

Returns
-------
[1, 3] : {ret_0}
   Hybridization function :math:`\Delta`.

[2, 4] : {ret_1}
   Hybridization function :math:`\Delta`.
)DOC",
   std::vector<std::string>{
      c2py::join(std::vector<std::string>{c2py::python_typename<const std::vector<nda::matrix<triqs::dcomplex>> &>()}, ", "),
      c2py::join(std::vector<std::string>{c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>(),
                                          c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &>()},
                 ", "),
      c2py::join(std::vector<std::string>{c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>(),
                                          c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &>()},
                 ", "),
      c2py::join(std::vector<std::string>{c2py::python_typename<const std::vector<nda::matrix<triqs::dcomplex>> &>()}, ", ")},
   std::vector<std::string>{
      std::vector<std::string>{c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>>(),
                               c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>>()}});
static const auto doc_d_1 =
   fun_1.doc(R"DOC(
Compute the atomic (impurity) levels from an obe.

Parameters
----------
obe : {par_0}
   One-body elements.

Returns
-------
{ret_0}
   Impurity levels stored in the format [n_atoms, n_sigma].
)DOC",
             std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>(),
                                                                          c2py::python_typename<const triqs::modest::one_body_elements_tb &>()},
                                                 ", ")},
             std::vector<std::string>{std::vector<std::string>{c2py::python_typename<nda::array<nda::matrix<triqs::dcomplex>, 2>>()}});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"hybridization", (PyCFunction)c2py::pyfkw<fun_0>, METH_VARARGS | METH_KEYWORDS, doc_d_0.c_str()},
   {"impurity_levels", (PyCFunction)c2py::pyfkw<fun_1>, METH_VARARGS | METH_KEYWORDS, doc_d_1.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "atomic_levels_and_delta", /* name of module */
                                        R"RAWDOC(Impurity levels and hybridization function.

Compute the hybridziation function

.. math::

    [ \Delta^{\sigma} ]_{m m'} = \omega  -\tilde{\varepsilon} - G_{\mathrm{loc}}^{-1}(\omega) - \Sigma_{\mathrm{imp}}(\omega),

where :math:`\omega` is a frequency, :math:`\tilde{\varepsilon}` are the effective local levels of the impurity, and 
:math:`\Sigma_{\mathrm{imp}}` is the impurity self-energy.

The effecitve local levels are defined as:

.. math::

    \tilde{\varepsilon} = \varepsilon - \mu - \Sigma_{\mathrm{DC}},

where :math:`\mu` is the chemical potential and :math:`\Sigma_{\mathrm{DC}}` is the double counting self-energy.
)RAWDOC",                                                          /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_atomic_levels_and_delta() {

  if (not c2py::check_python_version("atomic_levels_and_delta")) return NULL;

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
