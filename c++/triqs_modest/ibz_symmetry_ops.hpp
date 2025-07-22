#pragma once
#include "utils/defs.hpp"
#include "utils/gf_supp.hpp"
#include <nda/nda.hpp>
#include <triqs/gfs.hpp>
#include "utils/enumerate_slice.hpp"

namespace triqs::modest {

  // IBZ symmetry operations to obtain observables (equivalent to summing on FBZ).

  // NB: if SOC and time inversion present <T*O> = <K^-1 O K> = <transpose(O)>.
  // FIXME : OP : document the size of matrices, meaning of permutations.
  // FIXME OP : rename into ibz_symmetrizer
  // FIMXE HL : can we generalize to work on any type T that supports matrix multiplication?
  /**
 * @ingroup ibz_symmetry_ops
 * @brief ibz symmetry operations
 * 
 */
  struct ibz_symmetry_ops {

    //-----------------------------------------------------------------------------------
    struct op {
      std::vector<nda::matrix<dcomplex>> mats; // mats[i_alpha]
      std::vector<long> permutation;           // permutation [i_alpha]
      long time_inv;                           // FIXME : we should supress/comment this ?
    };
    std::vector<op> ops; // ops[i_sym]

    // printing
    friend std::ostream &operator<<(std::ostream &out, ibz_symmetry_ops const &ibz);

    //-----------------------------------------------------------------------------------
    template <typename T> nda::array<T, 2> symmetrize(nda::array<T, 2> const &X) const {
      auto [n_alpha, n_sigma] = X.shape();
      nda::array<T, 2> result(n_alpha, n_sigma);
      for (auto [a, s] : result.indices()) { result(a, s) = nda::zeros<dcomplex>(X(a, s).shape()); }
      auto n_symm = double(this->ops.size());
      for (auto const &sym_op : this->ops) {
        for (auto &&[ialpha, jalpha, mat] : zip(range(n_alpha), sym_op.permutation, sym_op.mats)) {
          for (auto sigma : range(n_sigma)) {
            result(jalpha, sigma) +=
               mat * (sym_op.time_inv ? transpose(X(ialpha, sigma)) : X(ialpha, sigma)) * dagger(mat) / n_symm;
          } // end loop on sigma
        } // end loop on alpha
      } // end loop on ops
      return result;
    }

    // -------------------------------------------------------------------------------------------

    // FIXME : Pas Gin in the atomic decomposition directly ??
    // as a block2_gf_view ??
    // Or array<gf_view, 2> ??
    template <typename Mesh>
    block2_gf<Mesh, matrix_valued> symmetrize(block2_gf<Mesh, matrix_valued> const &Gin,
                                              auto const &atomic_decomposition) const {
      // FIXME : copy it , take atom_ivew of it, operate and return it.
      // FIXME : are we sure all Gin have the same mesh ? We use this internally and we have a check at E.embed that the meshes are the same. We could add this here.
      auto Gout    = make_block2_gf(Gin(0, 0).mesh(), get_struct(Gin));
      auto n_sigma = Gout.size2();
      auto n_symm  = double(this->ops.size());

      auto range_list = enumerated_sub_slices(atomic_decomposition)
         | stdv::transform([](auto &x) { return std::get<1>(x); }) | tl::to<std::vector>();

      for (auto [alpha, R_alpha] : enumerated_sub_slices(atomic_decomposition)) { // loop on alpha
        for (auto sigma : range(n_sigma)) {                                       // loop on sigma
          for (auto const &op : this->ops) {                                      // loop on ops
            auto const &mat = op.mats[alpha];
            auto R_jalpha   = range_list[op.permutation[alpha]];
            for (auto om : Gin(0, sigma).mesh()) { // loop on mesh
              auto A = nda::matrix<dcomplex>{Gin(0, sigma)[om](R_alpha, R_alpha)};
              Gout(0, sigma)[om](R_jalpha, R_jalpha) += mat * (op.time_inv ? transpose(A) : A) * dagger(mat) / n_symm;
            } // end loop on mesh
          } // end loop on ops
        } // end loop on sigma
      } // end loop on alpha
      return Gout;
    }
  };

  /**
 * @brief Change basis 
 * 
 * @param U Rotations by block in atomic decomposition 
 * @param x ibz_symmetry_ops to rotate 
 * @return A new symmetrizer operating in the rotated basis U^† U
 */
  inline ibz_symmetry_ops rotate_local_basis(nda::array<nda::matrix<dcomplex>, 2> const &U, ibz_symmetry_ops const &x) {
    auto result = x;
    for (auto &sym_op : result.ops)
      for (auto &&[a, b] : zip(range(U.extent(0)), sym_op.permutation))
        sym_op.mats[a] = dagger(U(b, 0)) * sym_op.mats[a] * U(a, 0);
    return result;
  }

} // namespace triqs::modest
