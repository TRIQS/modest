
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

template <> constexpr bool c2py::is_wrapped<triqs::modest::spectral_function_kw> = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::spectral_function_w>  = true;

// ==================== enums =====================

// ==================== module classes =====================

template <> inline constexpr auto c2py::tp_name<triqs::modest::spectral_function_kw> = "triqs_modest.post_processing.SpectralFunctionKw";

static int synth_constructor_0(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::spectral_function_kw.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<triqs::modest::spectral_function_kw> *)self)->_c = new triqs::modest::spectral_function_kw{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::spectral_function_kw from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<triqs::modest::spectral_function_kw> *)self)->_c);
  de("data", self_c.data, false);
  de("proj_data", self_c.proj_data, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<triqs::modest::spectral_function_kw> = synth_constructor_0;

template <>
const std::string c2py::tp_ctor_doc<triqs::modest::spectral_function_kw> =
   c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
data : {par_0}

proj_data : {par_1}

)DOC",
                      "par", {c2py::python_typename<nda::array<double, 3>>(), c2py::python_typename<nda::array<double, 5>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::spectral_function_kw>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_0 = R"DOC(:math:`A^\sigma(k,\omega)`.)DOC";
constexpr auto doc_member_1 = R"DOC(:math:`A^\sigma(k,\omega)`.)DOC";
static PyObject *prop_get_dict_0(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<triqs::modest::spectral_function_kw> *)self)->_c);
  c2py::pydict dic;
  dic["data"]      = self_c.data;
  dic["proj_data"] = self_c.proj_data;
  return dic.new_ref();
}

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::spectral_function_kw>[] = {
   c2py::getsetdef_from_member<&triqs::modest::spectral_function_kw::data, triqs::modest::spectral_function_kw>("data", doc_member_0),
   c2py::getsetdef_from_member<&triqs::modest::spectral_function_kw::proj_data, triqs::modest::spectral_function_kw>("proj_data", doc_member_1),
   {"__dict__", (getter)prop_get_dict_0, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<triqs::modest::spectral_function_kw> =
   R"DOC(Store data of spectral functions.)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<triqs::modest::spectral_function_kw>;
template <> inline constexpr auto c2py::tp_name<triqs::modest::spectral_function_w> = "triqs_modest.post_processing.SpectralFunctionW";

static int synth_constructor_1(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::spectral_function_w.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<triqs::modest::spectral_function_w> *)self)->_c = new triqs::modest::spectral_function_w{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::spectral_function_w from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<triqs::modest::spectral_function_w> *)self)->_c);
  de("total", self_c.total, false);
  de("per_theta", self_c.per_theta, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<triqs::modest::spectral_function_w> = synth_constructor_1;

template <>
const std::string c2py::tp_ctor_doc<triqs::modest::spectral_function_w> =
   c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
total : {par_0}

per_theta : {par_1}

)DOC",
                      "par", {c2py::python_typename<nda::array<double, 2>>(), c2py::python_typename<nda::array<double, 4>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::spectral_function_w>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_2 = R"DOC(:math:`A^\sigma(\omega)`.)DOC";
constexpr auto doc_member_3 = R"DOC(:math:`A^{\sigma}_{mm'}(\omega)`.)DOC";
static PyObject *prop_get_dict_1(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<triqs::modest::spectral_function_w> *)self)->_c);
  c2py::pydict dic;
  dic["total"]     = self_c.total;
  dic["per_theta"] = self_c.per_theta;
  return dic.new_ref();
}

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::spectral_function_w>[] = {
   c2py::getsetdef_from_member<&triqs::modest::spectral_function_w::total, triqs::modest::spectral_function_w>("total", doc_member_2),
   c2py::getsetdef_from_member<&triqs::modest::spectral_function_w::per_theta, triqs::modest::spectral_function_w>("per_theta", doc_member_3),
   {"__dict__", (getter)prop_get_dict_1, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<triqs::modest::spectral_function_w> =
   R"DOC(Store data of spectral functions.)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<triqs::modest::spectral_function_w>;

// ==================== module functions ====================

// projected_spectral_function
static auto const fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::modest::one_body_elements_on_grid &obe_theta, const triqs::modest::downfolding_projector &Proj, double mu,
                 const triqs::gfs::block2_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &Sigma_w,
                 double broadening) { return triqs::modest::projected_spectral_function(obe_theta, Proj, mu, Sigma_w, broadening); },
              "obe_theta", "Proj", "mu", "Sigma_w", "broadening"_a = 0.01)};

// spectral_function
static auto const fun_1 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const triqs::modest::one_body_elements_on_grid &obe, double mu,
                                         const triqs::gfs::block2_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &Sigma_w,
                                         double broadening) { return triqs::modest::spectral_function(obe, mu, Sigma_w, broadening); },
                                      "obe", "mu", "Sigma_w", "broadening"_a = 0.01)};

// spectral_function_on_high_symmetry_path
static auto const fun_2 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::modest::one_body_elements_on_grid &obe, double mu,
                 const triqs::gfs::block2_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &Sigma_w,
                 double broadening) { return triqs::modest::spectral_function_on_high_symmetry_path(obe, mu, Sigma_w, broadening); },
              "obe", "mu", "Sigma_w", "broadening"_a = 0.01)};

static const auto doc_d_0 = fun_0.doc(R"DOC(
Compute the atom- and orbital-resolved spectral function (interacting density of states).

Parameters
----------
obe_theta : {par_0}
   One-body elements on grid created from one_body_elements_with_theta_projectors.
Proj : {par_1}
   Downfolding projector defined in the correlated space using to upfold the DMFT self-energies.
mu : {par_2}
   Chemical potential.
Sigma_w : {par_3}
   Self-energy in real-frequencies.
broadening : {par_4}
   Spectral broadening.

Returns
-------
{ret_0}
   Atom- and orbital-resolved spectral function.
)DOC",
                                      {{c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()},
                                       {c2py::python_typename<const triqs::modest::downfolding_projector &>()},
                                       {c2py::python_typename<double>()},
                                       {c2py::python_typename<const triqs::gfs::block2_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &>()},
                                       {c2py::python_typename<double>()}},
                                      {c2py::python_typename<triqs::modest::spectral_function_w>()});
static const auto doc_d_1 = fun_1.doc(R"DOC(
Compute the k-summed band-resolved spectral function matrix.

The returned array has shape (n_sigma, n_omega, n_bands, n_bands), with

\f[
  A(\omega) = -\frac{1}{2\pi i}\left[G(\omega) - G^{\dagger}(\omega)\right].
\f]

Parameters
----------
obe : {par_0}
   One-body elements on grid.
mu : {par_1}
   Chemical potential.
Sigma_w : {par_2}
   Self-energy in real-frequencies.
broadening : {par_3}
   Spectral broadening.

Returns
-------
{ret_0}
   Band-resolved spectral function matrix.
)DOC",
                                      {{c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()},
                                       {c2py::python_typename<double>()},
                                       {c2py::python_typename<const triqs::gfs::block2_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &>()},
                                       {c2py::python_typename<double>()}},
                                      {c2py::python_typename<nda::array<double, 4>>()});
static const auto doc_d_2 = fun_2.doc(R"DOC(
Compute momentum-resolved spectral function :math:`A^\sigma(k, \omega)` along high-symmetry path.

Parameters
----------
obe : {par_0}
   One-body elements on grid created from one_body_elements_on_high_symmetry_path.
mu : {par_1}
   Chemical potential.
Sigma_w : {par_2}
   Self-energy in real-frequencies.
broadening : {par_3}
   Spectral broadening.

Returns
-------
{ret_0}
   Momentum-resolved spectral function.
)DOC",
                                      {{c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()},
                                       {c2py::python_typename<double>()},
                                       {c2py::python_typename<const triqs::gfs::block2_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &>()},
                                       {c2py::python_typename<double>()}},
                                      {c2py::python_typename<triqs::modest::spectral_function_kw>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"projected_spectral_function", (PyCFunction)c2py::pyfkw<fun_0>, METH_VARARGS | METH_KEYWORDS, doc_d_0.c_str()},
   {"spectral_function", (PyCFunction)c2py::pyfkw<fun_1>, METH_VARARGS | METH_KEYWORDS, doc_d_1.c_str()},
   {"spectral_function_on_high_symmetry_path", (PyCFunction)c2py::pyfkw<fun_2>, METH_VARARGS | METH_KEYWORDS, doc_d_2.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "post_processing", /* name of module */
                                        R"RAWDOC(Methods to compute spectral functions.

The momentum-resolved spectral function from the lattice Green's function:

.. math::

    A(\mathbf{k}, \omega) = -\frac{1}{\pi} \mathrm{Im} \mathrm{Tr}G_{\nu\nu'}(\mathbf{k}, \omega).

The atom- and orbital-resolved :math:`k`-summed spectral function:

.. math::

    [A(\omega)]_{mm'} = -\frac{1}{\pi} \mathrm{Im} \Big [ \sum_{\mathbf{k}} \Theta_{m\nu}G_{\nu\nu'}(\mathbf{k}, \omega)\Theta_{m'\nu'}^{\dagger} \Big ].)RAWDOC", /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_post_processing() {

  if (not c2py::check_python_version("post_processing")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::spectral_function_kw>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::spectral_function_w>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
  c2py::add_type_object_to_main<triqs::modest::spectral_function_kw>("SpectralFunctionKw", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::spectral_function_w>("SpectralFunctionW", m, conv_table);

  return m;
}
#endif
// CLAIR_WRAP_GEN
