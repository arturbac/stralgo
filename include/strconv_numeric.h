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
  template<char_case_e char_case = char_case_e::uppercase>
  constexpr char value_to_hex( uint8_t value )
    {
    return detail::value_to_hex_(value);
    }
  //--------------------------------------------------------------------------------------------------------
  ///\brief converts binary representation to hexadecimal representation
  template<char_case_e char_case = char_case_e::uppercase,
           typename iterator, typename output_iterator,
           typename = std::enable_if_t< strconcept::is_forward_iterator_v<iterator>
                                     && strconcept::is_writable_iterator_v<output_iterator>>
          >
  ///\warning \param outit must have double size capacity of source range
  constexpr output_iterator to_hex_ascii( iterator sbeg, iterator send, output_iterator outit )
    {
    return detail::to_hex_ascii_<char_case>(sbeg, send, outit);
    }
  
  //--------------------------------------------------------------------------------------------------------
  template<char_case_e char_case = char_case_e::uppercase,
           typename char_type,
           typename string_type = strconcept::string_by_value_type_t<char_type>,
           typename iterator, 
           typename = std::enable_if_t<strconcept::is_char_type_v<char_type>
                                    && strconcept::is_forward_iterator_v<iterator>>
            >
  constexpr auto to_hex_ascii( iterator sbeg, iterator send )
    {
    return detail::to_hex_ascii_<char_type,char_case,string_type>(sbeg, send);
    }

  //--------------------------------------------------------------------------------------------------------
  ///\brief converts hexadecimal representation to binary representation
  template<typename iterator, typename output_iterator,
    typename = std::enable_if_t<strconcept::is_forward_iterator_v<iterator>
                             && strconcept::is_writable_iterator_v<output_iterator>>
          >
  ///\warning \param outit must have half the size capacity of source range and soruce range must be multiple of 2
  constexpr output_iterator from_hex_ascii(iterator sbeg, iterator send, output_iterator outit)
    {
    return detail::from_hex_ascii_(sbeg,send,outit);
    }
    
  //--------------------------------------------------------------------------------------------------------
  ///\brief estimates size info for integral_to_string with output_iterator
  template<integral_format_traits traits, typename value_type,
           typename = std::enable_if_t<std::is_integral_v<value_type>>
           >
  constexpr auto estimate_integral_to_str( value_type value )
    {
    return detail::estimate_integral_to_str_<traits>(value);
    }
    
  ///\brief converts itnergral signed or unsigned type to string using specified formating traits
  template<integral_format_traits traits = integral_format_traits{},
           typename output_iterator, typename value_type,
           typename = std::enable_if_t<std::is_integral_v<value_type> 
                                    && strconcept::is_writable_iterator_v<output_iterator>>
    >
  [[nodiscard]]
  constexpr output_iterator integral_to_string( value_type value, output_iterator oit ) noexcept
    {
    return detail::integral_to_string_<traits>(value, oit );
    }
  
  ///\brief converts itnergral signed or unsigned type to string using specified formating traits
  template<integral_format_traits traits = integral_format_traits{},
           typename char_type = char, 
           typename string_type = strconcept::string_by_char_type_t<char_type>,
           typename value_type,
           typename = std::enable_if_t<std::is_integral_v<value_type>
                             && strconcept::is_char_type_v<char_type>>
          >
  [[nodiscard]]
  auto integral_to_string( value_type value ) noexcept
    {
    return detail::integral_to_string_<traits,char_type,string_type>(value);
    }
  
  ///\brief converts itnergral signed or unsigned type to string using specified formating traits
  template<typename char_type = char, integral_format_traits traits = integral_format_traits{}, typename value_type,
    typename string_type = strconcept::string_by_char_type_t<char_type>,
    typename = std::enable_if_t<std::is_integral_v<value_type>
                             && strconcept::is_char_type_v<char_type>>
          >
  auto int2str( value_type value ) noexcept
    { return detail::integral_to_string_<char_type,traits,value_type,string_type>(value); }
  
  //--------------------------------------------------------------------------------------------------------
  ///\brief estimates requires space for given value and formating traits, first pass of convertion, est_info.size() may be used to obtaion requires storage size
  template<float_format_traits traits, typename float_type,
          typename = std::enable_if_t<std::is_floating_point_v<float_type>>
          >
  [[nodiscard]]
  constexpr auto estimate_float_to_string( float_type value ) noexcept
    { return detail::estimate_float_to_string_<traits>(value); }
  
  ///\brief converts number into string using precalculated info, final pass
  template<float_format_traits traits, typename output_iterator, typename float_type,
          typename = std::enable_if_t<std::is_floating_point_v<float_type> 
                                   && strconcept::is_writable_iterator_v<output_iterator>>
          >
  [[nodiscard]]
  constexpr output_iterator float_to_string( detail::float_estimate_info_t<float_type> const & est_info, output_iterator oit ) noexcept
    {
    return detail::float_to_string_<traits>(est_info,oit);
    }
    
  template<float_format_traits traits = float_format_traits{},
          typename output_iterator,
          typename float_type,
          typename = std::enable_if_t<std::is_floating_point_v<float_type> 
                                   && strconcept::is_writable_iterator_v<output_iterator>>
          >
  [[nodiscard]]
  constexpr output_iterator float_to_string( float_type value, output_iterator oit ) noexcept
    {
    auto est_info{ detail::estimate_float_to_string_<traits>(value) };
    return detail::float_to_string_<traits>(est_info,oit);
    }
    
  ///\brief converts floating point type to string
  template<float_format_traits traits = float_format_traits{},
            typename char_type = char,
            typename string_type = strconcept::string_by_char_type_t<char_type>,
            typename value_type,
            typename = std::enable_if_t<std::is_floating_point_v<value_type>
                                   && strconcept::is_char_type_v<char_type>>>
  [[nodiscard]]
  auto float_to_string( value_type value ) noexcept
    {
    return detail::float_to_string_<traits, char_type,string_type>(value);
    }
  
  ///\brief converts floating point type to string, alias
  template<float_format_traits traits = float_format_traits{},
            typename char_type = char,
            typename string_type = strconcept::string_by_char_type_t<char_type>,
            typename value_type,
            typename = std::enable_if_t<std::is_floating_point_v<value_type>
                                   && strconcept::is_char_type_v<char_type>>>
  [[nodiscard]]
  auto f2str( value_type value ) noexcept
    {
    return detail::float_to_string_<traits,char_type,string_type>(value);
    }

  //--------------------------------------------------------------------------------------------------------
  ///\brief integral convertion from string supports untrimed strings of decimal [+/-]d[n] and hexadecimal lower and uppercase [+/-]0xh[n] numbers
  ///\return pair of decoded integral value and iterator to source view pass the last parsed character
  template<typename integral_type,
           input_format_e input_format = input_format_e::undetermined,
           typename string_view_type,
           typename = std::enable_if_t< std::is_integral_v<integral_type> &&
                      strconcept::is_convertible_to_string_view_v<string_view_type>>>
  constexpr auto string_to_integral( string_view_type str_number ) 
    {
     return detail::string_to_integral_<integral_type,input_format>(str_number);
    }

  //--------------------------------------------------------------------------------------------------------
  ///\brief integral convertion from string supports untrimed strings of decimal [+/-]d[n] and hexadecimal lower and uppercase [+/-]0xh[n] numbers
  ///\return pair of decoded integral value and iterator pass the last parsed character
  template<typename integral_type,
           input_format_e input_format = input_format_e::undetermined,
           typename string_view_type,
           typename = std::enable_if_t< std::is_integral_v<integral_type> &&
                      strconcept::is_convertible_to_string_view_v<string_view_type>>>
  constexpr auto str2int( string_view_type str_number ) 
    { return detail::string_to_integral_<integral_type,input_format>(str_number); }
      
  //--------------------------------------------------------------------------------------------------------
  ///\brief signed float convertion from string supports untrimed strings of decimal notation [+/-]d[n] and hexadecimal lower and uppercase [+/-]0xh[n] numbers
  template<typename float_type, typename string_view_type,
    typename = std::enable_if_t< std::is_floating_point_v<float_type> &&
                      strconcept::is_convertible_to_string_view_v<string_view_type>>>
  constexpr float_type string_to_float( string_view_type str_number ) 
    {
    detail::string_to_float_( str_number );
    }
  //--------------------------------------------------------------------------------------------------------
  //alias
  template<typename float_type, typename string_view_type,
    std::enable_if_t< std::is_floating_point_v<float_type> &&
                      strconcept::is_convertible_to_string_view_v<string_view_type>, bool> = true>
  constexpr float_type str2f( string_view_type str_number ) 
    { return string_to_float<float_type>(str_number); }
  //--------------------------------------------------------------------------------------------------------
  //compose
  //--------------------------------------------------------------------------------------------------------


    
  ///\brief composes a string from multiple arguments, arguments other that char/string are first converted into string representation
  template<typename char_type = char, typename ... input_argument_type_n>
  [[nodiscard]]
  auto compose(input_argument_type_n const & ... args) noexcept
    {
    return detail::compose_(args ...);
    }
    
  using detail::fmt;
//   template<typename char_type = char,
//            typename input_argument_type, typename ... input_argument_type_n,
//     std::enable_if_t<
//       strconcept::is_char_type_v<input_argument_type> ||
//       (std::is_convertible_v<input_argument_type, std::basic_string_view<char_type>> && 
//             (!std::is_array_v< strconcept::remove_cvref_t<input_argument_type>> && !std::is_pointer_v< strconcept::remove_cvref_t<input_argument_type>>)) ||
//       std::is_integral_v<input_argument_type> ||
//       std::is_floating_point_v<input_argument_type>
//       , bool> = true>
//   [[nodiscard]]
//   auto compose( input_argument_type const & view, input_argument_type_n const & ... args ) noexcept
//     {
//     static_assert(sizeof...(input_argument_type_n) != 0);
//     using string_type = strconcept::string_by_char_type_t<char_type>;
//     using size_type = typename string_type::size_type;
//     size_type aprox_size{ static_cast<size_type>(detail::count_size<char_type>(view,args...))};
//     string_type result;
//     result.resize(aprox_size);
//     char_type * buff{ result.data() };
//     auto new_end { detail::copy_views<char_type>( buff, view, args ... ) };
//     auto new_size{ static_cast<size_type>(std::distance(result.data(),new_end)) };
//     assert(new_size<=aprox_size);
//     result.resize( new_size );
//     return result;
//     }
}

