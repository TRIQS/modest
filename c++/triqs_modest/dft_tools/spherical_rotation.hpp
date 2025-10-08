// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "../utils/defs.hpp"
#include "utils.hpp"

namespace triqs::modest::dft_tools {

  namespace wien2k {
    /// Get rotation matrix from spherical harmonics to Wien2k orbital basis
    /// @param l Angular momentum quantum number
    /// @return Rotation matrix Y_lm -> Wien2k basis
    nda::matrix<dcomplex> get_spherical_to_dft_rotation(long l);
  } // namespace wien2k

  namespace vasp {
    /// Get rotation matrix from spherical harmonics to VASP orbital basis
    /// @param l Angular momentum quantum number
    /// @return Rotation matrix Y_lm -> VASP basis
    nda::matrix<dcomplex> get_spherical_to_dft_rotation(long l);
  } // namespace vasp

  namespace qe {
    /// Get rotation matrix from spherical harmonics to Quantum Espresso orbital basis
    /// @param l Angular momentum quantum number
    /// @return Rotation matrix Y_lm -> QE basis
    nda::matrix<dcomplex> get_spherical_to_dft_rotation(long l);
  } // namespace qe

  // Dispatcher function for backward compatibility with existing code
  inline nda::matrix<dcomplex> get_spherical_to_dft_rotation(DFTCode code, long l) {
    switch (code) {
      case DFTCode::Wien2k: return wien2k::get_spherical_to_dft_rotation(l);
      case DFTCode::VASP: return vasp::get_spherical_to_dft_rotation(l);
      case DFTCode::W90:
      case DFTCode::QuantumEspresso: return qe::get_spherical_to_dft_rotation(l);
      default: throw std::invalid_argument("Unknown DFTCode");
    }
  }

} // namespace triqs::modest::dft_tools
