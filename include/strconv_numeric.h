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
  template<typename char_type = char, integral_format_traits traits = integral_format_traits{}, typename value_type,
    typename string_type = strconcept::string_by_char_type_t<char_type>,
    typename = std::enable_if_t<std::is_integral_v<value_type>
                             && strconcept::is_char_type_v<char_type>>
          >
  [[nodiscard]]
  auto integral_to_string( value_type value ) noexcept
    {
    return detail::integral_to_string_<char_type,traits,value_type,string_type>(value);
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
  ///\brief converts floating point type to string
  template<typename char_type = char, uint32_t decimal_places = detail::default_decimal_places,
            trailing_zeros_e trailing = trailing_zeros_e::skip,
            typename string_type = strconcept::string_by_char_type_t<char_type>,
            typename value_type,
            typename = std::enable_if_t<std::is_floating_point_v<value_type>
                                   && strconcept::is_char_type_v<char_type>>>
  [[nodiscard]]
  auto float_to_string( value_type value ) noexcept
    {
    return detail::float_to_string_<char_type,decimal_places,trailing,string_type>(value);
    }
    
  template<typename char_type = char, uint32_t decimal_places = detail::default_decimal_places,
            trailing_zeros_e trailing = trailing_zeros_e::skip,
            typename string_type = strconcept::string_by_char_type_t<char_type>,
            typename value_type,
            typename = std::enable_if_t<std::is_floating_point_v<value_type>
                                   && strconcept::is_char_type_v<char_type>>>
  [[nodiscard]]
  auto f2str( value_type value ) noexcept
    {
    return detail::float_to_string_<char_type,decimal_places,trailing,string_type>(value);
    }

  //--------------------------------------------------------------------------------------------------------
  namespace detail
    {

    template<typename integral_type, typename base_conv_t, typename iterator>
    constexpr integral_type trimed_string_to_unsigned_integral( iterator beg, iterator end) 
      {
      using char_type = strconcept::remove_cvref_t<decltype(*beg)>;
      
      integral_type total{};
      auto it{ beg };

      while( it != end )
        {
        char_type c { *it };
        if(base_conv_t::is_number(c))
          {
          total = base_conv_t::base * total + base_conv_t:: template convert<integral_type>(c);
          ++it;
          }
        else
          break;
        }
      return total;
      }

    template<typename char_type>
    constexpr bool is_hex_prefix( char_type c0, char_type c1 )
      {
      return c0 == char_type('0') && ( c1 == char_type('x') || c1 == char_type('X') );
      }

    }
  //--------------------------------------------------------------------------------------------------------
  ///\brief unsigned integral convertion from string supports untrimed strings of decimal [+]d[n] and hexadecimal [+]0xh[n] numbers
  template<typename integral_type, typename string_view_type,
    std::enable_if_t< std::is_unsigned_v<integral_type> &&
                      strconcept::is_convertible_to_string_view_v<string_view_type>, bool> = true>
  constexpr integral_type string_to_integral( string_view_type str_number ) 
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    auto snumber{ stralgo::trim_left(str_number) };
    
    auto it{ std::begin(snumber) };
    integral_type total{};
    if( it != std::end( snumber ) )
      {
      char_type c { *it };
      if (c != char_type('-') )
        {
        if ( c == char_type('+'))
          ++it;
        
        auto next_it{ it+1 };
        if( next_it < std::end( snumber ) && detail::is_hex_prefix(*it, *next_it) )
          {
          it += 2;
          return detail::trimed_string_to_unsigned_integral<
                    integral_type, detail::base_16_t>( it, std::end( snumber ) );
          }
        else
          return detail::trimed_string_to_unsigned_integral<
                    integral_type, detail::base_10_t>( it, std::end( snumber ) );
        }
      }
    return total;
    }
    
  //--------------------------------------------------------------------------------------------------------
  ///\brief signed integral convertion from string supports untrimed strings of decimal [+/-]d[n] and hexadecimal lower and uppercase [+/-]0xh[n] numbers
  template<typename integral_type, typename string_view_type,
    std::enable_if_t< std::is_signed_v<integral_type> &&
                      strconcept::is_convertible_to_string_view_v<string_view_type>, bool> = true>
  constexpr integral_type string_to_integral( string_view_type str_number ) 
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    using unsigned_itegral_type = strconcept::make_unsigned_t<integral_type>;
    
    auto snumber{ stralgo::trim_left(str_number) };
    integral_type total{};
    char_type sign{};
    auto it{ std::begin(snumber) };
    if( it != std::end( snumber ) )
      {
      char_type c { *it };
      sign = c;          // save sign indication if '-', then negative, otherwise positive 
      if (c == char_type('-') || c == char_type('+'))
        ++it;
      auto next_it{ it+1 };
      if( next_it < std::end( snumber ) && detail::is_hex_prefix(*it, *next_it) )
        {
        it += 2;
        total = static_cast<integral_type>(
              detail::trimed_string_to_unsigned_integral<
                  unsigned_itegral_type, detail::base_16_t>( it, std::end( snumber ) ));
        }
      else
        total =  static_cast<integral_type>(
            detail::trimed_string_to_unsigned_integral<
                unsigned_itegral_type, detail::base_10_t>( it, std::end( snumber ) ));
      }
    if (sign == '-')
      return -total;
    else
      return total;
    }
  //--------------------------------------------------------------------------------------------------------
  //alias
  template<typename integral_type, typename string_view_type,
    std::enable_if_t< std::is_integral_v<integral_type> &&
                      strconcept::is_convertible_to_string_view_v<string_view_type>, bool> = true>
  constexpr integral_type str2int( string_view_type str_number ) 
    { return string_to_integral<integral_type>(str_number); }
      
  //--------------------------------------------------------------------------------------------------------
  namespace detail
    {
      
    template<typename float_type, typename base_conv_t, typename iterator>
    constexpr float_type trimed_string_to_float( iterator beg, iterator end) 
      {
      using char_type = strconcept::remove_cvref_t<decltype(*beg)>;
      
      float_type total{};
      float_type fraction{};
      constexpr float_type ibase { float_type(1) / base_conv_t::base };
      
      auto it{ beg };

      for( ;it != end; ++it )
        {
        char_type c { *it };
        if(base_conv_t::is_number(c))
          total = base_conv_t::base * total + base_conv_t:: template convert<float_type>(c);
        else
          break;
        }
      if( it != end && *it == char_type('.'))
        {
        ++it;
        float_type divider{ ibase };
        for(;it != end; ++it )
          {
          char_type c { *it };
          if(base_conv_t::is_number(c))
            fraction = fraction + base_conv_t:: template convert<float_type>(c) * divider;
          else
            break;
          divider = divider * ibase;
          }
        }
      return total + fraction;
      }
    }
  //--------------------------------------------------------------------------------------------------------
  ///\brief signed float convertion from string supports untrimed strings of decimal notation [+/-]d[n] and hexadecimal lower and uppercase [+/-]0xh[n] numbers
  template<typename float_type, typename string_view_type,
    std::enable_if_t< std::is_floating_point_v<float_type> &&
                      strconcept::is_convertible_to_string_view_v<string_view_type>, bool> = true>
  constexpr float_type string_to_float( string_view_type str_number ) 
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    
    auto snumber{ stralgo::trim_left(str_number) };
    float_type total{};
    char_type sign{};
    auto it{ std::begin(snumber) };
    if( it != std::end( snumber ) )
      {
      char_type c { *it };
      sign = c;          // save sign indication if '-', then negative, otherwise positive 
      if (c == char_type('-') || c == char_type('+'))
        ++it;
      auto next_it{ it+1 };
      if( next_it != std::end( snumber ) && detail::is_hex_prefix(*it, *next_it) )
        {
        it += 2;
        total = detail::trimed_string_to_float<float_type, detail::base_16_t>( it, std::end( snumber ));
        }
      else
        total = detail::trimed_string_to_float<float_type, detail::base_10_t>( it, std::end( snumber ));
      }
    if (sign == '-')
      return -total;
    else
      return total;
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
  namespace detail
    {
    // view size aproxymation not less than required
    template<typename char_type, typename maybe_char_type,
      std::enable_if_t<strconcept::is_char_type_v<maybe_char_type>, bool> = true>
    constexpr size_t view_size( maybe_char_type ) noexcept { return 1u; }
    
    template<typename char_type, typename maybe_integral_type,
      std::enable_if_t<std::is_integral_v<maybe_integral_type> &&
        !strconcept::is_char_type_v<maybe_integral_type> &&
        std::is_signed_v<maybe_integral_type>, bool> = true>
    constexpr size_t view_size( maybe_integral_type const & v) noexcept 
      { return static_cast<size_t>(1+std::log(std::max<maybe_integral_type>(1,std::abs(v)))); }

    template<typename char_type, typename maybe_integral_type,
      std::enable_if_t<std::is_integral_v<maybe_integral_type> &&
        !strconcept::is_char_type_v<maybe_integral_type> &&
        !std::is_signed_v<maybe_integral_type>, bool> = true>
    constexpr size_t view_size( maybe_integral_type const & v) noexcept 
      { return static_cast<size_t>(1+std::log(std::max<maybe_integral_type>(1,v))); }
      
    template<typename char_type, typename maybe_float_type,
      std::enable_if_t<std::is_floating_point_v<maybe_float_type>, bool> = true>
    constexpr size_t view_size( maybe_float_type const & v ) noexcept 
      { return static_cast<size_t>(1+std::log(std::max<maybe_float_type>(1,std::abs(v)))) + detail::default_decimal_places; }
    
    template<typename char_type, typename maybe_enum_type,
      std::enable_if_t<std::is_enum_v<maybe_enum_type>, bool> = true>
    constexpr size_t view_size( maybe_enum_type const & ) noexcept 
      { return detail::base_10_t::integral_to_string_max_size + detail::default_decimal_places; }
      
    template<typename char_type, typename string_view_type,
      std::enable_if_t< std::is_convertible_v<string_view_type, std::basic_string_view<char_type>> &&
        !strconcept::is_char_type_v<string_view_type> &&
        !std::is_array_v< strconcept::remove_cvref_t<string_view_type>> &&
        !std::is_pointer_v< strconcept::remove_cvref_t<string_view_type>>
        , bool> = true>
    constexpr size_t view_size( string_view_type const & view ) noexcept { return std::size(view); }
    
    template<typename char_type, size_t size>
    constexpr size_t view_size( std::array<char_type,size> const & ) noexcept 
      { return size; }
      
    template<typename char_type, typename input_argument_type, typename ... args_type >
    constexpr auto count_size(input_argument_type const & viewl, args_type const & ... args) noexcept
      {
      if constexpr (sizeof...(args_type) != 0)
        return view_size<char_type>(viewl) + count_size<char_type>(args...);
      else
        return view_size<char_type>(viewl);
      }
    
    //type convertion to any form of view
    template<typename char_type, typename maybe_enum_type,
      std::enable_if_t<std::is_enum_v<maybe_enum_type>, bool> = true>
    auto convert_argument( maybe_enum_type const & view ) noexcept
      {
      using underlying_type = std::underlying_type_t<maybe_enum_type>;
      return integral_to_string<char_type>(static_cast<underlying_type>(view));
      }
      
    template<typename char_type, typename maybe_integral_type,
      std::enable_if_t<std::is_integral_v<maybe_integral_type> &&
        !strconcept::is_char_type_v<maybe_integral_type>, bool> = true>
    auto convert_argument( maybe_integral_type const & view ) noexcept
      {
      return integral_to_string<char_type>(view);
      }
      
    template<typename char_type, typename maybe_float_type,
      std::enable_if_t<std::is_floating_point_v<maybe_float_type>, bool> = true>
    auto convert_argument( maybe_float_type const & view ) noexcept
      {
      return float_to_string<char_type>(view);
      }
      
    template<typename char_type,  typename string_view_type,
      std::enable_if_t< std::is_convertible_v<string_view_type, std::basic_string_view<char_type>> &&
        !strconcept::is_char_type_v<string_view_type> &&
        !std::is_array_v< strconcept::remove_cvref_t<string_view_type>> &&
        !std::is_pointer_v< strconcept::remove_cvref_t<string_view_type>>
        , bool> = true>
    auto convert_argument( string_view_type const & view ) noexcept
      {
      return view;
      }
      
    template<typename char_type, size_t size>
    auto convert_argument( std::array<char_type,size> const & data ) noexcept
      {
      return std::basic_string_view<char_type>{ data.data(), size }; 
      }
      
    template<typename char_type, typename iterator, typename string_view_type, typename ... args_type>
    iterator copy_views( iterator it, string_view_type const & view_or_char_value, args_type const & ... args ) noexcept
      {
      if constexpr (strconcept::is_char_type_v<string_view_type>)
        {
        *it = view_or_char_value;
        ++it;
        if constexpr (sizeof...(args_type) != 0)
          it = copy_views<char_type>(it, args ... );
        }
      else
        {
        auto temporary_transformed{ convert_argument<char_type>(view_or_char_value) };
        it = std::copy(std::begin(temporary_transformed),std::end(temporary_transformed), it);
        if constexpr (sizeof...(args_type) != 0)
          it = copy_views<char_type>(it, args ... );
        }
      return it;
      }
    }
    
  ///\brief composes a string from multiple arguments, arguments other that char/string are first converted into string representation
  template<typename char_type = char,
           typename input_argument_type, typename ... input_argument_type_n,
    std::enable_if_t<
      strconcept::is_char_type_v<input_argument_type> ||
      (std::is_convertible_v<input_argument_type, std::basic_string_view<char_type>> && 
            (!std::is_array_v< strconcept::remove_cvref_t<input_argument_type>> && !std::is_pointer_v< strconcept::remove_cvref_t<input_argument_type>>)) ||
      std::is_integral_v<input_argument_type> ||
      std::is_floating_point_v<input_argument_type>
      , bool> = true>
  [[nodiscard]]
  auto compose( input_argument_type const & view, input_argument_type_n const & ... args ) noexcept
    {
    static_assert(sizeof...(input_argument_type_n) != 0);
    using string_type = strconcept::string_by_char_type_t<char_type>;
    using size_type = typename string_type::size_type;
    size_type aprox_size{ static_cast<size_type>(detail::count_size<char_type>(view,args...))};
    string_type result;
    result.resize(aprox_size);
    char_type * buff{ result.data() };
    auto new_end { detail::copy_views<char_type>( buff, view, args ... ) };
    auto new_size{ static_cast<size_type>(std::distance(result.data(),new_end)) };
    assert(new_size<=aprox_size);
    result.resize( new_size );
    return result;
    }
}

