// SPDX-FileCopyrightText: 2026 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/stralgo
#pragma once

// Unicode simple case mapping (UnicodeData.txt fields 12 and 13), requires linking with stralgo::utf_case

#include <stralgo/strconcept.h>
#include <algorithm>
#include <concepts>
#include <ranges>
#include <string>
#include <type_traits>

namespace stralgo::inline v1_4::utf
  {
namespace concepts
  {
  ///\brief character types holding whole code points or UTF-16 code units, for which simple case mapping applies
  ///\note char and char8_t hold UTF-8 code units only, use ASCII stralgo::to_lower/to_upper for them
  template<typename value_type>
  concept case_mapped_char
    = std::same_as<std::remove_cvref_t<value_type>, char16_t> || std::same_as<std::remove_cvref_t<value_type>, char32_t>
      || std::same_as<std::remove_cvref_t<value_type>, wchar_t>;

  template<typename range_type>
  concept case_mapped_range
    = std::ranges::forward_range<range_type> && case_mapped_char<std::ranges::range_value_t<range_type>>;
  }  // namespace concepts

namespace detail
  {
  ///\returns simple lowercase mapping of code point or code point itself when there is no mapping
  [[nodiscard]]
  char32_t to_lower_code_point(char32_t code) noexcept;

  ///\returns simple uppercase mapping of code point or code point itself when there is no mapping
  [[nodiscard]]
  char32_t to_upper_code_point(char32_t code) noexcept;
  }  // namespace detail

//--------------------------------------------------------------------------------------------------------
///\brief Unicode simple lowercase mapping
///\note for char16_t surrogate code units are returned unchanged, no simple mapping crosses BMP boundary
struct to_lower_t
  {
  template<concepts::case_mapped_char char_type>
  [[nodiscard]]
  stralgo_static_call_operator auto operator()(char_type c) stralgo_static_call_operator_const noexcept -> char_type
    {
    return static_cast<char_type>(detail::to_lower_code_point(static_cast<char32_t>(c)));
    }
  };

inline constexpr to_lower_t to_lower;

//--------------------------------------------------------------------------------------------------------
///\brief Unicode simple uppercase mapping
///\note for char16_t surrogate code units are returned unchanged, no simple mapping crosses BMP boundary
struct to_upper_t
  {
  template<concepts::case_mapped_char char_type>
  [[nodiscard]]
  stralgo_static_call_operator auto operator()(char_type c) stralgo_static_call_operator_const noexcept -> char_type
    {
    return static_cast<char_type>(detail::to_upper_code_point(static_cast<char32_t>(c)));
    }
  };

inline constexpr to_upper_t to_upper;

//--------------------------------------------------------------------------------------------------------
///\brief compares two ranges of the same character type after simple lowercase mapping of each character,
/// result is negative, zero or positive value like std::char_traits::compare
struct compare_no_case_t
  {
  template<concepts::case_mapped_range range_type, concepts::case_mapped_range range_type2>
    requires std::same_as<std::ranges::range_value_t<range_type>, std::ranges::range_value_t<range_type2>>
  [[nodiscard]]
  stralgo_static_call_operator int
    operator()(range_type && s1, range_type2 && s2) stralgo_static_call_operator_const noexcept
    {
    using char_type = std::ranges::range_value_t<range_type>;
    using traits = std::char_traits<char_type>;

    auto first1{std::ranges::begin(s1)};
    auto const last1{std::ranges::end(s1)};
    auto first2{std::ranges::begin(s2)};
    auto const last2{std::ranges::end(s2)};
    for(; first1 != last1 && first2 != last2;)
      {
      char_type const l1{to_lower(*first1)};
      char_type const l2{to_lower(*first2)};
      if(!traits::eq(l1, l2))
        return traits::lt(l1, l2) ? -1 : 1;
      std::ranges::advance(first1, 1);
      std::ranges::advance(first2, 1);
      }
    if(first1 != last1)
      return 1;
    if(first2 != last2)
      return -1;
    return 0;
    }
  };

inline constexpr compare_no_case_t compare_no_case;
  }  // namespace stralgo::utf
