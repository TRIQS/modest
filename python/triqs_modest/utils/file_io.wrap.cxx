
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

// --------- class _c2py_cls_0 -----------
using _c2py_cls_0                                            = triqs::modest::iteration_data;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs_modest.utils.file_io.IterationData";

static int synth_constructor_0(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::iteration_data.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_0> *)self)->_c = new _c2py_cls_0{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::iteration_data from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_0> *)self)->_c);
  de("mu", self_c.mu, false);
  de("Sigma_imp_list", self_c.Sigma_imp_list, false);
  de("Sigma_hartree_list", self_c.Sigma_hartree_list, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = synth_constructor_0;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> =
   c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
mu : {par_0}

Sigma_imp_list : {par_1}

Sigma_hartree_list : {par_2}

)DOC",
                      "par",
                      {c2py::python_typename<double>(), c2py::python_typename<std::vector<triqs::modest::block_gf_imfreq_t>>(),
                       c2py::python_typename<std::vector<triqs::modest::block_mat_t>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_0 = R"DOC()DOC";
constexpr auto _c2py_doc_member_1 = R"DOC()DOC";
constexpr auto _c2py_doc_member_2 = R"DOC()DOC";
static PyObject *prop_get_dict_0(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_0> *)self)->_c);
  c2py::pydict dic;
  dic["mu"]                 = self_c.mu;
  dic["Sigma_imp_list"]     = self_c.Sigma_imp_list;
  dic["Sigma_hartree_list"] = self_c.Sigma_hartree_list;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_0>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_0::mu, _c2py_cls_0>("mu", _c2py_doc_member_0),
   c2py::getsetdef_from_member<&_c2py_cls_0::Sigma_imp_list, _c2py_cls_0>("Sigma_imp_list", _c2py_doc_member_1),
   c2py::getsetdef_from_member<&_c2py_cls_0::Sigma_hartree_list, _c2py_cls_0>("Sigma_hartree_list", _c2py_doc_member_2),
   {"__dict__", (getter)prop_get_dict_0, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_0> =
   R"DOC(Minimal iteration data required to restart a DMFT loop.)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1                                            = triqs::modest::checkpoint<triqs::modest::iteration_data>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs_modest.utils.file_io.CheckpointBase";
static const auto _c2py_init_0                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_1, std::string>("dirname")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_1>    = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_1> = _c2py_init_0.doc(R"DOC(
Open existing or create new checkpoint directory.
)DOC");
// append
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_1 &self, const triqs::modest::iteration_data &x) -> decltype(auto) { return self.append(x); }, "self", "x")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {
   {"append", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0 = R"DOC()DOC";
static constexpr auto prop_doc_1 = R"DOC()DOC";
static constexpr auto prop_doc_2 = R"DOC()DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_1>[] = {

   {"dirname", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::checkpoint<triqs::modest::iteration_data>::dirname)>, nullptr, prop_doc_0,
    nullptr},
   {"empty", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::checkpoint<triqs::modest::iteration_data>::empty)>, nullptr, prop_doc_1,
    nullptr},
   {"last", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::checkpoint<triqs::modest::iteration_data>::last)>, nullptr, prop_doc_2, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_0(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_1, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_1> = {c2py::tpxx_size<_c2py_cls_1>, getitem_0, nullptr};

template <> const std::string c2py::tp_doc<_c2py_cls_1> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_1>;

// ==================== module functions ====================

//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "file_io", /* name of module */
                                        R"RAWDOC(File IO.

The checkpoint object allows users to seamlessly save their results within their DMFT calculation to an organized HDF5 file.)RAWDOC", /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_file_io() {

  if (not c2py::check_python_version("file_io")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_0>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_1>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_0, "IterationData");
  _add_type(_c2py_cls_1, "CheckpointBase");
#undef _add_type

  return m;
}
#endif
// CLAIR_WRAP_GEN
