
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
using _c2py_cls_0                                            = triqs::modest::initial_data;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs_modest.utils.file_io.InitialData";

static int synth_constructor_0(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::initial_data.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_0> *)self)->_c = new _c2py_cls_0{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::initial_data from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_0> *)self)->_c);
  de("obe", self_c.obe, false);
  de("embed", self_c.embed, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = synth_constructor_0;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> =
   c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
obe : {par_0}

embed : {par_1}

)DOC",
                      "par", {c2py::python_typename<triqs::modest::one_body_elements_on_grid>(), c2py::python_typename<triqs::modest::embedding>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_0 = R"DOC(One-body elements.)DOC";
constexpr auto _c2py_doc_member_1 = R"DOC(Embedding object.)DOC";
static PyObject *prop_get_dict_0(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_0> *)self)->_c);
  c2py::pydict dic;
  dic["obe"]   = self_c.obe;
  dic["embed"] = self_c.embed;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_0>[] = {c2py::getsetdef_from_member<&_c2py_cls_0::obe, _c2py_cls_0>("obe", _c2py_doc_member_0),
                                                        c2py::getsetdef_from_member<&_c2py_cls_0::embed, _c2py_cls_0>("embed", _c2py_doc_member_1),
                                                        {"__dict__", (getter)prop_get_dict_0, nullptr, "", nullptr},
                                                        {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_0> =
   R"DOC(Initial data used for checkpointing.)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1                                            = triqs::modest::iteration_data;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs_modest.utils.file_io.IterationData";

static int synth_constructor_1(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::iteration_data.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_1> *)self)->_c = new _c2py_cls_1{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::iteration_data from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_1> *)self)->_c);
  de("mu", self_c.mu, false);
  de("Sigma_imp_list", self_c.Sigma_imp_list, false);
  de("Sigma_hartree_list", self_c.Sigma_hartree_list, false);
  de("Gimp_freq_list", self_c.Gimp_freq_list, false);
  de("Gimp_time_list", self_c.Gimp_time_list, false);
  de("Sigma_dc_list", self_c.Sigma_dc_list, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_1> = synth_constructor_1;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_1> = c2py::replace_tags(
   R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
mu : {par_0}

Sigma_imp_list : {par_1}

Sigma_hartree_list : {par_2}

Gimp_freq_list : {par_3}

Gimp_time_list : {par_4}

Sigma_dc_list : {par_5}

)DOC",
   "par",
   {c2py::python_typename<double>(), c2py::python_typename<std::vector<triqs::modest::block_gf_imfreq_t>>(),
    c2py::python_typename<triqs::modest::block_mat_t>(), c2py::python_typename<std::vector<triqs::modest::block_gf_imfreq_t>>(),
    c2py::python_typename<std::vector<triqs::modest::block_gf_imtime_t>>(), c2py::python_typename<std::vector<triqs::modest::block_gf_imfreq_t>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_2 = R"DOC(Chemical potential.)DOC";
constexpr auto _c2py_doc_member_3 = R"DOC(Impurities self-energies.)DOC";
constexpr auto _c2py_doc_member_4 = R"DOC(Sigma Hartree.)DOC";
constexpr auto _c2py_doc_member_5 = R"DOC(Impurities Green's functions.)DOC";
constexpr auto _c2py_doc_member_6 = R"DOC(Impurities Green's functions.)DOC";
constexpr auto _c2py_doc_member_7 = R"DOC(Impurities self-energies.)DOC";
static PyObject *prop_get_dict_1(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_1> *)self)->_c);
  c2py::pydict dic;
  dic["mu"]                 = self_c.mu;
  dic["Sigma_imp_list"]     = self_c.Sigma_imp_list;
  dic["Sigma_hartree_list"] = self_c.Sigma_hartree_list;
  dic["Gimp_freq_list"]     = self_c.Gimp_freq_list;
  dic["Gimp_time_list"]     = self_c.Gimp_time_list;
  dic["Sigma_dc_list"]      = self_c.Sigma_dc_list;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_1>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_1::mu, _c2py_cls_1>("mu", _c2py_doc_member_2),
   c2py::getsetdef_from_member<&_c2py_cls_1::Sigma_imp_list, _c2py_cls_1>("Sigma_imp_list", _c2py_doc_member_3),
   c2py::getsetdef_from_member<&_c2py_cls_1::Sigma_hartree_list, _c2py_cls_1>("Sigma_hartree_list", _c2py_doc_member_4),
   c2py::getsetdef_from_member<&_c2py_cls_1::Gimp_freq_list, _c2py_cls_1>("Gimp_freq_list", _c2py_doc_member_5),
   c2py::getsetdef_from_member<&_c2py_cls_1::Gimp_time_list, _c2py_cls_1>("Gimp_time_list", _c2py_doc_member_6),
   c2py::getsetdef_from_member<&_c2py_cls_1::Sigma_dc_list, _c2py_cls_1>("Sigma_dc_list", _c2py_doc_member_7),
   {"__dict__", (getter)prop_get_dict_1, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_1> =
   R"DOC(Iteration data used for checkpointing.)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_1>;
// --------- class _c2py_cls_2 -----------
using _c2py_cls_2                                            = triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_2>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_2> = "triqs_modest.utils.file_io.DMFTCheckpoint";
static const auto _c2py_init_0 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_2, std::string>("dirname"),
                           c2py::c_constructor<_c2py_cls_2, std::string, const triqs::modest::initial_data &>("dirname", "initial_data")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_2> = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_2> =
   _c2py_init_0.doc(R"DOC(
[1] Open checkpoint from an existing file (rw).

------

[2] Create a new checkpoint from initial_data.

------

Parameters
----------
dirname : {par_0}
   Name of the directory containing the checkpoint files.
initial_data : {par_1}
   Initial data to store in the checkpoint.
)DOC",
                    {{c2py::python_typename<std::string>()}, {c2py::python_typename<const triqs::modest::initial_data &>()}});
// append
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_2 &self, const triqs::modest::iteration_data &x) -> decltype(auto) { return self.append(x); }, "self", "x")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_2>[] = {
   {"append", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0 = R"DOC(Directory name containing the checkpoint files.)DOC";
static constexpr auto prop_doc_1 = R"DOC(Initial data.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_2>[] = {

   {"dirname",
    c2py::getter_from_method<c2py::castmc<>(&triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>::dirname)>,
    nullptr, prop_doc_0, nullptr},
   {"initial_data",
    c2py::getter_from_method<c2py::castmc<>(&triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>::initial_data)>,
    nullptr, prop_doc_1, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_0(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_2, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_2> = {c2py::tpxx_size<_c2py_cls_2>, getitem_0, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_2> =
   R"DOC(A checkpoint manager for logging data after each DMFT iteration.)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_2>;

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
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_2>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_0, "InitialData");
  _add_type(_c2py_cls_1, "IterationData");
  _add_type(_c2py_cls_2, "DMFTCheckpoint");
#undef _add_type

  return m;
}
#endif
// CLAIR_WRAP_GEN
