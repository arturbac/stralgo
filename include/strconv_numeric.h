//--------------------------------------------------------------------------
// string view manipulation algorithms without dependency on null termination
// {C} Artur Bac 2021
//--------------------------------------------------------------------------
#pragma once

#include <stralgo.h>
#include <strconv_numeric_common.h>
#include <detail/strconv_numeric.h>
#include <array>
#include <numeric>
#include <cstdint>
#include <cassert>
#include <cmath>

namespace stralgo
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
  using detail::string_to_integral;
  using detail::string_to_float;
    
  //--------------------------------------------------------------------------------------------------------
  //compose
  //--------------------------------------------------------------------------------------------------------

  ///\brief composes a string from multiple arguments, arguments other that char/string are first converted into string representation
  template<template<typename > typename basic_string_type>
  struct compose_t
    {
    template<typename ... input_argument_type_n>
      requires (sizeof ...(input_argument_type_n) > 1 )
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()(input_argument_type_n const & ... args)
        stralgo_static_call_operator_const
      {
      using char_type = concepts::decl_chartype_from_args_t<input_argument_type_n...>;
      return detail::compose_t<basic_string_type,char_type>{}(args...);
      }
    };

  inline constexpr compose_t<coll::basic_string> compose;
  namespace stl
    {
    inline constexpr compose_t<detail::stl::basic_string> compose;
    }
  using detail::fmt;
#if 0

  //--------------------------------------------------------------------------------------------------------
  
  template<stralgo::concepts::forward_iterator iterator, stralgo::concepts::writable_iterator output_iterator>
  
  constexpr output_iterator from_hex_ascii(iterator sbeg, iterator send, output_iterator outit)
    {
    return detail::from_hex_ascii_(sbeg,send,outit);
    }
    
  //--------------------------------------------------------------------------------------------------------
  ///\brief estimates size info for integral_to_string with output_iterator
  template<integral_format_traits traits, stralgo::concepts::integral value_type>
  constexpr auto estimate_integral_to_str( value_type value )
    {
    return detail::estimate_integral_to_str_<traits>(value);
    }
    
  
  template<integral_format_traits traits = integral_format_traits{},
           stralgo::concepts::writable_iterator output_iterator,
           stralgo::concepts::integral value_type>
  [[nodiscard]]
  constexpr output_iterator integral_to_string( value_type value, output_iterator oit ) noexcept
    {
    return detail::integral_to_string_<traits>(value, oit );
    }
  
  ///\brief converts itnergral signed or unsigned type to string using specified formating traits
  template<integral_format_traits traits = integral_format_traits{},
           stralgo::concepts::char_type char_type = char, 
           typename string_type = stralgo::concepts::string_by_char_type_t<char_type>,
           stralgo::concepts::integral value_type
          >
  [[nodiscard]]
  auto integral_to_string( value_type value ) noexcept
    {
    return detail::integral_to_string_<traits,char_type,string_type>(value);
    }
  
  ///\brief converts itnergral signed or unsigned type to string using specified formating traits
  template<integral_format_traits traits = integral_format_traits{},
           stralgo::concepts::char_type char_type = char,
           stralgo::concepts::integral value_type,
    typename string_type = stralgo::concepts::string_by_char_type_t<char_type>>
  auto int2str( value_type value ) noexcept
    { return detail::integral_to_string_<traits,char_type,string_type>(value); }
  
  //--------------------------------------------------------------------------------------------------------
  ///\brief estimates requires space for given value and formating traits, first pass of convertion, est_info.size() may be used to obtaion requires storage size
  template<float_format_traits traits, stralgo::concepts::floating_point float_type>
  [[nodiscard]]
  constexpr auto estimate_float_to_string( float_type value ) noexcept
    { return detail::estimate_float_to_string_<traits>(value); }
  
  ///\brief converts number into string using precalculated info, final pass
  template<float_format_traits traits,
          stralgo::concepts::writable_iterator output_iterator,
          stralgo::concepts::floating_point float_type>
  [[nodiscard]]
  constexpr output_iterator float_to_string( detail::float_estimate_info_t<float_type> const & est_info, output_iterator oit ) noexcept
    {
    return detail::float_to_string_<traits>(est_info,oit);
    }
    
  template<float_format_traits traits = float_format_traits{},
          stralgo::concepts::writable_iterator output_iterator,
          stralgo::concepts::floating_point float_type>
  [[nodiscard]]
  constexpr output_iterator float_to_string( float_type value, output_iterator oit ) noexcept
    {
    auto est_info{ detail::estimate_float_to_string_<traits>(value) };
    return detail::float_to_string_<traits>(est_info,oit);
    }
    
  ///\brief converts floating point type to string
  template<float_format_traits traits = float_format_traits{},
            stralgo::concepts::char_type char_type = char,
            typename string_type = stralgo::concepts::string_by_char_type_t<char_type>,
            stralgo::concepts::floating_point value_type>
  [[nodiscard]]
  auto float_to_string( value_type value ) noexcept
    {
    return detail::float_to_string_<traits, char_type,string_type>(value);
    }
  
  ///\brief converts floating point type to string, alias
  template<float_format_traits traits = float_format_traits{},
            stralgo::concepts::char_type char_type = char,
            typename string_type = stralgo::concepts::string_by_char_type_t<char_type>,
            stralgo::concepts::floating_point value_type>
  [[nodiscard]]
  auto f2str( value_type value ) noexcept
    {
    return detail::float_to_string_<traits,char_type,string_type>(value);
    }

  //--------------------------------------------------------------------------------------------------------
  ///\brief integral convertion from string supports untrimed strings of decimal [+/-]d[n] and hexadecimal lower and uppercase [+/-]0xh[n] numbers
  ///\return pair of decoded integral value and iterator to source view pass the last parsed character
  template<stralgo::concepts::integral integral_type,
           input_format_e input_format = input_format_e::undetermined,
           stralgo::concepts::convertible_to_string_view string_view_type>
  constexpr auto string_to_integral( string_view_type str_number ) 
    {
     return detail::string_to_integral_<integral_type,input_format>(str_number);
    }

  //--------------------------------------------------------------------------------------------------------
  ///\brief integral convertion from string supports untrimed strings of decimal [+/-]d[n] and hexadecimal lower and uppercase [+/-]0xh[n] numbers
  ///\return pair of decoded integral value and iterator pass the last parsed character
  template<stralgo::concepts::integral integral_type,
           input_format_e input_format = input_format_e::undetermined,
           stralgo::concepts::convertible_to_string_view string_view_type>
  constexpr auto str2int( string_view_type str_number ) 
    { return detail::string_to_integral_<integral_type,input_format>(str_number); }
      
  //--------------------------------------------------------------------------------------------------------
  ///\brief signed float convertion from string supports untrimed strings of decimal notation [+/-]d[n] and hexadecimal lower and uppercase [+/-]0xh[n] numbers
  ///\return pair of value with pass the end iterator to source string view
  template<stralgo::concepts::floating_point float_type,
           stralgo::concepts::convertible_to_string_view string_view_type>
  constexpr auto string_to_float( string_view_type str_number ) 
    {
    return detail::string_to_float_<float_type>( str_number );
    }
  //--------------------------------------------------------------------------------------------------------
  //alias
  template<stralgo::concepts::floating_point float_type,
           stralgo::concepts::convertible_to_string_view string_view_type>
  constexpr auto str2f( string_view_type str_number ) 
    {
    return detail::string_to_float_<float_type>(str_number);
    }

#endif
}

