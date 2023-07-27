//--------------------------------------------------------------------------
// string view manipulation algorithms without dependency on null termination
// {C} Artur Bac 2021
//--------------------------------------------------------------------------
#pragma once

#include <stralgo/detail/stralgo.h>
#include <algorithm>
#include <numeric>
#include <cwctype>
#include <iterator>

namespace stralgo
{
  using detail::isblank;
  using detail::isspace;
  using detail::isdigit;
  using detail::isxdigit;
  using detail::islower;
  using detail::isupper;
  using detail::to_lower;
  using detail::to_upper;
  using detail::find_first_of;

  //--------------------------------------------------------------------------------------------------------
  using detail::trim_left_with_pred;
  
  struct trim_left_t
    {
    ///\returns trimmed left view of \ref view from values that match isspace, if string_view_type meets ranges::contiguous_range concept then return type is basic_string_view and ranges::subrange if not.
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( concepts::char_range auto const & view)
      stralgo_static_call_operator_const noexcept
      { return detail::trim_left_with_pred( view, detail::not_is_space ); }
      
    ///\returns trimmed left view \ref view from values that match value, if string_view_type meets ranges::contiguous_range concept then return type is basic_string_view and ranges::subrange if not.
    template<concepts::char_range string_view_type, concepts::char_type char_type>
      requires std::same_as<ranges::range_value_t<string_view_type>,char_type> 
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( string_view_type const & view, char_type value)
        stralgo_static_call_operator_const noexcept
      { return detail::trim_left_with_pred( view, detail::not_is_char_pred_t<char_type>{value} ); }
      
    ///\returns trimmed left view \ref view from values that match any chars in any_of, if string_view_type meets ranges::contiguous_range concept then return type is basic_string_view and ranges::subrange if not.
    template<concepts::char_range string_view_type, concepts::char_range string_view_type2>
      requires concepts::same_range_type<string_view_type,string_view_type2>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( string_view_type const & view, string_view_type2 const & any_of)
        stralgo_static_call_operator_const noexcept
      { return detail::trim_left_with_pred( view, detail::not_is_any_of{ ranges::begin(any_of), ranges::end(any_of) }); }
    };
  inline constexpr trim_left_t trim_left;

  //--------------------------------------------------------------------------------------------------------
  using detail::trim_right_with_pred;
  
  struct trim_right_t
    {
    ///\returns trimmed right view of \ref view from values that match isspace,if string_view_type meets ranges::contiguous_range concept then return type is basic_string_view and ranges::subrange if not.
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( concepts::char_range auto const & view)
        stralgo_static_call_operator_const noexcept
      { return detail::trim_right_with_pred( view, detail::not_is_space ); }
      
    ///\returns trimmed right view of \ref view from values that match value,if string_view_type meets ranges::contiguous_range concept then return type is basic_string_view and ranges::subrange if not.
    template<concepts::char_range string_view_type, concepts::char_type char_type>
      requires std::same_as<ranges::range_value_t<string_view_type>,char_type>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( string_view_type const & view, char_type value)
        stralgo_static_call_operator_const noexcept
      { return detail::trim_right_with_pred( view, detail::not_is_char_pred_t<char_type>{value} ); }
      
    ///\returns trimmed right view of \ref view from values that match any chars in any_of,if string_view_type meets ranges::contiguous_range concept then return type is basic_string_view and ranges::subrange if not.
    template<concepts::char_range string_view_type, concepts::char_range string_view_type2>
      requires concepts::same_range_type<string_view_type,string_view_type2>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( string_view_type const & view, string_view_type2 const & any_of)
        stralgo_static_call_operator_const noexcept
      { return detail::trim_right_with_pred( view, detail::not_is_any_of{ ranges::begin(any_of), ranges::end(any_of) }); }
    };
  inline constexpr trim_right_t trim_right;
  //--------------------------------------------------------------------------------------------------------
  using detail::npos;
  using detail::substr;
  using detail::left;
  using detail::right;

  //--------------------------------------------------------------------------------------------------------
  using detail::trim_pred;
  
  struct trim_t
    {
    ///\returns trimmed view of \param view from values that match isspace
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( concepts::char_range auto const & view)
        stralgo_static_call_operator_const noexcept
      { return detail::trim_pred( view, detail::not_is_space ); }

    ///\returns trimmed view of \param view from values that match value
    template<concepts::char_range string_view_type, concepts::char_type char_type>
        requires std::same_as<ranges::range_value_t<string_view_type>,char_type>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( string_view_type const & view, char_type value)
        stralgo_static_call_operator_const noexcept
      { return detail::trim_pred( view, detail::not_is_char_pred_t<char_type>{value} ); }
      
    ///\returns trimmed view of \param view from values that match any chars in any_of
    template<concepts::char_range string_view_type, concepts::char_range string_view_type2>
      requires concepts::same_range_type<string_view_type,string_view_type2>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( string_view_type const & view, string_view_type2 const & any_of)
        stralgo_static_call_operator_const noexcept
      { return detail::trim_pred( view, detail::not_is_any_of{ ranges::begin(any_of), ranges::end(any_of) }); }
    };
  inline constexpr trim_t trim;
  //--------------------------------------------------------------------------------------------------------
  using detail::merge;
  using detail::merge_range;
  namespace stl
    {
    using detail::stl::merge;
    using detail::stl::merge_range;
    }
  using detail::ends_with;
  using detail::starts_with;
  using detail::compare_no_case;
  using detail::is_number;
  using detail::is_hexnumber;

  }

  
