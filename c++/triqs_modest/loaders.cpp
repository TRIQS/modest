#include "loaders.hpp"
#include "dft_code_specific.hpp"
#include "utils/nda_supp.hpp"
#include "utils/h5_proxy.hpp"
#include "utils/to_vector.hpp"
#include "utils/graph_algo.hpp"

namespace triqs::modest {
  /// utility to flatten a nested vector (move to utils/ ?)

  /// free function for svd (FIXME: nda::linalg::svd) (see TRIQS/nda PR#103)
  nda::matrix<dcomplex> svd(const nda::matrix<dcomplex> &A) {
    auto Acopy = nda::matrix<dcomplex, nda::F_layout>{transpose(A)};
    long m     = Acopy.extent(0);
    // auto S     = nda::vector<double>(m);
    auto U                       = nda::matrix<dcomplex, nda::F_layout>(m, m);
    auto VT                      = nda::matrix<dcomplex, nda::F_layout>(m, m);
    [[maybe_unused]] auto status = nda::lapack::gesvd(Acopy, nda::vector<double>(m), U, VT);
    return nda::matrix<dcomplex>{transpose(U * VT)};
  }

  namespace detail {
    /// inject an object T from the C space to W space (used in the context of post-processing).
    nda::array<std::vector<long>, 2> inject_to_new_space(nda::array<std::vector<long>, 2> const &T, std::vector<atomic_shell_t> const &old_space,
                                                         std::vector<atomic_shell_t> const &new_space) {
      auto n_atoms = new_space.size();
      auto n_sigma = T.extent(1);
      auto Tembed  = nda::array<std::vector<long>, 2>(n_atoms, n_sigma);

      auto old_indices = old_space | stdv::transform([](auto &x) { return x.dft_idx; }) | tl::to<std::vector>();
      auto new_indices = new_space | stdv::transform([](auto &x) { return x.dft_idx; }) | tl::to<std::vector>();

      for (auto const &[n, nidx] : enumerate(new_indices)) {
        if (auto it = std::find(begin(old_indices), end(old_indices), nidx); it != end(old_indices)) {
          auto uidx        = std::distance(begin(old_indices), it);
          Tembed(n, r_all) = T(uidx, r_all);
        } else {
          Tembed(n, r_all) = std::vector<long>{long(new_space[n].dim)};
        }
      }
      return Tembed;
    }
  } // namespace detail

  //-------------------------------------------------------
  /// Disovers (approximate) irreducible symmetries for Green's function from the non-interacting part of the local
  /// Hamiltonian (H0 = ∑k P(k) Hνν' P†(k) ), which represents the block structure of the TRIQS Gf.
  std::pair<nda::array<std::vector<long>, 2>, nda::array<nda::matrix<dcomplex>, 2>>
  discover_symmetries(nda::array<nda::matrix<dcomplex>, 2> const &Hloc0, std::vector<atomic_shell_t> const &atomic_shells, double block_threshold,
                      bool diagonalize_hloc) {

    auto [n_atoms, n_sigma] = Hloc0.shape();
    auto decomposition      = nda::array<std::vector<long>, 2>(n_atoms, n_sigma);
    auto U_rotation         = nda::array<nda::matrix<dcomplex>, 2>(n_atoms, n_sigma);

    for (auto const &[atom, shell] : enumerate(atomic_shells)) {
      for (auto sigma : nda::range(n_sigma)) {
        auto D                     = find_blocks(nda::matrix<double>{abs(Hloc0(atom, sigma))}, block_threshold);
        U_rotation(atom, sigma)    = nda::make_matrix_from_permutation<dcomplex>(nda::flatten(D));
        decomposition(atom, sigma) = D | stdv::transform([](auto &x) { return long(x.size()); }) | tl::to<std::vector>();

        // diagonalize the hloc0 to find a new basis of orbitals
        if (diagonalize_hloc) {
          auto U_diag = nda::matrix<dcomplex>(shell.dim, shell.dim);
          for (auto &&[idx, r_idx] : enumerated_sub_slices(decomposition(atom, sigma))) {
            auto hperm           = dagger(U_rotation(atom, sigma)) * Hloc0(atom, sigma) * U_rotation(atom, sigma);
            U_diag(r_idx, r_idx) = std::get<1>(nda::linalg::eigenelements(hperm(r_idx, r_idx)));
          }
          U_rotation(atom, sigma) *= U_diag;
        }
      }
    }
    return {decomposition, U_rotation};
  }

  //-------------------------------------------------------
  /// Enumerate the different reading modes for the obe factory functions.
  enum class ReadMode {
    Correlated,      // reads bands, projectors, and correlated shells from "dft_input"
    ThetaProjectors, // read bands ands shells from "dft_input" and projectors from "dft_parproj_input" ("proj_mat_all")
    Bands            // read bands and projectors from "dft_bands_input"
  };

  //-------------------------------------------------------
  /// Read rotation matrices from hdf5. (internal)
  std::vector<cmat_t> read_rotation_matrices(std::string const &filename, ReadMode mode) {

    auto root = h5::proxy{filename, 'r'};
    auto SO   = long(root["dft_input"]["SO"]);

    auto read_mats = [SO](auto group, auto name1, auto name2) {
      auto mats             = to_vector<cmat_t>(sort_keys_as_int(group[name1]));
      auto mats_tinv        = to_vector<long>(sort_keys_as_int(group[name2]));
      auto mats_tinv_and_SO = mats_tinv | stdv::transform([SO](long const &x) { return (x == 1 && SO == 1) ? 1 : 0; }) | tl::to<std::vector<long>>();

      // NB: mat = D(R_{\alpaha})
      // if mat has time inversion symmetry (T), then T*D(R_{\alpha}) = -D(R_{\alpha})
      return mats
         | stdv::transform([mats_tinv_and_SO, i = 0](cmat_t const &x) mutable { return (mats_tinv_and_SO[i++] == 1) ? -conj(svd(x)) : svd(x); })
         | tl::to<std::vector<cmat_t>>();
    };
    return (mode == ReadMode::Correlated)  ? read_mats(root["dft_input"], "rot_mat", "rot_mat_time_inv") :
       (mode == ReadMode::ThetaProjectors) ? read_mats(root["dft_parproj_input"], "rot_mat_all", "rot_mat_all_time_inv") :
                                             throw std::runtime_error("This should not happen!");
  }

  //-------------------------------------------------------
  /// Read projectors using ReadMode, rotate to local frame, and embed them in the M space. (internal)
  nda::array<dcomplex, 4> load_rotate_and_format_projectors(std::string const &filename, ReadMode mode, std::vector<cmat_t> const &rot_mats,
                                                            std::vector<long> const &atom_decomp) {
    auto load_Pks = [](auto f, auto m) {
      auto root = h5::proxy{f, 'r'};
      if (m == ReadMode::Correlated || m == ReadMode::Bands) {
        return (m == ReadMode::Correlated) ? as<nda::array<dcomplex, 5>>(root["dft_input"]["proj_mat"]) :
                                             as<nda::array<dcomplex, 5>>(root["dft_bands_input"]["proj_mat"]);
      } else if (m == ReadMode::ThetaProjectors) {
        auto tmp = as<nda::array<dcomplex, 6>>(root["dft_parproj_input"]["proj_mat_all"]);
        // FIXME: The θ projectors have an extra dimesion called ir. Not sure why it is there...
        return nda::array<dcomplex, 5>{tmp(r_all, r_all, r_all, 0, r_all, r_all)};
      } else {
        throw std::runtime_error{"This should not happen!"};
      }
    };

    auto P_k_tmp  = load_Pks(filename, mode);
    auto n_atoms  = P_k_tmp.extent(2);
    auto P_k_list = range(n_atoms) | stdv::transform([&](auto atom) { return P_k_tmp(r_all, r_all, atom, r_all, r_all); }) | tl::to<std::vector>();

    // Merge the rotation matrices R into the Ps: P <- dagger(R) * P
    auto [n_k, n_sigma, n_m, n_nu] = P_k_list[0].shape(); // NOLINT
    for (auto isig : range(n_sigma)) {
      for (auto ik : range(n_k)) {
        for (auto const &[R, P] : zip(rot_mats, P_k_list)) {
          P(ik, isig, nda::range(0, R.extent(0)), r_all) = dagger(R) * nda::matrix<dcomplex>{P(ik, isig, nda::range(0, R.extent(0)), r_all)};
        }
      }
    }

    // Place the P_k_list into the P_k living in the M x M space.
    long M   = stdr::fold_left(atom_decomp, 0, std::plus<>());
    auto P_k = nda::zeros<dcomplex>(n_k, n_sigma, M, n_nu);
    for (auto const &[atom, sli] : enumerated_sub_slices(atom_decomp)) {
      P_k(r_all, r_all, sli, r_all) = P_k_list[atom](r_all, r_all, nda::range(0, atom_decomp[atom]), r_all);
    }
    return P_k;
  }

  //-------------------------------------------------------
  /// Read band dispersion and k-weights according to ReadMode. (internal)
  std::tuple<nda::array<dcomplex, 4>, nda::matrix<long>, nda::array<double, 1>> read_bands_and_weights(std::string filename, ReadMode mode) {
    auto root = h5::proxy{filename, 'r'};
    if (mode == ReadMode::Correlated || mode == ReadMode::ThetaProjectors) {
      auto g_dft = root["dft_input"];
      return {as<nda::array<dcomplex, 4>>(g_dft["hopping"]), as<nda::matrix<long>>(g_dft["n_orbitals"]),
              as<nda::array<double, 1>>(g_dft["bz_weights"])};
    } else if (mode == ReadMode::Bands) {
      auto g_dft = root["dft_bands_input"];
      return {as<nda::array<dcomplex, 4>>(g_dft["hopping"]), as<nda::matrix<long>>(g_dft["n_orbitals"]), {}};
    } else {
      throw std::runtime_error{"This should not happen!"};
    }
  }

  //-------------------------------------------------------
  /// Setup spin_kind enum. (internal)
  spin_kind_e read_spin_kind(auto const &filename) {
    // set up spin_type
    auto g_dft = h5::proxy{filename, 'r'}["dft_input"];
    return (long(g_dft["SO"]) == 1) ? spin_kind_e::NonColinear : ((long(g_dft["SP"]) == 1) ? spin_kind_e::Polarized : spin_kind_e::NonPolarized);
  }

  //-------------------------------------------------------
  /// Read atomic shells according to ReadMode. (internal)
  std::vector<atomic_shell_t> read_atomic_shells(auto const &filename, ReadMode mode) {
    auto g_dft = h5::proxy{filename, 'r'}["dft_input"];
    return ((mode == ReadMode::Correlated) ? sort_keys_as_int(g_dft["corr_shells"]) : sort_keys_as_int(g_dft["shells"]))
       | stdv::transform([](auto const &g) {
             //NB: as<long>(g["atom"]), as<long>(g["dim"]), as<long>(g["equiv_cls_idx"]) );
             return atomic_shell_t{long(g["dim"]), long(g["l"]), long(g["sort"]), long(g["atom"])};
           })
       | tl::to<std::vector>();
  }

  //-------------------------------------------------------
  /// Prepare the spherical Ylm to DFT orbital basis rotations. (internal)
  nda::array<nda::matrix<dcomplex>, 1> read_spherical_to_dft_basis(std::string dft, std::vector<atomic_shell_t> const &atomic_shells) {
    //TODO: finish this!
    auto code = dft_code::dft_code_to_enum(dft);
    auto Ylms = nda::array<nda::matrix<dcomplex>, 1>(atomic_shells.size());
    for (auto const &[iatom, atom] : enumerate(atomic_shells)) { Ylms(iatom) = dft_code::get_spherical_to_dft_rotation(code, atom.l); }
    return Ylms;
  }

  //-------------------------------------------------------
  /// Construct the ibz_symmetry_ops according to ReadMode.
  ibz_symmetry_ops read_ibz_symmetry_ops(auto const &filename, ReadMode mode) {

    auto root = h5::proxy{filename, 'r'};

    auto rot_mats      = read_rotation_matrices(filename, mode);
    auto atomic_shells = read_atomic_shells(filename, mode);

    auto g_symm   = (mode == ReadMode::Correlated) ? root["dft_symmcorr_input"] : root["dft_symmpar_input"];
    auto symm_ops = ibz_symmetry_ops{};

    // read the symmetrization matrices (Q) for each symmetry operation S in the space group G.
    auto symm_ops_mats = sort_keys_as_int(g_symm["mat"]) | stdv::transform([](auto const &g) { return to_vector<cmat_t>(sort_keys_as_int(g)); })
       | tl::to<std::vector>();

    // read the permutation table which contains: for each symmetry op S in space group G,
    // the permutations of all atoms in the crystal structure under S.
    // dim(perm_table_all) = [number of sym ops][number of atoms in crystal]
    auto perm_table_all =
       sort_keys_as_int(g_symm["perm"]) | stdv::transform([](auto const &g) { return to_vector<long>(sort_keys_as_int(g)); }) | tl::to<std::vector>();

    // The permutation table is written for all atoms in the crystal. We must filter to obtain only the correlated atoms.
    auto corr_atom_to_alpha = std::unordered_map<long, long>{};
    for (auto iatom : range(atomic_shells.size())) corr_atom_to_alpha[atomic_shells[iatom].dft_idx] = iatom;

    // The filtered permutation table (perm_table_corr): for each sym S, filter only the correlated atoms
    // and map their indices from dft_idx correlated idx.
    // dim(perm_table_correlated) = [number of sym ops][number of correlated atoms in crystal]
    auto perm_table_correlated = perm_table_all | stdv::transform([&](auto const &sym) {
                                   return atomic_shells
                                      | stdv::transform([&](auto corr_atom) { return corr_atom_to_alpha[sym[corr_atom.dft_idx - 1]]; })
                                      | tl::to<std::vector>();
                                 })
       | tl::to<std::vector>();

    // Merge the rotation matrices R into the symmetrization matrices (Qs).
    for (auto const &[iq, perm] : enumerate(perm_table_correlated)) {
      for (auto const &[a, b] : enumerate(perm)) { symm_ops_mats[iq][a] = dagger(rot_mats[b]) * symm_ops_mats[iq][a] * rot_mats[a]; }
    }

    // Does this op have time inversion symmetry
    auto time_inv_op = to_vector<long>(sort_keys_as_int(g_symm["time_inv"]));
    // save the symmetry ops in to the ibz_symmetrizer
    symm_ops.ops = range(perm_table_correlated.size()) | stdv::transform([symm_ops_mats, perm_table_correlated, time_inv_op](auto const &isym) {
                     return ibz_symmetry_ops::op{symm_ops_mats[isym], perm_table_correlated[isym], time_inv_op[isym]};
                   })
       | tl::to<std::vector>();

    return symm_ops;
  };

  //-------------------------------------------------------
  /// Prepare one-body elements for a DMFT calculation.
  std::pair<double, one_body_elements_on_grid> one_body_elements_from_dft_converter(std::string const &filename, double threshold,
                                                                                    bool diagonalize_hloc) {
    //TODO: add verbosity, currently not used.
    auto g_dft = h5::proxy{filename, 'r'}["dft_input"];

    //TODO: FIXME! hdf5 read to strict on long
    auto total_density  = as<double>(g_dft["density_required"]);
    double charge_below = 10000;
    try {
      charge_below = as<double>(g_dft["charge_below"]);
    } catch (std::exception const &e) { charge_below = as<long>(g_dft["charge_below"]); }
    total_density -= charge_below;

    /// set up spin_type
    auto spin_kind = read_spin_kind(filename);

    /// set up atomic shells
    auto atomic_shells = read_atomic_shells(filename, ReadMode::Correlated);

    /// disperion and k weights
    auto [H_k, n_bands_per_k, k_weights] = read_bands_and_weights(filename, ReadMode::Correlated);

    /// rotation matrices in csc mode
    auto rot_mats = read_rotation_matrices(filename, ReadMode::Correlated);

    /// read and rotate projectors
    auto atom_decomp = atomic_shells | stdv::transform([](auto &x) { return x.dim; }) | tl::to<std::vector<long>>();
    auto P_k         = load_rotate_and_format_projectors(filename, ReadMode::Correlated, rot_mats, atom_decomp);

    /// read symmetry ops
    //FIXME: auto symm_ops = (long(g_dft["symm_op"]) == 0) ? ibz_symmetry_ops{} : read_ibz(root, atomic_shells, Rmats);
    auto symm_ops = (long(g_dft["symm_op"]) == 0) ? std::optional<ibz_symmetry_ops>{} :
                                                    std::optional<ibz_symmetry_ops>(read_ibz_symmetry_ops(filename, ReadMode::Correlated));

    auto eps_k = band_dispersion{.spin_kind = spin_kind, .H_k = std::move(H_k), .n_bands_per_k = n_bands_per_k, .k_weights = k_weights};
    auto proj  = downfolding_projector{.spin_kind = spin_kind, .P_k = std::move(P_k), .n_bands_per_k = n_bands_per_k};

    /// build a first version without symmetries
    auto C_space_no_symm = local_space{spin_kind, atomic_shells, {}, {}, {}};
    auto obe             = one_body_elements_on_grid{.H = eps_k, .C_space = C_space_no_symm, .P = proj, .ibz_symm_ops = std::move(symm_ops)};

    // TODO: verbosity should report the symmetries found (or at least the result of the symmetries found).
    // auto out = ostream_with_verbosity(std::cout, verbosity);
    // FIXME: verbosity reporting of local H0 which is used to analyze the symmetries
    auto Hloc0                        = obe.C_space.atomic_view(impurity_levels(obe));
    auto [decomposition, U_rotations] = discover_symmetries(Hloc0, atomic_shells, threshold, diagonalize_hloc);
    // FIXME: printing of the local symmetries discovered: eigenvalues (and eigenvectors of Hloc0)
    // Should we print the rotation matrices as well?
    // This is a lot of matrices that will be printed...is there a more compact representation?
    // FIXME : NORMALLY the rotate_local_basis shuold also rotate the local_basis !!!
    // FIXME !! ---> @ HL : the question is multiply rotation_from_dft_basis LEFT OR RIGHT ?
    // update the orbital set with the decomposition

    // FIXME: h5_proxy bug?
    // if (!g_dft.has_key("dft_code")) {
    //   throw std::runtime_error{fmt::format(
    //      "The dft_input group in the hdf5 {} does not contain the key dft_code.\nMost likely, this hdf5 file generated using a legacy dft_tools converter.",
    //      filename)};
    // }

    obe.C_space = local_space{spin_kind, atomic_shells, decomposition, U_rotations,
                              read_spherical_to_dft_basis(as<std::string>(g_dft["dft_code"]), atomic_shells)};
    // rotate to the local basis.
    auto obe_final = rotate_local_basis(U_rotations, std::move(obe));
    // FIXME: The dft_tools converters do not write the spherical_to_dft rotation for all cases.
    // Solution: hard-code the rotations for each dft_code (read from hdf5) and save in the local space.
    // The only issue is dft_code breaks backward compatibility with older hdf5 files.
    // Therefore the user can either pass at this stage this rotation or in the function that constructs the
    // slate hamiltonian.
    return {total_density, std::move(obe_final)};
  }

  //-------------------------------------------------------
  /// Prepare one-body elements with the Θ projectors.
  one_body_elements_on_grid one_body_elements_with_theta_projectors(std::string const &filename, one_body_elements_on_grid const &obe) {
    //check for group and throw error
    auto root = h5::proxy{filename, 'r'};
    if (!root.has_group("dft_parproj_input")) {
      throw std::runtime_error{fmt::format("The hdf5 file {} does not contain the group dft_parproj_input", filename)};
    }

    /// all the atomic shells
    auto atomic_shells = read_atomic_shells(filename, ReadMode::ThetaProjectors);

    /// The decomposition and rotations must be embedded from the C spcae to the W space
    auto new_block_decomposition = detail::inject_to_new_space(obe.C_space.atoms_block_decomposition(), obe.C_space.atomic_shells(), atomic_shells);

    /// The local space expanded from C to W
    auto W_space = local_space{obe.C_space.spin_kind(), atomic_shells, new_block_decomposition, {}, {}};

    /// rotation matrices using ThetaProjector mode
    auto rot_mats = read_rotation_matrices(filename, ReadMode::ThetaProjectors);

    /// read and rotate projectors
    auto atom_decomp = W_space.atomic_decomposition() | tl::to<std::vector<long>>();
    auto P_k         = load_rotate_and_format_projectors(filename, ReadMode::ThetaProjectors, rot_mats, atom_decomp);
    auto theta_proj  = downfolding_projector{obe.C_space.spin_kind(), std::move(P_k), obe.H.n_bands_per_k};

    /// create a new IBZ symmetrizer that spans all atoms instead of just the correlated atoms
    auto symm_ops = (obe.ibz_symm_ops) ? std::optional<ibz_symmetry_ops>(read_ibz_symmetry_ops(filename, ReadMode::ThetaProjectors)) :
                                         std::optional<ibz_symmetry_ops>{};

    return one_body_elements_on_grid{.H = obe.H, .C_space = W_space, .P = theta_proj, .ibz_symm_ops = symm_ops};
  }

  //-------------------------------------------------------
  /// Prepare one-body elements along high-symmetry k-path.
  one_body_elements_on_grid one_body_elements_on_high_symmetry_path(std::string const &filename, one_body_elements_on_grid const &obe) {
    /// check for group and throw error
    auto root = h5::proxy{filename, 'r'};
    if (!root.has_group("dft_bands_input")) {
      throw std::runtime_error{fmt::format("The hdf5 file {} does not contain the group dft_bands_input", filename)};
    }

    /// disperion and k weights
    auto [H_k, n_bands_per_k, k_weights] = read_bands_and_weights(filename, ReadMode::Bands);

    /// rotation matrices in csc mode
    auto rot_mats = read_rotation_matrices(filename, ReadMode::Correlated);

    /// read and rotate projectors
    auto atom_decomp = obe.C_space.atomic_decomposition() | tl::to<std::vector<long>>();
    auto P_k         = load_rotate_and_format_projectors(filename, ReadMode::Bands, rot_mats, atom_decomp);

    /// construct one-body elements (ibz_symm_ops are needed so we drop)
    auto eps_k = band_dispersion{obe.C_space.spin_kind(), std::move(H_k), n_bands_per_k, {}};
    auto proj  = downfolding_projector{obe.C_space.spin_kind(), std::move(P_k), n_bands_per_k};
    auto obe1  = one_body_elements_on_grid{.H = eps_k, .C_space = obe.C_space, .P = proj, .ibz_symm_ops = {}};

    /// rotate to the local basis that the self-energies will be defined in.
    return rotate_local_basis(obe.C_space.rotation_from_dft_to_local_basis(), std::move(obe1));
  }
} // namespace triqs::modest
