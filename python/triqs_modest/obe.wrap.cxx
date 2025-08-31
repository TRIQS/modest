
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

template <> constexpr bool c2py::is_wrapped<triqs::modest::local_space>               = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::band_dispersion>           = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::downfolding_projector>     = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::one_body_elements_on_grid> = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::one_body_elements_tb>      = true;

// ==================== enums =====================

template <>
std::map<triqs::modest::spin_kind_e, str_t> c2py::enum_to_string<triqs::modest::spin_kind_e> = {
   {triqs::modest::spin_kind_e::Polarized, "Polarized"},
   {triqs::modest::spin_kind_e::NonPolarized, "NonPolarized"},
   {triqs::modest::spin_kind_e::NonColinear, "NonColinear"}};

// ==================== module classes =====================

template <> inline constexpr auto c2py::tp_name<triqs::modest::local_space> = "triqs_modest.obe.LocalSpace";
static auto init_0                                                          = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<triqs::modest::local_space, triqs::modest::spin_kind_e, std::vector<triqs::modest::atomic_orbs>,
                                                                                nda::array<std::vector<long>, 2>, nda::array<nda::matrix<triqs::dcomplex>, 2>, nda::array<nda::matrix<triqs::dcomplex>, 1>>(
      "spin_kind", "atomic_shells", "irreps_decomp_per_atom", "rotation_from_dft_to_local_basis", "rotation_from_spherical_to_dft_basis"),
   c2py::c_constructor<triqs::modest::local_space>()};
template <> constexpr initproc c2py::tp_init<triqs::modest::local_space> = c2py::pyfkw_constructor<init_0>;
template <>
const std::string c2py::tp_ctor_doc<triqs::modest::local_space> = init_0.doc(R"DOC()DOC", std::vector<std::string>{}, std::vector<std::string>{});
// atomic_view
static auto const fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::local_space &self, const triqs::gfs::block2_gf<triqs::mesh::imfreq> &G_C) { return self.atomic_view(G_C); },
                 "self", "G_C"),
   c2py::cmethod(
      [](triqs::modest::local_space &self, const nda::array<nda::matrix<triqs::dcomplex>, 2> &matrix_C) { return self.atomic_view(matrix_C); },
      "self", "matrix_C")};

// first_shell_of_its_equiv_cls
static auto const fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::local_space const &self, long idx) { return self.first_shell_of_its_equiv_cls(idx); }, "self", "idx")};

static const auto doc_d_0 = fun_0.doc(
   R"DOC(
[1] Views a block2gf according to the atomic decomposition.

------

[2] Creates an atomic view of block matrices  by extracting specific slices
       based on the atomic decomposition of the current object.

------

Parameters
----------
G_C : {par_0}
   A block2_gf spanning the entire C space [0, sigma](M, M)
matrix_C : {par_1}
   The input block matrices object from which the atomic view is created.

Returns
-------
[1] : {ret_0}
   A block2_gf in the atomic decomposition view [atom, sigma][m_orb, m_orb]

[2] : {ret_1}
   new block matrices in the atomic view.
)DOC",
   std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<const triqs::gfs::block2_gf<triqs::mesh::imfreq> &>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<const nda::array<nda::matrix<triqs::dcomplex>, 2> &>()}, ", ")},
   std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::gfs::block2_gf<triqs::mesh::imfreq>>(),
                                                     c2py::python_typename<nda::array<nda::matrix<triqs::dcomplex>, 2>>()}});
static const auto doc_d_1 = fun_1.doc(R"DOC(
Given the index idx of an atomic shell, return the index of the first atomic shell of its equivalence class
)DOC",
                                      std::vector<std::string>{}, std::vector<std::string>{});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::local_space>[] = {
   {"atomic_view", (PyCFunction)c2py::pyfkw<fun_0>, METH_VARARGS | METH_KEYWORDS, doc_d_0.c_str()},
   {"first_shell_of_its_equiv_cls", (PyCFunction)c2py::pyfkw<fun_1>, METH_VARARGS | METH_KEYWORDS, doc_d_1.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0  = R"DOC(Names of the atoms in the orbital set.)DOC";
static constexpr auto prop_doc_1  = R"DOC(Generates [dimension of the atomic shell].)DOC";
static constexpr auto prop_doc_2  = R"DOC(List of all atomic shells spanning the 𝓒 space)DOC";
static constexpr auto prop_doc_3  = R"DOC(List of all blocks spanning 𝓒 space -> atoms_block_decomposition)DOC";
static constexpr auto prop_doc_4  = R"DOC(Dimension of the correlated space)DOC";
static constexpr auto prop_doc_5  = R"DOC(The number of atoms)DOC";
static constexpr auto prop_doc_6  = R"DOC(Dimension of the σ index)DOC";
static constexpr auto prop_doc_7  = R"DOC(List of all (a, sigma) local rotation matices that rotate the data)DOC";
static constexpr auto prop_doc_8  = R"DOC(List of rotation matrices from spherical harmonics to dft specific orbital basis)DOC";
static constexpr auto prop_doc_9  = R"DOC(names of spin indices for naming blocks in block_gf)DOC";
static constexpr auto prop_doc_10 = R"DOC()DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::local_space>[] = {

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
const std::string c2py::tp_doc<triqs::modest::local_space> = R"DOC(Describe the atomic orbitals within downfolded :math:`\mathcal{C}` space.

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
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<triqs::modest::local_space>;
template <> inline constexpr auto c2py::tp_name<triqs::modest::band_dispersion> = "triqs_modest.obe.BandDispersion";

static int synth_constructor_0(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::band_dispersion.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<triqs::modest::band_dispersion> *)self)->_c = new triqs::modest::band_dispersion{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::band_dispersion from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<triqs::modest::band_dispersion> *)self)->_c);
  de("spin_kind", self_c.spin_kind, false);
  de("H_k", self_c.H_k, false);
  de("n_bands_per_k", self_c.n_bands_per_k, false);
  de("k_weights", self_c.k_weights, false);
  de("matrix_valued", self_c.matrix_valued, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<triqs::modest::band_dispersion> = synth_constructor_0;

template <>
const std::string c2py::tp_ctor_doc<triqs::modest::band_dispersion> = c2py::replace_tags(
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
   std::vector<std::string>{std::vector<std::string>{
      c2py::python_typename<triqs::modest::spin_kind_e>(), c2py::python_typename<nda::array<triqs::dcomplex, 4>>(),
      c2py::python_typename<nda::array<long, 2>>(), c2py::python_typename<nda::array<double, 1>>(), c2py::python_typename<bool>()}});
// H
static auto const fun_2 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::band_dispersion const &self, long sigma, long k_idx) { return self.H(sigma, k_idx); }, "self", "sigma", "k_idx")};

// N_nu
static auto const fun_3 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](triqs::modest::band_dispersion const &self, long sigma, long k_idx) { return self.N_nu(sigma, k_idx); }, "self", "sigma", "k_idx")};

static const auto doc_d_2 = fun_2.doc(R"DOC(
H^σ(k)_ν, returned as a MATRIX in (ν, ν)
)DOC",
                                      std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_3 = fun_3.doc(R"DOC(
Number of bands #ν
)DOC",
                                      std::vector<std::string>{}, std::vector<std::string>{});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::band_dispersion>[] = {
   {"H", (PyCFunction)c2py::pyfkw<fun_2>, METH_VARARGS | METH_KEYWORDS, doc_d_2.c_str()},
   {"N_nu", (PyCFunction)c2py::pyfkw<fun_3>, METH_VARARGS | METH_KEYWORDS, doc_d_3.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_0 = R"DOC(Spin kind of the one-body data)DOC";
constexpr auto doc_member_1 = R"DOC(H_k [k_idx, σ', nu, nu'])DOC";
constexpr auto doc_member_2 = R"DOC(n_bands_per_k [k_idx, σ'] = # of nu)DOC";
constexpr auto doc_member_3 = R"DOC(k_weights[k_idx])DOC";
constexpr auto doc_member_4 = R"DOC(Is the dispersion matrix-valued?)DOC";
static PyObject *prop_get_dict_0(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<triqs::modest::band_dispersion> *)self)->_c);
  c2py::pydict dic;
  dic["spin_kind"]     = self_c.spin_kind;
  dic["H_k"]           = self_c.H_k;
  dic["n_bands_per_k"] = self_c.n_bands_per_k;
  dic["k_weights"]     = self_c.k_weights;
  dic["matrix_valued"] = self_c.matrix_valued;
  return dic.new_ref();
}
static constexpr auto prop_doc_11 = R"DOC(Number of k points in the grid)DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::band_dispersion>[] = {
   c2py::getsetdef_from_member<&triqs::modest::band_dispersion::spin_kind, triqs::modest::band_dispersion>("spin_kind", doc_member_0),
   c2py::getsetdef_from_member<&triqs::modest::band_dispersion::H_k, triqs::modest::band_dispersion>("H_k", doc_member_1),
   c2py::getsetdef_from_member<&triqs::modest::band_dispersion::n_bands_per_k, triqs::modest::band_dispersion>("n_bands_per_k", doc_member_2),
   c2py::getsetdef_from_member<&triqs::modest::band_dispersion::k_weights, triqs::modest::band_dispersion>("k_weights", doc_member_3),
   c2py::getsetdef_from_member<&triqs::modest::band_dispersion::matrix_valued, triqs::modest::band_dispersion>("matrix_valued", doc_member_4),
   {"n_k", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::band_dispersion::n_k)>, nullptr, prop_doc_11, nullptr},
   {"__dict__", (getter)prop_get_dict_0, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<triqs::modest::band_dispersion> = R"DOC(The one-body dispersion as a function of momentum.

The band dispersion typically corresponds to the solution of a (Kohn-Sham) Hamiltonian which has been diagonalized in
momentum space and formulated in a basis of Bloch states :math:`| \phi_{\nu\mathbf{k}} \rangle` with corresponding eigenvalues
(:math:`\varepsilon_{\nu\mathbf{k}}^{\sigma}`).

A band dispersion object contains the DFT band structure :math:`\varepsilon_{\nu\mathbf{k}}^{\sigma}`, weights in the Brillouin zone,
and the spin kind used in the DFT calculation.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<triqs::modest::band_dispersion>;
template <> inline constexpr auto c2py::tp_name<triqs::modest::downfolding_projector> = "triqs_modest.obe.DownfoldingProjector";

static int synth_constructor_1(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::downfolding_projector.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<triqs::modest::downfolding_projector> *)self)->_c = new triqs::modest::downfolding_projector{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::downfolding_projector from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<triqs::modest::downfolding_projector> *)self)->_c);
  de("spin_kind", self_c.spin_kind, false);
  de("P_k", self_c.P_k, false);
  de("n_bands_per_k", self_c.n_bands_per_k, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<triqs::modest::downfolding_projector> = synth_constructor_1;

template <>
const std::string c2py::tp_ctor_doc<triqs::modest::downfolding_projector> =
   c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
spin_kind : {par_0}

P_k : {par_1}

n_bands_per_k : {par_2}

)DOC",
                      "par",
                      std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::modest::spin_kind_e>(),
                                                                        c2py::python_typename<nda::array<triqs::dcomplex, 4>>(),
                                                                        c2py::python_typename<nda::array<long, 2>>()}});
// P
static auto const fun_4 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](triqs::modest::downfolding_projector const &self, long sigma, long k_idx) { return self.P(sigma, k_idx); }, "self", "sigma", "k_idx")};

// rotate_local_basis
static auto const fun_5 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](triqs::modest::downfolding_projector const &self, const nda::array<nda::matrix<triqs::dcomplex>, 2> &U) { return self.rotate_local_basis(U); },
   "self", "U")};

static const auto doc_d_4 = fun_4.doc(R"DOC(
P^σ(k)_mν, returned as a matrix in (m ν)
)DOC",
                                      std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_5 = fun_5.doc(R"DOC(
Rotates the local basis of the downfolding projector
)DOC",
                                      std::vector<std::string>{}, std::vector<std::string>{});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::downfolding_projector>[] = {
   {"P", (PyCFunction)c2py::pyfkw<fun_4>, METH_VARARGS | METH_KEYWORDS, doc_d_4.c_str()},
   {"rotate_local_basis", (PyCFunction)c2py::pyfkw<fun_5>, METH_VARARGS | METH_KEYWORDS, doc_d_5.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_5 = R"DOC()DOC";
constexpr auto doc_member_6 = R"DOC(Pk[alpha][k_idx, σ', m_alpha, nu])DOC";
constexpr auto doc_member_7 = R"DOC(n_bands_per_k [k_idx, σ'] = # of nu)DOC";
static PyObject *prop_get_dict_1(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<triqs::modest::downfolding_projector> *)self)->_c);
  c2py::pydict dic;
  dic["spin_kind"]     = self_c.spin_kind;
  dic["P_k"]           = self_c.P_k;
  dic["n_bands_per_k"] = self_c.n_bands_per_k;
  return dic.new_ref();
}

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::downfolding_projector>[] = {
   c2py::getsetdef_from_member<&triqs::modest::downfolding_projector::spin_kind, triqs::modest::downfolding_projector>("spin_kind", doc_member_5),
   c2py::getsetdef_from_member<&triqs::modest::downfolding_projector::P_k, triqs::modest::downfolding_projector>("P_k", doc_member_6),
   c2py::getsetdef_from_member<&triqs::modest::downfolding_projector::n_bands_per_k, triqs::modest::downfolding_projector>("n_bands_per_k",
                                                                                                                           doc_member_7),
   {"__dict__", (getter)prop_get_dict_1, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<triqs::modest::downfolding_projector> =
   R"DOC(The projector that downfolds the energy bands onto a set of localized atomic-like orbitals.

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
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<triqs::modest::downfolding_projector>;
template <> inline constexpr auto c2py::tp_name<triqs::modest::one_body_elements_on_grid> = "triqs_modest.obe.OneBodyElementsOnGrid";

static int synth_constructor_2(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::one_body_elements_on_grid.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<triqs::modest::one_body_elements_on_grid> *)self)->_c = new triqs::modest::one_body_elements_on_grid{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::one_body_elements_on_grid from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<triqs::modest::one_body_elements_on_grid> *)self)->_c);
  de("H", self_c.H, false);
  de("C_space", self_c.C_space, false);
  de("P", self_c.P, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<triqs::modest::one_body_elements_on_grid> = synth_constructor_2;

template <>
const std::string c2py::tp_ctor_doc<triqs::modest::one_body_elements_on_grid> =
   c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
H : {par_0}

C_space : {par_1}

P : {par_2}

)DOC",
                      "par",
                      std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::modest::band_dispersion>(),
                                                                        c2py::python_typename<triqs::modest::local_space>(),
                                                                        c2py::python_typename<triqs::modest::downfolding_projector>()}});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::one_body_elements_on_grid>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_8  = R"DOC()DOC";
constexpr auto doc_member_9  = R"DOC()DOC";
constexpr auto doc_member_10 = R"DOC()DOC";
static PyObject *prop_get_dict_2(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<triqs::modest::one_body_elements_on_grid> *)self)->_c);
  c2py::pydict dic;
  dic["H"]       = self_c.H;
  dic["C_space"] = self_c.C_space;
  dic["P"]       = self_c.P;
  return dic.new_ref();
}

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::one_body_elements_on_grid>[] = {
   c2py::getsetdef_from_member<&triqs::modest::one_body_elements_on_grid::H, triqs::modest::one_body_elements_on_grid>("H", doc_member_8),
   c2py::getsetdef_from_member<&triqs::modest::one_body_elements_on_grid::C_space, triqs::modest::one_body_elements_on_grid>("C_space", doc_member_9),
   c2py::getsetdef_from_member<&triqs::modest::one_body_elements_on_grid::P, triqs::modest::one_body_elements_on_grid>("P", doc_member_10),
   {"__dict__", (getter)prop_get_dict_2, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<triqs::modest::one_body_elements_on_grid> =
   R"DOC(A one-body elements struct where all of the underlying data exists on a fixed momentum grid.)DOC" + std::string{"\n\n----------\n\n"}
   + c2py::tp_ctor_doc<triqs::modest::one_body_elements_on_grid>;
template <> inline constexpr auto c2py::tp_name<triqs::modest::one_body_elements_tb> = "triqs_modest.obe.OneBodyElementsTb";

static int synth_constructor_3(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::one_body_elements_tb.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<triqs::modest::one_body_elements_tb> *)self)->_c = new triqs::modest::one_body_elements_tb{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::one_body_elements_tb from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<triqs::modest::one_body_elements_tb> *)self)->_c);
  de("C_space", self_c.C_space, false);
  de("H", self_c.H, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<triqs::modest::one_body_elements_tb> = synth_constructor_3;

template <>
const std::string c2py::tp_ctor_doc<triqs::modest::one_body_elements_tb> =
   c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
C_space : {par_0}

H : {par_1}

)DOC",
                      "par",
                      std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::modest::local_space>(),
                                                                        c2py::python_typename<std::vector<triqs::tb_hamiltonian>>()}});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::one_body_elements_tb>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_11 = R"DOC()DOC";
constexpr auto doc_member_12 = R"DOC()DOC";
static PyObject *prop_get_dict_3(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<triqs::modest::one_body_elements_tb> *)self)->_c);
  c2py::pydict dic;
  dic["C_space"] = self_c.C_space;
  dic["H"]       = self_c.H;
  return dic.new_ref();
}

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::one_body_elements_tb>[] = {
   c2py::getsetdef_from_member<&triqs::modest::one_body_elements_tb::C_space, triqs::modest::one_body_elements_tb>("C_space", doc_member_11),
   c2py::getsetdef_from_member<&triqs::modest::one_body_elements_tb::H, triqs::modest::one_body_elements_tb>("H", doc_member_12),
   {"__dict__", (getter)prop_get_dict_3, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<triqs::modest::one_body_elements_tb> = R"DOC(A one-body elements object using a tight-binding Hamiltonian.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<triqs::modest::one_body_elements_tb>;

// ==================== module functions ====================

// one_body_elements_from_dft_converter
static auto const fun_6 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const std::string &filename, double threshold,
                 bool diagonalize_hloc) { return triqs::modest::one_body_elements_from_dft_converter(filename, threshold, diagonalize_hloc); },
              "filename", "threshold"_a = 1.e-5, "diagonalize_hloc"_a = false)};

// one_body_elements_from_wannier90
static auto const fun_7 = c2py::dispatcher_f_kw_t{
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

// one_body_elements_on_high_symmetry_path
static auto const fun_8 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const std::string &filename, const triqs::modest::one_body_elements_on_grid &obe) {
     return triqs::modest::one_body_elements_on_high_symmetry_path(filename, obe);
   },
   "filename", "obe")};

// one_body_elements_with_theta_projectors
static auto const fun_9 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const std::string &filename, const triqs::modest::one_body_elements_on_grid &obe) {
     return triqs::modest::one_body_elements_with_theta_projectors(filename, obe);
   },
   "filename", "obe")};

static const auto doc_d_6 = fun_6.doc(
   R"DOC(
Create a one-body elements object with orthonormalized projectors.

Using the data from the "dft_input" group, the band dispersion, local space, downfolding projector, and optional 
IBZ symmetry ops are prepared to create a one-body elements object. This object is intended to be used in DMFT 
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
   std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<const std::string &>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<double>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<bool>()}, ", ")},
   std::vector<std::string>{std::vector<std::string>{c2py::python_typename<std::pair<double, triqs::modest::one_body_elements_on_grid>>()}});
static const auto doc_d_7 =
   fun_7.doc(R"DOC(
[1] Construct a obe_tb from Wannier90 in the case of a single spin index.

------

[2] Construct a obe_tb from Wannier90 in the case with separate spin up/spin down channels.

------

Parameters
----------
wannier_file_path : {par_0}
   string to Wannier90 files, including the prefix, 
         as in "path/to/file/seedname" to specify a Wannier files named in the format "seedname_tb.dat"
spin_kind : {par_1}
   spin type for this calculation
atomic_shells : {par_2}
   list of atomic shells input by the user
wannier_file_path_up : {par_3}
   string to Wannier90 files, including the prefix, for the up spin channel, 
       as in "path/to/file/seedname" to specify a Wannier files named in the format "seedname_tb.dat"
wannier_file_path_dn : {par_4}
   string to Wannier90 files, including the prefix, for the down spin channel  
       as in "path/to/file/seedname" to specify a Wannier files named in the format "seedname_tb.dat"

Returns
-------
{ret_0}
   One_body_elements object containing the Wannier90 tight binding Hamiltonian
)DOC",
             std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<const std::string &>()}, ", "),
                                      c2py::join(std::vector<std::string>{c2py::python_typename<triqs::modest::spin_kind_e>()}, ", "),
                                      c2py::join(std::vector<std::string>{c2py::python_typename<std::vector<triqs::modest::atomic_orbs>>()}, ", "),
                                      c2py::join(std::vector<std::string>{c2py::python_typename<const std::string &>()}, ", "),
                                      c2py::join(std::vector<std::string>{c2py::python_typename<const std::string &>()}, ", ")},
             std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::modest::one_body_elements_tb>()}});
static const auto doc_d_8 = fun_8.doc(
   R"DOC(
Create a one-body elements object along specific k-grid.

Using the data from the "dft_bands_input" group, the band_disperion and downfolding_projector
are prepared to create one-body elements object. This object is intended to be used for post-processing
the momentum-resolved spectral function.

Parameters
----------
filename : {par_0}
   Hdf5 file from the dft_tools converter.
obe : {par_1}
   One-body elements that was ued in the DMFT calculation.

Returns
-------
{ret_0}
   One-body elements along high-symmetry k-path.
)DOC",
   std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<const std::string &>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()}, ", ")},
   std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::modest::one_body_elements_on_grid>()}});
static const auto doc_d_9 = fun_9.doc(
   R"DOC(
Create a one-body elements object with the Θ projectors.

Using the data from the "dft_parproj_input" group, the local_space, downfolding_projectors,
and optional ibz_symmetry_ops are prepared to create a one-body elements object.  This object is
intended to be used for post-processing the atom- and orbitally-resolved k-summed spectral functions.

Parameters
----------
filename : {par_0}
   Hdf5 file from DFTtools converter with dft_parproj_input group.
obe : {par_1}
   One-body elements that was used in the DMFT calculation.

Returns
-------
{ret_0}
   One-body elements using the Θ projectors.
)DOC",
   std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<const std::string &>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<const triqs::modest::one_body_elements_on_grid &>()}, ", ")},
   std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::modest::one_body_elements_on_grid>()}});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"one_body_elements_from_dft_converter", (PyCFunction)c2py::pyfkw<fun_6>, METH_VARARGS | METH_KEYWORDS, doc_d_6.c_str()},
   {"one_body_elements_from_wannier90", (PyCFunction)c2py::pyfkw<fun_7>, METH_VARARGS | METH_KEYWORDS, doc_d_7.c_str()},
   {"one_body_elements_on_high_symmetry_path", (PyCFunction)c2py::pyfkw<fun_8>, METH_VARARGS | METH_KEYWORDS, doc_d_8.c_str()},
   {"one_body_elements_with_theta_projectors", (PyCFunction)c2py::pyfkw<fun_9>, METH_VARARGS | METH_KEYWORDS, doc_d_9.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "obe", /* name of module */
                                        R"RAWDOC(The one-body elements abstraction in ModEST.

One-body elements (obe for short) contains all of the one-body data that is typically converted from a density-functional theory (DFT) 
calculation or Wannier90 calculation. The one-body elements struct contains four components:

- **Local space** defines the local correlated subspace to be solved within DMFT.
- **Band dispersion** contains the band dispersion :math:`\varepsilon(k, \nu, \sigma)`  where :math:`\varepsilon` is the energy at 
  momentum k, band index :math:`\sigma`, and spin :math:`\sigma`.
- **Downfolding projector** contains the projector which downfolds from Bloch space :math:`\mathcal{B}` (indexed by :math:`\nu`) to the 
  correlated space (index by :math:`m`).    

To make it easy to build this object, ModEST provides factory functions that convert raw DFT data (from codes like VASP, Wien2k, or 
Wannier90) into fully prepared OBE objects ready for DMFT.
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
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::local_space>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::band_dispersion>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::downfolding_projector>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::one_body_elements_on_grid>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::one_body_elements_tb>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
  c2py::add_type_object_to_main<triqs::modest::local_space>("LocalSpace", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::band_dispersion>("BandDispersion", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::downfolding_projector>("DownfoldingProjector", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::one_body_elements_on_grid>("OneBodyElementsOnGrid", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::one_body_elements_tb>("OneBodyElementsTb", m, conv_table);

  return m;
}
#endif
// CLAIR_WRAP_GEN
