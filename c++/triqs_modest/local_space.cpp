#include "./local_space.hpp"

namespace triqs::modest {

  local_space::local_space(spin_kind_e spin_kind, std::vector<atomic_shell_t> atomic_shells,
                           nda::array<std::vector<long>, 2> irreps_decomp_per_atom,
                           nda::array<nda::matrix<dcomplex>, 2> rotation_from_dft_to_local_basis,
                           nda::array<nda::matrix<dcomplex>, 1> rotation_from_spherical_to_dft_basis)
     : _spin_kind{spin_kind},
       _atomic_shells{std::move(atomic_shells)},
       _irreps_decomp_per_atom{std::move(irreps_decomp_per_atom)},
       _rotation_from_dft_to_local_basis{std::move(rotation_from_dft_to_local_basis)},
       _rotation_from_spherical_to_dft_basis{std::move(rotation_from_spherical_to_dft_basis)} {

    //  Compute the dimension of the correlated space
    _dim_C = stdr::fold_left(_atomic_shells | stdv::transform([](auto const &s) { return s.dim; }), 0, std::plus<>());

    // Pre-compute the atom names
    _atom_names =
       range(_atomic_shells.size()) | stdv::transform([](auto i) { return std::to_string(i); }) | tl::to<std::vector>();
  };

  [[nodiscard]] long local_space::first_shell_of_its_equiv_cls(long idx) const {
    auto cls_idx = _atomic_shells[idx].cls_idx;
    for (auto const &[iatom, sh] : enumerate(_atomic_shells)) {
      if (cls_idx == sh.cls_idx) return iatom;
    }
    throw std::runtime_error("Can not find the atom in first_shell_of_its_equiv_cls");
  };

  nda::array<nda::matrix<dcomplex>, 2> local_space::atomic_view(nda::array<nda::matrix<dcomplex>, 2> const &matrix_C) {
    // FIXME : ASSERT check the dimension of G_C m x m , one a block, n_sigma ...
    auto matrix_out = nda::array<nda::matrix<dcomplex>, 2>{n_atoms(), n_sigma()};
    for (auto sigma : range(n_sigma()))
      for (auto const &[atom, r_atom] : enumerated_sub_slices(atomic_decomposition())) {
        matrix_out(atom, sigma) = nda::matrix<dcomplex>{matrix_C(0, sigma)(r_atom, r_atom)};
      }
    return matrix_out;
  }

  gfs::gf_struct2_t local_space::Gc_block_shape() const {
    auto res      = nda::zeros<long>(1, n_sigma());
    res(0, r_all) = dim();
    return {.names = {{"0"s}, sigma_names()}, .dims = std::move(res)};
  }
} // namespace triqs::modest
