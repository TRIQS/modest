
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

template <> constexpr bool c2py::is_wrapped<triqs::modest::atomic_shell_t>                                                         = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::local_space>                                                            = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::band_dispersion>                                                        = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::downfolding_projector>                                                  = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::one_body_elements_on_grid>                                              = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::embedding>                                                              = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::embedding::imp_block_t>                                                 = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::dc_solver>                                                              = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::spectral_function_kw>                                                   = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::spectral_function_w>                                                    = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::initial_data>                                                           = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::iteration_data>                                                         = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::detail::union_find>                                                     = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>> = true;

// ==================== enums =====================

template <>
std::map<triqs::modest::spin_kind_e, str_t> c2py::enum_to_string<triqs::modest::spin_kind_e> = {
   {triqs::modest::spin_kind_e::Polarized, "Polarized"},
   {triqs::modest::spin_kind_e::NonPolarized, "NonPolarized"},
   {triqs::modest::spin_kind_e::NonColinear, "NonColinear"}};

// ==================== module classes =====================

template <> inline const std::string c2py::cpp_name<triqs::modest::atomic_shell_t>   = "triqs::modest::atomic_shell_t";
template <> inline constexpr auto c2py::tp_name<triqs::modest::atomic_shell_t>       = "triqs_modest.module.AtomicShellT";
template <> inline constexpr const char *c2py::tp_doc<triqs::modest::atomic_shell_t> = R"DOC(   Info on an atomic shell)DOC";

static auto init_0 = c2py::dispatcher_c_kw_t{c2py::c_constructor<triqs::modest::atomic_shell_t>()};
template <> constexpr initproc c2py::tp_init<triqs::modest::atomic_shell_t> = c2py::pyfkw_constructor<init_0>;

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::atomic_shell_t>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_0 = R"DOC()DOC";
constexpr auto doc_member_1 = R"DOC()DOC";
constexpr auto doc_member_2 = R"DOC()DOC";
constexpr auto doc_member_3 = R"DOC()DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::atomic_shell_t>[] = {
   c2py::getsetdef_from_member<&triqs::modest::atomic_shell_t::dim, triqs::modest::atomic_shell_t>("dim", doc_member_0),
   c2py::getsetdef_from_member<&triqs::modest::atomic_shell_t::l, triqs::modest::atomic_shell_t>("l", doc_member_1),
   c2py::getsetdef_from_member<&triqs::modest::atomic_shell_t::cls_idx, triqs::modest::atomic_shell_t>("cls_idx", doc_member_2),
   c2py::getsetdef_from_member<&triqs::modest::atomic_shell_t::dft_idx, triqs::modest::atomic_shell_t>("dft_idx", doc_member_3),

   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> inline const std::string c2py::cpp_name<triqs::modest::local_space> = "triqs::modest::local_space";
template <> inline constexpr auto c2py::tp_name<triqs::modest::local_space>     = "triqs_modest.module.LocalSpace";
template <>
inline constexpr const char *c2py::tp_doc<triqs::modest::local_space> =
   R"DOC(   It contains: * the spin kind (NonPolarized, Polarized, NonColinear) * the shell structure. * the decomposition of the 𝓒 space into atomic and irreps decomposition. * the transformation matrices to the DFT-basis and to the spherical basis. FIXME : Cf note : EXPLAIN)DOC";

static auto init_1 = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<triqs::modest::local_space, triqs::modest::spin_kind_e, std::vector<triqs::modest::atomic_shell_t>,
                       nda::array<std::vector<long>, 2>, nda::array<nda::matrix<triqs::dcomplex>, 2>, nda::array<nda::matrix<triqs::dcomplex>, 1>>(
      "spin_kind", "atomic_shells", "irreps_decomp_per_atom", "rotation_from_dft_to_local_basis", "rotation_from_spherical_to_dft_basis"),
   c2py::c_constructor<triqs::modest::local_space>()};
template <> constexpr initproc c2py::tp_init<triqs::modest::local_space> = c2py::pyfkw_constructor<init_1>;
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
static constexpr auto doc_f_0_0 = R"DOC(   Parameters
   ----------
   
   G_C:
      The input block2_gf object from which the atomic view is created.
   
   Returns
   -------
   
      A new block2_gf object representing the atomic view.)DOC";
static constexpr auto doc_f_0_1 = R"DOC(   Parameters
   ----------
   
   matrix_C:
      The input block matrices object from which the atomic view is created.
   
   Returns
   -------
   
      new block matrices in the atomic view.)DOC";
static const auto doc_d_0       = fun_0.doc({doc_f_0_0, doc_f_0_1});
static const auto doc_d_1 =
   fun_1.doc({R"DOC(   Given the index idx of an atomic shell, return the index of the first atomic shell of its equivalence class)DOC"});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::local_space>[] = {
   {"atomic_view", (PyCFunction)c2py::pyfkw<fun_0>, METH_VARARGS | METH_KEYWORDS, doc_d_0.c_str()},
   {"first_shell_of_its_equiv_cls", (PyCFunction)c2py::pyfkw<fun_1>, METH_VARARGS | METH_KEYWORDS, doc_d_1.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0  = R"DOC(/// Names of the atoms in the orbital set.)DOC";
static constexpr auto prop_doc_1  = R"DOC(/// Generates [dimension of the atomic shell].)DOC";
static constexpr auto prop_doc_2  = R"DOC(/// List of all atomic shells spanning the 𝓒 space)DOC";
static constexpr auto prop_doc_3  = R"DOC(/// List of all blocks spanning 𝓒 space -> atoms_block_decomposition)DOC";
static constexpr auto prop_doc_4  = R"DOC(/// Dimension of the correlated space)DOC";
static constexpr auto prop_doc_5  = R"DOC(/// The number of atoms)DOC";
static constexpr auto prop_doc_6  = R"DOC(/// Dimension of the σ index)DOC";
static constexpr auto prop_doc_7  = R"DOC(/// List of all (a, sigma) local rotation matices that rotate the data)DOC";
static constexpr auto prop_doc_8  = R"DOC(/// List of rotation matrices from spherical harmonics to dft specific orbital basis)DOC";
static constexpr auto prop_doc_9  = R"DOC(/// names of spin indices for naming blocks in block_gf)DOC";
static constexpr auto prop_doc_10 = R"DOC(/// Accessor for the spin kind)DOC";

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

template <> inline const std::string c2py::cpp_name<triqs::modest::band_dispersion> = "triqs::modest::band_dispersion";
template <> inline constexpr auto c2py::tp_name<triqs::modest::band_dispersion>     = "triqs_modest.module.BandDispersion";
template <>
inline constexpr const char *c2py::tp_doc<triqs::modest::band_dispersion> = R"DOC(   * spin_kind: triqs::modest::spin_kind_e
      
   
   * H_k: nda::array<triqs::dcomplex,4>
      
   
   * n_bands_per_k: nda::array<long,2>
      
   
   * k_weights: nda::array<double,1>)DOC";

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
  return de.check();
}

template <> constexpr initproc c2py::tp_init<triqs::modest::band_dispersion> = synth_constructor_0;

// H
static auto const fun_2 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::band_dispersion const &self, long sigma, long k_idx) { return self.H(sigma, k_idx); }, "self", "sigma", "k_idx")};

// N_nu
static auto const fun_3   = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](triqs::modest::band_dispersion const &self, long sigma, long k_idx) { return self.N_nu(sigma, k_idx); }, "self", "sigma", "k_idx")};
static const auto doc_d_2 = fun_2.doc({R"DOC(   H^σ(k)_ν, returned as a MATRIX in (ν, ν))DOC"});
static const auto doc_d_3 = fun_3.doc({R"DOC(   Number of bands #ν)DOC"});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::band_dispersion>[] = {
   {"H", (PyCFunction)c2py::pyfkw<fun_2>, METH_VARARGS | METH_KEYWORDS, doc_d_2.c_str()},
   {"N_nu", (PyCFunction)c2py::pyfkw<fun_3>, METH_VARARGS | METH_KEYWORDS, doc_d_3.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_4 = R"DOC()DOC";
constexpr auto doc_member_5 = R"DOC()DOC";
constexpr auto doc_member_6 = R"DOC()DOC";
constexpr auto doc_member_7 = R"DOC()DOC";
static PyObject *prop_get_dict_0(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<triqs::modest::band_dispersion> *)self)->_c);
  c2py::pydict dic;
  dic["spin_kind"]     = self_c.spin_kind;
  dic["H_k"]           = self_c.H_k;
  dic["n_bands_per_k"] = self_c.n_bands_per_k;
  dic["k_weights"]     = self_c.k_weights;
  return dic.new_ref();
}
static constexpr auto prop_doc_11 = R"DOC(/// Number of k points in the grid)DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::band_dispersion>[] = {
   c2py::getsetdef_from_member<&triqs::modest::band_dispersion::spin_kind, triqs::modest::band_dispersion>("spin_kind", doc_member_4),
   c2py::getsetdef_from_member<&triqs::modest::band_dispersion::H_k, triqs::modest::band_dispersion>("H_k", doc_member_5),
   c2py::getsetdef_from_member<&triqs::modest::band_dispersion::n_bands_per_k, triqs::modest::band_dispersion>("n_bands_per_k", doc_member_6),
   c2py::getsetdef_from_member<&triqs::modest::band_dispersion::k_weights, triqs::modest::band_dispersion>("k_weights", doc_member_7),
   {"n_k", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::band_dispersion::n_k)>, nullptr, prop_doc_11, nullptr},
   {"__dict__", (getter)prop_get_dict_0, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> inline const std::string c2py::cpp_name<triqs::modest::downfolding_projector> = "triqs::modest::downfolding_projector";
template <> inline constexpr auto c2py::tp_name<triqs::modest::downfolding_projector>     = "triqs_modest.module.DownfoldingProjector";
template <>
inline constexpr const char *c2py::tp_doc<triqs::modest::downfolding_projector> = R"DOC(   * spin_kind: triqs::modest::spin_kind_e
      
   
   * P_k: nda::array<triqs::dcomplex,4>
      
   
   * n_bands_per_k: nda::array<long,2>)DOC";

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

// P
static auto const fun_4 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](triqs::modest::downfolding_projector const &self, long sigma, long k_idx) { return self.P(sigma, k_idx); }, "self", "sigma", "k_idx")};

// rotate_local_basis
static auto const fun_5   = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](triqs::modest::downfolding_projector const &self, const nda::array<nda::matrix<triqs::dcomplex>, 2> &U) { return self.rotate_local_basis(U); },
   "self", "U")};
static const auto doc_d_4 = fun_4.doc({R"DOC(   P^σ(k)_mν, returned as a matrix in (m ν))DOC"});
static const auto doc_d_5 = fun_5.doc({R"DOC(   Rotates the local basis of the downfolding projector)DOC"});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::downfolding_projector>[] = {
   {"P", (PyCFunction)c2py::pyfkw<fun_4>, METH_VARARGS | METH_KEYWORDS, doc_d_4.c_str()},
   {"rotate_local_basis", (PyCFunction)c2py::pyfkw<fun_5>, METH_VARARGS | METH_KEYWORDS, doc_d_5.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_8  = R"DOC()DOC";
constexpr auto doc_member_9  = R"DOC()DOC";
constexpr auto doc_member_10 = R"DOC()DOC";
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
   c2py::getsetdef_from_member<&triqs::modest::downfolding_projector::spin_kind, triqs::modest::downfolding_projector>("spin_kind", doc_member_8),
   c2py::getsetdef_from_member<&triqs::modest::downfolding_projector::P_k, triqs::modest::downfolding_projector>("P_k", doc_member_9),
   c2py::getsetdef_from_member<&triqs::modest::downfolding_projector::n_bands_per_k, triqs::modest::downfolding_projector>("n_bands_per_k",
                                                                                                                           doc_member_10),
   {"__dict__", (getter)prop_get_dict_1, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> inline const std::string c2py::cpp_name<triqs::modest::one_body_elements_on_grid> = "triqs::modest::one_body_elements_on_grid";
template <> inline constexpr auto c2py::tp_name<triqs::modest::one_body_elements_on_grid>     = "triqs_modest.module.OneBodyElementsOnGrid";
template <>
inline constexpr const char *c2py::tp_doc<triqs::modest::one_body_elements_on_grid> = R"DOC(   * H: triqs::modest::band_dispersion
      
   
   * C_space: triqs::modest::local_space
      
   
   * P: triqs::modest::downfolding_projector)DOC";

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

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::one_body_elements_on_grid>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_11 = R"DOC()DOC";
constexpr auto doc_member_12 = R"DOC()DOC";
constexpr auto doc_member_13 = R"DOC()DOC";
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
   c2py::getsetdef_from_member<&triqs::modest::one_body_elements_on_grid::H, triqs::modest::one_body_elements_on_grid>("H", doc_member_11),
   c2py::getsetdef_from_member<&triqs::modest::one_body_elements_on_grid::C_space, triqs::modest::one_body_elements_on_grid>("C_space",
                                                                                                                             doc_member_12),
   c2py::getsetdef_from_member<&triqs::modest::one_body_elements_on_grid::P, triqs::modest::one_body_elements_on_grid>("P", doc_member_13),
   {"__dict__", (getter)prop_get_dict_2, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> inline const std::string c2py::cpp_name<triqs::modest::embedding>   = "triqs::modest::embedding";
template <> inline constexpr auto c2py::tp_name<triqs::modest::embedding>       = "triqs_modest.module.Embedding";
template <> inline constexpr const char *c2py::tp_doc<triqs::modest::embedding> = R"DOC(   )DOC";

static auto init_2                                                     = c2py::dispatcher_c_kw_t{c2py::c_constructor<triqs::modest::embedding>(),
                                             c2py::c_constructor<triqs::modest::embedding, std::vector<long>, std::vector<std::vector<long>>,
                                                                                                                     nda::array<triqs::modest::embedding::imp_block_t, 2>, std::vector<std::string>>(
                                                "sigma_embed_decomposition", "imp_decompositions", "psi", "sigma_names")};
template <> constexpr initproc c2py::tp_init<triqs::modest::embedding> = c2py::pyfkw_constructor<init_2>;
// drop
static auto const fun_6 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](triqs::modest::embedding const &self, long imp_idx) { return self.drop(imp_idx); }, "self", "imp_idx")};

// embed
static auto const fun_7 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](triqs::modest::embedding const &self,
         const std::vector<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>> &Sigma_imp_vec) {
        return self.embed(Sigma_imp_vec);
      },
      "self", "Sigma_imp_vec"),
   c2py::cmethod(
      [](triqs::modest::embedding const &self,
         const std::vector<triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>> &Sigma_imp_vec) {
        return self.embed(Sigma_imp_vec);
      },
      "self", "Sigma_imp_vec"),
   c2py::cmethod(
      [](triqs::modest::embedding const &self,
         const std::vector<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>> &Sigma_imp_vec) {
        return self.embed(Sigma_imp_vec);
      },
      "self", "Sigma_imp_vec"),
   c2py::cmethod(
      [](triqs::modest::embedding const &self, const std::vector<std::vector<nda::matrix<triqs::dcomplex>>> &Sigma_imp_hartree_vec) {
        return self.embed(Sigma_imp_hartree_vec);
      },
      "self", "Sigma_imp_hartree_vec")};

// extract
static auto const fun_8 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self,
                    const triqs::gfs::block2_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &g) { return self.extract(g); },
                 "self", "g"),
   c2py::cmethod([](triqs::modest::embedding const &self,
                    const triqs::gfs::block2_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &g) { return self.extract(g); },
                 "self", "g"),
   c2py::cmethod([](triqs::modest::embedding const &self,
                    const triqs::gfs::block2_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &g) { return self.extract(g); },
                 "self", "g"),
   c2py::cmethod(
      [](triqs::modest::embedding const &self, const nda::array<nda::matrix<triqs::dcomplex>, 2> &matrix_C) { return self.extract(matrix_C); },
      "self", "matrix_C")};

// flip_spin
static auto const fun_9 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self, long alpha) { return self.flip_spin(alpha); }, "self", "alpha"),
   c2py::cmethod([](triqs::modest::embedding const &self, std::vector<long> alphas) { return self.flip_spin(alphas); }, "self", "alphas")};

// imp_decomposition
static auto const fun_10 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self, long imp_idx) { return self.imp_decomposition(imp_idx); }, "self", "imp_idx")};

// n_gamma
static auto const fun_11 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self, long imp_idx) { return self.n_gamma(imp_idx); }, "self", "imp_idx")};

// replace
static auto const fun_12 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](triqs::modest::embedding const &self, long imp_idx_to_remove,
                                            long imp_idx_to_replace_with) { return self.replace(imp_idx_to_remove, imp_idx_to_replace_with); },
                                         "self", "imp_idx_to_remove", "imp_idx_to_replace_with")};

// split
static auto const fun_13 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self, long imp_idx, std::function<bool(long)> p) { return self.split(imp_idx, p); }, "self",
                 "imp_idx", "p"),
   c2py::cmethod(
      [](triqs::modest::embedding const &self, long imp_idx, const std::vector<long> &block_list) { return self.split(imp_idx, block_list); }, "self",
      "imp_idx", "block_list")};
static const auto doc_d_6        = fun_6.doc({R"DOC(   Parameters
   ----------
   
   imp_idx:
      Index of the impurity to remove from the table ψ
   
   Returns
   -------
   
      New embedding with the updated ψ map.)DOC"});
static constexpr auto doc_f_7_0  = R"DOC(   _{embed}^{ασ} =
   _{imp}^{ψ(α,σ)} ψ(α,σ) -> (n_imp, γ, τ)
   
   Parameters
   ----------
   
   Sigma_imp_vec:
      
   
   Returns
   -------
   
      Σ_embed in C space)DOC";
static constexpr auto doc_f_7_1  = R"DOC(   _{embed}^{ασ} =
   _{imp}^{ψ(α,σ)} ψ(α,σ) -> (n_imp, γ, τ)
   
   Parameters
   ----------
   
   Sigma_imp_vec:
      
   
   Returns
   -------
   
      Σ_embed in C space)DOC";
static constexpr auto doc_f_7_2  = R"DOC(   _{embed}^{ασ} =
   _{imp}^{ψ(α,σ)} ψ(α,σ) -> (n_imp, γ, τ)
   
   Parameters
   ----------
   
   Sigma_imp_vec:
      
   
   Returns
   -------
   
      Σ_embed in C space)DOC";
static constexpr auto doc_f_7_3  = R"DOC(   embed block matrices)DOC";
static const auto doc_d_7        = fun_7.doc({doc_f_7_0, doc_f_7_1, doc_f_7_2, doc_f_7_3});
static constexpr auto doc_f_8_0  = R"DOC(   Parameters
   ----------
   
   g_loc:
      Block2Gf of gloc in MxM space
   
   Returns
   -------
   
      local impurity Green's function std::vector<block_gf<Mesh, matrix_valued>>)DOC";
static constexpr auto doc_f_8_1  = R"DOC(   Parameters
   ----------
   
   g_loc:
      Block2Gf of gloc in MxM space
   
   Returns
   -------
   
      local impurity Green's function std::vector<block_gf<Mesh, matrix_valued>>)DOC";
static constexpr auto doc_f_8_2  = R"DOC(   Parameters
   ----------
   
   g_loc:
      Block2Gf of gloc in MxM space
   
   Returns
   -------
   
      local impurity Green's function std::vector<block_gf<Mesh, matrix_valued>>)DOC";
static constexpr auto doc_f_8_3  = R"DOC(   extract matrices)DOC";
static const auto doc_d_8        = fun_8.doc({doc_f_8_0, doc_f_8_1, doc_f_8_2, doc_f_8_3});
static constexpr auto doc_f_9_0  = R"DOC(   Parameters
   ----------
   
   alpha:
      the index of the alpha block to flip the spins.
   
   Returns
   -------
   
      a new embedding with the updated ψ map.)DOC";
static constexpr auto doc_f_9_1  = R"DOC(   Parameters
   ----------
   
   alphas:
      a list of alpha blocks to flip the spins.
   
   Returns
   -------
   
      New embedding with the updated ψ map.)DOC";
static const auto doc_d_9        = fun_9.doc({doc_f_9_0, doc_f_9_1});
static const auto doc_d_10       = fun_10.doc({R"DOC(   The impurity decomposition)DOC"});
static const auto doc_d_11       = fun_11.doc({R"DOC(   Number of blocks in γ for the Σ_imp[imp_idx])DOC"});
static const auto doc_d_12       = fun_12.doc({R"DOC(   Parameters
   ----------
   
   imp_idx_to:
      the index of the impurity solver to be replaced.
   imp_idx_from:
      the index of the impurity solver that will replace imp_idx_to.
   
   Returns
   -------
   
      a new embedding with the updated ψ map.)DOC"});
static constexpr auto doc_f_13_0 = R"DOC(   Predicate p (long block_idx) -> 0 or 1.)DOC";
static constexpr auto doc_f_13_1 = R"DOC(   Parameters
   ----------
   
   imp_idx:
      Impurity number
   block_list:
      A list of blocks of imp_idx to split off.
   
   Returns
   -------
   
      New embedding with the updated ψ map.)DOC";
static const auto doc_d_13       = fun_13.doc({doc_f_13_0, doc_f_13_1});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::embedding>[] = {
   {"drop", (PyCFunction)c2py::pyfkw<fun_6>, METH_VARARGS | METH_KEYWORDS, doc_d_6.c_str()},
   {"embed", (PyCFunction)c2py::pyfkw<fun_7>, METH_VARARGS | METH_KEYWORDS, doc_d_7.c_str()},
   {"extract", (PyCFunction)c2py::pyfkw<fun_8>, METH_VARARGS | METH_KEYWORDS, doc_d_8.c_str()},
   {"flip_spin", (PyCFunction)c2py::pyfkw<fun_9>, METH_VARARGS | METH_KEYWORDS, doc_d_9.c_str()},
   {"imp_decomposition", (PyCFunction)c2py::pyfkw<fun_10>, METH_VARARGS | METH_KEYWORDS, doc_d_10.c_str()},
   {"n_gamma", (PyCFunction)c2py::pyfkw<fun_11>, METH_VARARGS | METH_KEYWORDS, doc_d_11.c_str()},
   {"replace", (PyCFunction)c2py::pyfkw<fun_12>, METH_VARARGS | METH_KEYWORDS, doc_d_12.c_str()},
   {"split", (PyCFunction)c2py::pyfkw<fun_13>, METH_VARARGS | METH_KEYWORDS, doc_d_13.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_12 = R"DOC(/// the mapping table ψ)DOC";
static constexpr auto prop_doc_13 = R"DOC(/// Gf block structure for the impurity solvers)DOC";
static constexpr auto prop_doc_14 = R"DOC(/// Number of blocks in α for the Σ_embed)DOC";
static constexpr auto prop_doc_15 = R"DOC(/// Number of impurities)DOC";
static constexpr auto prop_doc_16 = R"DOC(/// Number of blocks in σ for the Σ_embed)DOC";
static constexpr auto prop_doc_17 = R"DOC(/// The names of the sigma indices)DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::embedding>[] = {

   {"get_psi", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::get_psi)>, nullptr, prop_doc_12, nullptr},
   {"imp_block_shape", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::imp_block_shape)>, nullptr, prop_doc_13, nullptr},
   {"n_alpha", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::n_alpha)>, nullptr, prop_doc_14, nullptr},
   {"n_impurities", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::n_impurities)>, nullptr, prop_doc_15, nullptr},
   {"n_sigma", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::n_sigma)>, nullptr, prop_doc_16, nullptr},
   {"sigma_names", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::sigma_names)>, nullptr, prop_doc_17, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_0(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<triqs::modest::embedding, long, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<triqs::modest::embedding> = {nullptr, getitem_0, nullptr};

template <> inline const std::string c2py::cpp_name<triqs::modest::embedding::imp_block_t>   = "triqs::modest::embedding::imp_block_t";
template <> inline constexpr auto c2py::tp_name<triqs::modest::embedding::imp_block_t>       = "triqs_modest.module.ImpBlockT";
template <> inline constexpr const char *c2py::tp_doc<triqs::modest::embedding::imp_block_t> = R"DOC(   struct of tuples imp_idx, γ, τ)DOC";

static auto init_3 = c2py::dispatcher_c_kw_t{c2py::c_constructor<triqs::modest::embedding::imp_block_t>(),
                                             c2py::c_constructor<triqs::modest::embedding::imp_block_t, long, long, long>("n_imp", "gamma", "tau")};
template <> constexpr initproc c2py::tp_init<triqs::modest::embedding::imp_block_t> = c2py::pyfkw_constructor<init_3>;

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::embedding::imp_block_t>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_14 = R"DOC()DOC";
constexpr auto doc_member_15 = R"DOC()DOC";
constexpr auto doc_member_16 = R"DOC()DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::embedding::imp_block_t>[] = {
   c2py::getsetdef_from_member<&triqs::modest::embedding::imp_block_t::imp_idx, triqs::modest::embedding::imp_block_t>("imp_idx", doc_member_14),
   c2py::getsetdef_from_member<&triqs::modest::embedding::imp_block_t::gamma, triqs::modest::embedding::imp_block_t>("gamma", doc_member_15),
   c2py::getsetdef_from_member<&triqs::modest::embedding::imp_block_t::tau, triqs::modest::embedding::imp_block_t>("tau", doc_member_16),

   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> inline const std::string c2py::cpp_name<triqs::modest::dc_solver>   = "triqs::modest::dc_solver";
template <> inline constexpr auto c2py::tp_name<triqs::modest::dc_solver>       = "triqs_modest.module.DcSolver";
template <> inline constexpr const char *c2py::tp_doc<triqs::modest::dc_solver> = R"DOC(   )DOC";

static auto init_4 = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<triqs::modest::dc_solver, std::vector<std::string>, std::string, double, double>("spin_names", "method", "U_int", "J_hund")};
template <> constexpr initproc c2py::tp_init<triqs::modest::dc_solver> = c2py::pyfkw_constructor<init_4>;
// dc_energy
static auto const fun_14 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::dc_solver &self,
                    const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &gimp) { return self.dc_energy(gimp); },
                 "self", "gimp")};

// dc_self_energy
static auto const fun_15 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::dc_solver &self,
                    const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &gimp) { return self.dc_self_energy(gimp); },
                 "self", "gimp")};
static const auto doc_d_14 = fun_14.doc({R"DOC(   )DOC"});
static const auto doc_d_15 = fun_15.doc({R"DOC(   )DOC"});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::dc_solver>[] = {
   {"dc_energy", (PyCFunction)c2py::pyfkw<fun_14>, METH_VARARGS | METH_KEYWORDS, doc_d_14.c_str()},
   {"dc_self_energy", (PyCFunction)c2py::pyfkw<fun_15>, METH_VARARGS | METH_KEYWORDS, doc_d_15.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_17 = R"DOC()DOC";
constexpr auto doc_member_18 = R"DOC()DOC";
constexpr auto doc_member_19 = R"DOC()DOC";
constexpr auto doc_member_20 = R"DOC()DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::dc_solver>[] = {
   c2py::getsetdef_from_member<&triqs::modest::dc_solver::spin_names, triqs::modest::dc_solver>("spin_names", doc_member_17),
   c2py::getsetdef_from_member<&triqs::modest::dc_solver::method, triqs::modest::dc_solver>("method", doc_member_18),
   c2py::getsetdef_from_member<&triqs::modest::dc_solver::U_int, triqs::modest::dc_solver>("U_int", doc_member_19),
   c2py::getsetdef_from_member<&triqs::modest::dc_solver::J_hund, triqs::modest::dc_solver>("J_hund", doc_member_20),

   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> inline const std::string c2py::cpp_name<triqs::modest::spectral_function_kw>   = "triqs::modest::spectral_function_kw";
template <> inline constexpr auto c2py::tp_name<triqs::modest::spectral_function_kw>       = "triqs_modest.module.SpectralFunctionKw";
template <> inline constexpr const char *c2py::tp_doc<triqs::modest::spectral_function_kw> = R"DOC(   * data: nda::array<double,3>)DOC";

static int synth_constructor_3(PyObject *self, PyObject *args, PyObject *kwargs) {
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
  return de.check();
}

template <> constexpr initproc c2py::tp_init<triqs::modest::spectral_function_kw> = synth_constructor_3;

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::spectral_function_kw>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_21 = R"DOC()DOC";
static PyObject *prop_get_dict_3(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<triqs::modest::spectral_function_kw> *)self)->_c);
  c2py::pydict dic;
  dic["data"] = self_c.data;
  return dic.new_ref();
}

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::spectral_function_kw>[] = {
   c2py::getsetdef_from_member<&triqs::modest::spectral_function_kw::data, triqs::modest::spectral_function_kw>("data", doc_member_21),
   {"__dict__", (getter)prop_get_dict_3, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> inline const std::string c2py::cpp_name<triqs::modest::spectral_function_w> = "triqs::modest::spectral_function_w";
template <> inline constexpr auto c2py::tp_name<triqs::modest::spectral_function_w>     = "triqs_modest.module.SpectralFunctionW";
template <>
inline constexpr const char *c2py::tp_doc<triqs::modest::spectral_function_w> = R"DOC(   * total: nda::array<double,2>
      
   
   * per_theta: nda::array<double,4>)DOC";

static int synth_constructor_4(PyObject *self, PyObject *args, PyObject *kwargs) {
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

template <> constexpr initproc c2py::tp_init<triqs::modest::spectral_function_w> = synth_constructor_4;

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::spectral_function_w>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_22 = R"DOC()DOC";
constexpr auto doc_member_23 = R"DOC()DOC";
static PyObject *prop_get_dict_4(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<triqs::modest::spectral_function_w> *)self)->_c);
  c2py::pydict dic;
  dic["total"]     = self_c.total;
  dic["per_theta"] = self_c.per_theta;
  return dic.new_ref();
}

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::spectral_function_w>[] = {
   c2py::getsetdef_from_member<&triqs::modest::spectral_function_w::total, triqs::modest::spectral_function_w>("total", doc_member_22),
   c2py::getsetdef_from_member<&triqs::modest::spectral_function_w::per_theta, triqs::modest::spectral_function_w>("per_theta", doc_member_23),
   {"__dict__", (getter)prop_get_dict_4, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> inline const std::string c2py::cpp_name<triqs::modest::initial_data> = "triqs::modest::initial_data";
template <> inline constexpr auto c2py::tp_name<triqs::modest::initial_data>     = "triqs_modest.module.InitialData";
template <>
inline constexpr const char *c2py::tp_doc<triqs::modest::initial_data> = R"DOC(   * obe: triqs::modest::one_body_elements_on_grid
      One body elements
   
   * embed: triqs::modest::embedding
      Embedding)DOC";

static int synth_constructor_5(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::initial_data.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<triqs::modest::initial_data> *)self)->_c = new triqs::modest::initial_data{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::initial_data from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<triqs::modest::initial_data> *)self)->_c);
  de("obe", self_c.obe, false);
  de("embed", self_c.embed, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<triqs::modest::initial_data> = synth_constructor_5;

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::initial_data>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_24 = R"DOC(/// One body elements)DOC";
constexpr auto doc_member_25 = R"DOC(/// Embedding)DOC";
static PyObject *prop_get_dict_5(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<triqs::modest::initial_data> *)self)->_c);
  c2py::pydict dic;
  dic["obe"]   = self_c.obe;
  dic["embed"] = self_c.embed;
  return dic.new_ref();
}

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::initial_data>[] = {
   c2py::getsetdef_from_member<&triqs::modest::initial_data::obe, triqs::modest::initial_data>("obe", doc_member_24),
   c2py::getsetdef_from_member<&triqs::modest::initial_data::embed, triqs::modest::initial_data>("embed", doc_member_25),
   {"__dict__", (getter)prop_get_dict_5, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> inline const std::string c2py::cpp_name<triqs::modest::iteration_data> = "triqs::modest::iteration_data";
template <> inline constexpr auto c2py::tp_name<triqs::modest::iteration_data>     = "triqs_modest.module.IterationData";
template <>
inline constexpr const char *c2py::tp_doc<triqs::modest::iteration_data> = R"DOC(   * mu: double
      Chemical potential
   
   * Sigma_imp_list: std::vector<triqs::modest::self_energy_t>
      Impurities self-energies
   
   * Sigma_dc_list: std::vector<triqs::modest::self_energy_t>
      Impurities self-energies)DOC";

static int synth_constructor_6(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::modest::iteration_data.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<triqs::modest::iteration_data> *)self)->_c = new triqs::modest::iteration_data{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::modest::iteration_data from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<triqs::modest::iteration_data> *)self)->_c);
  de("mu", self_c.mu, false);
  de("Sigma_imp_list", self_c.Sigma_imp_list, false);
  de("Sigma_dc_list", self_c.Sigma_dc_list, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<triqs::modest::iteration_data> = synth_constructor_6;

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::iteration_data>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_26 = R"DOC(/// Chemical potential)DOC";
constexpr auto doc_member_27 = R"DOC(/// Impurities self-energies)DOC";
constexpr auto doc_member_28 = R"DOC(/// Impurities self-energies)DOC";
static PyObject *prop_get_dict_6(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<triqs::modest::iteration_data> *)self)->_c);
  c2py::pydict dic;
  dic["mu"]             = self_c.mu;
  dic["Sigma_imp_list"] = self_c.Sigma_imp_list;
  dic["Sigma_dc_list"]  = self_c.Sigma_dc_list;
  return dic.new_ref();
}

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::iteration_data>[] = {
   c2py::getsetdef_from_member<&triqs::modest::iteration_data::mu, triqs::modest::iteration_data>("mu", doc_member_26),
   c2py::getsetdef_from_member<&triqs::modest::iteration_data::Sigma_imp_list, triqs::modest::iteration_data>("Sigma_imp_list", doc_member_27),
   c2py::getsetdef_from_member<&triqs::modest::iteration_data::Sigma_dc_list, triqs::modest::iteration_data>("Sigma_dc_list", doc_member_28),
   {"__dict__", (getter)prop_get_dict_6, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> inline const std::string c2py::cpp_name<triqs::modest::detail::union_find> = "triqs::modest::detail::union_find";
template <> inline constexpr auto c2py::tp_name<triqs::modest::detail::union_find>     = "triqs_modest.module.UnionFind";
template <>
inline constexpr const char *c2py::tp_doc<triqs::modest::detail::union_find> =
   R"DOC(   Union-Find: Data structure that efficiently tracks a collection of disjoint sets.)DOC";

static auto init_5 = c2py::dispatcher_c_kw_t{c2py::c_constructor<triqs::modest::detail::union_find, long>("n")};
template <> constexpr initproc c2py::tp_init<triqs::modest::detail::union_find> = c2py::pyfkw_constructor<init_5>;
// find
static auto const fun_16 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](triqs::modest::detail::union_find &self, long x) { return self.find(x); }, "self", "x")};

// unite
static auto const fun_17 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](triqs::modest::detail::union_find &self, long x, long y) { return self.unite(x, y); }, "self", "x", "y")};
static const auto doc_d_16 = fun_16.doc({R"DOC(   returns the representative of the set containing x)DOC"});
static const auto doc_d_17 = fun_17.doc({R"DOC(   merges the set containing x and y)DOC"});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::detail::union_find>[] = {
   {"find", (PyCFunction)c2py::pyfkw<fun_16>, METH_VARARGS | METH_KEYWORDS, doc_d_16.c_str()},
   {"unite", (PyCFunction)c2py::pyfkw<fun_17>, METH_VARARGS | METH_KEYWORDS, doc_d_17.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_29 = R"DOC()DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::detail::union_find>[] = {
   c2py::getsetdef_from_member<&triqs::modest::detail::union_find::parent, triqs::modest::detail::union_find>("parent", doc_member_29),

   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
inline const std::string c2py::cpp_name<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>> =
   "triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>";
template <>
inline constexpr auto c2py::tp_name<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>> =
   "triqs_modest.module.DMFTCheckpoint";
template <>
inline constexpr const char *c2py::tp_doc<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>> =
   R"DOC(   name = DMFTCheckpoint)DOC";

static auto init_6 = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>, std::string>("dirname"),
   c2py::c_constructor<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>, std::string,
                       const triqs::modest::initial_data &>("dirname", "initial_data")};
template <>
constexpr initproc c2py::tp_init<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>> =
   c2py::pyfkw_constructor<init_6>;
// append
static auto const fun_18 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data> &self,
                                            const triqs::modest::iteration_data &x) { return self.append(x); },
                                         "self", "x")};
static const auto doc_d_18 = fun_18.doc({R"DOC(   )DOC"});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>>[] = {
   {"append", (PyCFunction)c2py::pyfkw<fun_18>, METH_VARARGS | METH_KEYWORDS, doc_d_18.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_18 = R"DOC()DOC";
static constexpr auto prop_doc_19 = R"DOC()DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>>[] = {

   {"dirname",
    c2py::getter_from_method<c2py::castmc<>(&triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>::dirname)>,
    nullptr, prop_doc_18, nullptr},
   {"initial_data",
    c2py::getter_from_method<c2py::castmc<>(&triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>::initial_data)>,
    nullptr, prop_doc_19, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_1(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {
     c2py::cfun2(c2py::getitem<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>, long>)};
  return disp(self, key);
}

template <>
PyMappingMethods c2py::tp_as_mapping<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>> = {
   c2py::tpxx_size<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>>, getitem_1, nullptr};

// ==================== module functions ====================

// G0_C_k_sigma
static auto const fun_19 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::modest::one_body_elements_on_grid &obe, double mu, long k_idx, long sigma, const std::vector<triqs::dcomplex> &omegas,
                 bool mu_derivative) { return triqs::modest::detail::G0_C_k_sigma(obe, mu, k_idx, sigma, omegas, mu_derivative); },
              "obe", "mu", "k_idx", "sigma", "omegas", "mu_derivative"_a = false)};

// U_matrix_in_spherical_basis
static auto const fun_20 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](long l, double U_int, double J_hund) { return triqs::U_matrix_in_spherical_basis(l, U_int, J_hund); }, "l", "U_int", "J_hund")};

// analyze_degenerate_blocks
static auto const fun_21 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Gimp,
                                         double threshold) { return triqs::modest::analyze_degenerate_blocks(Gimp, threshold); },
                                      "Gimp", "threshold"_a = 1.e-5)};

// dc_formulas
static auto const fun_22 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const std::string method, double N_tot, double N_sigma, long n_orb, double U,
                                         double J) { return triqs::modest::dc_formulas(method, N_tot, N_sigma, n_orb, U, J); },
                                      "method", "N_tot", "N_sigma", "n_orb", "U", "J")};

// density
static auto const fun_23 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::modest::one_body_elements_on_grid &obe, double mu,
      const triqs::gfs::block2_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &Sigma_dynamic,
      const nda::array<nda::matrix<triqs::dcomplex>, 2> &Sigma_hartree) { return triqs::modest::density(obe, mu, Sigma_dynamic, Sigma_hartree); },
   "obe", "mu", "Sigma_dynamic", "Sigma_hartree")};

// density_nk
static auto const fun_24 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::modest::one_body_elements_on_grid &obe, double mu, double beta) { return triqs::modest::density_nk(obe, mu, beta); },
              "obe", "mu", "beta")};

// density_slow
static auto const fun_25 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::modest::one_body_elements_on_grid &obe, double mu,
      const triqs::gfs::block2_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Sigma_dynamic,
      const nda::array<nda::matrix<triqs::dcomplex>, 2> &Sigma_hartree) {
     return triqs::modest::density_slow(obe, mu, Sigma_dynamic, Sigma_hartree);
   },
   "obe", "mu", "Sigma_dynamic", "Sigma_hartree")};

// double_counting
static auto const fun_26 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const nda::array<nda::matrix<triqs::dcomplex>, 2> &density_matrix, double U_int, double J_hund,
                                         const std::string method) { return triqs::modest::double_counting(density_matrix, U_int, J_hund, method); },
                                      "density_matrix", "U_int", "J_hund", "method")};

// extract_delta
static auto const fun_27 =
   c2py::dispatcher_f_kw_t{c2py::cfun(
                              [](const std::vector<nda::matrix<triqs::dcomplex>> &epsilon_levels,
                                 const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Gloc,
                                 const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Sigma_dynamic,
                                 const std::vector<nda::matrix<triqs::dcomplex>> &Sigma_hartree) {
                                return triqs::modest::extract_delta(epsilon_levels, Gloc, Sigma_dynamic, Sigma_hartree);
                              },
                              "epsilon_levels", "Gloc", "Sigma_dynamic", "Sigma_hartree"),
                           c2py::cfun(
                              [](const std::vector<nda::matrix<triqs::dcomplex>> &epsilon_levels,
                                 const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &Gloc,
                                 const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &Sigma_dynamic,
                                 const std::vector<nda::matrix<triqs::dcomplex>> &Sigma_hartree) {
                                return triqs::modest::extract_delta(epsilon_levels, Gloc, Sigma_dynamic, Sigma_hartree);
                              },
                              "epsilon_levels", "Gloc", "Sigma_dynamic", "Sigma_hartree"),
                           c2py::cfun(
                              [](const std::vector<nda::matrix<triqs::dcomplex>> &epsilon_levels,
                                 const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Gloc) {
                                return triqs::modest::extract_delta(epsilon_levels, Gloc);
                              },
                              "epsilon_levels", "Gloc"),
                           c2py::cfun(
                              [](const std::vector<nda::matrix<triqs::dcomplex>> &epsilon_levels,
                                 const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &Gloc) {
                                return triqs::modest::extract_delta(epsilon_levels, Gloc);
                              },
                              "epsilon_levels", "Gloc")};

// find_chemical_potential
static auto const fun_28 =
   c2py::dispatcher_f_kw_t{
      c2py::cfun(
         [](double target_density, const triqs::modest::one_body_elements_on_grid &obe, double beta, std::string method, double x_init,
            double precision, double delta_x, long max_loops, std::string x_name, std::string y_name, bool verbosity) {
           return triqs::modest::find_chemical_potential(target_density, obe, beta, method, x_init, precision, delta_x, max_loops, x_name, y_name,
                                                         verbosity);
         },
         "target_density", "obe", "beta", "method"_a = "dichotomy", "x_init"_a = 0.0, "precision"_a = 1.e-5, "delta_x"_a = 0.5, "max_loops"_a = 1000,
         "x_name"_a = "Chemical Potential", "y_name"_a = "Total Density", "verbosity"_a = true),
      c2py::cfun(
         [](double target_density, const triqs::modest::one_body_elements_on_grid &obe,
            const triqs::gfs::block2_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Sigma_dynamic,
            const nda::array<nda::matrix<triqs::dcomplex>, 2> &Sigma_hartree, std::string method, double x_init, double precision, double delta_x,
            long max_loops, std::string x_name, std::string y_name, bool verbosity) {
           return triqs::modest::find_chemical_potential(target_density, obe, Sigma_dynamic, Sigma_hartree, method, x_init, precision, delta_x,
                                                         max_loops, x_name, y_name, verbosity);
         },
         "target_density", "obe", "Sigma_dynamic", "Sigma_hartree", "method"_a = "dichotomy", "x_init"_a = 0.0, "precision"_a = 1.e-5,
         "delta_x"_a = 0.5, "max_loops"_a = 1000, "x_name"_a = "Chemical Potential", "y_name"_a = "Total Density", "verbosity"_a = true),
      c2py::cfun(
         [](double target_density, const triqs::modest::one_body_elements_on_grid &obe,
            const triqs::gfs::block2_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &Sigma_dynamic,
            const nda::array<nda::matrix<triqs::dcomplex>, 2> &Sigma_hartree, std::string method, double x_init, double precision, double delta_x,
            long max_loops, std::string x_name, std::string y_name, bool verbosity) {
           return triqs::modest::find_chemical_potential(target_density, obe, Sigma_dynamic, Sigma_hartree, method, x_init, precision, delta_x,
                                                         max_loops, x_name, y_name, verbosity);
         },
         "target_density", "obe", "Sigma_dynamic", "Sigma_hartree", "method"_a = "dichotomy", "x_init"_a = 0.0, "precision"_a = 1.e-5,
         "delta_x"_a = 0.5, "max_loops"_a = 1000, "x_name"_a = "Chemical Potential", "y_name"_a = "Total Density", "verbosity"_a = true)};

// gloc
static auto const fun_29 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](const triqs::modest::one_body_elements_on_grid &obe, double mu,
         const triqs::gfs::block2_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Sigma_dynamic,
         const nda::array<nda::matrix<triqs::dcomplex>, 2> &Sigma_hartree) { return triqs::modest::gloc(obe, mu, Sigma_dynamic, Sigma_hartree); },
      "obe", "mu", "Sigma_dynamic", "Sigma_hartree"),
   c2py::cfun(
      [](const triqs::modest::one_body_elements_on_grid &obe, double mu,
         const triqs::gfs::block2_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &Sigma_dynamic,
         const nda::array<nda::matrix<triqs::dcomplex>, 2> &Sigma_hartree) { return triqs::modest::gloc(obe, mu, Sigma_dynamic, Sigma_hartree); },
      "obe", "mu", "Sigma_dynamic", "Sigma_hartree"),
   c2py::cfun([](const triqs::mesh::imfreq &mesh, const triqs::modest::one_body_elements_on_grid &obe,
                 double mu) { return triqs::modest::gloc(mesh, obe, mu); },
              "mesh", "obe", "mu"),
   c2py::cfun([](const triqs::mesh::dlr_imfreq &mesh, const triqs::modest::one_body_elements_on_grid &obe,
                 double mu) { return triqs::modest::gloc(mesh, obe, mu); },
              "mesh", "obe", "mu")};

// h5_read
static auto const fun_30 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](h5::group g, const std::string &name, triqs::modest::one_body_elements_on_grid &x) { return triqs::modest::h5_read(g, name, x); },
              "g", "name", "x"),
   c2py::cfun([](h5::group g, const std::string &name, triqs::modest::initial_data &data) { return triqs::modest::h5_read(g, name, data); }, "g",
              "name", "data"),
   c2py::cfun([](h5::group g, const std::string &name, triqs::modest::iteration_data &data) { return triqs::modest::h5_read(g, name, data); }, "g",
              "name", "data")};

// h5_write
static auto const fun_31 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](h5::group g, const std::string &name, const triqs::modest::one_body_elements_on_grid &x) { return triqs::modest::h5_write(g, name, x); },
      "g", "name", "x"),
   c2py::cfun([](h5::group g, const std::string &name, const triqs::modest::initial_data &data) { return triqs::modest::h5_write(g, name, data); },
              "g", "name", "data"),
   c2py::cfun([](h5::group g, const std::string &name, const triqs::modest::iteration_data &data) { return triqs::modest::h5_write(g, name, data); },
              "g", "name", "data")};

// impurity_levels
static auto const fun_32 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::modest::one_body_elements_on_grid &obe) { return triqs::modest::impurity_levels(obe); }, "obe")};

// make_embedding
static auto const fun_33 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::modest::local_space &C_space, const nda::array<std::vector<long>, 2> &block_decomposition,
      const std::optional<std::vector<long>> &atom_to_imp) { return triqs::modest::make_embedding(C_space, block_decomposition, atom_to_imp); },
   "C_space", "block_decomposition", "atom_to_imp"_a = std::nullopt)};

// make_embedding_with_equivalences
static auto const fun_34 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::modest::local_space &C_space) { return triqs::modest::make_embedding_with_equivalences(C_space); }, "C_space")};

// make_embedding_with_no_equivalences
static auto const fun_35 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::modest::local_space &C_space) { return triqs::modest::make_embedding_with_no_equivalences(C_space); }, "C_space")};

// make_kanamori
static auto const fun_36 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const std::vector<std::string> &tau_names, const std::vector<long> &dim_gamma, double U_int, double U_prime, double J_hund, bool spin_flip,
      bool pair_hopping) { return triqs::make_kanamori(tau_names, dim_gamma, U_int, U_prime, J_hund, spin_flip, pair_hopping); },
   "tau_names", "dim_gamma", "U_int", "U_prime", "J_hund", "spin_flip"_a = true, "pair_hopping"_a = true)};

// make_slater
static auto const fun_37 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const std::vector<std::string> &tau_names, const std::vector<long> &dim_gamma, double U_int, double J_hund,
      const nda::matrix<triqs::dcomplex> &spherical_to_dft, const std::optional<nda::matrix<triqs::dcomplex>> &dft_to_local) {
     return triqs::make_slater(tau_names, dim_gamma, U_int, J_hund, spherical_to_dft, dft_to_local);
   },
   "tau_names", "dim_gamma", "U_int", "J_hund", "spherical_to_dft", "dft_to_local")};

// one_body_elements_from_dft_converter
static auto const fun_38 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const std::string &filename, double threshold,
                 bool diagonalize_hloc) { return triqs::modest::one_body_elements_from_dft_converter(filename, threshold, diagonalize_hloc); },
              "filename", "threshold"_a = 1.e-5, "diagonalize_hloc"_a = false)};

// one_body_elements_on_high_symmetry_path
static auto const fun_39 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const std::string &filename, const triqs::modest::one_body_elements_on_grid &obe) {
     return triqs::modest::one_body_elements_on_high_symmetry_path(filename, obe);
   },
   "filename", "obe")};

// one_body_elements_with_theta_projectors
static auto const fun_40 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const std::string &filename, const triqs::modest::one_body_elements_on_grid &obe) {
     return triqs::modest::one_body_elements_with_theta_projectors(filename, obe);
   },
   "filename", "obe")};

// projected_spectral_function
static auto const fun_41 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::modest::one_body_elements_on_grid &obe_theta, const triqs::modest::downfolding_projector &Proj, double mu,
      const triqs::gfs::block2_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &Sigma_w, const nda::array<nda::matrix<double>, 2> &Sigma_DC,
      double broadening) { return triqs::modest::projected_spectral_function(obe_theta, Proj, mu, Sigma_w, Sigma_DC, broadening); },
   "obe_theta", "Proj", "mu", "Sigma_w", "Sigma_DC", "broadening"_a = 0.01)};

// root_finder
static auto const fun_42 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](std::string method, std::function<double(double)> f, double x_init, double y_value, double precision, double delta_x, long max_loops,
      std::string x_name, std::string y_name,
      bool verbosity) { return triqs::root_finder(method, f, x_init, y_value, precision, delta_x, max_loops, x_name, y_name, verbosity); },
   "method", "f", "x_init", "y_value", "precision", "delta_x", "max_loops"_a = 1000, "x_name"_a = "", "y_name"_a = "", "verbosity"_a = true)};

// sigma_to_data_idx
static auto const fun_43 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](triqs::modest::spin_kind_e spin_kind, long sigma) { return triqs::modest::sigma_to_data_idx(spin_kind, sigma); }, "spin_kind", "sigma")};

// spectral_function_on_high_symmetry_path
static auto const fun_44 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::modest::one_body_elements_on_grid &obe, double mu,
      const triqs::gfs::block2_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &Sigma_w, const nda::array<nda::matrix<double>, 2> &Sigma_DC,
      double broadening) { return triqs::modest::spectral_function_on_high_symmetry_path(obe, mu, Sigma_w, Sigma_DC, broadening); },
   "obe", "mu", "Sigma_w", "Sigma_DC", "broadening"_a = 0.01)};

// symmetrize_gf
static auto const fun_45 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Gin,
                 std::vector<std::vector<long>> degenerate_blocks) { return triqs::modest::symmetrize_gf(Gin, degenerate_blocks); },
              "Gin", "degenerate_blocks")};

static const auto doc_d_19       = fun_19.doc({R"DOC(   The projected bare function on the C space, cf notes.)DOC"});
static const auto doc_d_20       = fun_20.doc({R"DOC(   Parameters
   ----------
   
   l:
      
   U_int:
      
   J_hund:
      
   
   Returns
   -------
   
      nda::array<double, 4>)DOC"});
static const auto doc_d_21       = fun_21.doc({R"DOC(   Parameters
   ----------
   
   Gimp:
      Block Green's function
   threshold:
      tolerance for equivalence of blocks
   
   Returns
   -------
   
      A list of equivalent blocks)DOC"});
static const auto doc_d_22       = fun_22.doc({R"DOC(   double counting formulas parameterized by density, U, and J)DOC"});
static const auto doc_d_23       = fun_23.doc({R"DOC(   Parameters
   ----------
   
   obe:
      one body elements on grid
   mu:
      chemical potential
   Sigma:
      Embedded self-energy
   
   Returns
   -------
   
      double)DOC"});
static const auto doc_d_24       = fun_24.doc({R"DOC(   Compute number of particles n = ∑f(β(e(k)-μ)))DOC"});
static const auto doc_d_25       = fun_25.doc({R"DOC(   Parameters
   ----------
   
   obe:
      one_body_elements_on_grid
   mu:
      chemical potential
   Sigma:
      Embedded self-energy
   
   Returns
   -------
   
      double)DOC"});
static const auto doc_d_26       = fun_26.doc({R"DOC(   Parameters
   ----------
   
   GC:
      Green's function in C space
   U_int:
      Coulomb interaction parameter
   J_hund:
      Hund's coupling interaction parameter
   method:
      dc_formula (sFLL, cFLL, sAMF, cAMF, cHeld)
   
   Returns
   -------
   
      H_DC nda::array<dc_t, 2>)DOC"});
static constexpr auto doc_f_27_0 = R"DOC(   )DOC";
static constexpr auto doc_f_27_1 = R"DOC(   )DOC";
static constexpr auto doc_f_27_2 = R"DOC(   )DOC";
static constexpr auto doc_f_27_3 = R"DOC(   )DOC";
static const auto doc_d_27       = fun_27.doc({doc_f_27_0, doc_f_27_1, doc_f_27_2, doc_f_27_3});
static constexpr auto doc_f_28_0 = R"DOC(   )DOC";
static constexpr auto doc_f_28_1 = R"DOC(   )DOC";
static constexpr auto doc_f_28_2 = R"DOC(   )DOC";
static const auto doc_d_28       = fun_28.doc({doc_f_28_0, doc_f_28_1, doc_f_28_2});
static constexpr auto doc_f_29_0 = R"DOC(   Parameters
   ----------
   
   obe:
      one_body_elements_on_grid
   mu:
      chemical potential
   Sigma:
      Embedded self-energy TODO: {Σ∞, ΣDLR }
   
   Returns
   -------
   
      block2_gf<Mesh, matrix_valued>)DOC";
static constexpr auto doc_f_29_1 = R"DOC(   Parameters
   ----------
   
   obe:
      one_body_elements_on_grid
   mu:
      chemical potential
   Sigma:
      Embedded self-energy TODO: {Σ∞, ΣDLR }
   
   Returns
   -------
   
      block2_gf<Mesh, matrix_valued>)DOC";
static constexpr auto doc_f_29_2 = R"DOC(   Parameters
   ----------
   
   mesh:
      mesh triqs::meshes::{imfreq, dlr_imfreq}
   obe:
      one-body elements
   mu:
      chemical potential
   
   Returns
   -------
   
      block2_gf<Mesh, matrix_valued>)DOC";
static constexpr auto doc_f_29_3 = R"DOC(   Parameters
   ----------
   
   mesh:
      mesh triqs::meshes::{imfreq, dlr_imfreq}
   obe:
      one-body elements
   mu:
      chemical potential
   
   Returns
   -------
   
      block2_gf<Mesh, matrix_valued>)DOC";
static const auto doc_d_29       = fun_29.doc({doc_f_29_0, doc_f_29_1, doc_f_29_2, doc_f_29_3});
static constexpr auto doc_f_30_0 = R"DOC(   )DOC";
static constexpr auto doc_f_30_1 = R"DOC(   )DOC";
static constexpr auto doc_f_30_2 = R"DOC(   )DOC";
static const auto doc_d_30       = fun_30.doc({doc_f_30_0, doc_f_30_1, doc_f_30_2});
static constexpr auto doc_f_31_0 = R"DOC(   )DOC";
static constexpr auto doc_f_31_1 = R"DOC(   )DOC";
static constexpr auto doc_f_31_2 = R"DOC(   )DOC";
static const auto doc_d_31       = fun_31.doc({doc_f_31_0, doc_f_31_1, doc_f_31_2});
static const auto doc_d_32       = fun_32.doc({R"DOC(   Compute the local impurity levels from the single-particle dispersion.)DOC"});
static const auto doc_d_33       = fun_33.doc({R"DOC(   Parameters
   ----------
   
   C_space:
      the local space from one_body_elements
   block_decomposition:
      the decomposition of atomic orbitals into their irreducible representations
   atom_to_imp:
      [optional] a mapping between equivalent atom sites.
   
   Returns
   -------
   
      embedding)DOC"});
static const auto doc_d_34       = fun_34.doc({R"DOC(   Parameters
   ----------
   
   C_space:
      the local space from one_body_elements
   
   Returns
   -------
   
      embedding)DOC"});
static const auto doc_d_35       = fun_35.doc({R"DOC(   Parameters
   ----------
   
   C_space:
      the local space from one_body_elements.
   
   Returns
   -------
   
      embedding)DOC"});
static const auto doc_d_36       = fun_36.doc({R"DOC(   Parameters
   ----------
   
   tau_names:
      names of tau indices ['up', 'down']
   dim_gamma:
      dimension of the blocks γ
   U_int:
      Hubbard U
   U_prime:
      U' typical U' = U - 2J
   J_hund:
      Kanamori J
   spin_flip:
      spin flip term
   pair_hopping:
      pair hopping term)DOC"});
static const auto doc_d_37       = fun_37.doc({R"DOC(   Parameters
   ----------
   
   tau_names:
      
   dim_gamma:
      
   U_int:
      
   J_hund:
      
   spherical_to_dft:
      
   dft_to_local:)DOC"});
static const auto doc_d_38       = fun_38.doc({R"DOC(   Parameters
   ----------
   
   filename:
      Hdf5 file from dft_tools converter.
   threshold:
      Off-diagonal threshold for discovery of symmetries.
   diagonalize_hloc:
      Diagonalize the local non-interacting hamiltonian.
   verbosity:
      Log steps of creating the one-body elements.
   
   Returns
   -------
   
      The total electron density and a one-body elements.)DOC"});
static const auto doc_d_39       = fun_39.doc({R"DOC(   Parameters
   ----------
   
   filename:
      Hdf5 file from the dft_tools converter.
   obe:
      One-body elements that was ued in the DMFT calculation.
   
   Returns
   -------
   
      One-body elements along high-symmetry k-path.)DOC"});
static const auto doc_d_40       = fun_40.doc({R"DOC(   Parameters
   ----------
   
   filename:
      Hdf5 file from DFTtools converter with dft_parproj_input group.
   obe:
      One-body elements that was used in the DMFT calculation.
   
   Returns
   -------
   
      One-body elements using the Θ projectors.)DOC"});
static const auto doc_d_41       = fun_41.doc({R"DOC(   Parameters
   ----------
   
   obe_theta:
      one-body elements on grid created from one_body_elements_with_theta_projectors (see obe factories).
   Pk:
      downfolding_projector defined in the correlated space using to upfold the DMFT self-energies.
   mu:
      chemical potential
   Sigma_w:
      self-energy on real-frequency mesh
   Sigma_DC:
      double counting term
   broadening:
      spectral broadening
   
   Returns
   -------
   
      spectral_function_w)DOC"});
static const auto doc_d_42       = fun_42.doc({R"DOC(   Parameters
   ----------
   
   method:
      root finding method (dichtomy, bisection)
   f:
      f(x) (double -> double)
   x_init:
      initial value for x
   y_value:
      target value for y
   precision:
      precision for algorihtm
   delta_x:
      increment of x
   max_loops:
      max number of iterations for method
   x_name:
      name of x variable
   y_name:
      name of y variable f(x)
   verbosity:
      logging of root finder
   
   Returns
   -------
   
      x, f(x) std::pair<double, double>)DOC"});
static const auto doc_d_43       = fun_43.doc({R"DOC(   )DOC"});
static const auto doc_d_44       = fun_44.doc({R"DOC(   Parameters
   ----------
   
   obe:
      one-body elements on grid created from one_body_elements_on_high_symmetry_path (see obe factories)
   mu:
      chemical potential
   Sigma_w:
      self-energy on real-frequency mesh
   Sigma_DC:
      double-counting term
   broadening:
      spectral broadening
   
   Returns
   -------
   
      spectral_function_kw)DOC"});
static const auto doc_d_45       = fun_45.doc({R"DOC(   )DOC"});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"G0_C_k_sigma", (PyCFunction)c2py::pyfkw<fun_19>, METH_VARARGS | METH_KEYWORDS, doc_d_19.c_str()},
   {"U_matrix_in_spherical_basis", (PyCFunction)c2py::pyfkw<fun_20>, METH_VARARGS | METH_KEYWORDS, doc_d_20.c_str()},
   {"analyze_degenerate_blocks", (PyCFunction)c2py::pyfkw<fun_21>, METH_VARARGS | METH_KEYWORDS, doc_d_21.c_str()},
   {"dc_formulas", (PyCFunction)c2py::pyfkw<fun_22>, METH_VARARGS | METH_KEYWORDS, doc_d_22.c_str()},
   {"density", (PyCFunction)c2py::pyfkw<fun_23>, METH_VARARGS | METH_KEYWORDS, doc_d_23.c_str()},
   {"density_nk", (PyCFunction)c2py::pyfkw<fun_24>, METH_VARARGS | METH_KEYWORDS, doc_d_24.c_str()},
   {"density_slow", (PyCFunction)c2py::pyfkw<fun_25>, METH_VARARGS | METH_KEYWORDS, doc_d_25.c_str()},
   {"double_counting", (PyCFunction)c2py::pyfkw<fun_26>, METH_VARARGS | METH_KEYWORDS, doc_d_26.c_str()},
   {"extract_delta", (PyCFunction)c2py::pyfkw<fun_27>, METH_VARARGS | METH_KEYWORDS, doc_d_27.c_str()},
   {"find_chemical_potential", (PyCFunction)c2py::pyfkw<fun_28>, METH_VARARGS | METH_KEYWORDS, doc_d_28.c_str()},
   {"gloc", (PyCFunction)c2py::pyfkw<fun_29>, METH_VARARGS | METH_KEYWORDS, doc_d_29.c_str()},
   {"h5_read", (PyCFunction)c2py::pyfkw<fun_30>, METH_VARARGS | METH_KEYWORDS, doc_d_30.c_str()},
   {"h5_write", (PyCFunction)c2py::pyfkw<fun_31>, METH_VARARGS | METH_KEYWORDS, doc_d_31.c_str()},
   {"impurity_levels", (PyCFunction)c2py::pyfkw<fun_32>, METH_VARARGS | METH_KEYWORDS, doc_d_32.c_str()},
   {"make_embedding", (PyCFunction)c2py::pyfkw<fun_33>, METH_VARARGS | METH_KEYWORDS, doc_d_33.c_str()},
   {"make_embedding_with_equivalences", (PyCFunction)c2py::pyfkw<fun_34>, METH_VARARGS | METH_KEYWORDS, doc_d_34.c_str()},
   {"make_embedding_with_no_equivalences", (PyCFunction)c2py::pyfkw<fun_35>, METH_VARARGS | METH_KEYWORDS, doc_d_35.c_str()},
   {"make_kanamori", (PyCFunction)c2py::pyfkw<fun_36>, METH_VARARGS | METH_KEYWORDS, doc_d_36.c_str()},
   {"make_slater", (PyCFunction)c2py::pyfkw<fun_37>, METH_VARARGS | METH_KEYWORDS, doc_d_37.c_str()},
   {"one_body_elements_from_dft_converter", (PyCFunction)c2py::pyfkw<fun_38>, METH_VARARGS | METH_KEYWORDS, doc_d_38.c_str()},
   {"one_body_elements_on_high_symmetry_path", (PyCFunction)c2py::pyfkw<fun_39>, METH_VARARGS | METH_KEYWORDS, doc_d_39.c_str()},
   {"one_body_elements_with_theta_projectors", (PyCFunction)c2py::pyfkw<fun_40>, METH_VARARGS | METH_KEYWORDS, doc_d_40.c_str()},
   {"projected_spectral_function", (PyCFunction)c2py::pyfkw<fun_41>, METH_VARARGS | METH_KEYWORDS, doc_d_41.c_str()},
   {"root_finder", (PyCFunction)c2py::pyfkw<fun_42>, METH_VARARGS | METH_KEYWORDS, doc_d_42.c_str()},
   {"sigma_to_data_idx", (PyCFunction)c2py::pyfkw<fun_43>, METH_VARARGS | METH_KEYWORDS, doc_d_43.c_str()},
   {"spectral_function_on_high_symmetry_path", (PyCFunction)c2py::pyfkw<fun_44>, METH_VARARGS | METH_KEYWORDS, doc_d_44.c_str()},
   {"symmetrize_gf", (PyCFunction)c2py::pyfkw<fun_45>, METH_VARARGS | METH_KEYWORDS, doc_d_45.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "module",                                                                      /* name of module */
   R"RAWDOC(Python Module for TRIQS Modular Electronic Structure Toolkit)RAWDOC", /* module documentation, may be NULL */
   -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
   module_methods,
   NULL,
   NULL,
   NULL,
   NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_module() {

  if (not c2py::check_python_version("module")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::atomic_shell_t>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::local_space>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::band_dispersion>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::downfolding_projector>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::one_body_elements_on_grid>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::embedding>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::embedding::imp_block_t>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::dc_solver>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::spectral_function_kw>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::spectral_function_w>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::initial_data>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::iteration_data>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::detail::union_find>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
  c2py::add_type_object_to_main<triqs::modest::atomic_shell_t>("AtomicShellT", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::local_space>("LocalSpace", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::band_dispersion>("BandDispersion", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::downfolding_projector>("DownfoldingProjector", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::one_body_elements_on_grid>("OneBodyElementsOnGrid", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::embedding>("Embedding", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::embedding::imp_block_t>("ImpBlockT", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::dc_solver>("DcSolver", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::spectral_function_kw>("SpectralFunctionKw", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::spectral_function_w>("SpectralFunctionW", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::initial_data>("InitialData", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::iteration_data>("IterationData", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::detail::union_find>("UnionFind", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::checkpoint<triqs::modest::initial_data, triqs::modest::iteration_data>>("DMFTCheckpoint", m,
                                                                                                                       conv_table);

  return m;
}
#endif
// CLAIR_WRAP_GEN
