// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "../utils/defs.hpp"
#include <string>
#include <stdexcept>

namespace triqs::modest {

  /// Enumeration of supported DFT codes
  enum class DFTCode { Wien2k, QuantumEspresso, VASP, Elk, W90, Hk };

  namespace dft_tools {

    /// Convert DFT code name string to DFTCode enum
    /// @param code String identifier for the DFT code
    /// @return Corresponding DFTCode enum value
    /// @throws std::runtime_error if code string is not recognized
    inline DFTCode dft_code_to_enum(std::string const &code) {
      if (code == "wien2k") return DFTCode::Wien2k;
      if (code == "vasp") return DFTCode::VASP;
      if (code == "w90") return DFTCode::W90;
      if (code == "elk") return DFTCode::Elk;
      if (code == "hk") return DFTCode::Hk;
      if (code == "qe") return DFTCode::QuantumEspresso;
      throw std::runtime_error{"Not a valid dft_code: " + code};
    }

  } // namespace dft_tools

} // namespace triqs::modest
