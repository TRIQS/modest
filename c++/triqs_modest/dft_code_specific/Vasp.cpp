#include "../dft_code_specific.hpp"
#include <stdexcept>

namespace triqs::modest::dft_code {

  nda::matrix<dcomplex> get_spherical_to_dft_rotation_VASP(long l) {
    if (l == 0) std::runtime_error("No l=0 implementation");
    auto size = 2 * l + 1;
    auto Ylm  = nda::zeros<dcomplex>(size, size);
    if (l == 1) {
      Ylm(0, 0) = 1.0 / std::sqrt(2.0);
      Ylm(1, 0) = dcomplex(0, 1.0) / std::sqrt(2.0);
      Ylm(2, 1) = 1.0;
      Ylm(0, 2) = -1.0 / std::sqrt(2.0);
      Ylm(1, 2) = dcomplex(0, 1.0) / std::sqrt(2.0);
    } else if (l == 2) {
      Ylm(0, 2) = 1.0;
      Ylm(1, 0) = 1.0 / std::sqrt(2);
      Ylm(2, 0) = -1.0 / std::sqrt(2);
      Ylm(3, 1) = 1.0 / std::sqrt(2);
      Ylm(4, 1) = 1.0 / std::sqrt(2);
      Ylm(1, 4) = 1.0 / std::sqrt(2);
      Ylm(2, 4) = 1.0 / std::sqrt(2);
      Ylm(3, 3) = -1.0 / std::sqrt(2);
      Ylm(4, 3) = 1.0 / std::sqrt(2);
    } else if (l == 3) {
      Ylm(0, 0) = 1.0 / std::sqrt(2);
      Ylm(1, 1) = 1.0 / std::sqrt(2);
      Ylm(2, 2) = 1.0 / std::sqrt(2);
      Ylm(0, 6) = 1.0 / std::sqrt(2);
      Ylm(1, 5) = 1.0 / std::sqrt(2);
      Ylm(2, 4) = 1.0 / std::sqrt(2);
      Ylm(3, 3) = 1.0;
      Ylm(4, 2) = dcomplex(0, 1.0) / std::sqrt(2);
      Ylm(5, 1) = dcomplex(0, 1.0) / std::sqrt(2);
      Ylm(6, 0) = dcomplex(0, 1.0) / std::sqrt(2);
      Ylm(4, 4) = dcomplex(0, 1.0) / std::sqrt(2);
      Ylm(5, 5) = dcomplex(0, -1.0) / std::sqrt(2);
      Ylm(6, 6) = dcomplex(0, 1.0) / std::sqrt(2);
    }
    return Ylm;
  }
} // namespace triqs::modest::dft_code