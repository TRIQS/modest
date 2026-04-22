
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
using _c2py_cls_0                                            = triqs::modest::embedding;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs_modest.embedding.Embedding";
static auto _c2py_init_0 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_0, std::vector<long>, std::vector<std::vector<long>>,
                                               nda::basic_array<triqs::modest::embedding::imp_block_t, 2, nda::C_layout, 'A',
                                                                nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
                                               std::vector<std::string>>("sigma_embed_decomposition", "imp_decompositions", "psi", "sigma_names"),
                           c2py::c_constructor<_c2py_cls_0>()};
template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> =
   _c2py_init_0.doc(R"DOC(
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
                    {{c2py::python_typename<std::vector<long>>()},
                     {c2py::python_typename<std::vector<std::vector<long>>>()},
                     {c2py::python_typename<nda::basic_array<triqs::modest::embedding::imp_block_t, 2, nda::C_layout, 'A',
                                                             nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()},
                     {c2py::python_typename<std::vector<std::string>>()}});
// description
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_0 const &self, bool verbosity) -> decltype(auto) { return self.description(verbosity); }, "self", "verbosity"_a = false)};

// drop
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 const &self, long imp_idx) -> decltype(auto) { return self.drop(imp_idx); }, "self", "imp_idx")};

// embed
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_0 const &self,
         const std::vector<std::vector<
            nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>
            &imps_blocks) -> decltype(auto) { return self.template embed<2>(imps_blocks); },
      "self", "imps_blocks"),
   c2py::cmethod(
      [](_c2py_cls_0 const &self,
         const std::vector<std::vector<
            nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>
            &imps_blocks) -> decltype(auto) { return self.template embed<3>(imps_blocks); },
      "self", "imps_blocks"),
   c2py::cmethod(
      [](_c2py_cls_0 const &self,
         const std::vector<std::vector<
            nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>
            &imps_blocks) -> decltype(auto) { return self.template embed<4>(imps_blocks); },
      "self", "imps_blocks"),
   c2py::cmethod(
      [](_c2py_cls_0 const &self,
         const std::vector<std::vector<
            nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>
            &imps_blocks) -> decltype(auto) { return self.template embed<5>(imps_blocks); },
      "self", "imps_blocks"),
   c2py::cmethod([](_c2py_cls_0 const &self, const std::vector<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>> &Sigma_imp_vec)
                    -> decltype(auto) { return self.template embed<triqs::mesh::imfreq>(Sigma_imp_vec); },
                 "self", "Sigma_imp_vec"),
   c2py::cmethod([](_c2py_cls_0 const &self, const std::vector<triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>> &Sigma_imp_vec)
                    -> decltype(auto) { return self.template embed<triqs::mesh::refreq>(Sigma_imp_vec); },
                 "self", "Sigma_imp_vec"),
   c2py::cmethod(
      [](_c2py_cls_0 const &self, const std::vector<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>> &Sigma_imp_vec)
         -> decltype(auto) { return self.template embed<triqs::mesh::dlr_imfreq>(Sigma_imp_vec); },
      "self", "Sigma_imp_vec"),
   c2py::cmethod([](_c2py_cls_0 const &self, const std::vector<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>> &Sigma_imp_vec,
                    const std::vector<triqs::block_matrix_t> &Sigma_imp_static_vec)
                    -> decltype(auto) { return self.template embed<triqs::mesh::imfreq>(Sigma_imp_vec, Sigma_imp_static_vec); },
                 "self", "Sigma_imp_vec", "Sigma_imp_static_vec"),
   c2py::cmethod([](_c2py_cls_0 const &self, const std::vector<triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>> &Sigma_imp_vec,
                    const std::vector<triqs::block_matrix_t> &Sigma_imp_static_vec)
                    -> decltype(auto) { return self.template embed<triqs::mesh::refreq>(Sigma_imp_vec, Sigma_imp_static_vec); },
                 "self", "Sigma_imp_vec", "Sigma_imp_static_vec"),
   c2py::cmethod(
      [](_c2py_cls_0 const &self, const std::vector<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>> &Sigma_imp_vec,
         const std::vector<triqs::block_matrix_t> &Sigma_imp_static_vec) -> decltype(auto) {
        return self.template embed<triqs::mesh::dlr_imfreq>(Sigma_imp_vec, Sigma_imp_static_vec);
      },
      "self", "Sigma_imp_vec", "Sigma_imp_static_vec"),
   c2py::cmethod([](_c2py_cls_0 const &self,
                    const std::vector<triqs::block_matrix_t> &Sigma_imp_static_vec) -> decltype(auto) { return self.embed(Sigma_imp_static_vec); },
                 "self", "Sigma_imp_static_vec")};

// extract
static auto const _c2py_fun_3 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_0 const &self,
         const std::vector<
            nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &X)
         -> decltype(auto) { return self.template extract<2>(X); },
      "self", "X"),
   c2py::cmethod(
      [](_c2py_cls_0 const &self,
         const std::vector<
            nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &X)
         -> decltype(auto) { return self.template extract<3>(X); },
      "self", "X"),
   c2py::cmethod(
      [](_c2py_cls_0 const &self,
         const std::vector<
            nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &X)
         -> decltype(auto) { return self.template extract<4>(X); },
      "self", "X"),
   c2py::cmethod(
      [](_c2py_cls_0 const &self,
         const std::vector<
            nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &X)
         -> decltype(auto) { return self.template extract<5>(X); },
      "self", "X"),
   c2py::cmethod([](_c2py_cls_0 const &self, const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &g_loc)
                    -> decltype(auto) { return self.template extract<triqs::mesh::imfreq>(g_loc); },
                 "self", "g_loc"),
   c2py::cmethod([](_c2py_cls_0 const &self, const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &g_loc)
                    -> decltype(auto) { return self.template extract<triqs::mesh::refreq>(g_loc); },
                 "self", "g_loc"),
   c2py::cmethod([](_c2py_cls_0 const &self, const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &g_loc)
                    -> decltype(auto) { return self.template extract<triqs::mesh::dlr_imfreq>(g_loc); },
                 "self", "g_loc"),
   c2py::cmethod([](_c2py_cls_0 const &self, const triqs::block2_matrix_t &matrix_C) -> decltype(auto) { return self.extract(matrix_C); }, "self",
                 "matrix_C")};

// flip_spin
static auto const _c2py_fun_4 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 const &self, long alpha) -> decltype(auto) { return self.flip_spin(alpha); }, "self", "alpha"),
   c2py::cmethod([](_c2py_cls_0 const &self, std::vector<long> alphas) -> decltype(auto) { return self.flip_spin(alphas); }, "self", "alphas")};

// imp_decomposition
static auto const _c2py_fun_5 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 const &self, long imp_idx) -> decltype(auto) { return self.imp_decomposition(imp_idx); }, "self", "imp_idx")};

// make_zero_imp_self_energies
static auto const _c2py_fun_6 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 &self, const triqs::mesh::imfreq &mesh)
                                            -> decltype(auto) { return self.template make_zero_imp_self_energies<triqs::mesh::imfreq>(mesh); },
                                         "self", "mesh"),
                           c2py::cmethod([](_c2py_cls_0 &self, const triqs::mesh::refreq &mesh)
                                            -> decltype(auto) { return self.template make_zero_imp_self_energies<triqs::mesh::refreq>(mesh); },
                                         "self", "mesh"),
                           c2py::cmethod([](_c2py_cls_0 &self, const triqs::mesh::dlr_imfreq &mesh)
                                            -> decltype(auto) { return self.template make_zero_imp_self_energies<triqs::mesh::dlr_imfreq>(mesh); },
                                         "self", "mesh")};

// n_gamma
static auto const _c2py_fun_7 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 const &self, long imp_idx) -> decltype(auto) { return self.n_gamma(imp_idx); }, "self", "imp_idx")};

// replace
static auto const _c2py_fun_8 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 const &self, long imp_idx_to_remove,
                    long imp_idx_to_replace_with) -> decltype(auto) { return self.replace(imp_idx_to_remove, imp_idx_to_replace_with); },
                 "self", "imp_idx_to_remove", "imp_idx_to_replace_with")};

// split
static auto const _c2py_fun_9 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 const &self, long imp_idx, std::function<bool(long)> p) -> decltype(auto) { return self.split(imp_idx, p); }, "self",
                 "imp_idx", "p"),
   c2py::cmethod(
      [](_c2py_cls_0 const &self, long imp_idx, const std::vector<long> &block_list) -> decltype(auto) { return self.split(imp_idx, block_list); },
      "self", "imp_idx", "block_list")};

// split_block
static auto const _c2py_fun_10 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 const &self, long imp_idx, long gamma,
                    const std::vector<long> &new_dims) -> decltype(auto) { return self.split_block(imp_idx, gamma, new_dims); },
                 "self", "imp_idx", "gamma", "new_dims")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC(
Summarize the embedding object.
)DOC");
static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC(
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
                                                {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::modest::embedding>()});
static const auto _c2py_doc_2 = _c2py_fun_2.doc(
   R"DOC(
[1, 2, 3, 4] Embed impurity data into the full correlated space.

This method maps impurity solver data into the embedded representation using the
:math:`\psi` mapping table. Given data from each impurity solver organized by blocks
:math:`(\gamma, \tau)`, the method constructs the full embedded arrays indexed by
:math:`(\alpha, \sigma)`.

The algorithm proceeds as follows:
1. Initialize zero arrays for each :math:`(\alpha, \sigma)` block with dimensions from
   `sigma_embed_decomp`.
2. For each :math:`(\alpha, \sigma)`, look up :math:`\psi(\alpha, \sigma) = (n\_imp, \gamma, \tau)`
   and copy the corresponding impurity block data.
3. Assemble the blocks into full diagonal tensors for each spin channel :math:`\sigma`.

Blocks with `imp_idx == -1` (disconnected from any impurity) remain zero.

The template parameter `Rank` determines the dimensionality of the data arrays. For
frequency-dependent quantities (Rank ∈ {3,5}), the first dimension is the frequency index.

------

[5, 6, 7] Embed Green's function containers.

Embed impurity solver self-energies into an embedded self-energy.

* :math:`\Sigma_{\text{embed}}^{ασ} = \Sigma_{\text{imp}}^{ψ(α,σ)}`,
* :math:`\psi(\alpha,\sigma) \to` (n_imp, :math:`\gamma`, :math:`\tau`).

------

[8, 9, 10] Embed pairs Green's function containers and matrices.

Embed the impurity self-energy decomposed as a list of dynamic and a list of static parts.

------

[11] Embed block matrices into the full correlated space.

Embeds static (frequency-independent) impurity data stored as block matrices
into the :math:`(\alpha, \sigma)`-indexed embedded representation. This is the matrix
analogue of the frequency-dependent embed operation.

------

Parameters
----------
imps_blocks : {par_0}
   Nested vector [n_imp][block] of impurity data arrays.
Sigma_imp_vec : {par_1}
   A list of impurity self-energies.
Sigma_imp_static_vec : {par_2}
   A list of static impurity self-energies.

Returns
-------
[1] : {ret_0}
   Vector of embedded arrays, one per spin channel :math:`\sigma`.

[2] : {ret_1}
   Vector of embedded arrays, one per spin channel :math:`\sigma`.

[3] : {ret_2}
   Vector of embedded arrays, one per spin channel :math:`\sigma`.

[4] : {ret_3}
   Vector of embedded arrays, one per spin channel :math:`\sigma`.

[5] : {ret_4}
   :math:`\Sigma_{\text{embed}}[\alpha, \sigma]` in :math:`\mathcal{C}` space using the embed decomposition.

[6] : {ret_5}
   :math:`\Sigma_{\text{embed}}[\alpha, \sigma]` in :math:`\mathcal{C}` space using the embed decomposition.

[7] : {ret_6}
   :math:`\Sigma_{\text{embed}}[\alpha, \sigma]` in :math:`\mathcal{C}` space using the embed decomposition.

[8] : {ret_7}
   The self-energy in the :math:`\mathcal{C}` space as a pair (dynamic, static).

[9] : {ret_8}
   The self-energy in the :math:`\mathcal{C}` space as a pair (dynamic, static).

[10] : {ret_9}
   The self-energy in the :math:`\mathcal{C}` space as a pair (dynamic, static).

[11] : {ret_10}
   Embedded block2 matrix in :math:`\mathcal{C}` space.
)DOC",
   {{c2py::python_typename<const std::vector<std::vector<
        nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &>(),
     c2py::python_typename<const std::vector<std::vector<
        nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &>(),
     c2py::python_typename<const std::vector<std::vector<
        nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &>(),
     c2py::python_typename<const std::vector<std::vector<
        nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &>()},
    {c2py::python_typename<const std::vector<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>> &>(),
     c2py::python_typename<const std::vector<triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>> &>(),
     c2py::python_typename<const std::vector<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>> &>()},
    {c2py::python_typename<const std::vector<triqs::block_matrix_t> &>()}},
   {c2py::python_typename<std::vector<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>(),
    c2py::python_typename<std::vector<
       nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>(),
    c2py::python_typename<std::vector<
       nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>(),
    c2py::python_typename<std::vector<
       nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>(),
    c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2>>(),
    c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_layout, 2>>(),
    c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2>>(),
    c2py::python_typename<
       std::pair<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2>, triqs::block2_matrix_t>>(),
    c2py::python_typename<
       std::pair<triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_layout, 2>, triqs::block2_matrix_t>>(),
    c2py::python_typename<
       std::pair<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2>, triqs::block2_matrix_t>>(),
    c2py::python_typename<triqs::block2_matrix_t>()});
static const auto _c2py_doc_3 = _c2py_fun_3.doc(
   R"DOC(
[1, 2, 3, 4] Extract impurity data from embedded arrays.

This method performs the inverse of the embed operation: it takes data in the full correlated
space and extracts the individual impurity blocks using the reverse mapping :math:`\psi^{-1}`.

The algorithm proceeds in two steps:
1. For each :math:`(\alpha, \sigma)` pair, extract the diagonal block from the input array `X`
   using the `sigma_embed_decomp` decomposition.
2. For each impurity, use the `reverse_psi` mapping to gather the appropriate blocks into
   the impurity's block structure, indexed by :math:`(\gamma, \tau)`.

The template parameter `Rank` determines the dimensionality of the underlying data arrays.
For frequency-dependent quantities (Rank ∈ {3,5}), the first dimension is interpreted as the
frequency index.

------

[5, 6, 7] Extract Green's function containers.

------

[8] Extract block matrices from the embedded representation.

Extracts static (frequency-independent) impurity data from the embedded
:math:`(\alpha, \sigma)`-indexed representation back into per-impurity block matrices.
This is the inverse of the block matrix embed operation.

------

Parameters
----------
X : {par_0}
   Vector of arrays in the embedded space, one per spin channel :math:`\sigma`.
g_loc : {par_1}
   Block2Gf of gloc in :math:`M \times M` space.
matrix_C : {par_2}
   Embedded block2 matrix in :math:`\mathcal{C}` space.

Returns
-------
[1] : {ret_0}
   Nested vector [n_imp][block] of extracted impurity data arrays.

[2] : {ret_1}
   Nested vector [n_imp][block] of extracted impurity data arrays.

[3] : {ret_2}
   Nested vector [n_imp][block] of extracted impurity data arrays.

[4] : {ret_3}
   Nested vector [n_imp][block] of extracted impurity data arrays.

[5] : {ret_4}
   Local impurity Green's function.

[6] : {ret_5}
   Local impurity Green's function.

[7] : {ret_6}
   Local impurity Green's function.

[8] : {ret_7}
   List of block matrices, one per impurity.
)DOC",
   {{c2py::python_typename<const std::vector<
        nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &>(),
     c2py::python_typename<const std::vector<
        nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &>(),
     c2py::python_typename<const std::vector<
        nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &>(),
     c2py::python_typename<const std::vector<
        nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &>()},
    {c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>(),
     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>(),
     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_layout, 2> &>()},
    {c2py::python_typename<const triqs::block2_matrix_t &>()}},
   {c2py::python_typename<std::vector<std::vector<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>>(),
    c2py::python_typename<std::vector<std::vector<
       nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>>(),
    c2py::python_typename<std::vector<std::vector<
       nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>>(),
    c2py::python_typename<std::vector<std::vector<
       nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>>(),
    c2py::python_typename<std::vector<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>>>(),
    c2py::python_typename<std::vector<triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>>>(),
    c2py::python_typename<std::vector<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>>>(),
    c2py::python_typename<std::vector<triqs::block_matrix_t>>()});
static const auto _c2py_doc_4 = _c2py_fun_4.doc(R"DOC(
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
                                                {{c2py::python_typename<long>()}, {c2py::python_typename<std::vector<long>>()}},
                                                {c2py::python_typename<triqs::modest::embedding>()});
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC(
The impurity decomposition.
)DOC");
static const auto _c2py_doc_6 = _c2py_fun_6.doc(
   R"DOC(
Create zero-initialized impurity self-energies.

Constructs a list of zero-initialized self-energy containers for each impurity,
with both dynamic (frequency-dependent) and static components. The block structure of each
impurity's self-energy is determined by `imp_block_structure()`.

Each impurity receives a pair consisting of:
- A `block_gf` initialized to zero on the provided mesh (dynamic part).
- A `block_matrix_t` of zero matrices matching the block dimensions (static part).

Parameters
----------
mesh : {par_0}
   The frequency mesh for the dynamic self-energy.

Returns
-------
[1] : {ret_0}
   List of (dynamic, static) self-energy pairs, one per impurity.

[2] : {ret_1}
   List of (dynamic, static) self-energy pairs, one per impurity.

[3] : {ret_2}
   List of (dynamic, static) self-energy pairs, one per impurity.
)DOC",
   {{c2py::python_typename<const triqs::mesh::imfreq &>(), c2py::python_typename<const triqs::mesh::refreq &>(),
     c2py::python_typename<const triqs::mesh::dlr_imfreq &>()}},
   {c2py::python_typename<std::vector<std::pair<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>, triqs::block_matrix_t>>>(),
    c2py::python_typename<std::vector<std::pair<triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>, triqs::block_matrix_t>>>(),
    c2py::python_typename<
       std::vector<std::pair<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>, triqs::block_matrix_t>>>()});
static const auto _c2py_doc_7 = _c2py_fun_7.doc(R"DOC(
Number of blocks in :math:`\gamma` for the :math:`\Sigma_{\text{imp}}` [imp_idx].
)DOC");
static const auto _c2py_doc_8 =
   _c2py_fun_8.doc(R"DOC(
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
                   {{c2py::python_typename<long>()}, {c2py::python_typename<long>()}}, {c2py::python_typename<triqs::modest::embedding>()});
static const auto _c2py_doc_9 = _c2py_fun_9.doc(
   R"DOC(
[1] Split impurity `imp_idx` using a predicate.

Partitions the blocks of impurity `imp_idx` into two new impurities based on
predicate `p`. Blocks for which `p(block_idx)` returns true stay with `imp_idx`;
the rest are assigned to a new impurity inserted at `imp_idx + 1`.

------

[2] Split impurity `imp_idx` based on `block_list`.

For impurity `imp_idx`, split part of its blocks into a new impurity.
The blocks that were split off remain mapped to their original alpha blocks with the updated `imp_idx`.

------

Parameters
----------
imp_idx : {par_0}
   Impurity number.
p : {par_1}
   Predicate taking a block index and returning true/false.
block_list : {par_2}
   A list of blocks of `imp_idx` to split off.

Returns
-------
{ret_0}
   New embedding with the updated :math:`\psi` map.
)DOC",
   {{c2py::python_typename<long>()}, {c2py::python_typename<std::function<bool(long)>>()}, {c2py::python_typename<const std::vector<long> &>()}},
   {c2py::python_typename<triqs::modest::embedding>()});
static const auto _c2py_doc_10 =
   _c2py_fun_10.doc(R"DOC(
Split the block (gamma) of an impurity (imp_idx) into multiple blocks with dimensions given by `new_dims`.

This method splits a specific block (gamma) of an impurity (imp_idx) into multiple blocks with dimensions
specified by `new_dims`. The sum of `new_dims` must equal the original dimension of the block. For example, if the
original block has a dimension of 5, it could be split into blocks of dimensions 2 (eg) and 3 (t2g).

Parameters
----------
imp_idx : {par_0}
   The index of the impurity to split.
gamma : {par_1}
   The block index of the impurity to split.
new_dims : {par_2}
   The new dimensions of the split blocks. The sum of `new_dims` must equal the original dimension of the block.

Returns
-------
{ret_0}
   New embedding with the impurity structure and :math:`\psi` mapping.
)DOC",
                    {{c2py::python_typename<long>()}, {c2py::python_typename<long>()}, {c2py::python_typename<const std::vector<long> &>()}},
                    {c2py::python_typename<triqs::modest::embedding>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {
   {"description", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"drop", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"embed", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"extract", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {"flip_spin", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"imp_decomposition", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_5.c_str()},
   {"make_zero_imp_self_energies", (PyCFunction)c2py::pyfkw<_c2py_fun_6>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_6.c_str()},
   {"n_gamma", (PyCFunction)c2py::pyfkw<_c2py_fun_7>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_7.c_str()},
   {"replace", (PyCFunction)c2py::pyfkw<_c2py_fun_8>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_8.c_str()},
   {"split", (PyCFunction)c2py::pyfkw<_c2py_fun_9>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_9.c_str()},
   {"split_block", (PyCFunction)c2py::pyfkw<_c2py_fun_10>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_10.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0 = R"DOC(Block structure (names and dimensions) for each impurity solver.)DOC";
static constexpr auto prop_doc_1 = R"DOC(Create the two-particle embedding from a single-particle embedding.

Merges consecutive :math:`\alpha` blocks that belong to the same atom into a
single block.  Atom boundaries are detected by monitoring the :math:`\gamma` index: within
one atom the :math:`\gamma` values are strictly increasing; when :math:`\gamma` resets or
the impurity index changes a new group is started.  Disconnected blocks (``imp_idx`` == -1)
are never merged and pass through unchanged.)DOC";
static constexpr auto prop_doc_2 = R"DOC(Convert the embedding to a spinless embedding.

Converts a spinful embedding (with two spin channels, e.g., "up" and "down") into a
single-channel embedding labelled "ud". This is useful when working with systems with
spin-orbit coupling or spin-averaged quantities, like the density-density susceptibility.)DOC";
static constexpr auto prop_doc_3 = R"DOC(Number of blocks in :math:`\alpha` for the :math:`\Sigma_{\text{embed}}`.)DOC";
static constexpr auto prop_doc_4 = R"DOC(Number of impurities.)DOC";
static constexpr auto prop_doc_5 = R"DOC(Number of blocks in :math:`\sigma` for the :math:`\Sigma_{\text{embed}}`.)DOC";
static constexpr auto prop_doc_6 = R"DOC(The mapping table :math:`\psi`.)DOC";
static constexpr auto prop_doc_7 = R"DOC(The names of the sigma indices.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_0>[] = {

   {"imp_block_structure", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::imp_block_structure)>, nullptr, prop_doc_0, nullptr},
   {"make_2particle", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::make_2particle)>, nullptr, prop_doc_1, nullptr},
   {"make_spinless", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::make_spinless)>, nullptr, prop_doc_2, nullptr},
   {"n_alpha", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::n_alpha)>, nullptr, prop_doc_3, nullptr},
   {"n_impurities", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::n_impurities)>, nullptr, prop_doc_4, nullptr},
   {"n_sigma", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::n_sigma)>, nullptr, prop_doc_5, nullptr},
   {"psi_map", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::psi_map)>, nullptr, prop_doc_6, nullptr},
   {"sigma_names", c2py::getter_from_method<c2py::castmc<>(&triqs::modest::embedding::sigma_names)>, nullptr, prop_doc_7, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_0(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_0, long, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_0> = {nullptr, getitem_0, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC(The embedding class.

The embedding class encodes the mapping between impurities and the correlated space :math:`\mathcal{C}`
(cf. ModEST implementation notes). In particular, it maps the individual "blocks" within an impurity self-energy to
a block and spin of the embedded self-energy function, which can then be upfolded (downfolded) to obtain the
lattice self-energy and the local Green's function.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1                                            = triqs::modest::embedding::imp_block_t;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs_modest.embedding.ImpBlockT";
static auto _c2py_init_1 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_1>(), c2py::c_constructor<_c2py_cls_1, long, long, long>("n_imp", "gamma", "tau")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_1>    = c2py::pyfkw_constructor<_c2py_init_1>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_1> = _c2py_init_1.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_0 = R"DOC(Index of the impurity.)DOC";
constexpr auto _c2py_doc_member_1 = R"DOC(First index :math:`\gamma` of the block in the :math:`\Sigma_{\text{imp}}`.)DOC";
constexpr auto _c2py_doc_member_2 = R"DOC(Second index :math:`\tau` of the block in the :math:`\Sigma_{\text{imp}}`.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_1>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_1::imp_idx, _c2py_cls_1>("imp_idx", _c2py_doc_member_0),
   c2py::getsetdef_from_member<&_c2py_cls_1::gamma, _c2py_cls_1>("gamma", _c2py_doc_member_1),
   c2py::getsetdef_from_member<&_c2py_cls_1::tau, _c2py_cls_1>("tau", _c2py_doc_member_2),

   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_1> =
   R"DOC(Struct representing an impurity block.)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_1>;

// ==================== module functions ====================

// make_embedding
static auto const _c2py_fun_11 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const std::vector<std::string> &spin_names,
                 const nda::basic_array<std::vector<long, std::allocator<long>>, 2, nda::C_layout, 'A',
                                        nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &block_decomposition,
                 const std::vector<long> &atom_to_imp) { return triqs::modest::make_embedding(spin_names, block_decomposition, atom_to_imp); },
              "spin_names", "block_decomposition", "atom_to_imp"),
   c2py::cfun([](const std::vector<std::string> &spin_names, const std::vector<std::vector<long>> &block_decomposition,
                 const std::vector<long> &atom_to_imp) { return triqs::modest::make_embedding(spin_names, block_decomposition, atom_to_imp); },
              "spin_names", "block_decomposition", "atom_to_imp"),
   c2py::cfun([](const triqs::modest::local_space &C_space, bool use_atom_equivalences,
                 bool use_atom_decomp) { return triqs::modest::make_embedding(C_space, use_atom_equivalences, use_atom_decomp); },
              "C_space", "use_atom_equivalences"_a = true, "use_atom_decomp"_a = false)};

// make_embedding_with_clusters
static auto const _c2py_fun_12 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](triqs::modest::one_body_elements_on_grid obe,
                 const std::vector<std::vector<long>> &atom_partition) { return triqs::modest::make_embedding_with_clusters(obe, atom_partition); },
              "obe", "atom_partition")};

static const auto _c2py_doc_11 =
   _c2py_fun_11.doc(R"DOC(
[1, 2] Construct an embedding from spin names, a block decomposition per atom, and an atom-to-impurity mapping.

------

[3] Make an embedding from the local space.

This function creates an embedding object from the local space, which is typically used in ModEST for
embedding calculations. The default behavior is to use the equivalences between different atoms when constructing
the embedding and to use the irrep decomposition of the atomic orbitals. Instead of the irrep decomposition, one
can use the atomic decomposition.

------

Parameters
----------
spin_names : {par_0}
   The names of the spin indices (e.g., {"up", "down"}).
block_decomposition : {par_1}
   Block sizes for each atom: block_decomposition(atom, sigma) is a vector of block sizes.
atom_to_imp : {par_2}
   Mapping from atom index to impurity index. Atoms with the same value share a solver.
C_space : {par_3}
   The local space from a one-body elements (on grid/tight-binding).
use_atom_equivalences : {par_4}
   Use the equivalences between different atoms when constructing the embedding.
use_atom_decomp : {par_5}
   Use the atomic decomposition instead of the atomic orbital decomposition.

Returns
-------
{ret_0}
   Embedding object.
)DOC",
                    {{c2py::python_typename<const std::vector<std::string> &>()},
                     {c2py::python_typename<const nda::basic_array<std::vector<long, std::allocator<long>>, 2, nda::C_layout, 'A',
                                                                   nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()},
                     {c2py::python_typename<const std::vector<long> &>()},
                     {c2py::python_typename<const triqs::modest::local_space &>()},
                     {c2py::python_typename<bool>()},
                     {c2py::python_typename<bool>()}},
                    {c2py::python_typename<triqs::modest::embedding>()});
static const auto _c2py_doc_12 = _c2py_fun_12.doc(
   R"DOC(
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
   {{c2py::python_typename<triqs::modest::one_body_elements_on_grid>()}, {c2py::python_typename<const std::vector<std::vector<long>> &>()}},
   {c2py::python_typename<std::pair<triqs::modest::one_body_elements_on_grid, triqs::modest::embedding>>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"make_embedding", (PyCFunction)c2py::pyfkw<_c2py_fun_11>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_11.c_str()},
   {"make_embedding_with_clusters", (PyCFunction)c2py::pyfkw<_c2py_fun_12>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_12.c_str()},
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
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_0>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_1>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_0, "Embedding");
  _add_type(_c2py_cls_1, "ImpBlockT");
#undef _add_type

  return m;
}
#endif
// CLAIR_WRAP_GEN
