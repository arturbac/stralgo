// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/stralgo
#pragma once

#include <stralgo/stralgo.h>
#include <stralgo/strconv_numeric_common.h>
#include <stralgo/detail/strconv_numeric.h>
#include <array>
#include <numeric>
#include <cstdint>
#include <cassert>
#include <cmath>

namespace stralgo::inline v1_4
  {

//--------------------------------------------------------------------------------------------------------
using detail::value_to_hex;

///\warning \ref outit must have double size capacity of source range
using detail::to_hex_ascii;
using detail::to_hex_ascii_string;

namespace stl
  {
  using detail::stl::to_hex_ascii_string;
  }

using detail::from_xdigit;

///\brief converts hexadecimal representation to binary representation
///\warning \ref outit must have half the size capacity of source range and source range must be multiple of 2
using detail::from_hex_ascii;

///\brief converts itnergral signed or unsigned type to string using specified formatting traits
using detail::integral_to_ascii;
using detail::integral_to_string;

namespace stl
  {
  using detail::stl::integral_to_string;
  }

using detail::float_to_ascii;
using detail::float_to_string;

namespace stl
  {
  using detail::stl::float_to_string;
  }

using detail::string_to_float;
using detail::string_to_integral;

//--------------------------------------------------------------------------------------------------------
// compose
//--------------------------------------------------------------------------------------------------------

///\brief composes a string from multiple arguments, arguments other that char/string are first converted into string
///representation
template<template<typename> typename basic_string_type>
struct compose_t
  {
  template<typename... input_argument_type_n>
    requires(sizeof...(input_argument_type_n) > 1)
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(input_argument_type_n const &... args) stralgo_static_call_operator_const
    {
    using char_type = concepts::decl_chartype_from_args_t<input_argument_type_n...>;
    return detail::compose_t<basic_string_type, char_type>{}(args...);
    }
  };

inline constexpr compose_t<small_vectors::basic_string> compose;

namespace stl
  {
  inline constexpr compose_t<detail::stl::basic_string> compose;
  }

using detail::fmt;

///\brief converts integral signed or unsigned type to coll::basic_string using specified formatting traits
template<
  integral_format_traits traits = integral_format_traits{},
  stralgo::concepts::char_type char_type = char,
  std::integral value_type>
inline constexpr auto int2str(value_type value)
  {
  return integral_to_string<char_type, traits>(value);
  }

namespace stl
  {
  template<
    integral_format_traits traits = integral_format_traits{},
    stralgo::concepts::char_type char_type = char,
    std::integral value_type>
  ///\brief converts integral signed or unsigned type to std::basic_string using specified formatting traits
  inline constexpr auto int2str(value_type value)
    {
    return stl::integral_to_string<char_type, traits>(value);
    }
  }  // namespace stl

///\brief converts floating point type to coll::basic_string, alias
template<
  float_format_traits traits = float_format_traits{},
  stralgo::concepts::char_type char_type = char,
  std::floating_point value_type>
[[nodiscard]]
inline constexpr auto f2str(value_type value)
  {
  return float_to_string<char_type, traits>(value);
  }

namespace stl
  {
  ///\brief converts floating point type to std::basic_string, alias
  template<
    float_format_traits traits = float_format_traits{},
    stralgo::concepts::char_type char_type = char,
    std::floating_point value_type>
  [[nodiscard]]
  inline constexpr auto f2str(value_type value)
    {
    return stl::float_to_string<char_type, traits>(value);
    }
  }  // namespace stl

//--------------------------------------------------------------------------------------------------------
///\brief integral conversion from string supports untrimmed strings of decimal [+/-]d[n] and hexadecimal lower and
///uppercase [+/-]0xh[n] numbers \return pair of decoded integral value and iterator pass the last parsed character
template<
  std::integral integral_type,
  input_format_e input_format = input_format_e::undetermined,
  stralgo::concepts::char_range string_view_type>
inline constexpr auto str2int(string_view_type && str_number) noexcept
  {
  return string_to_integral<integral_type, input_format>(str_number);
  }

//--------------------------------------------------------------------------------------------------------
///\brief signed float conversion from string supports untrimmed strings of decimal notation [+/-]d[n] and hexadecimal
///lower and uppercase [+/-]0xh[n] numbers
template<std::floating_point float_type, stralgo::concepts::char_range string_view_type>
inline constexpr auto str2f(string_view_type && str_number) noexcept
  {
  return string_to_float<float_type>(str_number);
  }

  }  // namespace stralgo

