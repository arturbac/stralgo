// SPDX-FileCopyrightText: 2026 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/stralgo
#pragma once

#include <stralgo/version.h>
#include <array>
#include <span>

namespace stralgo::inline v1_4::utf::detail
  {
///\brief code point and its simple case mapping, tables are sorted by code
struct case_pair_t
  {
  char32_t code;
  char32_t mapped;
  };

extern std::span<case_pair_t const> const lower_case_pairs;
extern std::span<case_pair_t const> const upper_case_pairs;
  }  // namespace stralgo::utf::detail
