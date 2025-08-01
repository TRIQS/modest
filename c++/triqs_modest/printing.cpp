#include "./downfolding.hpp"
#include <triqs/utility/streams.hpp>
#include <fmt/ranges.h>

namespace triqs::modest {

  // -----------------------------  PRINT FUNCTIONS ---------------------------------------------

  std::ostream &operator<<(std::ostream &out, band_dispersion const &x) {
    out << "Band dispersion ε^σ(k) on a grid [band_dispersion]:\n";
    auto out1 = indented_ostream(out, 2);
    out1 << fmt::format("Number of bands (max): {}\n", x.H_k.extent(3));
    out1 << fmt::format("Represented on a fixed grid of {} points.\n", x.n_k());
    out1 << fmt::format("Shape of H[k_idx, σ, ν, ν'] = {}\n", x.H_k.shape());
    out1 << fmt::format("ε^σ(k) is matrix valued? = {}\n", x.matrix_valued);
    return out;
  }

  // ---------------------------------------------------------------------------------------------

  std::ostream &operator<<(std::ostream &out, downfolding_projector const &x) {
    out << "Downfolding projector P^σ_mν(k) on a grid [downfolding_projector]:\n";
    auto out1 = indented_ostream(out, 2);
    out1 << fmt::format("Shape of P[k_idx, σ, m, ν]  = {}\n", x.P_k.shape());
    return out;
  }

  // ---------------------------------------------------------------------------------------------

  std::ostream &operator<<(std::ostream &out, local_space const &x) {
    out << "Local space [orbital_set]:\n";
    auto out1 = indented_ostream(out, 2); // same stream, but shifted by 2 spaces
    auto out2 = indented_ostream(out, 4);
    auto out3 = indented_ostream(out, 6);
    out1 << fmt::format("Total dimension [M]: {}\n", x.dim());
    out1 << fmt::format("Number of correlated atoms: {}\n", x.atomic_shells().size());
    // auto ineq = 0;
    // for (auto iatom : range(x.atomic_shells().size())) {
    //   if (x.first_shell_of_its_equiv_cls(iatom) == iatom) ineq++;
    // }
    auto ineq = stdr::count_if(range(x.atomic_shells().size()), //
                               [&](auto i) { return x.first_shell_of_its_equiv_cls(i) == i; });
    out1 << fmt::format("Number of inequivalent correlated atoms = {}\n", ineq);

    out1 << "Atomic decomposition:\n";
    auto pr_vec = [](auto const &V) {
      return fmt::format("{}\n", fmt::join(V | stdv::transform([](auto x) { return fmt::format("{:>3}", x); }), " "));
    };
    auto dec = x.atomic_decomposition();
    out2 << " dim_a: " << pr_vec(dec);
    out2 << "     a: " << pr_vec(range(dec.size()));
    auto sub = x.atoms_block_decomposition()(r_all, 0) | stdv::transform([](auto x) { return fmt::format("{}", x); });
    out2 << "irreps: " << fmt::format("{}\n", fmt::join(sub, " "));
    return out;
  }

  // ---------------------------------------------------------------------------------------------
  std::ostream &operator<<(std::ostream &out, ibz_symmetry_ops const &ibz) {
    out << "[I]rreduicible [B]rillouin [Z]one symmetry operations from the DFT code [ibz_symmetry_ops]\n";
    auto out1 = indented_ostream(out, 2);
    out1 << fmt::format("Number of symmetry ops: {}\n", ibz.ops.size());
    out1 << fmt::format("Number of rotations per op: {}\n", ibz.ops[0].mats.size());
    return out;
  }

  // ---------------------------------------------------------------------------------------------

  std::ostream &operator<<(std::ostream &out, one_body_elements_on_grid const &obe) {
    auto out1 = indented_ostream(out, 2); // same stream, but shifted by 2 spaces
    auto out2 = indented_ostream(out, 4);
    out << "One body elements representing a downfolding from (restricted) Bloch 𝓑 to Correlated space 𝓒 from DFT code [one_body_elements_on_grid]\n";
    out1 << "H:\n";
    out2 << obe.H;
    out1 << "C_space:\n";
    out2 << obe.C_space;
    out1 << "P:\n";
    out2 << obe.P;
    out1 << fmt::format("IBZ = {}\n", bool(obe.ibz_symm_ops));
    if (obe.ibz_symm_ops) { out2 << obe.ibz_symm_ops.value(); };
    return out;
  }

} // namespace triqs::modest