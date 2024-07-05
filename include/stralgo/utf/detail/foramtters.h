#pragma once

#include <stralgo/utf/utf.h>
#include <format>
#include <ranges>

namespace stralgo::utf::detail
  {
template<typename input_range, typename output_iterator>
inline auto tansform_to_conetxt(input_range const & str, output_iterator oit)
  {
  stralgo::utf::utf_input_view_t source{str};
  stralgo::utf::utf_explicit_output_iterator_t<char>::iterator utf_oit{oit};
  return std::ranges::copy(source, utf_oit).out.base();
  }
  }  // namespace stralgo::utf::detail

namespace stralgo::concepts
  {
template<typename char_type>
concept format_char_type
  = std::same_as<char_type, wchar_t> or std::same_as<char_type, char16_t> or std::same_as<char_type, char32_t>;
  }

