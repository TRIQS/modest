
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

template <>
std::map<triqs::modest::spin_kind_e, str_t> c2py::enum_to_string<triqs::modest::spin_kind_e> = {
   {triqs::modest::spin_kind_e::Polarized, "Polarized"},
   {triqs::modest::spin_kind_e::NonPolarized, "NonPolarized"},
   {triqs::modest::spin_kind_e::NonColinear, "NonColinear"}};

// ==================== module classes =====================

// --------- class _c2py_cls_0 -----------
using _c2py_cls_0                                            = triqs::modest::atomic_orbs;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs_modest.misc.AtomicOrbs";

static int synth_constructor_0(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::atomic_orbs.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_0> *)self)->_c = new _c2py_cls_0{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::atomic_orbs from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_0> *)self)->_c);
  de("dim", self_c.dim, true);
  de("l", self_c.l, true);
  de("cls_idx", self_c.cls_idx, true);
  de("dft_idx", self_c.dft_idx, true);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = synth_constructor_0;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> =
   c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
dim : {par_0}, default=0

l : {par_1}, default=0

cls_idx : {par_2}, default=0

dft_idx : {par_3}, default=0

)DOC",
                      "par",
                      {c2py::python_typename<long>(), c2py::python_typename<long>(), c2py::python_typename<long>(), c2py::python_typename<long>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_0>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_0>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_0 = R"DOC(Dimension of the orbital space.)DOC";
constexpr auto _c2py_doc_member_1 = R"DOC(Angular quantum number.)DOC";
constexpr auto _c2py_doc_member_2 = R"DOC(Equivalent atoms will have the same sort index (sort).)DOC";
constexpr auto _c2py_doc_member_3 = R"DOC(Index of the atom in the dft code if any, or :math:`-1`.)DOC";
static PyObject *prop_get_dict_0(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_0> *)self)->_c);
  c2py::pydict dic;
  dic["dim"]     = self_c.dim;
  dic["l"]       = self_c.l;
  dic["cls_idx"] = self_c.cls_idx;
  dic["dft_idx"] = self_c.dft_idx;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_0>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_0::dim, _c2py_cls_0>("dim", _c2py_doc_member_0),
   c2py::getsetdef_from_member<&_c2py_cls_0::l, _c2py_cls_0>("l", _c2py_doc_member_1),
   c2py::getsetdef_from_member<&_c2py_cls_0::cls_idx, _c2py_cls_0>("cls_idx", _c2py_doc_member_2),
   c2py::getsetdef_from_member<&_c2py_cls_0::dft_idx, _c2py_cls_0>("dft_idx", _c2py_doc_member_3),
   {"__dict__", (getter)prop_get_dict_0, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_0> =
   R"DOC(Info on an atomic shell.)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1                                            = triqs::modest::ibz_symmetry_ops;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs_modest.misc.IbzSymmetryOps";

static int synth_constructor_1(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::ibz_symmetry_ops.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_1> *)self)->_c = new _c2py_cls_1{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::ibz_symmetry_ops from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_1> *)self)->_c);
  de("ops", self_c.ops, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_1> = synth_constructor_1;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_1> =
   c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
ops : {par_0}

)DOC",
                      "par", {c2py::python_typename<std::vector<triqs::modest::ibz_symmetry_ops::op>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_4 = R"DOC()DOC";
static PyObject *prop_get_dict_1(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_1> *)self)->_c);
  c2py::pydict dic;
  dic["ops"] = self_c.ops;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_1>[] = {c2py::getsetdef_from_member<&_c2py_cls_1::ops, _c2py_cls_1>("ops", _c2py_doc_member_4),
                                                        {"__dict__", (getter)prop_get_dict_1, nullptr, "", nullptr},
                                                        {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_1> =
   R"DOC(Irreducible Brillouin Zone (IBZ) symmetry operations to symmetrize observables over the entire Brillouin
zone.

The IBZ symmetry operations are optional and specific to DFT codes that work in the IBZ instead of the
full BZ. Currently, this is only used when DFT data is converted from Wien2k.

For computational efficiency, we can perform the one-body calculation on the irreducible Brillouin zone (IBZ).
However, in order to obtain observable quantities like the local Green's function, one needs to _symmetrize_ the
obversable summed on only the IBZ. For any observable :math:`\mathcal{O}`, the unsymmetrized quantity is

.. math::

   [\mathcal{O}^{\sigma}_{mm'}]_{\mathrm{unsymm}} = \sum_{\mathbf{k}\in\mathrm{IBZ}}\sum_{\nu\nu'}P_{m,\nu}^{\sigma}
   (\mathbf{k})O_{\nu\nu'}^{\sigma}(\mathbf{k})[P_{m'\nu'}^{\sigma}(\mathbf{k})]^{\dagger}.

To symmetrize, we must by apply all operations symmetry operations :math:`\mathcal{S}` of the crystallographic space
group :math:`\mathcal{G}`:

.. math::

   [\mathcal{O}^{\sigma}_{(am_{a}), (a'm_{a}')}]_{\mathrm{symm}}^{\mathcal{G}} = \sum_{\mathcal{S}\in\mathcal{G}}
   \sum_{n_{a}n_{a}'} \mathcal{D}_{m_{a} n_{a}}(\mathcal{S})[(\mathcal{O}^{\mathcal{S}^{-1}a,\mathcal{S}^{-1}
   \sigma})_{n_{a}n_{a}'}]_{\mathrm{unsymm}}\mathcal{D}(\mathcal{S}^{-1})_{n_{a}'m_{a}'}.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_1>;
// --------- class _c2py_cls_2 -----------
using _c2py_cls_2                                            = triqs::modest::ibz_symmetry_ops::op;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_2>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_2> = "triqs_modest.misc.Op";

static int synth_constructor_2(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::ibz_symmetry_ops::op.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_2> *)self)->_c = new _c2py_cls_2{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::ibz_symmetry_ops::op from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_2> *)self)->_c);
  de("mats", self_c.mats, false);
  de("permutation", self_c.permutation, false);
  de("time_inv", self_c.time_inv, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_2> = synth_constructor_2;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_2> = c2py::replace_tags(
   R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
mats : {par_0}

permutation : {par_1}

time_inv : {par_2}

)DOC",
   "par",
   {c2py::python_typename<std::vector<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>(),
    c2py::python_typename<std::vector<long>>(), c2py::python_typename<long>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_2>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_5 = R"DOC()DOC";
constexpr auto _c2py_doc_member_6 = R"DOC()DOC";
constexpr auto _c2py_doc_member_7 = R"DOC()DOC";
static PyObject *prop_get_dict_2(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_2> *)self)->_c);
  c2py::pydict dic;
  dic["mats"]        = self_c.mats;
  dic["permutation"] = self_c.permutation;
  dic["time_inv"]    = self_c.time_inv;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_2>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_2::mats, _c2py_cls_2>("mats", _c2py_doc_member_5),
   c2py::getsetdef_from_member<&_c2py_cls_2::permutation, _c2py_cls_2>("permutation", _c2py_doc_member_6),
   c2py::getsetdef_from_member<&_c2py_cls_2::time_inv, _c2py_cls_2>("time_inv", _c2py_doc_member_7),
   {"__dict__", (getter)prop_get_dict_2, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> const std::string c2py::tp_doc<_c2py_cls_2> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_2>;
// --------- class _c2py_cls_3 -----------
using _c2py_cls_3                                            = triqs::tb::superlattice;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_3>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_3> = "triqs_modest.misc.Superlattice";
static auto _c2py_init_0                                     = c2py::dispatcher_c_kw_t{c2py::c_constructor<
                                       _c2py_cls_3, nda::basic_array<long, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
                                       nda::basic_array<long, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>("sl_units", "cluster_pts")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_3>    = c2py::pyfkw_constructor<_c2py_init_0>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_3> = _c2py_init_0.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_3>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0 = R"DOC()DOC";
static constexpr auto prop_doc_1 = R"DOC()DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_3>[] = {

   {"dim", c2py::getter_from_method<c2py::castmc<>(&triqs::tb::superlattice::dim)>, nullptr, prop_doc_0, nullptr},
   {"n_cluster_sites", c2py::getter_from_method<c2py::castmc<>(&triqs::tb::superlattice::n_cluster_sites)>, nullptr, prop_doc_1, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> const std::string c2py::tp_doc<_c2py_cls_3> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_3>;
// --------- class _c2py_cls_4 -----------
using _c2py_cls_4                                            = triqs::tb::tb_hamiltonian;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_4>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_4> = "triqs_modest.misc.TbHamiltonian";
static auto _c2py_init_1                                     = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_4, std::vector<std::array<long, 3>>,
                                                           std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                                                        nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>("Rs", "hoppings"),
   c2py::c_constructor<_c2py_cls_4>()};
template <> constexpr initproc c2py::tp_init<_c2py_cls_4>    = c2py::pyfkw_constructor<_c2py_init_1>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_4> = _c2py_init_1.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_4 const &self,
         nda::basic_array_view<const double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>> k_list)
         -> decltype(auto) { return self.operator()(k_list); },
      "self", "k_list"),
   c2py::cmethod([](_c2py_cls_4 const &self, std::array<double, 3> ks) -> decltype(auto) { return self.operator()(ks); }, "self", "ks")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_4> = c2py::pyfkw<_c2py_fun_0>;

// eigenvalues
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_4 const &self,
                    nda::basic_array_view<double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>> k)
                    -> decltype(auto) { return self.eigenvalues(k); },
                 "self", "k")};

// eigenvectors
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_4 const &self,
                    nda::basic_array_view<double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>> k)
                    -> decltype(auto) { return self.eigenvectors(k); },
                 "self", "k")};

// get_R_idx
static auto const _c2py_fun_3 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_4 const &self, std::array<long, 3> R) -> decltype(auto) { return self.get_R_idx(R); }, "self", "R")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC()DOC");
static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC()DOC");
static const auto _c2py_doc_2 = _c2py_fun_2.doc(R"DOC()DOC");
static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_4>[] = {
   {"eigenvalues", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"eigenvectors", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"get_R_idx", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_4>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_h5<_c2py_cls_4>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_h5<_c2py_cls_4>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_2 = R"DOC()DOC";
static constexpr auto prop_doc_3 = R"DOC()DOC";
static constexpr auto prop_doc_4 = R"DOC()DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_4>[] = {

   {"elements", c2py::getter_from_method<c2py::castmc<>(&triqs::tb::tb_hamiltonian::elements)>, nullptr, prop_doc_2, nullptr},
   {"hoppings", c2py::getter_from_method<c2py::castmc<>(&triqs::tb::tb_hamiltonian::hoppings)>, nullptr, prop_doc_3, nullptr},
   {"n_orbitals", c2py::getter_from_method<c2py::castmc<>(&triqs::tb::tb_hamiltonian::n_orbitals)>, nullptr, prop_doc_4, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> const std::string c2py::tp_doc<_c2py_cls_4> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_4>;
// --------- class _c2py_cls_5 -----------
using _c2py_cls_5                                            = triqs::lattice::bz_int_options;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_5>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_5> = "triqs_modest.misc.BzIntOptions";

static int synth_constructor_3(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::lattice::bz_int_options.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_5> *)self)->_c = new _c2py_cls_5{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::lattice::bz_int_options from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_5> *)self)->_c);
  de("tolerance", self_c.tolerance, true);
  de("k_grid", self_c.k_grid, true);
  de("delta_k_grid", self_c.delta_k_grid, true);
  de("k_grid_max", self_c.k_grid_max, true);
  de("run_adaptive", self_c.run_adaptive, true);
  de("run_ptr", self_c.run_ptr, true);
  de("verbose", self_c.verbose, true);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_5> = synth_constructor_3;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_5> = c2py::replace_tags(
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
PyMethodDef c2py::tp_methods<_c2py_cls_5>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_8  = R"DOC()DOC";
constexpr auto _c2py_doc_member_9  = R"DOC()DOC";
constexpr auto _c2py_doc_member_10 = R"DOC()DOC";
constexpr auto _c2py_doc_member_11 = R"DOC()DOC";
constexpr auto _c2py_doc_member_12 = R"DOC()DOC";
constexpr auto _c2py_doc_member_13 = R"DOC()DOC";
constexpr auto _c2py_doc_member_14 = R"DOC()DOC";
static PyObject *prop_get_dict_3(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_5> *)self)->_c);
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
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_5>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_5::tolerance, _c2py_cls_5>("tolerance", _c2py_doc_member_8),
   c2py::getsetdef_from_member<&_c2py_cls_5::k_grid, _c2py_cls_5>("k_grid", _c2py_doc_member_9),
   c2py::getsetdef_from_member<&_c2py_cls_5::delta_k_grid, _c2py_cls_5>("delta_k_grid", _c2py_doc_member_10),
   c2py::getsetdef_from_member<&_c2py_cls_5::k_grid_max, _c2py_cls_5>("k_grid_max", _c2py_doc_member_11),
   c2py::getsetdef_from_member<&_c2py_cls_5::run_adaptive, _c2py_cls_5>("run_adaptive", _c2py_doc_member_12),
   c2py::getsetdef_from_member<&_c2py_cls_5::run_ptr, _c2py_cls_5>("run_ptr", _c2py_doc_member_13),
   c2py::getsetdef_from_member<&_c2py_cls_5::verbose, _c2py_cls_5>("verbose", _c2py_doc_member_14),
   {"__dict__", (getter)prop_get_dict_3, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> const std::string c2py::tp_doc<_c2py_cls_5> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_5>;

// ==================== module functions ====================

// Hloc
static auto const _c2py_fun_4 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const std::vector<triqs::tb::tb_hamiltonian> &H_sigma,
                 const std::vector<triqs::modest::atomic_orbs> &atomic_shells) { return triqs::modest::Hloc(H_sigma, atomic_shells); },
              "H_sigma", "atomic_shells")};

// add_local_term
static auto const _c2py_fun_5 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::modest::one_body_elements_tb &obe,
      const nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>
         &local_term) { return triqs::modest::add_local_term(obe, local_term); },
   "obe", "local_term")};

// discover_symmetries
static auto const _c2py_fun_6 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const nda::basic_array<
         nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
         nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &Hloc0,
      const std::vector<triqs::modest::atomic_orbs> &atomic_shells, double block_threshold,
      bool diagonalize_hloc) { return triqs::modest::discover_symmetries(Hloc0, atomic_shells, block_threshold, diagonalize_hloc); },
   "Hloc0", "atomic_shells", "block_threshold", "diagonalize_hloc")};

// double_counting
static auto const _c2py_fun_7 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const nda::basic_array<
         nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
         nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &density_matrix,
      double U_int, double J_hund, const std::string method) { return triqs::modest::double_counting(density_matrix, U_int, J_hund, method); },
   "density_matrix", "U_int", "J_hund", "method")};

// extend_to_spin
static auto const _c2py_fun_8 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const triqs::modest::one_body_elements_tb &obe) { return triqs::modest::extend_to_spin(obe); }, "obe")};

// fold
static auto const _c2py_fun_9 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::tb::superlattice &sl, const triqs::tb::tb_hamiltonian &tb) { return triqs::tb::fold(sl, tb); }, "sl", "tb"),
   c2py::cfun([](const triqs::tb::superlattice &sl, const triqs::modest::one_body_elements_tb &obe) { return triqs::modest::fold(sl, obe); }, "sl",
              "obe")};

// gloc
static auto const _c2py_fun_10 = c2py::dispatcher_f_kw_t{
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

// one_body_elements_from_model
static auto const _c2py_fun_11 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](const std::vector<std::array<long, 3>> &Rs,
         const std::vector<
            nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &HR,
         triqs::modest::spin_kind_e spin_kind, std::vector<triqs::modest::atomic_orbs> atomic_shells) {
        return triqs::modest::one_body_elements_from_model(Rs, HR, spin_kind, atomic_shells);
      },
      "Rs", "HR", "spin_kind", "atomic_shells"),
   c2py::cfun(
      [](const std::vector<std::array<long, 3>> &Rs,
         const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                            nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &HR_up,
         const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                            nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &HR_dn,
         triqs::modest::spin_kind_e spin_kind, std::vector<triqs::modest::atomic_orbs> atomic_shells) {
        return triqs::modest::one_body_elements_from_model(Rs, HR_up, HR_dn, spin_kind, atomic_shells);
      },
      "Rs", "HR_up", "HR_dn", "spin_kind", "atomic_shells")};

// permute_local_space
static auto const _c2py_fun_12 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const std::vector<std::vector<long>> &atom_partition,
                 const triqs::modest::one_body_elements_on_grid &x) { return triqs::modest::permute_local_space(atom_partition, x); },
              "atom_partition", "x")};

// rotate
static auto const _c2py_fun_13 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::modest::one_body_elements_tb &obe,
      const nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &U) {
     return triqs::modest::rotate(obe, U);
   },
   "obe", "U")};

// rotate_embedded_self_energy
static auto const _c2py_fun_14 =
   c2py::dispatcher_f_kw_t{
      c2py::cfun(
         [](const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_embed_dynamic_loc,
            const nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>
               &U) { return triqs::modest::rotate_embedded_self_energy<triqs::mesh::imfreq>(Sigma_embed_dynamic_loc, U); },
         "Sigma_embed_dynamic_loc", "U"),
      c2py::cfun(
         [](const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_embed_dynamic_loc,
            const nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>
               &U) { return triqs::modest::rotate_embedded_self_energy<triqs::mesh::refreq>(Sigma_embed_dynamic_loc, U); },
         "Sigma_embed_dynamic_loc", "U"),
      c2py::cfun(
         [](const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_embed_dynamic_loc,
            const nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>
               &U) { return triqs::modest::rotate_embedded_self_energy<triqs::mesh::dlr_imfreq>(Sigma_embed_dynamic_loc, U); },
         "Sigma_embed_dynamic_loc", "U"),
      c2py::cfun(
         [](const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_embed_dynamic_loc,
            const triqs::modest::one_body_elements_on_grid &obe) {
           return triqs::modest::rotate_embedded_self_energy<triqs::mesh::imfreq>(Sigma_embed_dynamic_loc, obe);
         },
         "Sigma_embed_dynamic_loc", "obe"),
      c2py::cfun(
         [](const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_embed_dynamic_loc,
            const triqs::modest::one_body_elements_on_grid &obe) {
           return triqs::modest::rotate_embedded_self_energy<triqs::mesh::refreq>(Sigma_embed_dynamic_loc, obe);
         },
         "Sigma_embed_dynamic_loc", "obe"),
      c2py::cfun(
         [](const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &Sigma_embed_dynamic_loc,
            const triqs::modest::one_body_elements_on_grid &obe) {
           return triqs::modest::rotate_embedded_self_energy<triqs::mesh::dlr_imfreq>(Sigma_embed_dynamic_loc, obe);
         },
         "Sigma_embed_dynamic_loc", "obe"),
      c2py::cfun(
         [](const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &Sigma_embed_dynamic_loc,
            const nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>
               &U) { return triqs::modest::rotate_embedded_self_energy(Sigma_embed_dynamic_loc, U); },
         "Sigma_embed_dynamic_loc", "U"),
      c2py::cfun(
         [](const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &Sigma_embed_dynamic_loc,
            const triqs::modest::one_body_elements_on_grid &obe) { return triqs::modest::rotate_embedded_self_energy(Sigma_embed_dynamic_loc, obe); },
         "Sigma_embed_dynamic_loc", "obe"),
      c2py::cfun(
         [](const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &Sigma_embed_static_loc,
            const nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>
               &U) { return triqs::modest::rotate_embedded_self_energy(Sigma_embed_static_loc, U); },
         "Sigma_embed_static_loc", "U"),
      c2py::cfun(
         [](const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &Sigma_embed_static_loc,
            const triqs::modest::one_body_elements_on_grid &obe) { return triqs::modest::rotate_embedded_self_energy(Sigma_embed_static_loc, obe); },
         "Sigma_embed_static_loc", "obe")};

// sigma_to_data_idx
static auto const _c2py_fun_15 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](triqs::modest::spin_kind_e spin_kind, long sigma) { return triqs::modest::sigma_to_data_idx(spin_kind, sigma); }, "spin_kind", "sigma")};

static const auto _c2py_doc_4 = _c2py_fun_4.doc(
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
   {{c2py::python_typename<const std::vector<triqs::tb::tb_hamiltonian> &>()},
    {c2py::python_typename<const std::vector<triqs::modest::atomic_orbs> &>()}},
   {c2py::python_typename<nda::basic_array<
      nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
      nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC()DOC");
static const auto _c2py_doc_6 = _c2py_fun_6.doc(
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
   {{c2py::python_typename<const nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()},
    {c2py::python_typename<const std::vector<triqs::modest::atomic_orbs> &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<bool>()}},
   {c2py::python_typename<
      std::pair<nda::basic_array<std::vector<long, std::allocator<long>>, 2, nda::C_layout, 'A',
                                 nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
                nda::basic_array<
                   nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
                   2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>()});
static const auto _c2py_doc_7 = _c2py_fun_7.doc(
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
   {{c2py::python_typename<const nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const std::string>()}},
   {c2py::python_typename<
      std::pair<nda::basic_array<nda::basic_array<double, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
                                 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
                nda::basic_array<double, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>()});
static const auto _c2py_doc_8 = _c2py_fun_8.doc(R"DOC()DOC");
static const auto _c2py_doc_9 = _c2py_fun_9.doc(
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
   {{c2py::python_typename<const triqs::tb::superlattice &>()}, {c2py::python_typename<const triqs::modest::one_body_elements_tb &>()}},
   {c2py::python_typename<triqs::modest::one_body_elements_tb>()});
static const auto _c2py_doc_10 = _c2py_fun_10.doc(R"DOC()DOC");
static const auto _c2py_doc_11 = _c2py_fun_11.doc(R"DOC()DOC");
static const auto _c2py_doc_12 = _c2py_fun_12.doc(R"DOC()DOC");
static const auto _c2py_doc_13 =
   _c2py_fun_13.doc(R"DOC(
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
static const auto _c2py_doc_14 = _c2py_fun_14.doc(
   R"DOC(
[1, 2, 3, 4, 5, 6, 7, 8] Rotate the dynamic part of the embedded self-energy from the local (solver) basis to the orbital basis.

The rotation is independent of frequency and is performe as UΣ(ω)U†.

------

[9, 10] Rotate the static part of the embedded self-energy from the local (solver) basis to the orbital basis.

The rotation is independent of frequency and is performe as UΣ(ω)U†.

------

Parameters
----------
Sigma_embed_dynamic_loc : {par_0}
   The dynamic part of the embedded self-energy in the local (solver) basis.
U : {par_1}
   The rotation matrix from the local (solver) basis to the orbital basis.
obe : {par_2}
   The one-body elements on grid containing the rotation matrices.
Sigma_embed_static_loc : {par_3}
   The static part of the embedded self-energy in the local (solver) basis.

Returns
-------
[1, 4] : {ret_0}
   The dynamic part of the embedded self-energy in the orbital basis.

[2, 5] : {ret_1}
   The dynamic part of the embedded self-energy in the orbital basis.

[3, 6] : {ret_2}
   The dynamic part of the embedded self-energy in the orbital basis.

[7, 8] : {ret_3}
   The dynamic part of the embedded self-energy in the orbital basis.

[9, 10] : {ret_4}
   The static part of the embedded self-energy in the orbital basis.
)DOC",
   {{c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>(),
     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>(),
     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>(),
     c2py::python_typename<const std::vector<
        nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &>()},
    {c2py::python_typename<
       const nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()},
    {c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()},
    {c2py::python_typename<const std::vector<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &>()}},
   {c2py::python_typename<std::vector<triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>>>(),
    c2py::python_typename<std::vector<triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>>>(),
    c2py::python_typename<std::vector<triqs::gfs::gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>>>(),
    c2py::python_typename<std::vector<
       nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>(),
    c2py::python_typename<std::vector<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>()});
static const auto _c2py_doc_15 = _c2py_fun_15.doc(R"DOC(
Map a spin index to a data index.

The mapping depends on the spin kind:

* `Polarized`: :math:`\sigma \to \sigma` (an object can have different values for different :math:`\sigma`).
* `NonPolarized`: :math:`\sigma \to 0` (an object is the same for both :math:`\sigma`, so only one copy is stored).
* `NonCollinear`: :math:`\sigma \to 0` (:math:`\sigma` is always 0).
)DOC");
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"Hloc", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"add_local_term", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_5.c_str()},
   {"discover_symmetries", (PyCFunction)c2py::pyfkw<_c2py_fun_6>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_6.c_str()},
   {"double_counting", (PyCFunction)c2py::pyfkw<_c2py_fun_7>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_7.c_str()},
   {"extend_to_spin", (PyCFunction)c2py::pyfkw<_c2py_fun_8>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_8.c_str()},
   {"fold", (PyCFunction)c2py::pyfkw<_c2py_fun_9>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_9.c_str()},
   {"gloc", (PyCFunction)c2py::pyfkw<_c2py_fun_10>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_10.c_str()},
   {"one_body_elements_from_model", (PyCFunction)c2py::pyfkw<_c2py_fun_11>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_11.c_str()},
   {"permute_local_space", (PyCFunction)c2py::pyfkw<_c2py_fun_12>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_12.c_str()},
   {"rotate", (PyCFunction)c2py::pyfkw<_c2py_fun_13>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_13.c_str()},
   {"rotate_embedded_self_energy", (PyCFunction)c2py::pyfkw<_c2py_fun_14>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_14.c_str()},
   {"sigma_to_data_idx", (PyCFunction)c2py::pyfkw<_c2py_fun_15>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_15.c_str()},
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
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_0>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_1>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_2>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_3>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_4>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_5>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_0, "AtomicOrbs");
  _add_type(_c2py_cls_1, "IbzSymmetryOps");
  _add_type(_c2py_cls_2, "Op");
  _add_type(_c2py_cls_3, "Superlattice");
  _add_type(_c2py_cls_4, "TbHamiltonian");
  _add_type(_c2py_cls_5, "BzIntOptions");
#undef _add_type

  c2py::pyref module = c2py::pyref::module("h5.formats");
  if (not module) return nullptr;
  c2py::pyref register_class = module.attr("register_class");

  register_h5_type<_c2py_cls_4>(register_class);

  return m;
}
#endif
// CLAIR_WRAP_GEN
