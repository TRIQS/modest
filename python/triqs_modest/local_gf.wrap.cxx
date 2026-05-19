
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

// charge_density_correction
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::modest::one_body_elements_on_grid &obe, double mu,
      const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_dynamic,
      const nda::basic_array<
         nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
         nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &Sigma_static) {
     return triqs::modest::charge_density_correction<triqs::mesh::imfreq>(obe, mu, Sigma_dynamic, Sigma_static);
   },
   "obe", "mu", "Sigma_dynamic", "Sigma_static")};

// gloc
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](const triqs::modest::one_body_elements_on_grid &obe, double mu,
         const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_dynamic,
         const nda::basic_array<
            nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
            nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &Sigma_static) {
        return triqs::modest::gloc<triqs::mesh::imfreq>(obe, mu, Sigma_dynamic, Sigma_static);
      },
      "obe", "mu", "Sigma_dynamic", "Sigma_static"),
   c2py::cfun(
      [](const triqs::modest::one_body_elements_on_grid &obe, double mu,
         const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_dynamic,
         const nda::basic_array<
            nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
            nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &Sigma_static) {
        return triqs::modest::gloc<triqs::mesh::dlr_imfreq>(obe, mu, Sigma_dynamic, Sigma_static);
      },
      "obe", "mu", "Sigma_dynamic", "Sigma_static"),
   c2py::cfun([](const triqs::mesh::imfreq &mesh, const triqs::modest::one_body_elements_on_grid &obe,
                 double mu) { return triqs::modest::gloc<triqs::mesh::imfreq>(mesh, obe, mu); },
              "mesh", "obe", "mu"),
   c2py::cfun([](const triqs::mesh::dlr_imfreq &mesh, const triqs::modest::one_body_elements_on_grid &obe,
                 double mu) { return triqs::modest::gloc<triqs::mesh::dlr_imfreq>(mesh, obe, mu); },
              "mesh", "obe", "mu"),
   c2py::cfun(
      [](const triqs::modest::one_body_elements_tb &obe, double mu,
         const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_dynamic,
         const nda::basic_array<
            nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
            nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &Sigma_static,
         const triqs::experimental::lattice::bz_int_options &opt) {
        return triqs::modest::gloc<triqs::mesh::imfreq>(obe, mu, Sigma_dynamic, Sigma_static, opt);
      },
      "obe", "mu", "Sigma_dynamic", "Sigma_static", "opt"),
   c2py::cfun([](const triqs::mesh::imfreq &mesh, const triqs::modest::one_body_elements_tb &obe, double mu,
                 const triqs::experimental::lattice::bz_int_options &opt) { return triqs::modest::gloc<triqs::mesh::imfreq>(mesh, obe, mu, opt); },
              "mesh", "obe", "mu", "opt")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(
   R"DOC(
Compute the charge density correction from DMFT

Compute the charge density correction in the band basis :math:`N_{\nu\nu'}(\mathbf{k})` from the lattice Green's function.

Parameters
----------
obe : {par_0}
   The one-body elements on the grid
mu : {par_1}
   The chemical potential
Sigma_dynamic : {par_2}
   The dynamic part of the self-energy
Sigma_static : {par_3}
   The static part of the self-energy

Returns
-------
{ret_0}
   The charge density correction in the band basis :math:`N_{\nu\nu'}(\mathbf{k})`
)DOC",
   {{c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>()},
    {c2py::python_typename<const nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()}},
   {c2py::python_typename<
      nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(
   R"DOC(
[1, 2] Compute local Green's function on a :math:`M \times M` mesh.

When the one-body dispersion is defined as fixed k-grid, which is the case when working with DFT codes
(e.g., VASP, Wien2k, Elk) or performing charge self-consistent calculations with any DFT code, :math:`H(\mathbf{k})`
is diagonal in the band basis and reduces to :math:`\varepsilon_{\nu}^{\sigma}(\mathbf{k})`. The local Green's
function becomes:

.. math::

   [ G_{\mathrm{loc}}^{\sigma} ]_{m m'} = \sum_{\mathbf{k}} P_{m\nu}^{\sigma}(\mathbf{k}) \Big [ (\omega + \mu -
   \varepsilon_{\nu}^{\sigma}(\mathbf{k}))\delta_{\nu\nu'} - [P_{m\nu}^{\sigma}]^{\dagger}\Sigma_{\mathrm{embed}}
   P_{m'\nu'}^{\sigma}(\mathbf{k}) \Big ]^{-1} [P_{m'\nu'}^{\sigma}]^{\dagger}.

For performance reasons, we can avoid performing the matrix inverstion in the larger band basis (:math:`N_{\nu}`)
using the Woodbury formula which allows us to perform the matrix inversion in the smaller orbital basis :math:`N_{M}`.

------

[3, 4, 5, 6] Compute the local Green's function without a self-energy.

See other overloads (gloc) for more details.

------

Parameters
----------
obe : {par_0}
   One-body elements on a fixed grid.
mu : {par_1}
   Chemical potential :math:`\mu`.
Sigma_dynamic : {par_2}
   The dynamic part of the embedded self-energy in the embedded view,
   :math:`\Sigma_{\text{dynamic}}[\alpha, \sigma]`.
Sigma_static : {par_3}
   The static part of the embedded self-energy in the embedded view,
   :math:`\Sigma_{\text{static}}[\alpha,\sigma]`.
mesh : {par_4}
   (DLR) imaginary frequency mesh.
opt : {par_5}
   Container for options related to integration of the BZ.

Returns
-------
[1] : {ret_0}
   :math:`G_{\mathrm{loc}}^{\sigma}`, the local Green's function in the full :math:`\mathcal{C}` space.

[2] : {ret_1}
   :math:`G_{\mathrm{loc}}^{\sigma}`, the local Green's function in the full :math:`\mathcal{C}` space.

[3, 5, 6] : {ret_2}
   :math:`G_{\mathrm{loc}}^{\sigma}`, the local Green's function.

[4] : {ret_3}
   :math:`G_{\mathrm{loc}}^{\sigma}`, the local Green's function.
)DOC",
   {{c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>(),
     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>()},
    {c2py::python_typename<const nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()},
    {c2py::python_typename<const triqs::mesh::imfreq &>(), c2py::python_typename<const triqs::mesh::dlr_imfreq &>()},
    {c2py::python_typename<const triqs::experimental::lattice::bz_int_options &>()}},
   {c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2>>(),
    c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2>>(),
    c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2>>(),
    c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2>>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"charge_density_correction", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"gloc", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "local_gf", /* name of module */
                                        R"RAWDOC(Local Green's functions.

Compute the local Green's function defined as:

.. math::
  
    [ G_{\mathrm{loc}}^{\sigma} ]_{m m'} = \sum_{\mathbf{k}} P_{m\nu}^{\sigma}(\mathbf{k}) \Big  [ (\omega + \mu)\delta_{\nu\nu'} - 
    H^{\sigma}_{\nu\nu'}(\mathbf{k}) - [P_{m\nu}^{\sigma}]^{\dagger}\Sigma_{\mathrm{embed}}P_{m'\nu'}^{\sigma}(\mathbf{k}) \Big ]^{-1} 
    [P_{m'\nu'}^{\sigma}]^{\dagger},

where :math:`\omega` is a frequency (either real- or Matsubra), :math:`\mu` is the chemical potential, :math:`H(\mathbf{k})` is the one-body 
Hamiltonian, :math:`P(\mathbf{k})` are the projectors from the band to the orbital basis, and :math:`\Sigma_{\mathrm{embed}}` is the embedded 
self-energy.

ModEST computes this efficiently:

* **Woodbury**: reduces the cost of matrix inversion from cubic in the number of bands to linear.
* **Adaptive Brillouin zone integration**: for tight-binding models, allows you to specify desired integration accuracy, 
  improving predictions of transport properties.
)RAWDOC",                                           /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_local_gf() {

  if (not c2py::check_python_version("local_gf")) return NULL;

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
