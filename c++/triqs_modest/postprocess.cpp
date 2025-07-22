#include "./postprocess.hpp"

namespace triqs::modest {

  constexpr auto lattice_gf(auto const &mesh, auto const &obe, auto const &Proj, auto const &mu,
                            auto const &embedding_decomp, auto const &Sigma_w, auto const &Sigma_DC,
                            auto const &broadening) {

    return [&](auto &k_idx, auto &sigma) {
      auto PSP = [&](auto &w, auto &k_idx, auto &sigma) {
        auto N_nu = obe.H.N_nu(sigma, k_idx);
        auto out  = nda::zeros<dcomplex>(N_nu, N_nu);
        for (auto &&[alpha, R] : enumerated_sub_slices(embedding_decomp)) {
          auto P = Proj.P(sigma, k_idx)(R, r_all);
          out(r_all, r_all) += dagger(P)
             * nda::matrix<dcomplex>{Sigma_w(alpha, sigma).data()(w, r_all, r_all) - Sigma_DC(alpha, sigma)} * P;
        }
        return out;
      };
      auto out = gf{mesh};
      for (auto &&[n, w] : enumerate(mesh)) {
        out.data()(n) =
           trace(inverse(w + dcomplex(0.0, broadening) + mu - obe.H.H(sigma, k_idx) - PSP(n, k_idx, sigma)));
      }
      return out;
    };
  }

  spectral_function_w projected_spectral_function(one_body_elements_on_grid const &obe_theta,
                                                  downfolding_projector const &Proj, double mu,
                                                  block2_gf<mesh::refreq, matrix_valued> const &Sigma_w,
                                                  nda::array<nda::matrix<double>, 2> const &Sigma_DC,
                                                  double broadening) {

    auto const &mesh = Sigma_w(0, 0).mesh();

    auto n_sigma = obe_theta.C_space.n_sigma();
    auto n_M     = obe_theta.C_space.dim();
    auto n_w     = mesh.size();
    auto n_k     = obe_theta.H.n_k();

    auto embedding_decomp = get_struct(Sigma_w).dims(r_all, 0) | tl::to<std::vector>();

    auto PSP = [&](auto &w, auto &k_idx, auto &sigma) {
      auto N_nu = obe_theta.H.N_nu(sigma, k_idx);
      auto out  = nda::zeros<dcomplex>(N_nu, N_nu);
      for (auto &&[alpha, R] : enumerated_sub_slices(embedding_decomp)) {
        auto P = Proj.P(sigma, k_idx)(R, r_all);
        out(r_all, r_all) += dagger(P)
           * nda::matrix<dcomplex>{Sigma_w(alpha, sigma).data()(w, r_all, r_all) - Sigma_DC(alpha, sigma)} * P;
      }
      return out;
    };
    auto Gloc = [&](auto &k_idx, auto &sigma) {
      auto out = gf{mesh, {n_M, n_M}};
      auto P   = obe_theta.P.P(sigma, k_idx);
      for (auto &&[n, w] : enumerate(mesh)) {
        out.data()(n, r_all, r_all) = P
           * inverse(w + dcomplex(0.0, broadening) + mu - obe_theta.H.H(sigma, k_idx) - PSP(n, k_idx, sigma))
           * dagger(P);
      }
      return out;
    };

    auto gloc_result = make_block2_gf(mesh, obe_theta.C_space.Gc_block_shape());
    for (auto sigma : nda::range(n_sigma)) {
      for (auto k_idx : nda::range(n_k)) {
        gloc_result(0, sigma).data()(r_all, r_all, r_all) += obe_theta.H.k_weights(k_idx) * Gloc(k_idx, sigma).data();
      }
    }

    if (auto const &S = obe_theta.ibz_symm_ops; S) {
      gloc_result = S->symmetrize(gloc_result, obe_theta.C_space.atomic_decomposition());
    }

    auto total     = nda::array<double, 2>(n_sigma, n_w);
    auto per_theta = nda::array<double, 4>(n_sigma, n_w, n_M, n_M);

    for (auto sigma : nda::range(n_sigma)) {
      auto g  = gloc_result(0, sigma).data();
      auto gC = conj(gloc_result(0, sigma)).data();
      for (auto [n, w] : enumerate(mesh)) {
        // auto g = gloc_result(0, sigma).data()(n, r_all, r_all);
        //FIXME: can trace ignore the mesh dimension (or in general the first axis dimension and broadcast over all the w-mesh points)
        total(sigma, n) = (-1.0 / M_PI) * imag(trace(g(n, r_all, r_all)));
        // missing sign?
        per_theta(sigma, n, r_all, r_all) =
           real(dcomplex(0, 1.0) * (g(n, r_all, r_all) - transpose(gC(n, r_all, r_all))) / (2.0 * M_PI));
      }
    }
    return {total, per_theta};
  }

  spectral_function_kw spectral_function_on_high_symmetry_path(one_body_elements_on_grid const &obe, double mu,
                                                               block2_gf<mesh::refreq, matrix_valued> const &Sigma_w,
                                                               nda::array<nda::matrix<double>, 2> const &Sigma_DC,
                                                               double broadening) {
    auto const &mesh = Sigma_w(0, 0).mesh();
    auto n_sigma     = obe.C_space.n_sigma();
    auto n_w         = mesh.size();
    auto n_k         = obe.H.n_k();

    auto data = nda::array<double, 3>(n_sigma, n_k, n_w);

    auto embedding_decomp = get_struct(Sigma_w).dims(r_all, 0) | tl::to<std::vector>();

    auto Glatt = lattice_gf(mesh, obe, obe.P, mu, embedding_decomp, Sigma_w, Sigma_DC, broadening);

    for (auto sigma : nda::range(n_sigma)) {
      for (auto k_idx : nda::range(n_k)) {
        data(sigma, k_idx, r_all) = (-1.0 / M_PI) * imag(Glatt(k_idx, sigma).data());
      }
    }

    return {data};
  }

} // namespace triqs::modest
