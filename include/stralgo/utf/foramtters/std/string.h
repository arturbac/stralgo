#pragma once

#include <stralgo/utf/utf.h>
#include <stralgo/utf/detail/foramtters.h>
#include <format>
#include <ranges>

template<stralgo::concepts::format_char_type char_type>
struct std::formatter<std::basic_string<char_type>, char>
  {
  template<typename ParseContext>
  constexpr auto parse(ParseContext & ctx) -> decltype(ctx.begin())
    {
    return ctx.begin();
    }

  template<typename FormatContext>
  auto format(std::basic_string<char_type> const & str, FormatContext & ctx) const -> decltype(ctx.out())
    {
    return stralgo::utf::detail::tansform_to_conetxt(str, ctx.out());
    }
  };

