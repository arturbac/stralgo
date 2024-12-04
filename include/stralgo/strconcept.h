// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/stralgo
#pragma once

#include <stralgo/version.h>
#include <cstdint>
#include <ranges>
#include <type_traits>
#include <concepts>
#include <iterator>
#include <string>

#ifdef __clang__
#define stralgo_clang_do_pragma(x) _Pragma(#x)
#define stralgo_clang_unsafe_buffer_usage_begin stralgo_clang_do_pragma(clang unsafe_buffer_usage begin)
#define stralgo_clang_unsafe_buffer_usage_end stralgo_clang_do_pragma(clang unsafe_buffer_usage end)
#else
#define stralgo_clang_unsafe_buffer_usage_begin
#define stralgo_clang_unsafe_buffer_usage_end
#endif

namespace stralgo
  {
namespace ranges = std::ranges;
  }

namespace stralgo::inline v1_4::concepts
  {
template<typename type>
inline constexpr bool is_const = !std::is_same_v<std::remove_const_t<type>, type>;

template<typename iterator>
using dereferenced_type = decltype(*std::declval<iterator>());

template<typename from, typename to>
concept convertible = std::is_convertible_v<from, to>;

template<typename maybe_uint8_t>
concept integral_uint8 = std::same_as<maybe_uint8_t, uint8_t>;

template<typename maybe_enum_type>
concept enumeration = std::is_enum_v<maybe_enum_type>;

namespace detail
  {
  // solution for lack of <concepts> equality_comaprable_v with c++17
  // source https://stackoverflow.com/questions/6534041/how-to-check-whether-operator-exists
  template<class T, class EqualTo = T>
  struct has_operator_equal
    {
    template<class U, class V>
    static auto test(U *) -> decltype(std::declval<U>() == std::declval<V>());
    template<typename, typename>
    static auto test(...) -> std::false_type;

    static constexpr bool value = std::is_same<bool, decltype(test<T, EqualTo>(nullptr))>::value;
    };
  }  // namespace detail

namespace detail
  {
  template<class T>
  struct has_operator_index
    {
    template<class U>
    static auto test(U *) -> decltype(std::declval<U>()[0]);
    template<typename>
    static auto test(...) -> void;

    static constexpr bool value = !std::is_same<void, decltype(test<T>(nullptr))>::value;
    };
  }  // namespace detail

template<typename value_type>
concept indexable = detail::has_operator_index<value_type>::value;

template<typename iterator>
using iterator_category_t = typename std::iterator_traits<iterator>::iterator_category;

// using std::forward_iterator;
// using std::input_iterator;
// using std::output_iterator;
// using std::bidirectional_iterator;
// using std::random_access_iterator;
//
// using std::iter_value_t;

template<typename iterator>
using iterator_value_type_t = std::iter_value_t<iterator>;

template<class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template<typename value_type>
concept char_1b_type
  = std::same_as<remove_cvref_t<value_type>, char> || std::same_as<remove_cvref_t<value_type>, char8_t>;

template<typename value_type>
concept char_type
  = char_1b_type<value_type> || std::same_as<remove_cvref_t<value_type>, char16_t>
    || std::same_as<remove_cvref_t<value_type>, char32_t> || std::same_as<remove_cvref_t<value_type>, wchar_t>;

template<typename iterator_type>
concept char_iterator = std::forward_iterator<iterator_type> && char_type<std::iter_value_t<iterator_type>>;

template<typename iterator_type, typename char_type_>
concept char_output_iterator = std::output_iterator<iterator_type, char_type_> && char_type<char_type_>;

template<typename iterator_type>
concept typed_char_output_iterator = requires {
  typename std::iter_value_t<iterator_type>;
  requires char_output_iterator<iterator_type, std::iter_value_t<iterator_type>>;
};

template<typename range>
concept char_range = requires {
  requires ranges::forward_range<range>;
  requires char_type<ranges::range_value_t<range>>;
  // poisoning use of pointers and C arrays to allow using working with strings containing 0 for merge, compose etc
  // this allows in code pinpointing all inefficient pointer and arrays use
  // this also prevents ambiguity between const char [] from string literal and intended use of char arrays with 0
  requires !std::is_pointer_v<std::remove_cvref_t<range>>;
  requires !std::is_array_v<std::remove_cvref_t<range>>;
};

template<typename range>
concept char_contiguous_range = char_range<range> && ranges::contiguous_range<range>;

template<typename iterator_type>
concept ui8_iterator = std::forward_iterator<iterator_type> && std::same_as<uint8_t, std::iter_value_t<iterator_type>>;

template<typename iterator_type>
concept iterator_over_char_ranges
  = std::forward_iterator<iterator_type> && char_range<std::iter_value_t<iterator_type>>;

template<char_range string_view_type>
///\returns string or view char_type
using string_view_value_type = ranges::range_value_t<string_view_type>;

template<typename string_view_type, typename string_view_type2>
///\brief true when both views have same base char_type but views may be different types (strings or views)
concept same_range_type = requires {
  requires char_range<string_view_type>;
  requires char_range<string_view_type2>;
  requires std::same_as<ranges::range_value_t<string_view_type>, ranges::range_value_t<string_view_type2>>;
};

template<char_range string_view_type>
///\returns string type based on view base char_type
using string_by_value_type_t = std::basic_string<ranges::range_value_t<string_view_type>>;

template<char_type char_type>
///\brief returns string type based on char_type
using string_by_char_type_t = std::basic_string<char_type>;

using std::make_unsigned_t;

namespace detail
  {
  enum struct arg_type
    {
    invalid,
    single,
    range
    };

  template<typename T>
  consteval arg_type decl_arg_type() noexcept
    {
    if constexpr(char_type<T>)
      return arg_type::single;
    else if constexpr(char_range<T>)
      return arg_type::range;
    else
      return arg_type::invalid;
    }

  template<typename templ_type, arg_type>
  struct char_type_from_view
    {
    using type = void;
    };

  template<typename templ_type>
  struct char_type_from_view<templ_type, arg_type::single>
    {
    using type = templ_type;
    };

  template<typename templ_type>
  struct char_type_from_view<templ_type, arg_type::range>
    {
    using type = ranges::range_value_t<templ_type>;
    };
  }  // namespace detail

template<typename templ_type>
using char_type_from_view_t =
  typename detail::char_type_from_view<templ_type, detail::decl_arg_type<templ_type>()>::type;

template<typename char_type, typename templ_type>
concept match_char_type_or_void
  = std::same_as<char_type, char_type_from_view_t<templ_type>> || std::same_as<void, char_type_from_view_t<templ_type>>;

template<typename templ_type, typename... args>
struct unpack_first
  {
  using type = templ_type;
  };

template<typename templ_type, typename... args>
using unpack_first_t = typename unpack_first<templ_type, args...>::type;

namespace detail
  {
  template<typename T, typename... args>
  struct decl_chartype_from_args
    {
    static constexpr bool is_valid_type = decl_arg_type<T>() != detail::arg_type::invalid;
    using type
      = std::conditional_t<is_valid_type, char_type_from_view_t<T>, typename decl_chartype_from_args<args...>::type>;
    };

  template<typename T>
  struct decl_chartype_from_args<T>
    {
    static constexpr bool is_valid_type = decl_arg_type<T>() != detail::arg_type::invalid;
    using type = char_type_from_view_t<T>;  // maybe void if no char_type or char_range
    };
  }  // namespace detail

template<typename... args>
inline constexpr bool decl_chartype_from_args_has_valid_type_v
  = detail::decl_chartype_from_args<args...>::is_valid_type;

template<typename... args>
using decl_chartype_from_args_t = typename detail::decl_chartype_from_args<args...>::type;

  }  // namespace stralgo::concepts

#if defined(__cpp_static_call_operator)
#define stralgo_static_call_operator static
#define stralgo_static_call_operator_const
#else
#define stralgo_static_call_operator
#define stralgo_static_call_operator_const const
#endif
