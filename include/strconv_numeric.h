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

/*
 * char value_to_hex( uint8_t value )
 * 
 * constexpr output_iterator to_hex_ascii( iterator sbeg, iterator send, output_iterator outit )
 * auto to_hex_ascii( iterator sbeg, iterator send )
 * 
 * constexpr output_iterator from_hex_ascii(iterator sbeg, iterator send, output_iterator outit)
 * 
 * auto integral_to_string( value_type value ) noexcept
 *   alias str2int
 * 
 * <decimal_places = 6, trailing_zeros_e::skip>
 * auto float_to_string( value_type value ) noexcept
 *  alias f2str
 * 
 * constexpr integral_type string_to_integral( string_view_type str_number )
 *   alias int2str
 * 
 * constexpr float_type string_to_float( string_view_type str_number )
 *  alias str2f
 * 
 * auto compose( input_argument_type const & view, input_argument_type_n const & ... args )
 * */
namespace strconv
{

  //--------------------------------------------------------------------------------------------------------
  ///\returns char representation of single decimal value, value must be in range 0..15
  template<char_case_e char_case = char_case_e::uppercase, strconcept::integral_uint8 value_type>
  constexpr char value_to_hex( value_type value )
    {
    return detail::value_to_hex_(value);
    }
  //--------------------------------------------------------------------------------------------------------
  ///\brief converts binary representation to hexadecimal representation
  template<char_case_e char_case = char_case_e::uppercase,
           strconcept::forward_iterator iterator,
           strconcept::writable_iterator output_iterator>
  ///\warning \param outit must have double size capacity of source range
  constexpr output_iterator to_hex_ascii( iterator sbeg, iterator send, output_iterator outit )
    {
    return detail::to_hex_ascii_<char_case>(sbeg, send, outit);
    }
  
  //--------------------------------------------------------------------------------------------------------
  template<char_case_e char_case = char_case_e::uppercase,
           strconcept::char_type char_type,
           typename string_type = strconcept::string_by_value_type_t<char_type>,
           strconcept::forward_iterator iterator>
  constexpr auto to_hex_ascii( iterator sbeg, iterator send )
    {
    return detail::to_hex_ascii_<char_type,char_case,string_type>(sbeg, send);
    }

  //--------------------------------------------------------------------------------------------------------
  ///\brief converts hexadecimal representation to binary representation
  template<strconcept::forward_iterator iterator, strconcept::writable_iterator output_iterator>
  ///\warning \param outit must have half the size capacity of source range and soruce range must be multiple of 2
  constexpr output_iterator from_hex_ascii(iterator sbeg, iterator send, output_iterator outit)
    {
    return detail::from_hex_ascii_(sbeg,send,outit);
    }
    
  //--------------------------------------------------------------------------------------------------------
  ///\brief estimates size info for integral_to_string with output_iterator
  template<integral_format_traits traits, strconcept::integral value_type>
  constexpr auto estimate_integral_to_str( value_type value )
    {
    return detail::estimate_integral_to_str_<traits>(value);
    }
    
  ///\brief converts itnergral signed or unsigned type to string using specified formating traits
  template<integral_format_traits traits = integral_format_traits{},
           strconcept::writable_iterator output_iterator,
           strconcept::integral value_type>
  [[nodiscard]]
  constexpr output_iterator integral_to_string( value_type value, output_iterator oit ) noexcept
    {
    return detail::integral_to_string_<traits>(value, oit );
    }
  
  ///\brief converts itnergral signed or unsigned type to string using specified formating traits
  template<integral_format_traits traits = integral_format_traits{},
           strconcept::char_type char_type = char, 
           typename string_type = strconcept::string_by_char_type_t<char_type>,
           strconcept::integral value_type
          >
  [[nodiscard]]
  auto integral_to_string( value_type value ) noexcept
    {
    return detail::integral_to_string_<traits,char_type,string_type>(value);
    }
  
  ///\brief converts itnergral signed or unsigned type to string using specified formating traits
  template<integral_format_traits traits = integral_format_traits{},
           strconcept::char_type char_type = char,
           strconcept::integral value_type,
    typename string_type = strconcept::string_by_char_type_t<char_type>>
  auto int2str( value_type value ) noexcept
    { return detail::integral_to_string_<traits,char_type,string_type>(value); }
  
  //--------------------------------------------------------------------------------------------------------
  ///\brief estimates requires space for given value and formating traits, first pass of convertion, est_info.size() may be used to obtaion requires storage size
  template<float_format_traits traits, strconcept::floating_point float_type>
  [[nodiscard]]
  constexpr auto estimate_float_to_string( float_type value ) noexcept
    { return detail::estimate_float_to_string_<traits>(value); }
  
  ///\brief converts number into string using precalculated info, final pass
  template<float_format_traits traits,
          strconcept::writable_iterator output_iterator,
          strconcept::floating_point float_type>
  [[nodiscard]]
  constexpr output_iterator float_to_string( detail::float_estimate_info_t<float_type> const & est_info, output_iterator oit ) noexcept
    {
    return detail::float_to_string_<traits>(est_info,oit);
    }
    
  template<float_format_traits traits = float_format_traits{},
          strconcept::writable_iterator output_iterator,
          strconcept::floating_point float_type>
  [[nodiscard]]
  constexpr output_iterator float_to_string( float_type value, output_iterator oit ) noexcept
    {
    auto est_info{ detail::estimate_float_to_string_<traits>(value) };
    return detail::float_to_string_<traits>(est_info,oit);
    }
    
  ///\brief converts floating point type to string
  template<float_format_traits traits = float_format_traits{},
            strconcept::char_type char_type = char,
            typename string_type = strconcept::string_by_char_type_t<char_type>,
            strconcept::floating_point value_type>
  [[nodiscard]]
  auto float_to_string( value_type value ) noexcept
    {
    return detail::float_to_string_<traits, char_type,string_type>(value);
    }
  
  ///\brief converts floating point type to string, alias
  template<float_format_traits traits = float_format_traits{},
            strconcept::char_type char_type = char,
            typename string_type = strconcept::string_by_char_type_t<char_type>,
            strconcept::floating_point value_type>
  [[nodiscard]]
  auto f2str( value_type value ) noexcept
    {
    return detail::float_to_string_<traits,char_type,string_type>(value);
    }

  //--------------------------------------------------------------------------------------------------------
  ///\brief integral convertion from string supports untrimed strings of decimal [+/-]d[n] and hexadecimal lower and uppercase [+/-]0xh[n] numbers
  ///\return pair of decoded integral value and iterator to source view pass the last parsed character
  template<strconcept::integral integral_type,
           input_format_e input_format = input_format_e::undetermined,
           strconcept::convertible_to_string_view string_view_type>
  constexpr auto string_to_integral( string_view_type str_number ) 
    {
     return detail::string_to_integral_<integral_type,input_format>(str_number);
    }

  //--------------------------------------------------------------------------------------------------------
  ///\brief integral convertion from string supports untrimed strings of decimal [+/-]d[n] and hexadecimal lower and uppercase [+/-]0xh[n] numbers
  ///\return pair of decoded integral value and iterator pass the last parsed character
  template<strconcept::integral integral_type,
           input_format_e input_format = input_format_e::undetermined,
           strconcept::convertible_to_string_view string_view_type>
  constexpr auto str2int( string_view_type str_number ) 
    { return detail::string_to_integral_<integral_type,input_format>(str_number); }
      
  //--------------------------------------------------------------------------------------------------------
  ///\brief signed float convertion from string supports untrimed strings of decimal notation [+/-]d[n] and hexadecimal lower and uppercase [+/-]0xh[n] numbers
  ///\return pair of value with pass the end iterator to source string view
  template<strconcept::floating_point float_type,
           strconcept::convertible_to_string_view string_view_type>
  constexpr auto string_to_float( string_view_type str_number ) 
    {
    return detail::string_to_float_<float_type>( str_number );
    }
  //--------------------------------------------------------------------------------------------------------
  //alias
  template<strconcept::floating_point float_type,
           strconcept::convertible_to_string_view string_view_type>
  constexpr auto str2f( string_view_type str_number ) 
    {
    return detail::string_to_float_<float_type>(str_number);
    }
    
  //--------------------------------------------------------------------------------------------------------
  //compose
  //--------------------------------------------------------------------------------------------------------

  ///\brief composes a string from multiple arguments, arguments other that char/string are first converted into string representation
  template<strconcept::char_type char_type = char, typename ... input_argument_type_n>
  [[nodiscard]]
  auto compose(input_argument_type_n const & ... args) noexcept
    {
    return detail::compose_(args ...);
    }
    
  using detail::fmt;

}

