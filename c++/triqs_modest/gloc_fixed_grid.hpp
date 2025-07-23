#pragma once
#include "./density.hpp"
#include "utils/scoped_timer.hpp"
#include <triqs/mesh.hpp>
#include "utils/gf_supp.hpp"

namespace triqs::modest {

// omp reduction operation for block2_gf
#pragma omp declare reduction(block2_gf_sum : block2_gf<imfreq, matrix_valued> : omp_out += omp_in)                    \
   initializer(omp_priv = make_block2_gf(omp_orig(0, 0).mesh(), get_struct(omp_orig)))
#pragma omp declare reduction(block2_gf_sum : block2_gf<dlr_imfreq, matrix_valued> : omp_out += omp_in)                \
   initializer(omp_priv = make_block2_gf(omp_orig(0, 0).mesh(), get_struct(omp_orig)))

  // ------------------------------------------------------------------
  /**
 * @brief compute G𝓒 local Green's function on Mesh(MxM) 
 * 
 * @tparam Mesh triqs::mesh::{dlr_imfreq,imfreq}
 * @param obe one_body_elements_on_grid
 * @param mu chemical potential
 * @param Sigma Embedded self-energy TODO: {Σ∞, ΣDLR }
 * @return block2_gf<Mesh, matrix_valued> 
 */
  template <typename Mesh>
  block2_gf<Mesh, matrix_valued> gloc(one_body_elements_on_grid const &obe, double mu,
                                      block2_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                                      nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree) {

    //std::optional<nda::array<nda::matrix<double>, 2>> const &Sigma_dc)
    // std::optional<block2_gf<Mesh, matrix_valued>> const &Sigma_dc
    auto n_sigma     = Sigma_dynamic.size2();
    auto M           = obe.C_space.dim();
    auto n_kpts      = long(obe.H.n_k());
    auto const &mesh = Sigma_dynamic(0, 0).mesh();
    auto gloc_result = make_block2_gf(mesh, obe.C_space.Gc_block_shape());
    auto omegas      = mesh | tl::to<std::vector<dcomplex>>();

    // Embedding decomposition from structure of Sigma
    // FIXME : better way ?
    auto embedding_decomp = get_struct(Sigma_dynamic).dims(r_all, 0) | tl::to<std::vector>();

    auto timer = scoped_timer{};
    // ---------
    // NOTE: Is there any reason why sigma loop should be the external one?
    // Internal is favorable for maximum parallelization.
    mpi::communicator comm = {};
#pragma omp parallel for collapse(2) reduction(block2_gf_sum : gloc_result) default(none)                              \
   shared(comm, r_all, n_kpts, n_sigma, obe, mu, omegas, mesh, M, embedding_decomp, Sigma_dynamic, Sigma_hartree)
    for (auto k_idx : mpi::chunk(range(n_kpts), comm)) {
      for (auto sigma : range(n_sigma)) {
        auto Y = detail::G0_C_k_sigma(obe, mu, k_idx, sigma, omegas, false);
        for (auto &&[n, om] : itertools::enumerate(mesh)) {
          auto Y1 = Y(n, r_all, r_all);
          auto B  = detail::calc_inv_G_G0(M, embedding_decomp, Sigma_dynamic, Sigma_hartree, om, sigma, Y1, Y1);
          gloc_result(0, sigma).data()(n, r_all, r_all) += obe.H.k_weights(k_idx) * B;
        }
      }
      // No normalization: the Pk are in obe ALREADY normalized.
    }
    gloc_result = mpi::all_reduce(gloc_result);

    // FIXME :: the IBZ should work on a proper gf_view with atomic decomposition
    // CHANGE IBZ accordingly ...
    if (auto const &S = obe.ibz_symm_ops; S)
      gloc_result = S->symmetrize(gloc_result, obe.C_space.atomic_decomposition());

    return gloc_result;
  }

  //TODO: Port to Woodbury? Wrap one gloc function?
  /**
   * @brief  Compute G𝓒 on with a Mesh with no self-energy.
   *
   * @details This implementation does not use the Woodbury formula.
   * 
   * @tparam Mesh 
   * @param mesh mesh triqs::meshes::{imfreq, dlr_imfreq}
   * @param obe one-body elements
   * @param mu chemical potential
   * @return block2_gf<Mesh, matrix_valued> 
   */
  template <typename Mesh>
  block2_gf<Mesh, matrix_valued> gloc(Mesh const &mesh, one_body_elements_on_grid const &obe, double mu) {

    auto n_sigma = obe.C_space.n_sigma();
    auto n_M     = obe.C_space.dim();
    auto n_kpts  = long(obe.H.n_k());

    auto gloc_k = [&](auto &k_idx, auto &sigma) {
      auto out = gf{mesh, {n_M, n_M}};
      auto P   = obe.P.P(sigma, k_idx);
      for (auto &&[n, w] : enumerate(mesh)) {
        out.data()(n, r_all, r_all) = P * inverse(w + mu - obe.H.H(sigma, k_idx)) * dagger(P);
      }
      return out;
    };

    auto gloc_result       = make_block2_gf(mesh, obe.C_space.Gc_block_shape());
    mpi::communicator comm = {};
#pragma omp parallel for collapse(2) reduction(block2_gf_sum : gloc_result) default(none)                              \
   shared(comm, r_all, gloc_k, n_kpts, n_sigma, obe)
    for (auto k_idx : mpi::chunk(range(n_kpts), comm)) {
      for (auto sigma : range(n_sigma)) {
        gloc_result(0, sigma).data()(r_all, r_all, r_all) += obe.H.k_weights(k_idx) * gloc_k(k_idx, sigma).data();
      }
    }
    gloc_result = mpi::all_reduce(gloc_result);

    if (auto const &S = obe.ibz_symm_ops; S) {
      gloc_result = S->symmetrize(gloc_result, obe.C_space.atomic_decomposition());
    }

    return gloc_result;
  }

  template <typename Mesh>
  block_gf<Mesh, matrix_valued> extract_delta(std::vector<nda::matrix<dcomplex>> const &epsilon_levels,
                                              block_gf<Mesh, matrix_valued> const &Gloc,
                                              block_gf<Mesh, matrix_valued> const &Sigma_dynamic,
                                              std::vector<nda::matrix<dcomplex>> const &Sigma_hartree) {
    auto gf_struct = get_struct(Gloc);
    auto mesh      = Gloc[0].mesh();
    auto Delta     = block_gf{mesh, gf_struct};
    auto n_blocks  = gf_struct.size();
    for (auto bl : range(n_blocks)) {
      for (auto &&[n, w] : enumerate(mesh))
        Delta[bl].data()(n, r_all, r_all) = (w - epsilon_levels[bl] - inverse(Gloc[bl]).data()(n, r_all, r_all)
                                             - (Sigma_dynamic[bl].data()(n, r_all, r_all) + Sigma_hartree[bl]));
    }
    return Delta;
  }

  template <typename Mesh>
  block_gf<Mesh, matrix_valued> extract_delta(std::vector<nda::matrix<dcomplex>> const &epsilon_levels,
                                              block_gf<Mesh, matrix_valued> const &Gloc) {
    auto Sigma_hartree = get_struct(Gloc)
       | stdv::transform([](auto &x) { return nda::zeros<dcomplex>(x.second, x.second); })
       | tl::to<std::vector<nda::matrix<dcomplex>>>();
    auto Sigma_dynamic = block_gf{Gloc[0].mesh(), get_struct(Gloc)};
    return extract_delta(epsilon_levels, Gloc, Sigma_dynamic, Sigma_hartree);
  }

  // ------------------------------------------------------

  /** @cond DOXYGEN_SKIP_THIS */
  // FIXME: INSTANTIATE(Mesh) doesn't seem to work...
  // Mesh = imfreq
  template block2_gf<imfreq, matrix_valued> gloc(one_body_elements_on_grid const &one_body, double mu,
                                                 block2_gf<imfreq, matrix_valued> const &Sigma_dynamic,
                                                 nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree);
  template block2_gf<imfreq, matrix_valued> gloc(imfreq const &mesh, one_body_elements_on_grid const &obe, double mu);
  template block2_gf<dlr_imfreq, matrix_valued> gloc(one_body_elements_on_grid const &one_body, double mu,
                                                     block2_gf<dlr_imfreq, matrix_valued> const &Sigma_dynamic,
                                                     nda::array<nda::matrix<dcomplex>, 2> const &Sigma_hartree);
  template block2_gf<dlr_imfreq, matrix_valued> gloc(dlr_imfreq const &mesh, one_body_elements_on_grid const &obe,
                                                     double mu);
  template block_gf<imfreq, matrix_valued> extract_delta(std::vector<nda::matrix<dcomplex>> const &epsilon_levels,
                                                         block_gf<imfreq, matrix_valued> const &Gloc);
  template block_gf<imfreq, matrix_valued> extract_delta(std::vector<nda::matrix<dcomplex>> const &epsilon_levels,
                                                         block_gf<imfreq, matrix_valued> const &Gloc,
                                                         block_gf<imfreq, matrix_valued> const &Sigma_dynamic,
                                                         std::vector<nda::matrix<dcomplex>> const &Sigma_hartree);
  template block_gf<dlr_imfreq, matrix_valued> extract_delta(std::vector<nda::matrix<dcomplex>> const &epsilon_levels,
                                                             block_gf<dlr_imfreq, matrix_valued> const &Gloc);
  template block_gf<dlr_imfreq, matrix_valued> extract_delta(std::vector<nda::matrix<dcomplex>> const &epsilon_levels,
                                                             block_gf<dlr_imfreq, matrix_valued> const &Gloc,
                                                             block_gf<dlr_imfreq, matrix_valued> const &Sigma_dynamic,
                                                             std::vector<nda::matrix<dcomplex>> const &Sigma_hartree);
  /** @endcond */

} // namespace triqs::modest
