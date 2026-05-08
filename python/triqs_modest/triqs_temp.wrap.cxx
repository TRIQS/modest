
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
#include <c2py/serialization/h5.hpp>

using c2py::operator""_a;

// ==================== enums =====================

// ==================== module classes =====================

// --------- class _c2py_cls_0 -----------
using _c2py_cls_0                                            = triqs::tb::tb_hamiltonian;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs.triqs_temp.TbHamiltonian";
static auto _c2py_init_0                                     = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_0, std::vector<std::array<long, 3>>,
                                                           std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                                                        nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>("Rs", "hoppings"),
   c2py::c_constructor<_c2py_cls_0>()};
template <> constexpr initproc c2py::tp_init<_c2py_cls_0>    = c2py::pyfkw_constructor<_c2py_init_0>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_0> = _c2py_init_0.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_0 const &self,
         nda::basic_array_view<const double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>> k_list)
         -> decltype(auto) { return self.operator()(k_list); },
      "self", "k_list"),
   c2py::cmethod([](_c2py_cls_0 const &self, std::array<double, 3> ks) -> decltype(auto) { return self.operator()(ks); }, "self", "ks")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_0> = c2py::pyfkw<_c2py_fun_0>;

// eigenvalues
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 const &self,
                    nda::basic_array_view<double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>> k)
                    -> decltype(auto) { return self.eigenvalues(k); },
                 "self", "k")};

// eigenvectors
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 const &self,
                    nda::basic_array_view<double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>> k)
                    -> decltype(auto) { return self.eigenvectors(k); },
                 "self", "k")};

// get_R_idx
static auto const _c2py_fun_3 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 const &self, std::array<long, 3> R) -> decltype(auto) { return self.get_R_idx(R); }, "self", "R")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC()DOC");
static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC()DOC");
static const auto _c2py_doc_2 = _c2py_fun_2.doc(R"DOC()DOC");
static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {
   {"eigenvalues", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"eigenvectors", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"get_R_idx", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_0>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_h5<_c2py_cls_0>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_h5<_c2py_cls_0>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0 = R"DOC()DOC";
static constexpr auto prop_doc_1 = R"DOC()DOC";
static constexpr auto prop_doc_2 = R"DOC()DOC";
static constexpr auto prop_doc_3 = R"DOC()DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_0>[] = {

   {"elements", c2py::getter_from_method<c2py::castmc<>(&triqs::tb::tb_hamiltonian::elements)>, nullptr, prop_doc_0, nullptr},
   {"hoppings", c2py::getter_from_method<c2py::castmc<>(&triqs::tb::tb_hamiltonian::hoppings)>, nullptr, prop_doc_1, nullptr},
   {"n_R", c2py::getter_from_method_B<triqs::tb::tb_hamiltonian, c2py::castmc<>(&triqs::tb::fourier_polynomial<2, 3>::n_R)>, nullptr, prop_doc_2,
    nullptr},
   {"n_orbitals", c2py::getter_from_method<c2py::castmc<>(&triqs::tb::tb_hamiltonian::n_orbitals)>, nullptr, prop_doc_3, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1                                            = triqs::lattice::bz_int_options;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs.triqs_temp.BzIntOptions";

static int synth_constructor_0(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::lattice::bz_int_options.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_1> *)self)->_c = new _c2py_cls_1{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::lattice::bz_int_options from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_1> *)self)->_c);
  de("tolerance", self_c.tolerance, true);
  de("k_grid", self_c.k_grid, true);
  de("delta_k_grid", self_c.delta_k_grid, true);
  de("k_grid_max", self_c.k_grid_max, true);
  de("run_adaptive", self_c.run_adaptive, true);
  de("run_ptr", self_c.run_ptr, true);
  de("verbose", self_c.verbose, true);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_1> = synth_constructor_0;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_1> = c2py::replace_tags(
   R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
tolerance : {par_0}, default=1.e-3

k_grid : {par_1}, default={10, 10, 10}

delta_k_grid : {par_2}, default={2, 2, 2}

k_grid_max : {par_3}, default={20, 20, 20}

run_adaptive : {par_4}, default=true

run_ptr : {par_5}, default=true

verbose : {par_6}, default=false

)DOC",
   "par",
   {c2py::python_typename<double>(), c2py::python_typename<std::array<long, 3>>(), c2py::python_typename<std::array<long, 3>>(),
    c2py::python_typename<std::array<long, 3>>(), c2py::python_typename<bool>(), c2py::python_typename<bool>(), c2py::python_typename<bool>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_0 = R"DOC()DOC";
constexpr auto _c2py_doc_member_1 = R"DOC()DOC";
constexpr auto _c2py_doc_member_2 = R"DOC()DOC";
constexpr auto _c2py_doc_member_3 = R"DOC()DOC";
constexpr auto _c2py_doc_member_4 = R"DOC()DOC";
constexpr auto _c2py_doc_member_5 = R"DOC()DOC";
constexpr auto _c2py_doc_member_6 = R"DOC()DOC";
static PyObject *prop_get_dict_0(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_1> *)self)->_c);
  c2py::pydict dic;
  dic["tolerance"]    = self_c.tolerance;
  dic["k_grid"]       = self_c.k_grid;
  dic["delta_k_grid"] = self_c.delta_k_grid;
  dic["k_grid_max"]   = self_c.k_grid_max;
  dic["run_adaptive"] = self_c.run_adaptive;
  dic["run_ptr"]      = self_c.run_ptr;
  dic["verbose"]      = self_c.verbose;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_1>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_1::tolerance, _c2py_cls_1>("tolerance", _c2py_doc_member_0),
   c2py::getsetdef_from_member<&_c2py_cls_1::k_grid, _c2py_cls_1>("k_grid", _c2py_doc_member_1),
   c2py::getsetdef_from_member<&_c2py_cls_1::delta_k_grid, _c2py_cls_1>("delta_k_grid", _c2py_doc_member_2),
   c2py::getsetdef_from_member<&_c2py_cls_1::k_grid_max, _c2py_cls_1>("k_grid_max", _c2py_doc_member_3),
   c2py::getsetdef_from_member<&_c2py_cls_1::run_adaptive, _c2py_cls_1>("run_adaptive", _c2py_doc_member_4),
   c2py::getsetdef_from_member<&_c2py_cls_1::run_ptr, _c2py_cls_1>("run_ptr", _c2py_doc_member_5),
   c2py::getsetdef_from_member<&_c2py_cls_1::verbose, _c2py_cls_1>("verbose", _c2py_doc_member_6),
   {"__dict__", (getter)prop_get_dict_0, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> const std::string c2py::tp_doc<_c2py_cls_1> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_1>;

// ==================== module functions ====================

// find_chemical_potential
static auto const _c2py_fun_4 =
   c2py::dispatcher_f_kw_t{
      c2py::cfun(
         [](const double target_density, const triqs::tb::tb_hamiltonian &H_k,
            const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Sigma, const triqs::lattice::bz_int_options &opt,
            std::string method, double precision, bool verbosity) {
           return triqs::lattice::find_chemical_potential<triqs::mesh::imfreq>(target_density, H_k, Sigma, opt, method, precision, verbosity);
         },
         "target_density", "H_k", "Sigma", "opt", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = false),
      c2py::cfun(
         [](const double target_density, const triqs::tb::tb_hamiltonian &H_k,
            const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &Sigma, const triqs::lattice::bz_int_options &opt,
            std::string method, double precision, bool verbosity) {
           return triqs::lattice::find_chemical_potential<triqs::mesh::refreq>(target_density, H_k, Sigma, opt, method, precision, verbosity);
         },
         "target_density", "H_k", "Sigma", "opt", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = false),
      c2py::cfun(
         [](const double target_density, const triqs::tb::tb_hamiltonian &H_k,
            const triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Sigma, const triqs::lattice::bz_int_options &opt,
            std::string method, double precision, bool verbosity) {
           return triqs::lattice::find_chemical_potential<triqs::mesh::imfreq>(target_density, H_k, Sigma, opt, method, precision, verbosity);
         },
         "target_density", "H_k", "Sigma", "opt", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = false),
      c2py::cfun(
         [](const double target_density, const triqs::tb::tb_hamiltonian &H_k,
            const triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &Sigma, const triqs::lattice::bz_int_options &opt,
            std::string method, double precision, bool verbosity) {
           return triqs::lattice::find_chemical_potential<triqs::mesh::refreq>(target_density, H_k, Sigma, opt, method, precision, verbosity);
         },
         "target_density", "H_k", "Sigma", "opt", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = false)};

// gloc
static auto const _c2py_fun_5 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::mesh::imfreq &w_mesh, const triqs::tb::tb_hamiltonian &H_k, double mu,
                 const triqs::lattice::bz_int_options &opt) { return triqs::lattice::gloc<triqs::mesh::imfreq>(w_mesh, H_k, mu, opt); },
              "w_mesh", "H_k", "mu", "opt"),
   c2py::cfun([](const triqs::mesh::refreq &w_mesh, const triqs::tb::tb_hamiltonian &H_k, double mu,
                 const triqs::lattice::bz_int_options &opt) { return triqs::lattice::gloc<triqs::mesh::refreq>(w_mesh, H_k, mu, opt); },
              "w_mesh", "H_k", "mu", "opt"),
   c2py::cfun([](const triqs::tb::tb_hamiltonian &H_k, double mu, const triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Sigma,
                 const triqs::lattice::bz_int_options &opt) { return triqs::lattice::gloc<triqs::mesh::imfreq>(H_k, mu, Sigma, opt); },
              "H_k", "mu", "Sigma", "opt"),
   c2py::cfun([](const triqs::tb::tb_hamiltonian &H_k, double mu, const triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &Sigma,
                 const triqs::lattice::bz_int_options &opt) { return triqs::lattice::gloc<triqs::mesh::refreq>(H_k, mu, Sigma, opt); },
              "H_k", "mu", "Sigma", "opt"),
   c2py::cfun([](const triqs::tb::tb_hamiltonian &H_k, double mu, const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Sigma,
                 const triqs::lattice::bz_int_options &opt) { return triqs::lattice::gloc<triqs::mesh::imfreq>(H_k, mu, Sigma, opt); },
              "H_k", "mu", "Sigma", "opt"),
   c2py::cfun([](const triqs::tb::tb_hamiltonian &H_k, double mu, const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &Sigma,
                 const triqs::lattice::bz_int_options &opt) { return triqs::lattice::gloc<triqs::mesh::refreq>(H_k, mu, Sigma, opt); },
              "H_k", "mu", "Sigma", "opt")};

static const auto _c2py_doc_4 = _c2py_fun_4.doc(R"DOC()DOC");
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC()DOC");
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"find_chemical_potential", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"gloc", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_5.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "triqs_temp", /* name of module */
                                        R"RAWDOC(Temporary method of wrapping of necessary objects from the TRIQS library.
)RAWDOC",                                             /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_triqs_temp() {

  if (not c2py::check_python_version("triqs_temp")) return NULL;

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
  _add_type(_c2py_cls_0, "TbHamiltonian");
  _add_type(_c2py_cls_1, "BzIntOptions");
#undef _add_type

  c2py::pyref module = c2py::pyref::module("h5.formats");
  if (not module) return nullptr;
  c2py::pyref register_class = module.attr("register_class");

  register_h5_type<_c2py_cls_0>(register_class);

  return m;
}
#endif
// CLAIR_WRAP_GEN
