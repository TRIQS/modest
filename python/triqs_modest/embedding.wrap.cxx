
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

template <> constexpr bool c2py::is_wrapped<triqs::modest::embedding>              = true;
template <> constexpr bool c2py::is_wrapped<triqs::modest::embedding::imp_block_t> = true;

// ==================== enums =====================

// ==================== module classes =====================

template <> inline constexpr auto c2py::tp_name<triqs::modest::embedding> = "triqs_modest.embedding.Embedding";
static auto init_0 = c2py::dispatcher_c_kw_t{c2py::c_constructor<triqs::modest::embedding, std::vector<long>, std::vector<std::vector<long>>,
                                                                 nda::array<triqs::modest::embedding::imp_block_t, 2>, std::vector<std::string>>(
                                                "sigma_embed_decomposition", "imp_decompositions", "psi", "sigma_names"),
                                             c2py::c_constructor<triqs::modest::embedding>()};
template <> constexpr initproc c2py::tp_init<triqs::modest::embedding> = c2py::pyfkw_constructor<init_0>;
template <>
const std::string c2py::tp_ctor_doc<triqs::modest::embedding> = init_0.doc(
   R"DOC(
[1] Construct an embedding object.

------

[2] Default constructor for reading from HDF5.

------

Parameters
----------
sigma_embed_decomposition : {par_0}
   Decomposition (list of block sizes) for :math:`\Sigma_{\text{embed}}`:
   :math:`[ \alpha ] =` block size. Note that the decompositions are given for :math:`\alpha`, :math:`\gamma` index,
   and are independent of :math:`\sigma`.
imp_decompositions : {par_1}
   List of decomposition of solvers: [n_imp][:math:`\gamma`] = block size.
psi : {par_2}
   The mapping :math:`\psi[\alpha,\sigma]` -> n_imp, :math:`\gamma`, :math:`\tau`.
sigma_names : {par_3}
   Names for the values of the :math:`\sigma` index (e.g., "up", "down").
)DOC",
   std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<std::vector<long>>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<std::vector<std::vector<long>>>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<nda::array<triqs::modest::embedding::imp_block_t, 2>>()}, ", "),
                            c2py::join(std::vector<std::string>{c2py::python_typename<std::vector<std::string>>()}, ", ")},
   std::vector<std::string>{});
// description
static auto const fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self, bool verbosity) { return self.description(verbosity); }, "self", "verbosity"_a = false)};

// drop
static auto const fun_1 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](triqs::modest::embedding const &self, long imp_idx) { return self.drop(imp_idx); }, "self", "imp_idx")};

// embed
static auto const fun_2 = c2py::dispatcher_f_kw_t{
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
   c2py::cmethod([](triqs::modest::embedding const &self,
                    const std::vector<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>> &Sigma_imp_vec,
                    const std::vector<triqs::block_matrix_t> &Sigma_imp_static_vec) { return self.embed(Sigma_imp_vec, Sigma_imp_static_vec); },
                 "self", "Sigma_imp_vec", "Sigma_imp_static_vec"),
   c2py::cmethod([](triqs::modest::embedding const &self,
                    const std::vector<triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>> &Sigma_imp_vec,
                    const std::vector<triqs::block_matrix_t> &Sigma_imp_static_vec) { return self.embed(Sigma_imp_vec, Sigma_imp_static_vec); },
                 "self", "Sigma_imp_vec", "Sigma_imp_static_vec"),
   c2py::cmethod([](triqs::modest::embedding const &self,
                    const std::vector<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>> &Sigma_imp_vec,
                    const std::vector<triqs::block_matrix_t> &Sigma_imp_static_vec) { return self.embed(Sigma_imp_vec, Sigma_imp_static_vec); },
                 "self", "Sigma_imp_vec", "Sigma_imp_static_vec"),
   c2py::cmethod([](triqs::modest::embedding const &self,
                    const std::vector<triqs::block_matrix_t> &Sigma_imp_static_vec) { return self.embed(Sigma_imp_static_vec); },
                 "self", "Sigma_imp_static_vec")};

// embed_ij
static auto const fun_3 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self,
                    const std::vector<triqs::block_matrix_t> &Sigma_imp_static_vec) { return self.embed_ij(Sigma_imp_static_vec); },
                 "self", "Sigma_imp_static_vec")};

// embed_ijkl
static auto const fun_4 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self,
                    const std::vector<nda::array<triqs::dcomplex, 4>> &U_tensor_vec) { return self.embed_ijkl(U_tensor_vec); },
                 "self", "U_tensor_vec")};

// embed_wij
static auto const fun_5 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self,
                    const std::vector<std::vector<nda::array<triqs::dcomplex, 3>>> &Sigma_imp_vec) { return self.embed_wij(Sigma_imp_vec); },
                 "self", "Sigma_imp_vec")};

// embed_wijkl
static auto const fun_6 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](triqs::modest::embedding const &self, const std::vector<nda::array<triqs::dcomplex, 5>> &pi_imp_vec) { return self.embed_wijkl(pi_imp_vec); },
   "self", "pi_imp_vec")};

// extract
static auto const fun_7 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self,
                    const triqs::gfs::block2_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &g_loc) { return self.extract(g_loc); },
                 "self", "g_loc"),
   c2py::cmethod([](triqs::modest::embedding const &self,
                    const triqs::gfs::block2_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &g_loc) { return self.extract(g_loc); },
                 "self", "g_loc"),
   c2py::cmethod([](triqs::modest::embedding const &self,
                    const triqs::gfs::block2_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &g_loc) { return self.extract(g_loc); },
                 "self", "g_loc"),
   c2py::cmethod([](triqs::modest::embedding const &self, const triqs::block2_matrix_t &matrix_C) { return self.extract(matrix_C); }, "self",
                 "matrix_C")};

// extract_ijkl
static auto const fun_8 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self, const nda::array<triqs::dcomplex, 4> &U_tensor) { return self.extract_ijkl(U_tensor); },
                 "self", "U_tensor")};

// extract_wij
static auto const fun_9 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](triqs::modest::embedding const &self, const nda::array<triqs::dcomplex, 4> &g_loc) { return self.extract_wij(g_loc); }, "self", "g_loc")};

// extract_wijkl
static auto const fun_10 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](triqs::modest::embedding const &self, const nda::array<triqs::dcomplex, 5> &Pi_loc) { return self.extract_wijkl(Pi_loc); }, "self", "Pi_loc")};

// flip_spin
static auto const fun_11 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self, long alpha) { return self.flip_spin(alpha); }, "self", "alpha"),
   c2py::cmethod([](triqs::modest::embedding const &self, std::vector<long> alphas) { return self.flip_spin(alphas); }, "self", "alphas")};

// imp_decomposition
static auto const fun_12 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self, long imp_idx) { return self.imp_decomposition(imp_idx); }, "self", "imp_idx")};

// make_zero_imp_self_energies
static auto const fun_13 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding &self, const triqs::mesh::imfreq &mesh) { return self.make_zero_imp_self_energies(mesh); }, "self",
                 "mesh"),
   c2py::cmethod([](triqs::modest::embedding &self, const triqs::mesh::refreq &mesh) { return self.make_zero_imp_self_energies(mesh); }, "self",
                 "mesh"),
   c2py::cmethod([](triqs::modest::embedding &self, const triqs::mesh::dlr_imfreq &mesh) { return self.make_zero_imp_self_energies(mesh); }, "self",
                 "mesh")};

// n_gamma
static auto const fun_14 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self, long imp_idx) { return self.n_gamma(imp_idx); }, "self", "imp_idx")};

// replace
static auto const fun_15 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](triqs::modest::embedding const &self, long imp_idx_to_remove,
                                            long imp_idx_to_replace_with) { return self.replace(imp_idx_to_remove, imp_idx_to_replace_with); },
                                         "self", "imp_idx_to_remove", "imp_idx_to_replace_with")};

// split
static auto const fun_16 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](triqs::modest::embedding const &self, long imp_idx, std::function<bool(long)> p) { return self.split(imp_idx, p); }, "self",
                 "imp_idx", "p"),
   c2py::cmethod(
      [](triqs::modest::embedding const &self, long imp_idx, const std::vector<long> &block_list) { return self.split(imp_idx, block_list); }, "self",
      "imp_idx", "block_list")};

static const auto doc_d_0 = fun_0.doc(R"DOC(
Summarize the embedding object.
)DOC",
                                      std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_1 = fun_1.doc(R"DOC(
Remove an impurity from the embedding table :math:`\psi`.

Remove the impurity `imp_idx`, map the corresponding :math:`\alpha` blocks to -1 (i.e. no longer
connected to an impurity) and relabel the other impurities with a number > `imp_idx`.

Parameters
----------
imp_idx : {par_0}
   Index of the impurity to remove from the table :math:`\psi`.

Returns
-------
{ret_0}
   New embedding with the updated :math:`\psi` map.
)DOC",
                                      std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<long>()}, ", ")},
                                      std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::modest::embedding>()}});
static const auto doc_d_2 = fun_2.doc(
   R"DOC(
[1, 2, 3] Embed single-particle quantities (TRIQS/ModEST).

Embed impurity solver self-energies into an embedded self-energy.

* :math:`\Sigma_{\text{embed}}^{ασ} = \Sigma_{\text{imp}}^{ψ(α,σ)}`,
* :math:`\psi(\alpha,\sigma) \to` (n_imp, :math:`\gamma`, :math:`\tau`).

------

[4, 5, 6] Embed single-particle quantities (TRIQS/ModEST).

Embed the impurity self-energy decomposed as a list of dynamic and a list of static parts.

------

[7] Embed block matrices (TRIQS/ModEST).

------

Parameters
----------
Sigma_imp_vec : {par_0}
   A list of impurity self-energies.
Sigma_imp_static_vec : {par_1}
   A list of static impurity self-energies.

Returns
-------
[1] : {ret_0}
   :math:`\Sigma_{\text{embed}}[\alpha, \sigma]` in :math:`\mathcal{C}` space using the embed decomposition.

[2] : {ret_1}
   :math:`\Sigma_{\text{embed}}[\alpha, \sigma]` in :math:`\mathcal{C}` space using the embed decomposition.

[3] : {ret_2}
   :math:`\Sigma_{\text{embed}}[\alpha, \sigma]` in :math:`\mathcal{C}` space using the embed decomposition.

[4] : {ret_3}
   The self-energy in the :math:`\mathcal{C}` space as a pair (dynamic, static).

[5] : {ret_4}
   The self-energy in the :math:`\mathcal{C}` space as a pair (dynamic, static).

[6] : {ret_5}
   The self-energy in the :math:`\mathcal{C}` space as a pair (dynamic, static).
)DOC",
   std::vector<std::string>{
      c2py::join(
         std::vector<std::string>{
            c2py::python_typename<const std::vector<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>> &>(),
            c2py::python_typename<const std::vector<triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>> &>(),
            c2py::python_typename<const std::vector<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>> &>()},
         ", "),
      c2py::join(std::vector<std::string>{c2py::python_typename<const std::vector<triqs::block_matrix_t> &>()}, ", ")},
   std::vector<std::string>{std::vector<std::string>{
      c2py::python_typename<triqs::gfs::block2_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>>(),
      c2py::python_typename<triqs::gfs::block2_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>>(),
      c2py::python_typename<triqs::gfs::block2_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>>(),
      c2py::python_typename<std::pair<triqs::gfs::block2_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>, triqs::block2_matrix_t>>(),
      c2py::python_typename<std::pair<triqs::gfs::block2_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>, triqs::block2_matrix_t>>(),
      c2py::python_typename<std::pair<triqs::gfs::block2_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>, triqs::block2_matrix_t>>()}});
static const auto doc_d_3 = fun_3.doc(R"DOC()DOC", std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_4 = fun_4.doc(R"DOC(
Embed tensors (CoQui).
)DOC",
                                      std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_5 = fun_5.doc(R"DOC(
Embed single-particle quantities (CoQui).
)DOC",
                                      std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_6 = fun_6.doc(R"DOC(
Embed two-particle quantities (CoQui).
)DOC",
                                      std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_7 = fun_7.doc(
   R"DOC(
[1, 2, 3] Extract single-particle quantities (TRIQS/ModEST).

------

[4] Extract matrices (TRIQS/ModEST).

------

Parameters
----------
g_loc : {par_0}
   Block2Gf of gloc in :math:`M \times M` space.

Returns
-------
[1] : {ret_0}
   Local impurity Green's function.

[2] : {ret_1}
   Local impurity Green's function.

[3] : {ret_2}
   Local impurity Green's function.
)DOC",
   std::vector<std::string>{
      c2py::join(std::vector<std::string>{c2py::python_typename<const triqs::gfs::block2_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>(),
                                          c2py::python_typename<const triqs::gfs::block2_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &>(),
                                          c2py::python_typename<const triqs::gfs::block2_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &>()},
                 ", ")},
   std::vector<std::string>{
      std::vector<std::string>{c2py::python_typename<std::vector<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>>>(),
                               c2py::python_typename<std::vector<triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>>>(),
                               c2py::python_typename<std::vector<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>>>()}});
static const auto doc_d_8  = fun_8.doc(R"DOC(
Extract tensors (CoQui).
)DOC",
                                       std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_9  = fun_9.doc(R"DOC(
Extract single-particle quantities (CoQui).
)DOC",
                                       std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_10 = fun_10.doc(R"DOC(
Extract two-particle quantities (CoQui).
)DOC",
                                        std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_11 =
   fun_11.doc(R"DOC(
[1] Flip the spins (:math:`\sigma`) for block :math:`\alpha`.

For block :math:`\alpha` transforms :math:`(\sigma,\sigma') \to (\tau, \tau')` to
:math:`(\sigma, \sigma') \to (\tau', \tau)`.

------

[2] Flip the spins (:math:`\sigma`) for all :math:`[\alpha]`.

For block :math:`\alpha` transforms :math:`(\sigma,\sigma') \to (\tau, \tau')` to
:math:`(\sigma, \sigma') \to (\tau', \tau)`.

------

Parameters
----------
alpha : {par_0}
   The index of the alpha block to flip the spins.
alphas : {par_1}
   A list of alpha blocks to flip the spins.

Returns
-------
{ret_0}
   New embedding with the updated :math:`\psi` map.
)DOC",
              std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<long>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<std::vector<long>>()}, ", ")},
              std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::modest::embedding>()}});
static const auto doc_d_12 = fun_12.doc(R"DOC(
The impurity decomposition.
)DOC",
                                        std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_13 = fun_13.doc(R"DOC(
Zero initialize impurity self-energies.
)DOC",
                                        std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_14 = fun_14.doc(R"DOC(
Number of blocks in :math:`\gamma` for the :math:`\Sigma_{\text{imp}}` [imp_idx].
)DOC",
                                        std::vector<std::string>{}, std::vector<std::string>{});
static const auto doc_d_15 = fun_15.doc(R"DOC(
Replaces one impurity in the embedding table :math:`\psi`.

Replaces the impurity solver corresponding to `imp_idx_to_remove` with the impurity solver at
`imp_idx_to_replace_with` and updates the underlying table :math:`\psi`.

Parameters
----------
imp_idx_to_remove : {par_0}
   The index of the impurity solver to be replaced.
imp_idx_to_replace_with : {par_1}
   The index of the impurity solver that will replace `imp_idx_to_remove`.

Returns
-------
{ret_0}
   A new embedding with the updated :math:`\psi` map.
)DOC",
                                        std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<long>()}, ", "),
                                                                 c2py::join(std::vector<std::string>{c2py::python_typename<long>()}, ", ")},
                                        std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::modest::embedding>()}});
static const auto doc_d_16 =
   fun_16.doc(R"DOC(
[1] Split impurity `imp_idx`.

Predicate p (long block_idx) -> 0 or 1.

------

[2] Split impurity `imp_idx` based on `block_list`.

For impurity `imp_idx`, split part of its blocks into a new impurity.
The blocks that were split off remain mapped to their original alpha blocks with the updated `imp_idx`.

------

Parameters
----------
imp_idx : {par_0}
   Impurity number.
block_list : {par_1}
   A list of blocks of `imp_idx` to split off.

Returns
-------
{ret_0}
   New embedding with the updated :math:`\psi` map.
)DOC",
              std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<long>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<const std::vector<long> &>()}, ", ")},
              std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::modest::embedding>()}});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::embedding>[] = {
   {"description", (PyCFunction)c2py::pyfkw<fun_0>, METH_VARARGS | METH_KEYWORDS, doc_d_0.c_str()},
   {"drop", (PyCFunction)c2py::pyfkw<fun_1>, METH_VARARGS | METH_KEYWORDS, doc_d_1.c_str()},
   {"embed", (PyCFunction)c2py::pyfkw<fun_2>, METH_VARARGS | METH_KEYWORDS, doc_d_2.c_str()},
   {"embed_ij", (PyCFunction)c2py::pyfkw<fun_3>, METH_VARARGS | METH_KEYWORDS, doc_d_3.c_str()},
   {"embed_ijkl", (PyCFunction)c2py::pyfkw<fun_4>, METH_VARARGS | METH_KEYWORDS, doc_d_4.c_str()},
   {"embed_wij", (PyCFunction)c2py::pyfkw<fun_5>, METH_VARARGS | METH_KEYWORDS, doc_d_5.c_str()},
   {"embed_wijkl", (PyCFunction)c2py::pyfkw<fun_6>, METH_VARARGS | METH_KEYWORDS, doc_d_6.c_str()},
   {"extract", (PyCFunction)c2py::pyfkw<fun_7>, METH_VARARGS | METH_KEYWORDS, doc_d_7.c_str()},
   {"extract_ijkl", (PyCFunction)c2py::pyfkw<fun_8>, METH_VARARGS | METH_KEYWORDS, doc_d_8.c_str()},
   {"extract_wij", (PyCFunction)c2py::pyfkw<fun_9>, METH_VARARGS | METH_KEYWORDS, doc_d_9.c_str()},
   {"extract_wijkl", (PyCFunction)c2py::pyfkw<fun_10>, METH_VARARGS | METH_KEYWORDS, doc_d_10.c_str()},
   {"flip_spin", (PyCFunction)c2py::pyfkw<fun_11>, METH_VARARGS | METH_KEYWORDS, doc_d_11.c_str()},
   {"imp_decomposition", (PyCFunction)c2py::pyfkw<fun_12>, METH_VARARGS | METH_KEYWORDS, doc_d_12.c_str()},
   {"make_zero_imp_self_energies", (PyCFunction)c2py::pyfkw<fun_13>, METH_VARARGS | METH_KEYWORDS, doc_d_13.c_str()},
   {"n_gamma", (PyCFunction)c2py::pyfkw<fun_14>, METH_VARARGS | METH_KEYWORDS, doc_d_14.c_str()},
   {"replace", (PyCFunction)c2py::pyfkw<fun_15>, METH_VARARGS | METH_KEYWORDS, doc_d_15.c_str()},
   {"split", (PyCFunction)c2py::pyfkw<fun_16>, METH_VARARGS | METH_KEYWORDS, doc_d_16.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0 = R"DOC(The mapping table :math:`\psi`.)DOC";
static constexpr auto prop_doc_1 = R"DOC(Gf block structure for the impurity solvers.)DOC";
static constexpr auto prop_doc_2 = R"DOC(Number of blocks in :math:`\alpha` for the :math:`\Sigma_{\text{embed}}`.)DOC";
static constexpr auto prop_doc_3 = R"DOC(Number of impurities.)DOC";
static constexpr auto prop_doc_4 = R"DOC(Number of blocks in :math:`\sigma` for the :math:`\Sigma_{\text{embed}}`.)DOC";
static constexpr auto prop_doc_5 = R"DOC(The names of the sigma indices.)DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::embedding>[] = {

   {"get_psi", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::get_psi)>, nullptr, prop_doc_0, nullptr},
   {"imp_block_shape", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::imp_block_shape)>, nullptr, prop_doc_1, nullptr},
   {"n_alpha", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::n_alpha)>, nullptr, prop_doc_2, nullptr},
   {"n_impurities", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::n_impurities)>, nullptr, prop_doc_3, nullptr},
   {"n_sigma", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::n_sigma)>, nullptr, prop_doc_4, nullptr},
   {"sigma_names", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::sigma_names)>, nullptr, prop_doc_5, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_0(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<triqs::modest::embedding, long, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<triqs::modest::embedding> = {nullptr, getitem_0, nullptr};

template <>
const std::string c2py::tp_doc<triqs::modest::embedding> = R"DOC(The embedding class.

The embedding class encodes the mapping between impurities and the correlated space :math:`\mathcal{C}`
(cf. ModEST implementation notes). In particular, it maps the individual "blocks" within an impurity self-energy to
a block and spin of the embedded self-energy function, which can then be upfolded (downfolded) to obtain the
lattice self-energy and the local Green's function.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<triqs::modest::embedding>;
template <> inline constexpr auto c2py::tp_name<triqs::modest::embedding::imp_block_t> = "triqs_modest.embedding.ImpBlockT";
static auto init_1 = c2py::dispatcher_c_kw_t{c2py::c_constructor<triqs::modest::embedding::imp_block_t>(),
                                             c2py::c_constructor<triqs::modest::embedding::imp_block_t, long, long, long>("n_imp", "gamma", "tau")};
template <> constexpr initproc c2py::tp_init<triqs::modest::embedding::imp_block_t> = c2py::pyfkw_constructor<init_1>;
template <>
const std::string c2py::tp_ctor_doc<triqs::modest::embedding::imp_block_t> =
   init_1.doc(R"DOC()DOC", std::vector<std::string>{}, std::vector<std::string>{});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<triqs::modest::embedding::imp_block_t>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_0 = R"DOC(Index of the impurity.)DOC";
constexpr auto doc_member_1 = R"DOC(First index :math:`\gamma` of the block in the :math:`\Sigma_{\text{imp}}`.)DOC";
constexpr auto doc_member_2 = R"DOC(Second index :math:`\tau` of the block in the :math:`\Sigma_{\text{imp}}`.)DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<triqs::modest::embedding::imp_block_t>[] = {
   c2py::getsetdef_from_member<&triqs::modest::embedding::imp_block_t::imp_idx, triqs::modest::embedding::imp_block_t>("imp_idx", doc_member_0),
   c2py::getsetdef_from_member<&triqs::modest::embedding::imp_block_t::gamma, triqs::modest::embedding::imp_block_t>("gamma", doc_member_1),
   c2py::getsetdef_from_member<&triqs::modest::embedding::imp_block_t::tau, triqs::modest::embedding::imp_block_t>("tau", doc_member_2),

   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<triqs::modest::embedding::imp_block_t> =
   R"DOC(Struct representing an impurity block.)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<triqs::modest::embedding::imp_block_t>;

// ==================== module functions ====================

// embedding_builder
static auto const fun_17 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const std::vector<std::string> &spin_names, const nda::array<std::vector<long>, 2> &block_decomposition,
                 const std::vector<long> &atom_to_imp) { return triqs::modest::embedding_builder(spin_names, block_decomposition, atom_to_imp); },
              "spin_names", "block_decomposition", "atom_to_imp"),
   c2py::cfun([](const std::vector<std::string> &spin_names, const std::vector<std::vector<long>> &block_decomposition,
                 const std::vector<long> &atom_to_imp) { return triqs::modest::embedding_builder(spin_names, block_decomposition, atom_to_imp); },
              "spin_names", "block_decomposition", "atom_to_imp")};

// make_embedding
static auto const fun_18 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::modest::local_space &C_space, bool use_atom_equivalences,
                 bool use_atom_decomp) { return triqs::modest::make_embedding(C_space, use_atom_equivalences, use_atom_decomp); },
              "C_space", "use_atom_equivalences"_a = true, "use_atom_decomp"_a = false)};

// make_embedding_with_clusters
static auto const fun_19 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](triqs::modest::one_body_elements_on_grid obe,
                 const std::vector<std::vector<long>> &atom_partition) { return triqs::modest::make_embedding_with_clusters(obe, atom_partition); },
              "obe", "atom_partition")};

static const auto doc_d_17 =
   fun_17.doc(R"DOC(
Construct the embedding class from the local space, a description of the block decomposition, and an
equivalence mapping between atom sites.

Parameters
----------
spin_names : {par_0}
   The names of the spin indices.
block_decomposition : {par_1}
   The decomposition of atomic orbitals into their irreducible representations.
atom_to_imp : {par_2}
   [optional] A mapping between equivalent atom sites.

Returns
-------
{ret_0}
   Embedding object.
)DOC",
              std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<const std::vector<std::string> &>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<const nda::array<std::vector<long>, 2> &>(),
                                                                           c2py::python_typename<const std::vector<std::vector<long>> &>()},
                                                  ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<const std::vector<long> &>()}, ", ")},
              std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::modest::embedding>()}});
static const auto doc_d_18 =
   fun_18.doc(R"DOC(
Make an embedding from the local space.

This function creates an embedding object from the local space, which is typically used in ModEST for
embedding calculations. The default behavior is to use the equivalences between different atoms when constructing
the embedding and to use the irrep decomposition of the atomic orbitals. Instead of the irrep decomposition, one
can use the atomic decomposition.

Parameters
----------
C_space : {par_0}
   The local space from a one-body elements (on grid/tight-binding).
use_atom_equivalences : {par_1}
   Use the equivalences between different atoms when constructing the embedding.
use_atom_decomp : {par_2}
   Use the atomic decomposition instead of the atomic orbital decomposition.

Returns
-------
{ret_0}
   Embedding object.
)DOC",
              std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<const triqs::modest::local_space &>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<bool>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<bool>()}, ", ")},
              std::vector<std::string>{std::vector<std::string>{c2py::python_typename<triqs::modest::embedding>()}});
static const auto doc_d_19 =
   fun_19.doc(R"DOC(
Make an embedding for clusters of atoms.

This function creates an embedding object from the one-body elements on grid (obe) and a partition of the
atoms into clusters. The clusters are defined by the `atom_partition`, which is a vector of vectors, where each
inner vector contains the indices of the atoms in the cluster.
The resulting one-body elements on grid will have the atoms permuted according to the `atom_partition`.

Parameters
----------
obe : {par_0}
   One-body elements on grid (obe) to permute.
atom_partition : {par_1}
   New partition of the atoms into clusters, where each inner vector contains the indices of the
   atoms in the cluster.

Returns
-------
{ret_0}
   A new one-body elements on grid and the corresponding embedding object.
)DOC",
              std::vector<std::string>{c2py::join(std::vector<std::string>{c2py::python_typename<triqs::modest::one_body_elements_on_grid>()}, ", "),
                                       c2py::join(std::vector<std::string>{c2py::python_typename<const std::vector<std::vector<long>> &>()}, ", ")},
              std::vector<std::string>{
                 std::vector<std::string>{c2py::python_typename<std::pair<triqs::modest::one_body_elements_on_grid, triqs::modest::embedding>>()}});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"embedding_builder", (PyCFunction)c2py::pyfkw<fun_17>, METH_VARARGS | METH_KEYWORDS, doc_d_17.c_str()},
   {"make_embedding", (PyCFunction)c2py::pyfkw<fun_18>, METH_VARARGS | METH_KEYWORDS, doc_d_18.c_str()},
   {"make_embedding_with_clusters", (PyCFunction)c2py::pyfkw<fun_19>, METH_VARARGS | METH_KEYWORDS, doc_d_19.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "embedding", /* name of module */
                                        R"RAWDOC(The embedding abstraction in ModEST. 

The **embedding** object is the central controller of the DMFT problem: it defines how the correlated subspace 
(:math:`\mathcal{C}`) is partitioned into impurities and handles the bookkeeping needed to map back and forth between 
the lattice and the impurities.

Typical use cases covered by embedding:

- **Equivalent atoms**: Use the same impurity model (i.e. same :math:`n_{\mathrm{imp}}`) for different blocks 
  :math:`\alpha`.
- **Magnetic order**: Just reverse the spin for some atoms, :math:`\tau(\sigma) = 1-\sigma`.
- **Splitting impurity models** : Depending on the block structure of the problem, the corresponding impurity model 
  connected to atom :math:`a` can be split. Different impurity solvers can then be used for each of these split impurity 
  models. The self-energies from both models map back to the a single atom :math:`a`. Or a trivial solver is used to 
  obtain zero self-energy (drop).
- **cluster DMFT**: Re-group a subset of atoms :math:`a` into a larger super atom which will be treated as a cluster 
  DMFT problem.

The embedding object provides:

* Factory functions to conveniently construct common embeddings.
* Operations to adapt and modify an embedding (e.g., split, drop, etc.).
* Methods to map data between impurity and embed spaces.

This abstrction simplifies the DMFT loop, making it easy to construct arbitrary embedding scenarios.
)RAWDOC",                                            /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_embedding() {

  if (not c2py::check_python_version("embedding")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::embedding>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<triqs::modest::embedding::imp_block_t>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
  c2py::add_type_object_to_main<triqs::modest::embedding>("Embedding", m, conv_table);
  c2py::add_type_object_to_main<triqs::modest::embedding::imp_block_t>("ImpBlockT", m, conv_table);

  return m;
}
#endif
// CLAIR_WRAP_GEN
