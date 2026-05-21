
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

template <> constexpr bool c2py::is_wrapped<triqs::modest::spin_kind_e> = true;
template <>
const std::map<triqs::modest::spin_kind_e, str_t> c2py::enum_to_string<triqs::modest::spin_kind_e> = {
   {triqs::modest::spin_kind_e::Polarized, "Polarized"},
   {triqs::modest::spin_kind_e::NonPolarized, "NonPolarized"},
   {triqs::modest::spin_kind_e::NonColinear, "NonColinear"}};

// ==================== module classes =====================

// --------- class _c2py_cls_0 -----------
using _c2py_cls_0                                            = triqs::modest::atomic_orbs;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs_modest.obe.AtomicOrbs";

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
using _c2py_cls_1                                            = triqs::modest::local_space;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs_modest.obe.LocalSpace";
static const auto _c2py_init_0                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_1, triqs::modest::spin_kind_e, std::vector<triqs::modest::atomic_orbs>,
                       nda::basic_array<std::vector<long, std::allocator<long>>, 2, nda::C_layout, 'A',
                                        nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
                       nda::basic_array<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M',
                                                         nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
                                        2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
                       nda::basic_array<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M',
                                                         nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
                                        1, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(
      "spin_kind", "atomic_shells", "irreps_decomp_per_atom", "rotation_from_dft_to_local_basis", "rotation_from_spherical_to_dft_basis"),
   c2py::c_constructor<_c2py_cls_1>()};
template <> constexpr initproc c2py::tp_init<_c2py_cls_1> = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_1> = _c2py_init_0.doc(
   R"DOC(
Construct a new local space object.

Parameters
----------
spin_kind : {par_0}
   Kind of :math:`\sigma` index.
atomic_shells : {par_1}
   List of all atomic orbitals.
irreps_decomp_per_atom : {par_2}
   List of all blocks spanning :math:`\mathcal{C}` space -> atoms_block_decomposition.
rotation_from_dft_to_local_basis : {par_3}
   Rotation matices from DFT to local basis.
rotation_from_spherical_to_dft_basis : {par_4}
   Rotation matrices from spherical to DFT basis.
)DOC",
   {{c2py::python_typename<triqs::modest::spin_kind_e>()},
    {c2py::python_typename<std::vector<triqs::modest::atomic_orbs>>()},
    {c2py::python_typename<nda::basic_array<std::vector<long, std::allocator<long>>, 2, nda::C_layout, 'A',
                                            nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()},
    {c2py::python_typename<nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()},
    {c2py::python_typename<nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 1,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()}});
// atomic_view
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_1 &self, const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &G_C)
                    -> decltype(auto) { return self.template atomic_view<triqs::mesh::imfreq>(G_C); },
                 "self", "G_C"),
   c2py::cmethod(
      [](_c2py_cls_1 &self,
         const nda::basic_array<
            nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
            nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &matrix_C) -> decltype(auto) {
        return self.atomic_view(matrix_C);
      },
      "self", "matrix_C")};

// first_shell_of_its_equiv_cls
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_1 const &self, long idx) -> decltype(auto) { return self.first_shell_of_its_equiv_cls(idx); }, "self", "idx")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(
   R"DOC(
[1] Views a 2-dim block GF according to the atomic decomposition.

------

[2] Creates an atomic view of block matrices by extracting specific slices based on the atomic decomposition
of the current object.

------

Parameters
----------
G_C : {par_0}
   A 2-dim block GF spanning the entire :math:`\mathcal{C}` space [0, sigma](M, M).
matrix_C : {par_1}
   The input block matrices object from which the atomic view is created.

Returns
-------
[1] : {ret_0}
   A 2-dim block GF in the atomic decomposition view [atom, sigma][m_orb, m_orb].

[2] : {ret_1}
   2-dim array containing the new block matrices in the atomic view.
)DOC",
   {{c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>()},
    {c2py::python_typename<const nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()}},
   {c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2>>(),
    c2py::python_typename<nda::basic_array<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
       nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC(
Given the index of an atomic shell, return the index of the first atomic shell of its equivalence class.
)DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {
   {"atomic_view", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"first_shell_of_its_equiv_cls", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0  = R"DOC(Names of the atoms in the orbital set.)DOC";
static constexpr auto prop_doc_1  = R"DOC(Dimensions of each atomic shell, in order.)DOC";
static constexpr auto prop_doc_2  = R"DOC(List of all atomic shells spanning the :math:`\mathcal{C}` space.)DOC";
static constexpr auto prop_doc_3  = R"DOC(2-dim array of all blocks spanning :math:`\mathcal{C}` space -> atoms_block_decomposition.)DOC";
static constexpr auto prop_doc_4  = R"DOC(Dimension of the correlated space.)DOC";
static constexpr auto prop_doc_5  = R"DOC(The number of atoms.)DOC";
static constexpr auto prop_doc_6  = R"DOC(Dimension of the :math:`\sigma` index.)DOC";
static constexpr auto prop_doc_7  = R"DOC(2-dim array of all :math:`(a, \sigma)` local rotation matices that rotate the data.)DOC";
static constexpr auto prop_doc_8  = R"DOC(Array of rotation matrices from spherical harmonics to dft specific orbital basis.)DOC";
static constexpr auto prop_doc_9  = R"DOC(Names of spin indices for naming blocks in block GFs.)DOC";
static constexpr auto prop_doc_10 = R"DOC(Spin kind of :math:`\sigma` index.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_1>[] = {

   {"atom_names", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::local_space::atom_names)>, nullptr, prop_doc_0, nullptr},
   {"atomic_decomposition", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::local_space::atomic_decomposition)>, nullptr, prop_doc_1,
    nullptr},
   {"atomic_shells", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::local_space::atomic_shells)>, nullptr, prop_doc_2, nullptr},
   {"atoms_block_decomposition", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::local_space::atoms_block_decomposition)>, nullptr,
    prop_doc_3, nullptr},
   {"dim", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::local_space::dim)>, nullptr, prop_doc_4, nullptr},
   {"n_atoms", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::local_space::n_atoms)>, nullptr, prop_doc_5, nullptr},
   {"n_sigma", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::local_space::n_sigma)>, nullptr, prop_doc_6, nullptr},
   {"rotation_from_dft_to_local_basis", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::local_space::rotation_from_dft_to_local_basis)>,
    nullptr, prop_doc_7, nullptr},
   {"rotation_from_spherical_to_dft_basis",
    c2py::getter_from_method<c2py::castmc<>(&triqs::modest::local_space::rotation_from_spherical_to_dft_basis)>, nullptr, prop_doc_8, nullptr},
   {"sigma_names", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::local_space::sigma_names)>, nullptr, prop_doc_9, nullptr},
   {"spin_kind", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::local_space::spin_kind)>, nullptr, prop_doc_10, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_1> = R"DOC(Describe the atomic orbitals within downfolded :math:`\mathcal{C}` space.

The local space :math:`\mathcal{C}` defines the correlated subspace which defines impurities to be solved
within DMFT.

The DFT + DMFT equations involve three different spaces, following (mostly) the notations of [S. Beck et al. 2022]:

* The (reduced) Bloch space :math:`{\cal B}` contains bands of dispersion :math:`\varepsilon_{\nu}^{\sigma}(\mathbf{k})`,
  in some window of energy. Here, :math:`\nu` is the band index (:math:`0 \leq \nu < N_\nu^{\mathbf{k}}`) and
  :math:`\mathbf{k}` is a point in the Brillouin zone. We define
  :math:`N_\nu \equiv \mathrm{max}_{\mathbf{k}} N_\nu^{\mathbf{k}}`.
* The Wannier space :math:`{\cal W}` is spanned from Wannier functions constructed from :math:`{\cal B}`.
* The correlated space :math:`{\cal C} \subseteq {\cal W}` containing :math:`M` Wannier orbitals, is a subspace of the
  Wannier space, in which the self-energy is approximated by the embedding. :math:`{\cal C}` is spanned by Wannier
  functions at several atoms/sites with index :math:`a` at position :math:`R_a` and orbital/Wannier index :math:`m_{a}`.
  :math:`{\cal C}` is indexed by a composite index :math:`m = (a, m_{a})`, with :math:`0\leq m \leq M-1` and
  :math:`M=\sum_{a}\mathrm{max}(m_{a}`). We will write the main equations with the composite :math:`m` index, as the
  :math:`m = (a, m_{a})` decomposition of :math:`m` is not, in general, appropriate for embeddings.

The :math:`\sigma` index is a general block diagonal index. In simple cases, it is the spin index, but not always.

* In "spin (non-)polarized" computations, :math:`\sigma` is the spin index.
* In spin-orbit or Nambu computations, the spin index is merged with :math:`m` and :math:`\nu`, so :math:`\sigma =0`
  (i.e. one value of the index, equivalent to no index at all).)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_1>;
// --------- class _c2py_cls_2 -----------
using _c2py_cls_2                                            = triqs::modest::ibz_symmetry_ops;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_2>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_2> = "triqs_modest.obe.IbzSymmetryOps";

static int synth_constructor_1(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::ibz_symmetry_ops.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_2> *)self)->_c = new _c2py_cls_2{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::ibz_symmetry_ops from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_2> *)self)->_c);
  de("ops", self_c.ops, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_2> = synth_constructor_1;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_2> =
   c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
ops : {par_0}

)DOC",
                      "par", {c2py::python_typename<std::vector<triqs::modest::ibz_symmetry_ops::op>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_2>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_4 = R"DOC()DOC";
static PyObject *prop_get_dict_1(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_2> *)self)->_c);
  c2py::pydict dic;
  dic["ops"] = self_c.ops;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_2>[] = {c2py::getsetdef_from_member<&_c2py_cls_2::ops, _c2py_cls_2>("ops", _c2py_doc_member_4),
                                                        {"__dict__", (getter)prop_get_dict_1, nullptr, "", nullptr},
                                                        {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_2> =
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
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_2>;
// --------- class _c2py_cls_3 -----------
using _c2py_cls_3                                            = triqs::modest::ibz_symmetry_ops::op;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_3>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_3> = "triqs_modest.obe.Op";

static int synth_constructor_2(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::ibz_symmetry_ops::op.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_3> *)self)->_c = new _c2py_cls_3{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::ibz_symmetry_ops::op from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_3> *)self)->_c);
  de("mats", self_c.mats, false);
  de("permutation", self_c.permutation, false);
  de("time_inv", self_c.time_inv, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_3> = synth_constructor_2;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_3> = c2py::replace_tags(
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
PyMethodDef c2py::tp_methods<_c2py_cls_3>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_5 = R"DOC()DOC";
constexpr auto _c2py_doc_member_6 = R"DOC()DOC";
constexpr auto _c2py_doc_member_7 = R"DOC()DOC";
static PyObject *prop_get_dict_2(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_3> *)self)->_c);
  c2py::pydict dic;
  dic["mats"]        = self_c.mats;
  dic["permutation"] = self_c.permutation;
  dic["time_inv"]    = self_c.time_inv;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_3>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_3::mats, _c2py_cls_3>("mats", _c2py_doc_member_5),
   c2py::getsetdef_from_member<&_c2py_cls_3::permutation, _c2py_cls_3>("permutation", _c2py_doc_member_6),
   c2py::getsetdef_from_member<&_c2py_cls_3::time_inv, _c2py_cls_3>("time_inv", _c2py_doc_member_7),
   {"__dict__", (getter)prop_get_dict_2, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> const std::string c2py::tp_doc<_c2py_cls_3> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_3>;
// --------- class _c2py_cls_4 -----------
using _c2py_cls_4                                            = triqs::modest::band_dispersion;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_4>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_4> = "triqs_modest.obe.BandDispersion";

static int synth_constructor_3(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::band_dispersion.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_4> *)self)->_c = new _c2py_cls_4{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::band_dispersion from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_4> *)self)->_c);
  de("spin_kind", self_c.spin_kind, false);
  de("H_k", self_c.H_k, false);
  de("n_bands_per_k", self_c.n_bands_per_k, false);
  de("k_weights", self_c.k_weights, false);
  de("matrix_valued", self_c.matrix_valued, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_4> = synth_constructor_3;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_4> = c2py::replace_tags(
   R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
spin_kind : {par_0}

H_k : {par_1}

n_bands_per_k : {par_2}

k_weights : {par_3}

matrix_valued : {par_4}

)DOC",
   "par",
   {c2py::python_typename<triqs::modest::spin_kind_e>(),
    c2py::python_typename<
       nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<nda::basic_array<long, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<nda::basic_array<double, 1, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<bool>()});
// H
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_4 const &self, long sigma, long k_idx) -> decltype(auto) { return self.H(sigma, k_idx); }, "self", "sigma", "k_idx")};

// N_nu
static auto const _c2py_fun_3 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_4 const &self, long sigma, long k_idx) -> decltype(auto) { return self.N_nu(sigma, k_idx); }, "self", "sigma", "k_idx")};

static const auto _c2py_doc_2 =
   _c2py_fun_2.doc(R"DOC(
Get :math:`H^{\sigma}_{\nu\nu'}(\mathbf{k})` for a given :math:`\mathbf{k}` and :math:`\sigma`.

Parameters
----------
sigma : {par_0}
   Spin index :math:`\sigma`.
k_idx : {par_1}
   Index of the k-point in the grid.

Returns
-------
{ret_0}
   Matrix view of :math:`H^{\sigma}_{\nu\nu'}(\mathbf{k})` in :math:`(\nu, \nu')` for a given
   :math:`\mathbf{k}` and :math:`\sigma`.
)DOC",
                   {{c2py::python_typename<long>()}, {c2py::python_typename<long>()}},
                   {c2py::python_typename<nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'M', nda::default_accessor,
                                                                nda::borrowed<nda::mem::AddressSpace::Host>>>()});
static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC(
Number of bands for a given k-point and spin :math:`\sigma`.
)DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_4>[] = {
   {"H", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"N_nu", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_8  = R"DOC(Spin kind of the one-body data.)DOC";
constexpr auto _c2py_doc_member_9  = R"DOC(Hamiltonian :math:`H^{\sigma}_{\nu\nu'}(\mathbf{k})`.)DOC";
constexpr auto _c2py_doc_member_10 = R"DOC(Number of bands for each k-point and :math:`\sigma`.)DOC";
constexpr auto _c2py_doc_member_11 = R"DOC(Weight in the BZ for each k-point.)DOC";
constexpr auto _c2py_doc_member_12 = R"DOC(Is the dispersion matrix-valued?)DOC";
static PyObject *prop_get_dict_3(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_4> *)self)->_c);
  c2py::pydict dic;
  dic["spin_kind"]     = self_c.spin_kind;
  dic["H_k"]           = self_c.H_k;
  dic["n_bands_per_k"] = self_c.n_bands_per_k;
  dic["k_weights"]     = self_c.k_weights;
  dic["matrix_valued"] = self_c.matrix_valued;
  return dic.new_ref();
}
static constexpr auto prop_doc_11 = R"DOC(Number of k-points in the grid.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_4>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_4::spin_kind, _c2py_cls_4>("spin_kind", _c2py_doc_member_8),
   c2py::getsetdef_from_member<&_c2py_cls_4::H_k, _c2py_cls_4>("H_k", _c2py_doc_member_9),
   c2py::getsetdef_from_member<&_c2py_cls_4::n_bands_per_k, _c2py_cls_4>("n_bands_per_k", _c2py_doc_member_10),
   c2py::getsetdef_from_member<&_c2py_cls_4::k_weights, _c2py_cls_4>("k_weights", _c2py_doc_member_11),
   c2py::getsetdef_from_member<&_c2py_cls_4::matrix_valued, _c2py_cls_4>("matrix_valued", _c2py_doc_member_12),
   {"n_k", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::band_dispersion::n_k)>, nullptr, prop_doc_11, nullptr},
   {"__dict__", (getter)prop_get_dict_3, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_4> = R"DOC(The one-body dispersion as a function of momentum.

The band dispersion typically corresponds to the solution of a (Kohn-Sham) Hamiltonian which has been
diagonalized in momentum space and formulated in a basis of Bloch states :math:`| \phi_{\nu\mathbf{k}} \rangle` with
corresponding eigenvalues (:math:`\varepsilon_{\nu\mathbf{k}}^{\sigma}`).

A band dispersion object contains the DFT band structure :math:`\varepsilon_{\nu\mathbf{k}}^{\sigma}`, weights in the
Brillouin zone, and the spin kind used in the DFT calculation.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_4>;
// --------- class _c2py_cls_5 -----------
using _c2py_cls_5                                            = triqs::modest::downfolding_projector;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_5>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_5> = "triqs_modest.obe.DownfoldingProjector";

static int synth_constructor_4(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::downfolding_projector.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_5> *)self)->_c = new _c2py_cls_5{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::downfolding_projector from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_5> *)self)->_c);
  de("spin_kind", self_c.spin_kind, false);
  de("P_k", self_c.P_k, false);
  de("n_bands_per_k", self_c.n_bands_per_k, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_5> = synth_constructor_4;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_5> = c2py::replace_tags(
   R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
spin_kind : {par_0}

P_k : {par_1}

n_bands_per_k : {par_2}

)DOC",
   "par",
   {c2py::python_typename<triqs::modest::spin_kind_e>(),
    c2py::python_typename<
       nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<nda::basic_array<long, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});
// P
static auto const _c2py_fun_4 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_5 const &self, long sigma, long k_idx) -> decltype(auto) { return self.P(sigma, k_idx); }, "self", "sigma", "k_idx")};

// rotate_local_basis
static auto const _c2py_fun_5 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_5 const &self,
      const nda::basic_array<
         nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 2,
         nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &U) -> decltype(auto) {
     return self.rotate_local_basis(U);
   },
   "self", "U")};

static const auto _c2py_doc_4 =
   _c2py_fun_4.doc(R"DOC(
Get :math:`P_{m\nu}^{\sigma}(\mathbf{k})` for a given :math:`\mathbf{k}` and :math:`\sigma`.

Parameters
----------
sigma : {par_0}
   Spin index :math:`\sigma`.
k_idx : {par_1}
   Index of the k-point in the grid.

Returns
-------
{ret_0}
   Matrix view of :math:`P_{m\nu}^{\sigma}(\mathbf{k})` in :math:`(m, \nu)` for the given :math:`\mathbf{k}`
   and :math:`\sigma`.
)DOC",
                   {{c2py::python_typename<long>()}, {c2py::python_typename<long>()}},
                   {c2py::python_typename<nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'M', nda::default_accessor,
                                                                nda::borrowed<nda::mem::AddressSpace::Host>>>()});
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC(
Rotates the local basis of the downfolding projector.
)DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_5>[] = {
   {"P", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"rotate_local_basis", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_5.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_13 = R"DOC(Spin kind of the one-body data.)DOC";
constexpr auto _c2py_doc_member_14 = R"DOC(Projector :math:`P_{m\nu}^{\sigma}(\mathbf{k})`.)DOC";
constexpr auto _c2py_doc_member_15 = R"DOC(Number of bands for each k-point and :math:`\sigma`.)DOC";
static PyObject *prop_get_dict_4(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_5> *)self)->_c);
  c2py::pydict dic;
  dic["spin_kind"]     = self_c.spin_kind;
  dic["P_k"]           = self_c.P_k;
  dic["n_bands_per_k"] = self_c.n_bands_per_k;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_5>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_5::spin_kind, _c2py_cls_5>("spin_kind", _c2py_doc_member_13),
   c2py::getsetdef_from_member<&_c2py_cls_5::P_k, _c2py_cls_5>("P_k", _c2py_doc_member_14),
   c2py::getsetdef_from_member<&_c2py_cls_5::n_bands_per_k, _c2py_cls_5>("n_bands_per_k", _c2py_doc_member_15),
   {"__dict__", (getter)prop_get_dict_4, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_5> = R"DOC(The projector that downfolds the energy bands onto a set of localized atomic-like orbitals.

A downfolding projector contains the projector, the kind of spin used in the projection, and the number of bands
per k-point for cases when a band goes outside of the projection window.

The projectors :math:`P_{m\nu}^{\sigma}(\mathbf{k})` connect the Bloch space :math:`{\cal B}` to :math:`{\cal C}`. The
projectors are obtained from DFT codes or Wannier90. They are defined by

.. math::

   P_{(a,m_{a})\nu}^{\sigma}(\mathbf{k})\equiv e^{-i \mathbf{k} R_a}
   \langle \chi_{m_{a}}^{R_a \sigma} | \psi_{\nu}^{\sigma}(\mathbf{k}) \rangle,

where :math:`| \chi_{m_{a}}^{R_a \sigma} \rangle` is a Wannier function localized at atom :math:`a` with index
:math:`m_a` at position :math:`R_a` and :math:`| \psi_{\nu}^{\sigma}(\mathbf{k}) \rangle` is the Kohn-Sham wavefunction.
The relation between the Wannier and  Bloch function is therefore

.. math::

   | \chi_{m_{a}}^{R_a \sigma} \rangle = \sum_{\mathbf{k} \nu} e^{-i \mathbf{k} R_a} \bigl(P^\sigma_{(a,m_{a})\nu}
   (\mathbf{k})\bigr)^* | \psi_{\nu}^{\sigma}(\mathbf{k}) \rangle.

Some properties:

* Basis change in :math:`\cal C` space: They are given by a unitary matrix :math:`U`, the projector transforms as
  :math:`P^{'\sigma}_{m\nu}(\mathbf{k}) = U^{\dagger}_{m, m'} P^{\sigma}_{m'\nu}(\mathbf{k}).`
* Partial unitarity property: In general :math:`P` is not unitary as :math:`N_\nu^{\mathbf{k}} > M`. However, if the
  Wannier functions are reorthonormalized with respect to the truncated band basis, we have
  :math:`\sum_{ \nu} P^{\sigma}_{m\nu}(\mathbf{k}) P^{\dagger\sigma}_{\nu m'}(\mathbf{k}) = \delta_{mm'}`.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_5>;
// --------- class _c2py_cls_6 -----------
using _c2py_cls_6                                            = triqs::modest::downfolding_projector_ext;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_6>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_6> = "triqs_modest.obe.DownfoldingProjectorExt";
static const auto _c2py_init_1                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_6>()};
template <> constexpr initproc c2py::tp_init<_c2py_cls_6>    = c2py::pyfkw_constructor<_c2py_init_1>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_6> = _c2py_init_1.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_6>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_16 = R"DOC()DOC";
constexpr auto _c2py_doc_member_17 = R"DOC()DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_6>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_6::band_window, _c2py_cls_6>("band_window", _c2py_doc_member_16),
   c2py::getsetdef_from_member<&_c2py_cls_6::kpts, _c2py_cls_6>("kpts", _c2py_doc_member_17),

   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_6> = R"DOC(The projector that downfolds the energy bands onto a set of localized atomic-like orbitals.

A downfolding projector contains the projector, the kind of spin used in the projection, and the number of bands
per k-point for cases when a band goes outside of the projection window.

The projectors :math:`P_{m\nu}^{\sigma}(\mathbf{k})` connect the Bloch space :math:`{\cal B}` to :math:`{\cal C}`. The
projectors are obtained from DFT codes or Wannier90. They are defined by

.. math::

   P_{(a,m_{a})\nu}^{\sigma}(\mathbf{k})\equiv e^{-i \mathbf{k} R_a}
   \langle \chi_{m_{a}}^{R_a \sigma} | \psi_{\nu}^{\sigma}(\mathbf{k}) \rangle,

where :math:`| \chi_{m_{a}}^{R_a \sigma} \rangle` is a Wannier function localized at atom :math:`a` with index
:math:`m_a` at position :math:`R_a` and :math:`| \psi_{\nu}^{\sigma}(\mathbf{k}) \rangle` is the Kohn-Sham wavefunction.
The relation between the Wannier and  Bloch function is therefore

.. math::

   | \chi_{m_{a}}^{R_a \sigma} \rangle = \sum_{\mathbf{k} \nu} e^{-i \mathbf{k} R_a} \bigl(P^\sigma_{(a,m_{a})\nu}
   (\mathbf{k})\bigr)^* | \psi_{\nu}^{\sigma}(\mathbf{k}) \rangle.

Some properties:

* Basis change in :math:`\cal C` space: They are given by a unitary matrix :math:`U`, the projector transforms as
  :math:`P^{'\sigma}_{m\nu}(\mathbf{k}) = U^{\dagger}_{m, m'} P^{\sigma}_{m'\nu}(\mathbf{k}).`
* Partial unitarity property: In general :math:`P` is not unitary as :math:`N_\nu^{\mathbf{k}} > M`. However, if the
  Wannier functions are reorthonormalized with respect to the truncated band basis, we have
  :math:`\sum_{ \nu} P^{\sigma}_{m\nu}(\mathbf{k}) P^{\dagger\sigma}_{\nu m'}(\mathbf{k}) = \delta_{mm'}`.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_6>;
// --------- class _c2py_cls_7 -----------
using _c2py_cls_7                                            = triqs::modest::one_body_elements_on_grid;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_7>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_7> = "triqs_modest.obe.OneBodyElementsOnGrid";

static int synth_constructor_5(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::one_body_elements_on_grid.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_7> *)self)->_c = new _c2py_cls_7{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::one_body_elements_on_grid from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_7> *)self)->_c);
  de("H", self_c.H, false);
  de("C_space", self_c.C_space, false);
  de("P", self_c.P, false);
  de("ibz_symm_ops", self_c.ibz_symm_ops, true);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_7> = synth_constructor_5;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_7> = c2py::replace_tags(
   R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
H : {par_0}

C_space : {par_1}

P : {par_2}

ibz_symm_ops : {par_3}, default={}

)DOC",
   "par",
   {c2py::python_typename<triqs::modest::band_dispersion>(), c2py::python_typename<triqs::modest::local_space>(),
    c2py::python_typename<triqs::modest::downfolding_projector>(), c2py::python_typename<std::optional<triqs::modest::ibz_symmetry_ops>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_7>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_18 = R"DOC(Band dispersion.)DOC";
constexpr auto _c2py_doc_member_19 = R"DOC(Local :math:`\mathcal{C}` space.)DOC";
constexpr auto _c2py_doc_member_20 = R"DOC(Downfolding projector :math:`P`.)DOC";
constexpr auto _c2py_doc_member_21 = R"DOC(IBZ symmetrizer after a k-sum)DOC";
static PyObject *prop_get_dict_5(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_7> *)self)->_c);
  c2py::pydict dic;
  dic["H"]            = self_c.H;
  dic["C_space"]      = self_c.C_space;
  dic["P"]            = self_c.P;
  dic["ibz_symm_ops"] = self_c.ibz_symm_ops;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_7>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_7::H, _c2py_cls_7>("H", _c2py_doc_member_18),
   c2py::getsetdef_from_member<&_c2py_cls_7::C_space, _c2py_cls_7>("C_space", _c2py_doc_member_19),
   c2py::getsetdef_from_member<&_c2py_cls_7::P, _c2py_cls_7>("P", _c2py_doc_member_20),
   c2py::getsetdef_from_member<&_c2py_cls_7::ibz_symm_ops, _c2py_cls_7>("ibz_symm_ops", _c2py_doc_member_21),
   {"__dict__", (getter)prop_get_dict_5, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_7> = R"DOC(A one-body elements struct where all of the underlying data exists on a fixed momentum grid.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_7>;
// --------- class _c2py_cls_8 -----------
using _c2py_cls_8                                            = triqs::modest::one_body_elements_gw;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_8>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_8> = "triqs_modest.obe.OneBodyElementsGw";

static int synth_constructor_6(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::one_body_elements_gw.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_8> *)self)->_c = new _c2py_cls_8{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::one_body_elements_gw from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_8> *)self)->_c);
  de("C_space", self_c.C_space, false);
  de("P", self_c.P, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_8> = synth_constructor_6;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_8> =
   c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
C_space : {par_0}

P : {par_1}

)DOC",
                      "par",
                      {c2py::python_typename<triqs::modest::local_space>(), c2py::python_typename<triqs::modest::downfolding_projector_ext>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_8>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_22 = R"DOC()DOC";
constexpr auto _c2py_doc_member_23 = R"DOC()DOC";
static PyObject *prop_get_dict_6(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_8> *)self)->_c);
  c2py::pydict dic;
  dic["C_space"] = self_c.C_space;
  dic["P"]       = self_c.P;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_8>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_8::C_space, _c2py_cls_8>("C_space", _c2py_doc_member_22),
   c2py::getsetdef_from_member<&_c2py_cls_8::P, _c2py_cls_8>("P", _c2py_doc_member_23),
   {"__dict__", (getter)prop_get_dict_6, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_8> =
   R"DOC(A one-body elements struct for GW (CoQui) calculations.)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_8>;

// ==================== module functions ====================

// make_one_body_elements_gw
static auto const _c2py_fun_6 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const std::string &filename, double threshold,
                 bool diagonalize_hloc) { return triqs::modest::make_one_body_elements_gw(filename, threshold, diagonalize_hloc); },
              "filename", "threshold"_a = 1e-5, "diagonalize_hloc"_a = false)};

// one_body_elements_from_dft_converter
static auto const _c2py_fun_7 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const std::string &filename, double threshold,
                 bool diagonalize_hloc) { return triqs::modest::one_body_elements_from_dft_converter(filename, threshold, diagonalize_hloc); },
              "filename", "threshold"_a = 1.e-5, "diagonalize_hloc"_a = false)};

// one_body_elements_on_high_symmetry_path
static auto const _c2py_fun_8 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const std::string &filename, const triqs::modest::one_body_elements_on_grid &obe) {
     return triqs::modest::one_body_elements_on_high_symmetry_path(filename, obe);
   },
   "filename", "obe")};

// one_body_elements_with_theta_projectors
static auto const _c2py_fun_9 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const std::string &filename, const triqs::modest::one_body_elements_on_grid &obe) {
     return triqs::modest::one_body_elements_with_theta_projectors(filename, obe);
   },
   "filename", "obe")};

static const auto _c2py_doc_6 =
   _c2py_fun_6.doc(R"DOC(
Create a one-body elements for GW calculations with CoQui.

Using the data from the "dft_input" group, the local space and downfolding projector
are prepared to create a one-body elements. This object is intended to be used in GW
calculations.

Parameters
----------
filename : {par_0}
   Hdf5 file from dft_tools converter.

Returns
-------
{ret_0}
   One-body elements for GW calculations.
)DOC",
                   {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<triqs::modest::one_body_elements_gw>()});
static const auto _c2py_doc_7 =
   _c2py_fun_7.doc(R"DOC(
Create a one-body elements with orthonormalized projectors.

Using the data from the "dft_input" group, the band dispersion, local space, downfolding projector, and optional
IBZ symmetry ops are prepared to create a one-body elements. This object is intended to be used in DMFT
calculations.

Our strategy is to decompose the :math:`{\cal C}` space using the suitable basis for embedding. Each block of
embedded self-energy will then be mapped to corresponding impurity models self-energy.

The projectors are obtained from the DFT code or Wannier90, in some global coordinate system of the crystal.
Some basis transformations are required before performing the embedding.

1. A coordinate system rotation :math:`R^{a}_{m_{a},m_{a}'}` from the global coordinate system of atom (site) :math:`a`
   into the local coordinate system of the crystal. This rotations ensure that equivalent atoms have the same
   self-energy in the new basis, hence can be solved by the same impurity model.

2. Optionally, we perform a second rotation :math:`U` to infer the irreps from the local Hamiltonian.
   Unfortunately, at this stage, the proper information about irreps is not retrieved from the electronic structure
   code. A workaround has been to examine the local non-interacting Hamiltonian:
   

.. math::

   [H_{\mathrm{loc}}^{0}]_{m_{a} m_{a}'}^{a,\sigma} \equiv \sum_{\mathbf{k}} P_{(a,m_{a})\nu}^{\sigma}
   (\mathbf{k}) \varepsilon_{\nu\nu'}^{\sigma}(\mathbf{k}) [P_{(a',m_{\alpha}')\nu'}^{\sigma}
   (\mathbf{k})]^{\dagger}.

Its block structure is infered (up to a user-defined threshold), by discovering  a permutation of the orbitals
:math:`m_{a}` which renders :math:`H_{\mathrm{loc}}^{0}` block-diagonal. Optionally, we can diagonalize these smaller
blocks of the local non-interacting Hamiltonian (to reduce the off-diagonal elements in the impurity model
hybridization functions :math:`\Delta_{mm'}^{\sigma}`).

The Wannier basis is therefore transformed as:

.. math::

   P^{\sigma}_{m\nu}(\mathbf{k}) \leftarrow (RU)^{\dagger}_{m, m'} P^{\sigma}_{m'\nu}(\mathbf{k}).

Parameters
----------
filename : {par_0}
   Hdf5 file from dft_tools converter.
threshold : {par_1}
   Off-diagonal threshold for discovery of symmetries.
diagonalize_hloc : {par_2}
   Diagonalize the local non-interacting hamiltonian.

Returns
-------
{ret_0}
   The total electron density and a one-body elements.
)DOC",
                   {{c2py::python_typename<const std::string &>()}, {c2py::python_typename<double>()}, {c2py::python_typename<bool>()}},
                   {c2py::python_typename<std::pair<double, triqs::modest::one_body_elements_on_grid>>()});
static const auto _c2py_doc_8 =
   _c2py_fun_8.doc(R"DOC(
Create a one-body elements along specific k-path.

Using the data from the "dft_bands_input" group, the band disperion and downfolding projector
are prepared to create one-body elements. This object is intended to be used for post-processing
the momentum-resolved spectral function.

Parameters
----------
filename : {par_0}
   Hdf5 file from the DFTtools converter.
obe : {par_1}
   One-body elements that was ued in the DMFT calculation.

Returns
-------
{ret_0}
   One-body elements along high-symmetry k-path.
)DOC",
                   {{c2py::python_typename<const std::string &>()}, {c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()}},
                   {c2py::python_typename<triqs::modest::one_body_elements_on_grid>()});
static const auto _c2py_doc_9 =
   _c2py_fun_9.doc(R"DOC(
Create a one-body elements with the :math:`\Theta` projectors.

Using the data from the "dft_parproj_input" group, the local space, downfolding projectors,
and optional IBZ symmetry ops are prepared to create a one-body elements. This object is
intended to be used for post-processing the atom- and orbitally-resolved k-summed spectral functions.

Parameters
----------
filename : {par_0}
   Hdf5 file from DFTtools converter with "dft_parproj_input" group.
obe : {par_1}
   One-body elements that was used in the DMFT calculation.

Returns
-------
{ret_0}
   One-body elements using the :math:`\Theta` projectors.
)DOC",
                   {{c2py::python_typename<const std::string &>()}, {c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()}},
                   {c2py::python_typename<triqs::modest::one_body_elements_on_grid>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"make_one_body_elements_gw", (PyCFunction)c2py::pyfkw<_c2py_fun_6>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_6.c_str()},
   {"one_body_elements_from_dft_converter", (PyCFunction)c2py::pyfkw<_c2py_fun_7>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_7.c_str()},
   {"one_body_elements_on_high_symmetry_path", (PyCFunction)c2py::pyfkw<_c2py_fun_8>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_8.c_str()},
   {"one_body_elements_with_theta_projectors", (PyCFunction)c2py::pyfkw<_c2py_fun_9>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_9.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "obe", /* name of module */
                                        R"RAWDOC(The one-body elements abstraction in ModEST.

One-body elements (obe for short) from DFT data contains all of the one-body data that is typically converted from a density-functional theory (DFT) 
calculation. The one-body elements struct from DFT contains four components:

- **Local space** defines the local correlated subspace to be solved within DMFT.
- **Band dispersion** contains the band dispersion :math:`\varepsilon(k, \nu, \sigma)`  where :math:`\varepsilon` is the energy at 
  momentum k, band index :math:`\sigma`, and spin :math:`\sigma`.
- **Downfolding projector** contains the projector which downfolds from Bloch space :math:`\mathcal{B}` (indexed by :math:`\nu`) to the 
  correlated space (index by :math:`m`).    

To make it easy to build this object, ModEST provides factory functions that convert raw DFT data (from codes like VASP, Wien2k) into fully prepared OBE objects ready for DMFT.
)RAWDOC",                                      /* module documentation, may be NULL */
                                        -1,    /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_obe() {

  if (not c2py::check_python_version("obe")) return NULL;

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
  c2py::wrap_pytype<_c2py_cls_6>.tp_base = &c2py::wrap_pytype<triqs::modest::downfolding_projector>;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_6>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_7>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_8>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_0, "AtomicOrbs");
  _add_type(_c2py_cls_1, "LocalSpace");
  _add_type(_c2py_cls_2, "IbzSymmetryOps");
  _add_type(_c2py_cls_3, "Op");
  _add_type(_c2py_cls_4, "BandDispersion");
  _add_type(_c2py_cls_5, "DownfoldingProjector");
  _add_type(_c2py_cls_6, "DownfoldingProjectorExt");
  _add_type(_c2py_cls_7, "OneBodyElementsOnGrid");
  _add_type(_c2py_cls_8, "OneBodyElementsGw");
#undef _add_type

  return m;
}
#endif
// CLAIR_WRAP_GEN
