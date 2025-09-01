
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

template <> constexpr bool c2py::is_wrapped<triqs::modest::dc_solver> = true;

// ==================== enums =====================

// ==================== module classes =====================

template <> inline constexpr auto c2py::tp_name<triqs::modest::dc_solver> = "triqs_modest.utils.dc.DcSolver";
static auto init_0 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<triqs::modest::dc_solver, long, std::string, double, double>("n_sigma", "method", "U_int", "J_hund")};
template <> constexpr initproc c2py::tp_init<triqs::modest::dc_solver> = c2py::pyfkw_constructor<init_0>;
template <>
const std::string c2py::tp_ctor_doc<triqs::modest::dc_solver> =
   init_0.doc(R"DOC(
Construct a double counting "solver".

Parameters
----------
n_sigma : {par_0}
   Dimension of the :math:`\sigma` index.
method : {par_1}
   Double counting formula (method) to call (options: `cFLL`, `sFLL`, `cAMF`, `sAMF`, `cHeld`).
U_int : {par_2}
   Hubbard :math:`U` to use in the DC formula.
J_hund : {par_3}
   Hund's coupling :math:`J` to use in the DC formula.
)DOC",
              std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<long>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<std::string>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", ")},
              std::vector<std::string>{});
// dc_energy
static auto const fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::dc_solver &self,
                    const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &gimp) { return self.dc_energy(gimp); },
                 "self", "gimp")};

// dc_self_energy
static auto const fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::dc_solver &self,
                    const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &gimp) { return self.dc_self_energy(gimp); },
                 "self", "gimp")};

static const auto doc_d_0 = fun_0.doc(
   R"DOC(
Compute the double counting correction to the energy.

Parameters
----------
gimp : {par_0}
   The impurity Green's function which is used to calculate the orbital-resolved density matrices to 
   evaluate the double counting formula.

Returns
-------
{ret_0}
   Double counting energy term.
)DOC",
   std::vector<std::string>{c2py::join(
      std::vector<std::string>{c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>()}, ", ")},
   std::vector<std::string>{std::vector<std::string>{c2py::python_typename<nda::matrix<double>>()}});
static const auto doc_d_1 = fun_1.doc(
   R"DOC(
Compute the double-counting self-energy.

Parameters
----------
gimp : {par_0}
   The impurity Green's function which is used to calculate the orbital-resolved density matrices to 
   evaluate the double counting formula.

Returns
-------
{ret_0}
   Double counting self-energy term.
)DOC",
   std::vector<std::string>{c2py::join(
      std::vector<std::string>{c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>()}, ", ")},
   std::vector<std::string>{std::vector<std::string>{c2py::python_typename<std::vector<nda::matrix<triqs::dcomplex>>>()}});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::dc_solver>[] = {
   {"dc_energy", (PyCFunction)c2py::pyfkw<fun_0>, METH_VARARGS | METH_KEYWORDS, doc_d_0.c_str()},
   {"dc_self_energy", (PyCFunction)c2py::pyfkw<fun_1>, METH_VARARGS | METH_KEYWORDS, doc_d_1.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::dc_solver>[] = {

   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<triqs::modest::dc_solver> = R"DOC(Double counting "solver".

It implements the double counting correction for DFT+DMFT, which is a phenomenlogical introduced double 
counting the interactions already taken into account at the mean-field level within DFT. This class implements 
several double counting formulas (all of which are functions of the density) relevant for different scenarios.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<triqs::modest::dc_solver>;

// ==================== module functions ====================

// dc_formulas
static auto const fun_2 = c2py::dispatcher_f_kw_t{c2py::cfun([](const std::string method, double N_tot, double N_sigma, long n_orb, double U,
                                                                double J) { return triqs::modest::dc_formulas(method, N_tot, N_sigma, n_orb, U, J); },
                                                             "method", "N_tot", "N_sigma", "n_orb", "U", "J")};

static const auto doc_d_2 = fun_2.doc(R"DOC(
Double counting formulas.

Parameters
----------
method : {par_0}
   Double counting formula to use.
N_tot : {par_1}
   Total density.
N_sigma : {par_2}
   Total density per spin.
n_orb : {par_3}
   Number of orbitals.
U : {par_4}
   Hubbard :math:`U`.
J : {par_5}
   Hund's coupling :math:`J`.

Returns
-------
{ret_0}
   :math:`\Sigma_{DC}` and :math:`E_{DC}`.
)DOC",
                                      std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<const std::string>()}, ", "),
                                                               c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", "),
                                                               c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", "),
                                                               c2py::join(std::vector<std::string>{c2py::python_typename<long>()}, ", "),
                                                               c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", "),
                                                               c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", ")},
                                      std::vector<std::string>{std::vector<std::string>{c2py::python_typename<std::pair<double, double>>()}});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"dc_formulas", (PyCFunction)c2py::pyfkw<fun_2>, METH_VARARGS | METH_KEYWORDS, doc_d_2.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "dc", /* name of module */
                                        R"RAWDOC(The implementation of the double counting correction within DFT+DMFT. 

DFT+DMFT requires subtracting a "double counting" term :math:`\Sigma_{\mathrm{DC}}` to avoid counting static correlations twice. 
ModEST abstracts this into a dedicated solver that computes :math:`\Sigma_{\mathrm{DC}}` based on dirrect formulas.

We abstract phenomenological term into an additional "solver" that takes as input a Green's function and returns the double counting 
correction to the self-energy and/or the doube counting correction to the total energy.
This solver is useful for cases when the double counting can not be absorbed into the chemical potential (e.g., charge self-consistent 
DFT+DMFT d-p models, etc.).)RAWDOC",          /* module documentation, may be NULL */
                                        -1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_dc() {

  if (not c2py::check_python_version("dc")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::dc_solver>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
  c2py::add_type_object_to_main<triqs::modest::dc_solver>("DcSolver", m, conv_table);

  return m;
}
#endif
// CLAIR_WRAP_GEN
