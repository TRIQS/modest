// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

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
   * @ingroup one_body_elements
   * @brief Irreducible Brillouin Zone (IBZ) symmetry operations to symmetrize observables over the entire Brillouin
   * zone.
   *
   * @details The IBZ symmetry operations are optional and specific to DFT codes that work in the IBZ instead of the
   * full BZ. Currently, this is only used when DFT data is converted from Wien2k.
   *
   * For computational efficiency, we can perform the one-body calculation on the irreducible Brillouin zone (IBZ).
   * However, in order to obtain observable quantities like the local Green's function, one needs to _symmetrize_ the
   * obversable summed on only the IBZ. For any observable \f$\mathcal{O}\f$, the unsymmetrized quantity is
   * \f[
   *   [\mathcal{O}^{\sigma}_{mm'}]_{\mathrm{unsymm}} = \sum_{\mathbf{k}\in\mathrm{IBZ}}\sum_{\nu\nu'}P_{m,\nu}^{\sigma}
   *   (\mathbf{k})O_{\nu\nu'}^{\sigma}(\mathbf{k})[P_{m'\nu'}^{\sigma}(\mathbf{k})]^{\dagger}.
   * \f]
   * To symmetrize, we must by apply all operations symmetry operations \f$\mathcal{S}\f$ of the crystallographic space
   * group \f$\mathcal{G}\f$:
   * \f[
   *   [\mathcal{O}^{\sigma}_{(am_{a}), (a'm_{a}')}]_{\mathrm{symm}}^{\mathcal{G}} = \sum_{\mathcal{S}\in\mathcal{G}}
   *   \sum_{n_{a}n_{a}'} \mathcal{D}_{m_{a} n_{a}}(\mathcal{S})[(\mathcal{O}^{\mathcal{S}^{-1}a,\mathcal{S}^{-1}
   *   \sigma})_{n_{a}n_{a}'}]_{\mathrm{unsymm}}\mathcal{D}(\mathcal{S}^{-1})_{n_{a}'m_{a}'}.
   * \f]
   */
  struct ibz_symmetry_ops {

    //-----------------------------------------------------------------------------------
    /** @cond DOXYGEN_SKIP_THIS */
    struct op {
      std::vector<nda::matrix<dcomplex>> mats; // mats[i_alpha]
      std::vector<long> permutation;           // permutation [i_alpha]
      long time_inv;                           // FIXME : we should supress/comment this ?
    };
    /** @endcond */
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
            result(jalpha, sigma) += mat * (sym_op.time_inv ? transpose(X(ialpha, sigma)) : X(ialpha, sigma)) * dagger(mat) / n_symm;
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
    block2_gf<Mesh, matrix_valued> symmetrize(block2_gf<Mesh, matrix_valued> const &Gin, auto const &atomic_decomposition) const {
      // FIXME : copy it , take atom_ivew of it, operate and return it.
      // FIXME : are we sure all Gin have the same mesh ? We use this internally and we have a check at E.embed that the meshes are the same. We could add this here.
      auto Gout    = make_block2_gf(Gin(0, 0).mesh(), get_struct(Gin));
      auto n_sigma = Gout.size2();
      auto n_symm  = double(this->ops.size());

      auto range_list =
         enumerated_sub_slices(atomic_decomposition) | stdv::transform([](auto &&x) { return std::get<1>(x); }) | tl::to<std::vector>();

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
      for (auto &&[a, b] : zip(range(U.extent(0)), sym_op.permutation)) sym_op.mats[a] = dagger(U(b, 0)) * sym_op.mats[a] * U(a, 0);
    return result;
  }

} // namespace triqs::modest
