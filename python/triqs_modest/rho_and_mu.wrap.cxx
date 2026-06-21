
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

// density
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](const triqs::modest::one_body_elements_tb &obe, double mu,
         const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_dynamic,
         const nda::basic_array<
            nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
            nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &Sigma_static,
         const triqs::experimental::lattice::bz_int_options &opt) {
        return triqs::modest::density<triqs::mesh::imfreq>(obe, mu, Sigma_dynamic, Sigma_static, opt);
      },
      "obe", "mu", "Sigma_dynamic", "Sigma_static", "opt"),
   c2py::cfun([](const triqs::modest::one_body_elements_tb &obe, double mu, const triqs::mesh::imfreq &mesh,
                 const triqs::experimental::lattice::bz_int_options &opt) { return triqs::modest::density<triqs::mesh::imfreq>(obe, mu, mesh, opt); },
              "obe", "mu", "mesh", "opt"),
   c2py::cfun(
      [](const triqs::modest::one_body_elements_on_grid &obe, double mu,
         const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_dynamic,
         const nda::basic_array<
            nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
            nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &Sigma_static) {
        return triqs::modest::density<triqs::mesh::imfreq>(obe, mu, Sigma_dynamic, Sigma_static);
      },
      "obe", "mu", "Sigma_dynamic", "Sigma_static"),
   c2py::cfun(
      [](const triqs::modest::one_body_elements_on_grid &obe, double mu,
         const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_dynamic,
         const nda::basic_array<
            nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
            nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &Sigma_static) {
        return triqs::modest::density<triqs::mesh::dlr_imfreq>(obe, mu, Sigma_dynamic, Sigma_static);
      },
      "obe", "mu", "Sigma_dynamic", "Sigma_static")};

// density_nk
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::modest::one_body_elements_on_grid &obe, double mu, double beta) { return triqs::modest::density_nk(obe, mu, beta); },
              "obe", "mu", "beta")};

// find_chemical_potential
static auto const _c2py_fun_2 =
   c2py::dispatcher_f_kw_t{
      c2py::cfun(
         [](const double target_density, const triqs::modest::one_body_elements_tb &obe,
            const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_dynamic,
            const nda::basic_array<
               nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
               nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &Sigma_static,
            const triqs::experimental::lattice::bz_int_options &opt, std::string method, double precision, bool verbosity) {
           return triqs::modest::find_chemical_potential<triqs::mesh::imfreq>(target_density, obe, Sigma_dynamic, Sigma_static, opt, method,
                                                                              precision, verbosity);
         },
         "target_density", "obe", "Sigma_dynamic", "Sigma_static", "opt", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = true),
      c2py::cfun(
         [](const double target_density, const triqs::modest::one_body_elements_tb &obe, const triqs::mesh::imfreq &mesh,
            const triqs::experimental::lattice::bz_int_options &opt, std::string method, double precision, bool verbosity) {
           return triqs::modest::find_chemical_potential<triqs::mesh::imfreq>(target_density, obe, mesh, opt, method, precision, verbosity);
         },
         "target_density", "obe", "mesh", "opt", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = true),
      c2py::cfun([](const double target_density, const triqs::modest::one_body_elements_on_grid &obe, double beta, std::string method,
                    double precision,
                    bool verbosity) { return triqs::modest::find_chemical_potential(target_density, obe, beta, method, precision, verbosity); },
                 "target_density", "obe", "beta", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = true),
      c2py::cfun(
         [](const double target_density, const triqs::modest::one_body_elements_on_grid &obe,
            const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_dynamic,
            const nda::basic_array<
               nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
               nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &Sigma_static,
            std::string method, double precision, bool verbosity) {
           return triqs::modest::find_chemical_potential<triqs::mesh::imfreq>(target_density, obe, Sigma_dynamic, Sigma_static, method, precision,
                                                                              verbosity);
         },
         "target_density", "obe", "Sigma_dynamic", "Sigma_static", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = true),
      c2py::cfun(
         [](const double target_density, const triqs::modest::one_body_elements_on_grid &obe,
            const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_dynamic,
            const nda::basic_array<
               nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
               nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &Sigma_static,
            std::string method, double precision, bool verbosity) {
           return triqs::modest::find_chemical_potential<triqs::mesh::dlr_imfreq>(target_density, obe, Sigma_dynamic, Sigma_static, method, precision,
                                                                                  verbosity);
         },
         "target_density", "obe", "Sigma_dynamic", "Sigma_static", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = true)};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(
   R"DOC(
[1] Compute the density of the lattice Green's function with a self-energy.

------

[2] Compute the density of the lattice Green's function without a self-energy.

------

[3, 4] Compute the density of the lattice Green's function with a self-energy using Woodbury.

------

Parameters
----------
obe : {par_0}
   One-body elements.
mu : {par_1}
   Chemical potential.
Sigma_dynamic : {par_2}
   Dynamic part of the embedded self-energy.
Sigma_static : {par_3}
   Static part of the embedded self-energy.
opt : {par_4}
   Container for options related to the integration of the BZ.
mesh : {par_5}
   Mesh on which local GF will be computed.

Returns
-------
{ret_0}
   Electron density of the lattice Green's function.
)DOC",
   {{c2py::python_typename<const triqs::modest::one_body_elements_tb &>(), c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>(),
     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>()},
    {c2py::python_typename<const nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()},
    {c2py::python_typename<const triqs::experimental::lattice::bz_int_options &>()},
    {c2py::python_typename<const triqs::mesh::imfreq &>()}},
   {c2py::python_typename<double>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC(
Compute number of particles :math:`n = \sum f(\beta(\varepsilon(k) - μ))`.
)DOC");
static const auto _c2py_doc_2 = _c2py_fun_2.doc(
   R"DOC(
[1, 2, 4, 5] Find the chemical potenital from the local Green's function and self-energy given a target density.

------

[3] Find the chemical potenital from the local Green's function given a target density.

------

Parameters
----------
target_density : {par_0}
   Total electron density.
obe : {par_1}
   One-body elements.
Sigma_dynamic : {par_2}
   Dynamic part of the embedded self-energy.
Sigma_static : {par_3}
   Static part of the embedded self-energy.
opt : {par_4}
   Container for options related to integration of the BZ.
method : {par_5}
   Root finding method to use (default = `dichotomy`).
precision : {par_6}
   Precision to end search (default = 1e-5).
verbosity : {par_7}
   Printing of the root finder's progress (default = true).
mesh : {par_8}
   Mesh on which local GF will be computed.
beta : {par_9}
   Inverse temperature (units 1/eV).

Returns
-------
{ret_0}
   Chemical potential corresponding to target density.
)DOC",
   {{c2py::python_typename<const double>()},
    {c2py::python_typename<const triqs::modest::one_body_elements_tb &>(), c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()},
    {c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>(),
     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>()},
    {c2py::python_typename<const nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()},
    {c2py::python_typename<const triqs::experimental::lattice::bz_int_options &>()},
    {c2py::python_typename<std::string>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<bool>()},
    {c2py::python_typename<const triqs::mesh::imfreq &>()},
    {c2py::python_typename<double>()}},
   {c2py::python_typename<double>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"density", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"density_nk", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"find_chemical_potential", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "rho_and_mu", /* name of module */
   R"RAWDOC(Functions to calculate the total electron density of lattice Green's functions using different methods 
and methods for chemical potenital searching given a target density.

Calculate the total electronic density from the lattice Green's function and search for the chemical potential that gives a 
target electron count. We provide several algorithms optimized for speed and robustness. Due to the modularity you can also 
leverage external root finding routines to write a custom chemical potential search.
)RAWDOC",        /* module documentation, may be NULL */
   -1,           /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
   module_methods,
   NULL,
   NULL,
   NULL,
   NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_rho_and_mu() {

  if (not c2py::check_python_version("rho_and_mu")) return NULL;

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
