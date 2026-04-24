
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

template <>
std::map<triqs::modest::spin_kind_e, str_t> c2py::enum_to_string<triqs::modest::spin_kind_e> = {
   {triqs::modest::spin_kind_e::Polarized, "Polarized"},
   {triqs::modest::spin_kind_e::NonPolarized, "NonPolarized"},
   {triqs::modest::spin_kind_e::NonColinear, "NonColinear"}};

// ==================== module classes =====================

// --------- class _c2py_cls_0 -----------
using _c2py_cls_0                                            = triqs::modest::dc_solver;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs_modest.utils.dc.DcSolver";
static auto _c2py_init_0                                     = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_0, triqs::modest::spin_kind_e, std::string, double, double>("spin_kind", "method", "U_int", "J_hund")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> = _c2py_init_0.doc(R"DOC(
Construct a double counting "solver".

Parameters
----------
spin_kind : {par_0}
   Spin kind of the correlated space (Polarized, NonPolarized, NonColinear).
method : {par_1}
   Double counting formula (method) to call (options: `cFLL`, `sFLL`, `cAMF`, `sAMF`, `cHeld`).
U_int : {par_2}
   Hubbard :math:`U` to use in the DC formula.
J_hund : {par_3}
   Hund's coupling :math:`J` to use in the DC formula.
)DOC",
                                                                    {{c2py::python_typename<triqs::modest::spin_kind_e>()},
                                                                     {c2py::python_typename<std::string>()},
                                                                     {c2py::python_typename<double>()},
                                                                     {c2py::python_typename<double>()}});
// dc_energy
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 &self, const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &gimp)
                    -> decltype(auto) { return self.dc_energy(gimp); },
                 "self", "gimp"),
   c2py::cmethod(
      [](_c2py_cls_0 &self,
         const nda::basic_array<
            nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
            nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &density_matrix) -> decltype(auto) {
        return self.dc_energy(density_matrix);
      },
      "self", "density_matrix")};

// dc_self_energy
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 &self, const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &gimp)
                    -> decltype(auto) { return self.dc_self_energy(gimp); },
                 "self", "gimp"),
   c2py::cmethod(
      [](_c2py_cls_0 &self,
         const nda::basic_array<
            nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
            nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &density_matrix) -> decltype(auto) {
        return self.dc_self_energy(density_matrix);
      },
      "self", "density_matrix")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(
   R"DOC(
[1] Compute the double counting correction to the energy from a Green's function.

------

[2] Compute the double counting correction to the energy from a density matrix.

------

Parameters
----------
gimp : {par_0}
   The impurity Green's function which is used to calculate the orbital-resolved density matrices to
   evaluate the double counting formula.
density_matrix : {par_1}
   Density matrix with shape [n_blocks, n_sigma].

Returns
-------
{ret_0}
   Double counting energy :math:`E_{DC}`.
)DOC",
   {{c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>()},
    {c2py::python_typename<const nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()}},
   {c2py::python_typename<double>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(
   R"DOC(
[1] Compute the double-counting self-energy from a Green's function.

------

[2] Compute the double-counting self-energy from a density matrix.

------

Parameters
----------
gimp : {par_0}
   The impurity Green's function which is used to calculate the orbital-resolved density matrices to
   evaluate the double counting formula.
density_matrix : {par_1}
   Density matrix with shape [n_blocks, n_sigma].

Returns
-------
{ret_0}
   Double counting self-energy term.
)DOC",
   {{c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>()},
    {c2py::python_typename<const nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()}},
   {c2py::python_typename<std::vector<
      nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {
   {"dc_energy", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"dc_self_energy", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <>
const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC(Double counting "solver".

It implements the double counting correction for DFT+DMFT, which is a phenomenlogical introduced double
counting the interactions already taken into account at the mean-field level within DFT. This class implements
several double counting formulas (all of which are functions of the density) relevant for different scenarios.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;

// ==================== module functions ====================

// dc_formulas
static auto const _c2py_fun_2 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const std::string method, const double N_tot, const double N_sigma, const long n_orb, const double U,
                                         const double J) { return triqs::modest::dc_formulas(method, N_tot, N_sigma, n_orb, U, J); },
                                      "method", "N_tot", "N_sigma", "n_orb", "U", "J")};

static const auto _c2py_doc_2 = _c2py_fun_2.doc(R"DOC(
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
                                                {{c2py::python_typename<const std::string>()},
                                                 {c2py::python_typename<const double>()},
                                                 {c2py::python_typename<const double>()},
                                                 {c2py::python_typename<const long>()},
                                                 {c2py::python_typename<const double>()},
                                                 {c2py::python_typename<const double>()}},
                                                {c2py::python_typename<std::pair<double, double>>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"dc_formulas", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
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
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_0>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_0, "DcSolver");
#undef _add_type

  return m;
}
#endif
// CLAIR_WRAP_GEN
