// SPDX-FileCopyrightText: 2026 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/stralgo
#include <stralgo/utf/case.h>
#include "utf_case_table.h"

namespace stralgo::inline v1_4::utf::detail
  {
namespace
  {
  [[nodiscard]]
  char32_t map_code_point(std::span<case_pair_t const> pairs, char32_t code) noexcept
    {
    auto const it{std::ranges::lower_bound(pairs, code, {}, &case_pair_t::code)};
    if(it != pairs.end() && it->code == code)
      return it->mapped;
    return code;
    }
  }  // namespace

char32_t to_lower_code_point(char32_t code) noexcept { return map_code_point(lower_case_pairs, code); }

char32_t to_upper_code_point(char32_t code) noexcept { return map_code_point(upper_case_pairs, code); }
  }  // namespace stralgo::utf::detail
