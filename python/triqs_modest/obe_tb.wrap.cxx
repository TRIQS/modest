
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
using _c2py_cls_0                                            = triqs::modest::one_body_elements_tb;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs_modest.obe_tb.OneBodyElementsTb";
static const auto _c2py_init_0                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_0, std::vector<triqs::experimental::lattice::tb_hk>, triqs::modest::local_space>("H_sigma", "ls"),
   c2py::c_constructor<_c2py_cls_0, std::vector<triqs::experimental::lattice::tb_hk>, triqs::modest::spin_kind_e,
                       std::vector<triqs::modest::atomic_orbs>>("H_sigma", "spin_kind", "atomic_shells")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> = _c2py_init_0.doc(R"DOC(
Construct a one-body elements TB object from a list of tb_hk objects.

Parameters
----------
H_sigma : {par_0}
   A list of TB Hamiltonians of length :math:`n_\sigma`.
ls : {par_1}
   Local :math:`\mathcal{C}` space.
spin_kind : {par_2}
   Spin kind for this calculation.
atomic_shells : {par_3}
   List of atomic shells input by the user.

Returns
-------
[1] : {ret_0}
   One-body elements containing the Wannier90 tight binding Hamiltonian.

[2] : {ret_1}
   One-body elements containing the tight binding Hamiltonian.
)DOC",
                                                                    {{c2py::python_typename<std::vector<triqs::experimental::lattice::tb_hk>>()},
                                                                     {c2py::python_typename<triqs::modest::local_space>()},
                                                                     {c2py::python_typename<triqs::modest::spin_kind_e>()},
                                                                     {c2py::python_typename<std::vector<triqs::modest::atomic_orbs>>()}});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_0 = R"DOC(Local :math:`\mathcal{C}` space.)DOC";
constexpr auto _c2py_doc_member_1 = R"DOC(List of TB Hamiltonians.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_0>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_0::C_space, _c2py_cls_0>("C_space", _c2py_doc_member_0),
   c2py::getsetdef_from_member<&_c2py_cls_0::H, _c2py_cls_0>("H", _c2py_doc_member_1),

   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_0> =
   R"DOC(A one-body elements using a tight-binding Hamiltonian.)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;

// ==================== module functions ====================

// Hloc
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const std::vector<triqs::experimental::lattice::tb_hk> &H_sigma,
                 const std::vector<triqs::modest::atomic_orbs> &atomic_shells) { return triqs::modest::Hloc(H_sigma, atomic_shells); },
              "H_sigma", "atomic_shells")};

// add_local_term
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::modest::one_body_elements_tb &obe,
      const nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>
         &local_term) { return triqs::modest::add_local_term(obe, local_term); },
   "obe", "local_term")};

// density
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{
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
              "obe", "mu", "mesh", "opt")};

// extend_to_spin
static auto const _c2py_fun_3 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const triqs::modest::one_body_elements_tb &obe) { return triqs::modest::extend_to_spin(obe); }, "obe")};

// find_chemical_potential
static auto const _c2py_fun_4 =
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
         "target_density", "obe", "mesh", "opt", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = true)};

// fold
static auto const _c2py_fun_5 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::experimental::lattice::superlattice &sl, const triqs::modest::one_body_elements_tb &obe) { return triqs::modest::fold(sl, obe); },
   "sl", "obe")};

// gloc
static auto const _c2py_fun_6 = c2py::dispatcher_f_kw_t{
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

// impurity_levels
static auto const _c2py_fun_7 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const triqs::modest::one_body_elements_tb &obe) { return triqs::modest::impurity_levels(obe); }, "obe")};

// one_body_elements_from_wannier90
static auto const _c2py_fun_8 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](const std::string &wannier_file_path, triqs::modest::spin_kind_e spin_kind, std::vector<triqs::modest::atomic_orbs> atomic_shells) {
        return triqs::modest::one_body_elements_from_wannier90(wannier_file_path, spin_kind, atomic_shells);
      },
      "wannier_file_path", "spin_kind", "atomic_shells"),
   c2py::cfun(
      [](const std::string &wannier_file_path_up, const std::string &wannier_file_path_dn, triqs::modest::spin_kind_e spin_kind,
         std::vector<triqs::modest::atomic_orbs> atomic_shells) {
        return triqs::modest::one_body_elements_from_wannier90(wannier_file_path_up, wannier_file_path_dn, spin_kind, atomic_shells);
      },
      "wannier_file_path_up", "wannier_file_path_dn", "spin_kind", "atomic_shells")};

// rotate
static auto const _c2py_fun_9 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::modest::one_body_elements_tb &obe,
      const nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &U) {
     return triqs::modest::rotate(obe, U);
   },
   "obe", "U")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(
   R"DOC(
Compute :math:`H_{\text{loc}} = H(R=0)` given :math:`n_\sigma` tight binding Hamiltonians.

Parameters
----------
H_sigma : {par_0}
   A list of TB Hamiltonians of length :math:`n_\sigma`.
atomic_shells : {par_1}
   A list of atomic shells corresponding to the orbitals contained in the TB Hamiltonians.

Returns
-------
{ret_0}
   :math:`H_{\text{loc}}`, formatted with dimensions :math:`[\alpha,\sigma]` each containing (n_orbitals_atom,
   n_orbitals_atom).
)DOC",
   {{c2py::python_typename<const std::vector<triqs::experimental::lattice::tb_hk> &>()},
    {c2py::python_typename<const std::vector<triqs::modest::atomic_orbs> &>()}},
   {c2py::python_typename<nda::basic_array<
      nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
      nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC()DOC");
static const auto _c2py_doc_2 = _c2py_fun_2.doc(
   R"DOC(
[1] Compute the density of the lattice Green's function with a self-energy.

------

[2] Compute the density of the lattice Green's function without a self-energy.

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
   {{c2py::python_typename<const triqs::modest::one_body_elements_tb &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>()},
    {c2py::python_typename<const nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()},
    {c2py::python_typename<const triqs::experimental::lattice::bz_int_options &>()},
    {c2py::python_typename<const triqs::mesh::imfreq &>()}},
   {c2py::python_typename<double>()});
static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC()DOC");
static const auto _c2py_doc_4 = _c2py_fun_4.doc(
   R"DOC(
Find the chemical potenital from the local Green's function and self-energy given a target density.

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

Returns
-------
{ret_0}
   Chemical potential corresponding to target density.
)DOC",
   {{c2py::python_typename<const double>()},
    {c2py::python_typename<const triqs::modest::one_body_elements_tb &>()},
    {c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>()},
    {c2py::python_typename<const nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()},
    {c2py::python_typename<const triqs::experimental::lattice::bz_int_options &>()},
    {c2py::python_typename<std::string>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<bool>()},
    {c2py::python_typename<const triqs::mesh::imfreq &>()}},
   {c2py::python_typename<double>()});
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC(
Convert a tight binding Hamiltonian to its superlattice equivalent.

Parameters
----------
sl : {par_0}
   The superlattice object containing its lattice vectors and locations of cluster points.
obe : {par_1}
   A one-body elements containing the TB Hamiltonian.

Returns
-------
{ret_0}
   One-body elements based on the superlattice tight binding Hamiltonian.
)DOC",
                                                {{c2py::python_typename<const triqs::experimental::lattice::superlattice &>()},
                                                 {c2py::python_typename<const triqs::modest::one_body_elements_tb &>()}},
                                                {c2py::python_typename<triqs::modest::one_body_elements_tb>()});
static const auto _c2py_doc_6 = _c2py_fun_6.doc(
   R"DOC(
Compute the local Green's function without a self-energy.

See other overloads (gloc) for more details.

Parameters
----------
obe : {par_0}
   One-body elements containing the TB Hamiltonian.
mu : {par_1}
   Chemical potential :math:`\mu`.
Sigma_dynamic : {par_2}
   The dynamic part of the embedded self-energy in the embedded view,
   :math:`\Sigma_{\text{dynamic}}[\alpha, \sigma]`.
Sigma_static : {par_3}
   The static part of the embedded self-energy in the embedded view,
   :math:`\Sigma_{\text{static}}[\alpha,\sigma]`.
opt : {par_4}
   Container for options related to integration of the BZ.
mesh : {par_5}
   The mesh on which :math:`G_{\mathrm{loc}}` will be computed.

Returns
-------
{ret_0}
   :math:`G_{\mathrm{loc}}^{\sigma}`, the local Green's function.
)DOC",
   {{c2py::python_typename<const triqs::modest::one_body_elements_tb &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>()},
    {c2py::python_typename<const nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()},
    {c2py::python_typename<const triqs::experimental::lattice::bz_int_options &>()},
    {c2py::python_typename<const triqs::mesh::imfreq &>()}},
   {c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2>>()});
static const auto _c2py_doc_7 = _c2py_fun_7.doc(
   R"DOC(
Compute the atomic (impurity) levels from an obe.

Parameters
----------
obe : {par_0}
   One-body elements.

Returns
-------
{ret_0}
   Impurity levels stored in the format [n_atoms, n_sigma].
)DOC",
   {{c2py::python_typename<const triqs::modest::one_body_elements_tb &>()}},
   {c2py::python_typename<nda::basic_array<
      nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
      nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});
static const auto _c2py_doc_8 = _c2py_fun_8.doc(R"DOC(
[1] Construct a one-body elements TB object from Wannier90 in the case of a single spin index.

------

[2] Construct a one-body elements TB object from Wannier90 in the case with separate spin up/spin down channels.

------

Parameters
----------
wannier_file_path : {par_0}
   String to Wannier90 files, including the prefix, as in "path/to/file/seedname" to specify
   a Wannier files named in the format "seedname_tb.dat".
spin_kind : {par_1}
   Spin kind for this calculation.
atomic_shells : {par_2}
   List of atomic shells input by the user.
wannier_file_path_up : {par_3}
   String to Wannier90 files, including the prefix, for the up spin channel, as in
   "path/to/file/seedname" to specify a Wannier files named in the format "seedname_tb.dat".
wannier_file_path_dn : {par_4}
   String to Wannier90 files, including the prefix, for the down spin channel as in
   "path/to/file/seedname" to specify a Wannier files named in the format "seedname_tb.dat"

Returns
-------
{ret_0}
   One-body elements containing the Wannier90 tight binding Hamiltonian.
)DOC",
                                                {{c2py::python_typename<const std::string &>()},
                                                 {c2py::python_typename<triqs::modest::spin_kind_e>()},
                                                 {c2py::python_typename<std::vector<triqs::modest::atomic_orbs>>()},
                                                 {c2py::python_typename<const std::string &>()},
                                                 {c2py::python_typename<const std::string &>()}},
                                                {c2py::python_typename<triqs::modest::one_body_elements_tb>()});
static const auto _c2py_doc_9 =
   _c2py_fun_9.doc(R"DOC(
Rotate a tight-binding Hamiltonian by a unitary matrix :math:`U`.

The rotation is performed as :math:`H' = U H U^\dagger`.

Parameters
----------
obe : {par_0}
   One-body elements containing the TB Hamiltonian.
U : {par_1}
   Unitary matrix used for the rotation.

Returns
-------
{ret_0}
   One-body elements containing the rotated TB Hamiltonian.
)DOC",
                   {{c2py::python_typename<const triqs::modest::one_body_elements_tb &>()},
                    {c2py::python_typename<const nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M',
                                                                  nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()}},
                   {c2py::python_typename<triqs::modest::one_body_elements_tb>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"Hloc", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"add_local_term", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"density", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"extend_to_spin", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {"find_chemical_potential", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"fold", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_5.c_str()},
   {"gloc", (PyCFunction)c2py::pyfkw<_c2py_fun_6>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_6.c_str()},
   {"impurity_levels", (PyCFunction)c2py::pyfkw<_c2py_fun_7>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_7.c_str()},
   {"one_body_elements_from_wannier90", (PyCFunction)c2py::pyfkw<_c2py_fun_8>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_8.c_str()},
   {"rotate", (PyCFunction)c2py::pyfkw<_c2py_fun_9>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_9.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "obe_tb", /* name of module */
                                        R"RAWDOC(The one-body elements abstraction in ModEST, to be used with tight binding Hamiltonians.

One-body elements (obe for short) from tight binding contains all of the one-body data that is typically converted from a
 Wannier90 calculation or other tight binding model. The one-body elements struct for tight binding contains two components:

- **H** List of tight binding Hamiltonians, of length number of spin channels (spin-polarized calculations should have both H_spin_up, H_spin_down).
- **Local space** defines the local correlated subspace to be solved within DMFT.

To set this up, a user will need to provide a list of atomic_orbitals used in the tight binding Hamiltonian, so that ModEST can construct the local space.
)RAWDOC",                                         /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_obe_tb() {

  if (not c2py::check_python_version("obe_tb")) return NULL;

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
  _add_type(_c2py_cls_0, "OneBodyElementsTb");
#undef _add_type

  return m;
}
#endif
// CLAIR_WRAP_GEN
