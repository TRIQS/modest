
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
using _c2py_cls_0                                            = triqs::modest::transport_distribution_t;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs_modest.bubble.TransportDistributionT";

static int synth_constructor_0(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::transport_distribution_t.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_0> *)self)->_c = new _c2py_cls_0{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::transport_distribution_t from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_0> *)self)->_c);
  de("Gamma", self_c.Gamma, false);
  de("Gamma_intra", self_c.Gamma_intra, false);
  de("Gamma_inter", self_c.Gamma_inter, false);
  de("omega_mesh", self_c.omega_mesh, false);
  de("Om_mesh", self_c.Om_mesh, false);
  de("directions", self_c.directions, false);
  de("spin_polarization", self_c.spin_polarization, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = synth_constructor_0;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> = c2py::replace_tags(
   R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
Gamma : {par_0}

Gamma_intra : {par_1}

Gamma_inter : {par_2}

omega_mesh : {par_3}

Om_mesh : {par_4}

directions : {par_5}

spin_polarization : {par_6}

)DOC",
   "par",
   {c2py::python_typename<nda::basic_array<double, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<nda::basic_array<double, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<nda::basic_array<double, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<nda::basic_array<double, 1, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<nda::basic_array<double, 1, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<std::vector<std::string>>(), c2py::python_typename<long>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_0>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_h5<_c2py_cls_0>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_h5<_c2py_cls_0>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_0 = R"DOC(Total :math:`\Gamma_{\alpha\beta}(\omega,\Omega)`: (n_dir, n_Om, n_omega).)DOC";
constexpr auto _c2py_doc_member_1 = R"DOC(Intraband (band-diagonal) contribution: (n_dir, n_Om, n_omega).)DOC";
constexpr auto _c2py_doc_member_2 = R"DOC(Interband contribution = total − intra: (n_dir, n_Om, n_omega).)DOC";
constexpr auto _c2py_doc_member_3 = R"DOC(Internal frequency grid :math:`\omega` (from Sigma_w's mesh): (n_omega,).)DOC";
constexpr auto _c2py_doc_member_4 = R"DOC(External frequency grid :math:`\Omega`: (n_Om,).)DOC";
constexpr auto _c2py_doc_member_5 = R"DOC(Direction labels (e.g. "xx", "xy"), aligned with the leading axis.)DOC";
constexpr auto _c2py_doc_member_6 = R"DOC(0 if a spin-degeneracy factor of 2 should be applied downstream, else 1.)DOC";
static PyObject *prop_get_dict_0(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_0> *)self)->_c);
  c2py::pydict dic;
  dic["Gamma"]             = self_c.Gamma;
  dic["Gamma_intra"]       = self_c.Gamma_intra;
  dic["Gamma_inter"]       = self_c.Gamma_inter;
  dic["omega_mesh"]        = self_c.omega_mesh;
  dic["Om_mesh"]           = self_c.Om_mesh;
  dic["directions"]        = self_c.directions;
  dic["spin_polarization"] = self_c.spin_polarization;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_0>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_0::Gamma, _c2py_cls_0>("Gamma", _c2py_doc_member_0),
   c2py::getsetdef_from_member<&_c2py_cls_0::Gamma_intra, _c2py_cls_0>("Gamma_intra", _c2py_doc_member_1),
   c2py::getsetdef_from_member<&_c2py_cls_0::Gamma_inter, _c2py_cls_0>("Gamma_inter", _c2py_doc_member_2),
   c2py::getsetdef_from_member<&_c2py_cls_0::omega_mesh, _c2py_cls_0>("omega_mesh", _c2py_doc_member_3),
   c2py::getsetdef_from_member<&_c2py_cls_0::Om_mesh, _c2py_cls_0>("Om_mesh", _c2py_doc_member_4),
   c2py::getsetdef_from_member<&_c2py_cls_0::directions, _c2py_cls_0>("directions", _c2py_doc_member_5),
   c2py::getsetdef_from_member<&_c2py_cls_0::spin_polarization, _c2py_cls_0>("spin_polarization", _c2py_doc_member_6),
   {"__dict__", (getter)prop_get_dict_0, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC(Kubo transport distribution :math:`\Gamma_{\alpha\beta}(\omega,\Omega)`.

.. math::

   \Gamma_{\alpha\beta}(\omega,\Omega) = \frac{1}{V} \sum_{\mathbf{k}}
   \mathrm{Tr}\left[ v_{\mathbf{k},\alpha} A_{\mathbf{k}}(\omega+\Omega)
   v_{\mathbf{k},\beta}  A_{\mathbf{k}}(\omega) \right],

with the band-basis spectral function :math:`A = -(G-G^\dagger)/(2\pi i)`. The result is decomposed into
total / intraband / interband contributions (band-diagonal vs off-diagonal in the dispersion basis).)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1                                            = triqs::modest::transport_function_t;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs_modest.bubble.TransportFunctionT";

static int synth_constructor_1(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::transport_function_t.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_1> *)self)->_c = new _c2py_cls_1{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::transport_function_t from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_1> *)self)->_c);
  de("Phi", self_c.Phi, false);
  de("omega_mesh", self_c.omega_mesh, false);
  de("directions", self_c.directions, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_1> = synth_constructor_1;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_1> = c2py::replace_tags(
   R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
Phi : {par_0}

omega_mesh : {par_1}

directions : {par_2}

)DOC",
   "par",
   {c2py::python_typename<nda::basic_array<double, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<nda::basic_array<double, 1, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<std::vector<std::string>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_1>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_h5<_c2py_cls_1>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_h5<_c2py_cls_1>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_7 = R"DOC(:math:`\Phi_{\alpha\beta}(\omega)`: (n_dir, n_omega).)DOC";
constexpr auto _c2py_doc_member_8 = R"DOC(Frequency grid :math:`\omega`: (n_omega,).)DOC";
constexpr auto _c2py_doc_member_9 = R"DOC(Direction labels, aligned with the leading axis.)DOC";
static PyObject *prop_get_dict_1(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_1> *)self)->_c);
  c2py::pydict dic;
  dic["Phi"]        = self_c.Phi;
  dic["omega_mesh"] = self_c.omega_mesh;
  dic["directions"] = self_c.directions;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_1>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_1::Phi, _c2py_cls_1>("Phi", _c2py_doc_member_7),
   c2py::getsetdef_from_member<&_c2py_cls_1::omega_mesh, _c2py_cls_1>("omega_mesh", _c2py_doc_member_8),
   c2py::getsetdef_from_member<&_c2py_cls_1::directions, _c2py_cls_1>("directions", _c2py_doc_member_9),
   {"__dict__", (getter)prop_get_dict_1, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_1> = R"DOC(Transport function :math:`\Phi_{\alpha\beta}(\omega) = \sum_{\mathbf{k}} \sum_n
v^{nn}_{\alpha} v^{nn}_{\beta}\, \delta(\omega - \varepsilon_n(\mathbf{k})) / V`.

The intraband (Drude) building block, evaluated with a Lorentzian-broadened delta and no
self-energy (bare DFT bands). Assumes a diagonal (band-basis) dispersion.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_1>;

// ==================== module functions ====================

// transport_distribution
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::modest::one_body_elements_on_grid &obe, double mu,
                 const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_w,
                 const nda::basic_array<double, 1, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &Om_mesh,
                 const std::vector<std::string> &directions,
                 double broadening) { return triqs::modest::transport_distribution(obe, mu, Sigma_w, Om_mesh, directions, broadening); },
              "obe", "mu", "Sigma_w", "Om_mesh", "directions", "broadening"_a = 0.01)};

// transport_function
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::modest::one_body_elements_on_grid &obe,
                 const nda::basic_array<double, 1, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &omega,
                 const std::vector<std::string> &directions,
                 double broadening) { return triqs::modest::transport_function(obe, omega, directions, broadening); },
              "obe", "omega", "directions", "broadening"_a = 0.01)};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(
   R"DOC(
Compute the Kubo transport distribution :math:`\Gamma_{\alpha\beta}(\omega,\Omega)`.

The band-basis spectral function :math:`A_{\mathbf{k}}(\omega)` is built with the rank-reduced Woodbury
identity (diagonal H(k) + rank-M self-energy); a matrix-valued H(k) is not supported. Requires
`obe.velocities` and `obe.cell_volume` (see
`one_body_elements_from_dft_converter(..., read_velocities=true)`). MPI-parallel over k-points.

Parameters
----------
obe : {par_0}
   One-body elements on grid, carrying the optional band velocities and cell volume.
mu : {par_1}
   Chemical potential.
Sigma_w : {par_2}
   Self-energy in real frequencies; its mesh defines the internal :math:`\omega` grid.
Om_mesh : {par_3}
   External frequency mesh :math:`\Omega` (must include 0.0 for DC transport).
directions : {par_4}
   Direction labels, e.g. {"xx","yy","zz","xy"}.
broadening : {par_5}
   Additional Lorentzian broadening η added to the frequency argument.

Returns
-------
{ret_0}
   The transport distribution (total/intra/inter) with its frequency meshes.
)DOC",
   {{c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>()},
    {c2py::python_typename<
       const nda::basic_array<double, 1, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()},
    {c2py::python_typename<const std::vector<std::string> &>()},
    {c2py::python_typename<double>()}},
   {c2py::python_typename<triqs::modest::transport_distribution_t>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(
   R"DOC(
Compute the transport function :math:`\Phi_{\alpha\beta}(\omega)` (bare DFT bands, no self-energy).

Parameters
----------
obe : {par_0}
   One-body elements on grid, carrying the optional band velocities and cell volume.
omega : {par_1}
   Frequency grid on which :math:`\Phi` is evaluated.
directions : {par_2}
   Direction labels, e.g. {"xx","yy","zz"}.
broadening : {par_3}
   Lorentzian broadening of the delta function.

Returns
-------
{ret_0}
   The transport function with its frequency mesh.
)DOC",
   {{c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()},
    {c2py::python_typename<
       const nda::basic_array<double, 1, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()},
    {c2py::python_typename<const std::vector<std::string> &>()},
    {c2py::python_typename<double>()}},
   {c2py::python_typename<triqs::modest::transport_function_t>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"transport_distribution", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"transport_function", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "bubble", /* name of module */
                                        R"RAWDOC(Bubble term of the current-current correlation function (the Kubo bubble).

The MPI-parallel k-sum kernels that evaluate the (dressed) current-current bubble and their result structs:

- ``transport_distribution`` -> ``transport_distribution_t`` (Gamma / Gamma_intra / Gamma_inter),
- ``transport_function`` -> ``transport_function_t`` (bare-band Phi).

The transport distribution

.. math::

    \Gamma_{\alpha\beta}(\omega,\Omega) = \frac{1}{V} \sum_{\mathbf{k}}
        \mathrm{Tr}\left[ v_{\mathbf{k},\alpha} A_{\mathbf{k}}(\omega+\Omega)
                          v_{\mathbf{k},\beta}  A_{\mathbf{k}}(\omega) \right],

with the band-basis spectral function :math:`A = -(G-G^\dagger)/(2\pi i)`. The frequency integration
producing the Onsager coefficients (conductivity, Seebeck, thermal conductivity) lives in
``triqs_modest.optics``.)RAWDOC",                 /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_bubble() {

  if (not c2py::check_python_version("bubble")) return NULL;

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
  _add_type(_c2py_cls_0, "TransportDistributionT");
  _add_type(_c2py_cls_1, "TransportFunctionT");
#undef _add_type

  c2py::pyref module = c2py::pyref::module("h5.formats");
  if (not module) return nullptr;
  c2py::pyref register_class = module.attr("register_class");

  register_h5_type<_c2py_cls_0>(register_class);
  register_h5_type<_c2py_cls_1>(register_class);

  return m;
}
#endif
// CLAIR_WRAP_GEN
