
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

template <> constexpr bool c2py::is_wrapped<triqs::modest::atomic_orbs>     = true;
template <> constexpr bool c2py::is_wrapped<triqs::lattice::superlattice>   = true;
template <> constexpr bool c2py::is_wrapped<triqs::tb_hamiltonian>          = true;
template <> constexpr bool c2py::is_wrapped<triqs::lattice::bz_int_options> = true;

// ==================== enums =====================

template <>
std::map<triqs::modest::spin_kind_e, str_t> c2py::enum_to_string<triqs::modest::spin_kind_e> = {
   {triqs::modest::spin_kind_e::Polarized, "Polarized"},
   {triqs::modest::spin_kind_e::NonPolarized, "NonPolarized"},
   {triqs::modest::spin_kind_e::NonColinear, "NonColinear"}};

// ==================== module classes =====================

template <> inline constexpr auto c2py::tp_name<triqs::modest::atomic_orbs> = "triqs_modest.misc.AtomicOrbs";

static int synth_constructor_0(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::atomic_orbs.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<triqs::modest::atomic_orbs> *)self)->_c = new triqs::modest::atomic_orbs{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::atomic_orbs from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<triqs::modest::atomic_orbs> *)self)->_c);
  de("dim", self_c.dim, true);
  de("l", self_c.l, true);
  de("cls_idx", self_c.cls_idx, true);
  de("dft_idx", self_c.dft_idx, true);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<triqs::modest::atomic_orbs> = synth_constructor_0;

template <>
const std::string c2py::tp_ctor_doc<triqs::modest::atomic_orbs> =
   c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
dim : {par_0}, default=0

l : {par_1}, default=0

cls_idx : {par_2}, default=0

dft_idx : {par_3}, default=0

)DOC",
                      "par",
                      std::vector<std::string>{std::vector<std::string>{c2py::python_typename<long>(), c2py::python_typename<long>(),
                                                                        c2py::python_typename<long>(), c2py::python_typename<long>()}});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::atomic_orbs>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_0 = R"DOC(Dimension of the orbital space.)DOC";
constexpr auto doc_member_1 = R"DOC(Angular quantum number.)DOC";
constexpr auto doc_member_2 = R"DOC(Equivalent atoms will have the same sort index (sort).)DOC";
constexpr auto doc_member_3 = R"DOC(Index of the atom in the dft code if any, or :math:`-1`.)DOC";
static PyObject *prop_get_dict_0(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<triqs::modest::atomic_orbs> *)self)->_c);
  c2py::pydict dic;
  dic["dim"]     = self_c.dim;
  dic["l"]       = self_c.l;
  dic["cls_idx"] = self_c.cls_idx;
  dic["dft_idx"] = self_c.dft_idx;
  return dic.new_ref();
}

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::atomic_orbs>[] = {
   c2py::getsetdef_from_member<&triqs::modest::atomic_orbs::dim, triqs::modest::atomic_orbs>("dim", doc_member_0),
   c2py::getsetdef_from_member<&triqs::modest::atomic_orbs::l, triqs::modest::atomic_orbs>("l", doc_member_1),
   c2py::getsetdef_from_member<&triqs::modest::atomic_orbs::cls_idx, triqs::modest::atomic_orbs>("cls_idx", doc_member_2),
   c2py::getsetdef_from_member<&triqs::modest::atomic_orbs::dft_idx, triqs::modest::atomic_orbs>("dft_idx", doc_member_3),
   {"__dict__", (getter)prop_get_dict_0, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<triqs::modest::atomic_orbs> =
   R"DOC(Info on an atomic shell.)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<triqs::modest::atomic_orbs>;
template <> inline constexpr auto c2py::tp_name<triqs::lattice::superlattice> = "triqs_modest.misc.Superlattice";
static auto init_0 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<triqs::lattice::superlattice, nda::array<long, 2>, nda::array<long, 2>>("sl_units", "cluster_pts")};
template <> constexpr initproc c2py::tp_init<triqs::lattice::superlattice> = c2py::pyfkw_constructor<init_0>;
template <>
const std::string c2py::tp_ctor_doc<triqs::lattice::superlattice> = init_0.doc(R"DOC()DOC", std::vector<std::string>{}, std::vector<std::string>{});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::lattice::superlattice>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0 = R"DOC()DOC";
static constexpr auto prop_doc_1 = R"DOC()DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::lattice::superlattice>[] = {

   {"dim", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::superlattice::dim)>, nullptr, prop_doc_0, nullptr},
   {"n_cluster_sites", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::superlattice::n_cluster_sites)>, nullptr, prop_doc_1, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> const std::string c2py::tp_doc<triqs::lattice::superlattice> = R"DOC()DOC" + c2py::tp_ctor_doc<triqs::lattice::superlattice>;
template <> inline constexpr auto c2py::tp_name<triqs::tb_hamiltonian>   = "triqs_modest.misc.TbHamiltonian";
static auto init_1                                                       = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<triqs::tb_hamiltonian, std::vector<std::array<long, 3>>, std::vector<nda::array<triqs::dcomplex, 2>>>("Rs", "hoppings")};
template <> constexpr initproc c2py::tp_init<triqs::tb_hamiltonian> = c2py::pyfkw_constructor<init_1>;
template <>
const std::string c2py::tp_ctor_doc<triqs::tb_hamiltonian> = init_1.doc(R"DOC()DOC", std::vector<std::string>{}, std::vector<std::string>{});
// __call__
static auto const fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::tb_hamiltonian const &self, nda::array_const_view<double, 2> k_list) { return self.operator()(k_list); }, "self",
                 "k_list"),
   c2py::cmethod([](triqs::tb_hamiltonian const &self, std::array<double, 3> ks) { return self.operator()(ks); }, "self", "ks")};

template <> inline constexpr ternaryfunc c2py::tp_call<triqs::tb_hamiltonian> = c2py::pyfkw<fun_0>;

// eigenvalues
static auto const fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::tb_hamiltonian const &self, nda::array_view<double, 2> k) { return self.eigenvalues(k); }, "self", "k")};

// get_R_idx
static auto const fun_2 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](triqs::tb_hamiltonian const &self, std::array<long, 3> R) { return self.get_R_idx(R); }, "self", "R")};

static const auto doc_d_0 = fun_0.doc(R"DOC()DOC", std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_1 = fun_1.doc(R"DOC()DOC", std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_2 = fun_2.doc(R"DOC()DOC", std::vector<std::string>{}, std::vector<std::string>{});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::tb_hamiltonian>[] = {
   {"eigenvalues", (PyCFunction)c2py::pyfkw<fun_1>, METH_VARARGS | METH_KEYWORDS, doc_d_1.c_str()},
   {"get_R_idx", (PyCFunction)c2py::pyfkw<fun_2>, METH_VARARGS | METH_KEYWORDS, doc_d_2.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_2 = R"DOC()DOC";
static constexpr auto prop_doc_3 = R"DOC()DOC";
static constexpr auto prop_doc_4 = R"DOC()DOC";
static constexpr auto prop_doc_5 = R"DOC()DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::tb_hamiltonian>[] = {

   {"elements", c2py::getter_from_method<c2py::castmc<>(&triqs::tb_hamiltonian::elements)>, nullptr, prop_doc_2, nullptr},
   {"hdf5_format", c2py::getter_from_method<c2py::cast<>(&triqs::tb_hamiltonian::hdf5_format)>, nullptr, prop_doc_3, nullptr},
   {"hoppings", c2py::getter_from_method<c2py::castmc<>(&triqs::tb_hamiltonian::hoppings)>, nullptr, prop_doc_4, nullptr},
   {"n_orbitals", c2py::getter_from_method<c2py::castmc<>(&triqs::tb_hamiltonian::n_orbitals)>, nullptr, prop_doc_5, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> const std::string c2py::tp_doc<triqs::tb_hamiltonian>               = R"DOC()DOC" + c2py::tp_ctor_doc<triqs::tb_hamiltonian>;
template <> inline constexpr auto c2py::tp_name<triqs::lattice::bz_int_options> = "triqs_modest.misc.BzIntOptions";

static int synth_constructor_1(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::lattice::bz_int_options.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<triqs::lattice::bz_int_options> *)self)->_c = new triqs::lattice::bz_int_options{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::lattice::bz_int_options from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<triqs::lattice::bz_int_options> *)self)->_c);
  de("tolerance", self_c.tolerance, true);
  de("k_grid", self_c.k_grid, true);
  de("delta_k_grid", self_c.delta_k_grid, true);
  de("k_grid_max", self_c.k_grid_max, true);
  de("run_adaptive", self_c.run_adaptive, true);
  de("run_ptr", self_c.run_ptr, true);
  de("verbose", self_c.verbose, true);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<triqs::lattice::bz_int_options> = synth_constructor_1;

template <>
const std::string c2py::tp_ctor_doc<triqs::lattice::bz_int_options> = c2py::replace_tags(
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
   std::vector<std::string>{std::vector<std::string>{c2py::python_typename<double>(), c2py::python_typename<std::array<long, 3>>(),
                                                     c2py::python_typename<std::array<long, 3>>(), c2py::python_typename<std::array<long, 3>>(),
                                                     c2py::python_typename<bool>(), c2py::python_typename<bool>(), c2py::python_typename<bool>()}});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::lattice::bz_int_options>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_4  = R"DOC()DOC";
constexpr auto doc_member_5  = R"DOC()DOC";
constexpr auto doc_member_6  = R"DOC()DOC";
constexpr auto doc_member_7  = R"DOC()DOC";
constexpr auto doc_member_8  = R"DOC()DOC";
constexpr auto doc_member_9  = R"DOC()DOC";
constexpr auto doc_member_10 = R"DOC()DOC";
static PyObject *prop_get_dict_1(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<triqs::lattice::bz_int_options> *)self)->_c);
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

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::lattice::bz_int_options>[] = {
   c2py::getsetdef_from_member<&triqs::lattice::bz_int_options::tolerance, triqs::lattice::bz_int_options>("tolerance", doc_member_4),
   c2py::getsetdef_from_member<&triqs::lattice::bz_int_options::k_grid, triqs::lattice::bz_int_options>("k_grid", doc_member_5),
   c2py::getsetdef_from_member<&triqs::lattice::bz_int_options::delta_k_grid, triqs::lattice::bz_int_options>("delta_k_grid", doc_member_6),
   c2py::getsetdef_from_member<&triqs::lattice::bz_int_options::k_grid_max, triqs::lattice::bz_int_options>("k_grid_max", doc_member_7),
   c2py::getsetdef_from_member<&triqs::lattice::bz_int_options::run_adaptive, triqs::lattice::bz_int_options>("run_adaptive", doc_member_8),
   c2py::getsetdef_from_member<&triqs::lattice::bz_int_options::run_ptr, triqs::lattice::bz_int_options>("run_ptr", doc_member_9),
   c2py::getsetdef_from_member<&triqs::lattice::bz_int_options::verbose, triqs::lattice::bz_int_options>("verbose", doc_member_10),
   {"__dict__", (getter)prop_get_dict_1, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> const std::string c2py::tp_doc<triqs::lattice::bz_int_options> = R"DOC()DOC" + c2py::tp_ctor_doc<triqs::lattice::bz_int_options>;

// ==================== module functions ====================

// Hloc
static auto const fun_3 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const std::vector<triqs::tb_hamiltonian> &H_sigma,
                 const std::vector<triqs::modest::atomic_orbs> &atomic_shells) { return triqs::modest::Hloc(H_sigma, atomic_shells); },
              "H_sigma", "atomic_shells")};

// discover_symmetries
static auto const fun_4 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const nda::array<nda::matrix<triqs::dcomplex>, 2> &Hloc0, const std::vector<triqs::modest::atomic_orbs> &atomic_shells, double block_threshold,
      bool diagonalize_hloc) { return triqs::modest::discover_symmetries(Hloc0, atomic_shells, block_threshold, diagonalize_hloc); },
   "Hloc0", "atomic_shells", "block_threshold", "diagonalize_hloc")};

// double_counting
static auto const fun_5 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const nda::array<nda::matrix<triqs::dcomplex>, 2> &density_matrix, double U_int, double J_hund,
                                         const std::string method) { return triqs::modest::double_counting(density_matrix, U_int, J_hund, method); },
                                      "density_matrix", "U_int", "J_hund", "method")};

// fold
static auto const fun_6 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::lattice::superlattice &sl, const triqs::tb_hamiltonian &tb) { return triqs::fold(sl, tb); }, "sl", "tb"),
   c2py::cfun([](const triqs::lattice::superlattice &sl, const triqs::modest::one_body_elements_tb &obe) { return triqs::modest::fold(sl, obe); },
              "sl", "obe")};

// h5_read
static auto const fun_7 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](h5::group g, const std::string &name, triqs::modest::one_body_elements_on_grid &x) { return triqs::modest::h5_read(g, name, x); },
              "g", "name", "x"),
   c2py::cfun([](h5::group g, const std::string &name, triqs::modest::initial_data &data) { return triqs::modest::h5_read(g, name, data); }, "g",
              "name", "data"),
   c2py::cfun([](h5::group g, const std::string &name, triqs::modest::iteration_data &data) { return triqs::modest::h5_read(g, name, data); }, "g",
              "name", "data")};

// h5_write
static auto const fun_8 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](h5::group g, const std::string &name, const triqs::modest::one_body_elements_on_grid &x) { return triqs::modest::h5_write(g, name, x); },
      "g", "name", "x"),
   c2py::cfun([](h5::group g, const std::string &name, const triqs::modest::initial_data &data) { return triqs::modest::h5_write(g, name, data); },
              "g", "name", "data"),
   c2py::cfun([](h5::group g, const std::string &name, const triqs::modest::iteration_data &data) { return triqs::modest::h5_write(g, name, data); },
              "g", "name", "data")};

// permute_local_space
static auto const fun_9 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const std::vector<std::vector<long>> &atom_partition,
                 const triqs::modest::one_body_elements_on_grid &x) { return triqs::modest::permute_local_space(atom_partition, x); },
              "atom_partition", "x")};

// root_finder
static auto const fun_10 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](std::string method, std::function<double(double)> f, double x_init, double y_value, double precision, double delta_x, long max_loops,
      std::string x_name, std::string y_name,
      bool verbosity) { return triqs::root_finder(method, f, x_init, y_value, precision, delta_x, max_loops, x_name, y_name, verbosity); },
   "method", "f", "x_init", "y_value", "precision", "delta_x", "max_loops"_a = 1000, "x_name"_a = "", "y_name"_a = "", "verbosity"_a = false)};

// sigma_to_data_idx
static auto const fun_11 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](triqs::modest::spin_kind_e spin_kind, long sigma) { return triqs::modest::sigma_to_data_idx(spin_kind, sigma); }, "spin_kind", "sigma")};

static const auto doc_d_3 = fun_3.doc(
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
   std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<const std::vector<triqs::tb_hamiltonian> &>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<const std::vector<triqs::modest::atomic_orbs> &>()}, ", ")},
   std::vector<std::string>{std::vector<std::string>{c2py::python_typename<nda::array<nda::matrix<triqs::dcomplex>, 2>>()}});
static const auto doc_d_4 = fun_4.doc(
   R"DOC(
Find symmetries of the :math:`R = 0` component of a Hamiltonian to determine a GF block structure.

Discovers (approximate) irreducible symmetries for Green's function from the non-interacting part of the 
local Hamiltonian (:math:`H_0 = \sum_k P(k) H_{\nu\nu'} P^\dagger(k)`), which represents the block structure of the 
TRIQS Gf.

Parameters
----------
Hloc0 : {par_0}
   The :math:`R = 0` part of the Hamiltonian as a vector of [n_atoms, n_sigma].
atomic_shells : {par_1}
   The list of atomic shells used to index :math:`H_{\text{loc}}`.
block_threshold : {par_2}
   The threshold of accuracy at which a symmetry is considered found.
diagonalize_hloc : {par_3}
   Whether or not to diagonalize :math:`H_{\text{loc}}`.

Returns
-------
{ret_0}
   Decomposition, :math:`U_{\text{rotation}}` describing the block structure of GF based on 
   :math:`H_{\text{loc}}`.
)DOC",
   std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<const nda::array<nda::matrix<triqs::dcomplex>, 2> &>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<const std::vector<triqs::modest::atomic_orbs> &>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<bool>()}, ", ")},
   std::vector<std::string>{
      std::vector<std::string>{c2py::python_typename<std::pair<nda::array<std::vector<long>, 2>, nda::array<nda::matrix<triqs::dcomplex>, 2>>>()}});
static const auto doc_d_5 = fun_5.doc(
   R"DOC(
Compute double counting correction for a DC type (method) from the density matrix of a Green's function.

Parameters
----------
density_matrix : {par_0}
   Density matrix [orbital, spin] indices.
U_int : {par_1}
   Coulomb interaction parameter.
J_hund : {par_2}
   Hund's coupling interaction parameter.
method : {par_3}
   DC formula (`sFLL`, `cFLL`, `sAMF`, `cAMF`, `cHeld`).

Returns
-------
{ret_0}
   A pair of :math:`\Sigma_{DC}` and :math:`E_{DC}`.
)DOC",
   std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<const nda::array<nda::matrix<triqs::dcomplex>, 2> &>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<const std::string>()}, ", ")},
   std::vector<std::string>{std::vector<std::string>{c2py::python_typename<std::pair<nda::array<nda::matrix<double>, 2>, nda::matrix<double>>>()}});
static const auto doc_d_6 = fun_6.doc(
   R"DOC(
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
   std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<const triqs::lattice::superlattice &>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<const triqs::modest::one_body_elements_tb &>()}, ", ")},
   std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::modest::one_body_elements_tb>()}});
static const auto doc_d_7 = fun_7.doc(R"DOC()DOC", std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_8 = fun_8.doc(R"DOC()DOC", std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_9 = fun_9.doc(R"DOC()DOC", std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_10 =
   fun_10.doc(R"DOC(
Root finder :math:`f(x) = 0`.

Parameters
----------
method : {par_0}
   Root finding method (`dichtomy` or `bisection`).
f : {par_1}
   :math:`f(x) : \mathbb{R} \to \mathbb{R}`.
x_init : {par_2}
   Initial value for :math:`x`.
y_value : {par_3}
   Target value for :math:`y`.
precision : {par_4}
   Precision for algorithm.
delta_x : {par_5}
   Increment of :math:`x`.
max_loops : {par_6}
   Max number of iterations.
x_name : {par_7}
   Name of :math:`x` variable.
y_name : {par_8}
   Name of :math:`y = f(x)` variable.
verbosity : {par_9}
   Turn on/off logging.

Returns
-------
{ret_0}
   :math:`x, f(x)` where :math:`f(x) = y`.
)DOC",
              std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<std::string>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<std::function<double(double)>>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<long>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<std::string>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<std::string>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<bool>()}, ", ")},
              std::vector<std::string>{std::vector<std::string>{c2py::python_typename<std::pair<double, double>>()}});
static const auto doc_d_11 = fun_11.doc(R"DOC(
Map a spin index to a data index.

The mapping depends on the spin kind:

* `Polarized`: :math:`\sigma \to \sigma` (an object can have different values for different :math:`\sigma`).
* `NonPolarized`: :math:`\sigma \to 0` (an object is the same for both :math:`\sigma`, so only one copy is stored).
* `NonCollinear`: :math:`\sigma \to 0` (:math:`\sigma` is always 0).
)DOC",
                                        std::vector<std::string>{}, std::vector<std::string>{});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"Hloc", (PyCFunction)c2py::pyfkw<fun_3>, METH_VARARGS | METH_KEYWORDS, doc_d_3.c_str()},
   {"discover_symmetries", (PyCFunction)c2py::pyfkw<fun_4>, METH_VARARGS | METH_KEYWORDS, doc_d_4.c_str()},
   {"double_counting", (PyCFunction)c2py::pyfkw<fun_5>, METH_VARARGS | METH_KEYWORDS, doc_d_5.c_str()},
   {"fold", (PyCFunction)c2py::pyfkw<fun_6>, METH_VARARGS | METH_KEYWORDS, doc_d_6.c_str()},
   {"h5_read", (PyCFunction)c2py::pyfkw<fun_7>, METH_VARARGS | METH_KEYWORDS, doc_d_7.c_str()},
   {"h5_write", (PyCFunction)c2py::pyfkw<fun_8>, METH_VARARGS | METH_KEYWORDS, doc_d_8.c_str()},
   {"permute_local_space", (PyCFunction)c2py::pyfkw<fun_9>, METH_VARARGS | METH_KEYWORDS, doc_d_9.c_str()},
   {"root_finder", (PyCFunction)c2py::pyfkw<fun_10>, METH_VARARGS | METH_KEYWORDS, doc_d_10.c_str()},
   {"sigma_to_data_idx", (PyCFunction)c2py::pyfkw<fun_11>, METH_VARARGS | METH_KEYWORDS, doc_d_11.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "misc",                                         /* name of module */
                                        R"RAWDOC(Other tools for triqs_modest.)RAWDOC", /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_misc() {

  if (not c2py::check_python_version("misc")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::atomic_orbs>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::lattice::superlattice>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::tb_hamiltonian>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::lattice::bz_int_options>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
  c2py::add_type_object_to_main<triqs::modest::atomic_orbs>("AtomicOrbs", m, conv_table);
  c2py::add_type_object_to_main<triqs::lattice::superlattice>("Superlattice", m, conv_table);
  c2py::add_type_object_to_main<triqs::tb_hamiltonian>("TbHamiltonian", m, conv_table);
  c2py::add_type_object_to_main<triqs::lattice::bz_int_options>("BzIntOptions", m, conv_table);

  return m;
}
#endif
// CLAIR_WRAP_GEN
