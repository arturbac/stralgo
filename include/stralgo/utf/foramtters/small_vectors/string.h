// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/stralgo
#pragma once

#include <stralgo/utf/utf.h>
#include <stralgo/utf/detail/foramtters.h>
#include <small_vectors/basic_string.h>
#include <format>
#include <ranges>

template<stralgo::concepts::format_char_type char_type, uint64_t capacity, typename tag>
struct std::formatter<small_vectors::basic_string_t<char_type, capacity, tag>, char>
  {
  template<typename ParseContext>
  constexpr auto parse(ParseContext & ctx) -> decltype(ctx.begin())
    {
    return ctx.begin();
    }

  template<typename FormatContext>
  auto format(small_vectors::basic_string_t<char_type, capacity, tag> const & str, FormatContext & ctx) const
    -> decltype(ctx.out())
    {
    return stralgo::utf::detail::tansform_to_conetxt(str, ctx.out());
    }
  };
