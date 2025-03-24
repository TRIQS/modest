// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "utils/defs.hpp"
namespace triqs::modest {

  enum class DFTCode { Wien2k, QuantumEspresso, VASP, Elk, W90, Hk };

  namespace dft_code {

    inline DFTCode dft_code_to_enum(std::string const &code) {
      if (code == "wien2k") return DFTCode::Wien2k;
      if (code == "vasp") return DFTCode::VASP;
      if (code == "w90") return DFTCode::W90;
      if (code == "elk") return DFTCode::Elk;
      if (code == "hk") return DFTCode::Hk;
      throw std::runtime_error{"Not a valid dft_code!"};
    }

    // Various functions implemented in C++ files
    nda::matrix<dcomplex> get_spherical_to_dft_rotation_Wien2k(long l);
    nda::matrix<dcomplex> get_spherical_to_dft_rotation_VASP(long l);
    // nda::matrix<dcomplex> get_spherical_to_dft_rotation_QuantumEspresso();
    // nda::matrix<dcomplex> get_spherical_to_dft_rotation_Elk();
    // nda::matrix<dcomplex> get_spherical_to_dft_rotation_W90();

    //
    inline nda::matrix<dcomplex> get_spherical_to_dft_rotation(DFTCode code, long l) {
      switch (code) {
        case DFTCode::Wien2k: return get_spherical_to_dft_rotation_Wien2k(l);
        case DFTCode::VASP: return get_spherical_to_dft_rotation_VASP(l);
        case DFTCode::W90: return get_spherical_to_dft_rotation_VASP(l); // FIXME
        case DFTCode::Hk:
          return get_spherical_to_dft_rotation_VASP(l); // FIXME
        // case DFTCode::Elk: return get_spherical_to_dft_rotation_Elk();
        // case DFTCode::QuantumEspresso: return get_spherical_to_dft_rotation_W90();
        default: throw std::invalid_argument("Unknown DFTCode");
      }
    }

  } // namespace dft_code

} // namespace triqs::modest