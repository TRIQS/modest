
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

// ==================== enums =====================

// ==================== module classes =====================

// ==================== module functions ====================

// analyze_degenerate_blocks
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &Gimp,
                 double threshold) { return triqs::modest::analyze_degenerate_blocks<triqs::mesh::dlr_imfreq>(Gimp, threshold); },
              "Gimp", "threshold"_a = 1.e-5),
   c2py::cfun([](const triqs::gfs::block_gf<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued> &Gimp,
                 double threshold) { return triqs::modest::analyze_degenerate_blocks<triqs::mesh::dlr_imtime>(Gimp, threshold); },
              "Gimp", "threshold"_a = 1.e-5),
   c2py::cfun([](const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Gimp,
                 double threshold) { return triqs::modest::analyze_degenerate_blocks<triqs::mesh::imfreq>(Gimp, threshold); },
              "Gimp", "threshold"_a = 1.e-5),
   c2py::cfun([](const triqs::gfs::block_gf<triqs::mesh::imtime, triqs::gfs::matrix_valued> &Gimp,
                 double threshold) { return triqs::modest::analyze_degenerate_blocks<triqs::mesh::imtime>(Gimp, threshold); },
              "Gimp", "threshold"_a = 1.e-5)};

static const auto _c2py_doc_0 =
   _c2py_fun_0.doc(R"DOC(
Find the generate blocks of a block GF by analyzing :math:`G(\tau=0)` or :math:`G(i\omega_0)` using the
union-find algorithm.

We use the union-find algorithm to group the blocks of the block GF (matrices) into equivalence
classes based on approximate numerical equality. If two matrices are approximately equal, their blocks are united
into the same set. The final result is a partitioning of the blocks (matrices) into degenerate groups.

Parameters
----------
Gimp : {par_0}
   Block Green's function.
threshold : {par_1}
   Tolerance for equivalent of blocks.

Returns
-------
{ret_0}
   A list of equivalent blocks.
)DOC",
                   {{c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imtime, triqs::gfs::matrix_valued> &>()},
                    {c2py::python_typename<double>()}},
                   {c2py::python_typename<std::vector<std::vector<long>>>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"analyze_degenerate_blocks", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "analyze_gfs", /* name of module */
   R"RAWDOC(Methods to analyze the degenerate blocks of a Green's function and use those blocks to symmetrize the Green's function object.)RAWDOC", /* module documentation, may be NULL */
   -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
   module_methods,
   NULL,
   NULL,
   NULL,
   NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_analyze_gfs() {

  if (not c2py::check_python_version("analyze_gfs")) return NULL;

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
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)

#undef _add_type

  return m;
}
#endif
// CLAIR_WRAP_GEN
