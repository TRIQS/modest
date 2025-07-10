
// C.f. https://numpy.org/doc/1.21/reference/c-api/array.html#importing-the-api
#define PY_ARRAY_UNIQUE_SYMBOL _cpp2py_ARRAY_API
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

#include "app4triqs_module.cpp"

#include <c2py/c2py.hpp>
#include <c2py/serialization/h5.hpp>

using c2py::operator""_a;

// ==================== Wrapped classes =====================

template <> constexpr bool c2py::is_wrapped<app4triqs::toto> = true;

// ==================== enums =====================

// ==================== module classes =====================

template <> inline const std::string c2py::cpp_name<app4triqs::toto>   = "app4triqs::toto";
template <> inline constexpr auto c2py::tp_name<app4triqs::toto>       = "app4triqs.app4triqs_module.Toto";
template <> inline constexpr const char *c2py::tp_doc<app4triqs::toto> = R"DOC(   A very useful and important class)DOC";

static auto init_0                                            = c2py::dispatcher_c_kw_t{c2py::c_constructor<app4triqs::toto>(), c2py::c_constructor<app4triqs::toto, int>("i_")};
template <> constexpr initproc c2py::tp_init<app4triqs::toto> = c2py::pyfkw_constructor<init_0>;
// f
static auto const fun_0 = c2py::dispatcher_f_kw_t{c2py::cmethod([](app4triqs::toto &self, int u) { return self.f(u); }, "self", "u")};

// get_i
static auto const fun_1 = c2py::dispatcher_f_kw_t{c2py::cmethod([](app4triqs::toto const &self) { return self.get_i(); }, "self")};

// hdf5_format
static auto const fun_2   = c2py::dispatcher_f_kw_t{c2py::cfun([]() { return app4triqs::toto::hdf5_format(); })};
static const auto doc_d_0 = fun_0.doc({R"DOC(   A simple function with :math:`G(
   )`
   
   Parameters
   ----------
   
   u:
   Nothing useful)DOC"});
static const auto doc_d_1 = fun_1.doc({R"DOC(   Simple accessor)DOC"});
static const auto doc_d_2 = fun_2.doc({R"DOC(   HDF5)DOC"});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<app4triqs::toto>[] = {
   {"f", (PyCFunction)c2py::pyfkw<fun_0>, METH_VARARGS | METH_KEYWORDS, doc_d_0.c_str()},
   {"get_i", (PyCFunction)c2py::pyfkw<fun_1>, METH_VARARGS | METH_KEYWORDS, doc_d_1.c_str()},
   {"hdf5_format", (PyCFunction)c2py::pyfkw<fun_2>, METH_VARARGS | METH_KEYWORDS | METH_STATIC, doc_d_2.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<app4triqs::toto>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<app4triqs::toto>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<app4triqs::toto>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<app4triqs::toto>[] = {

   {nullptr, nullptr, nullptr, nullptr, nullptr}};

// ==================== module functions ====================

// chain
static auto const fun_3 = c2py::dispatcher_f_kw_t{c2py::cfun([](int i, int j) { return app4triqs::chain(i, j); }, "i", "j")};

static const auto doc_d_3 = fun_3.doc({R"DOC(   Chain digits of two integers
   
   A set of functions that implement chaining
   
   Do I really need to explain more ?
   
   Parameters
   ----------
   
   i:
      The first integer
   j:
      The second integer
   
   Returns
   -------
   
   An integer containing the digits of both i and j)DOC"});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"chain", (PyCFunction)c2py::pyfkw<fun_3>, METH_VARARGS | METH_KEYWORDS, doc_d_3.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "app4triqs_module",                                         /* name of module */
                                        R"RAWDOC(Sample documentation for app4triqs module)RAWDOC", /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_app4triqs_module() {

  if (not c2py::check_python_version("app4triqs_module")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<app4triqs::toto>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
  c2py::add_type_object_to_main<app4triqs::toto>("Toto", m, conv_table);

  c2py::pyref module = c2py::pyref::module("h5.formats");
  if (not module) return nullptr;
  c2py::pyref register_class = module.attr("register_class");

  register_h5_type<app4triqs::toto>(register_class);

  return m;
}
