#pragma once

#include <stralgo.h>
#include <strconv_numeric.h>
namespace strconv::detail
{

  template<char_case_e char_case = char_case_e::uppercase>
  constexpr char value_to_hex_( uint8_t value )
    {
    if( value < 10) 
      return static_cast<char>('0' + value);
    else
      {
      if constexpr (char_case_e::uppercase == char_case)
        return static_cast<char>('A' + value - 10);
      else
        return static_cast<char>('a' + value - 10);
      }
    }
    
  template<char_case_e char_case = char_case_e::uppercase,
           typename iterator,
           typename output_iterator,
           typename = std::enable_if_t< strconcept::is_forward_iterator_v<iterator>
                                     && strconcept::is_writable_iterator_v<output_iterator>>
          >
  ///\brief converts binary representation to hexadecimal representation
  ///\warning \param outit must have double size capacity of source range
  constexpr output_iterator to_hex_ascii_( iterator sbeg, iterator send, output_iterator outit )
    {
    using input_value_type = typename std::iterator_traits<iterator>::value_type;
    static_assert( std::is_same_v<uint8_t,input_value_type>);

    for(;sbeg != send; ++sbeg)
      {
      uint8_t const source_byte = static_cast<uint8_t>( *sbeg );
      char c0 = value_to_hex_<char_case>( source_byte & 0xFu);
      char c1 = value_to_hex_<char_case>( (source_byte>>4) & 0xFu);
      *outit++ = c1;
      *outit++ = c0;
      }
    return outit;
    }
  //--------------------------------------------------------------------------------------------------------
  template<char_case_e char_case = char_case_e::uppercase,
           typename char_type = char,
           typename string_type = strconcept::string_by_value_type_t<char_type>,
           typename iterator, 
           typename = std::enable_if_t<strconcept::is_char_type_v<char_type>
                                    && strconcept::is_forward_iterator_v<iterator>>
            >
  constexpr auto to_hex_ascii_( iterator sbeg, iterator send )
    {
    using size_type = typename string_type::size_type;
    size_type output_size{ static_cast<size_type>(send - sbeg)*2};
    return stralgo::detail::copy_views_t<string_type>{}( output_size, [sbeg,send]( auto out_iterator )
                                                     {
                                                     static_assert( strconcept::is_writable_iterator_v<decltype(out_iterator)> );
                                                     to_hex_ascii_<char_case>( sbeg, send, out_iterator );
                                                    } );
    }
  //--------------------------------------------------------------------------------------------------------
  template<typename integral_type, typename char_type,
           typename = std::enable_if_t<strconcept::is_char_type_v<char_type>>
          >
  constexpr integral_type from_xdigit( char_type c )
    { 
    return static_cast<integral_type>( stralgo::isdigit(c) ? unsigned(c) - unsigned('0') 
                                                           : (stralgo::islower(c) ? (unsigned(c) - unsigned('a')) 
                                                                                  : (unsigned(c) - unsigned('A'))
                                                             ) + 10u );
    }
    
  //--------------------------------------------------------------------------------------------------------
  
  template<typename iterator, typename output_iterator,
    typename = std::enable_if_t<strconcept::is_forward_iterator_v<iterator>
                             && strconcept::is_writable_iterator_v<output_iterator>>
          >
  ///\warning \param outit must have half the size capacity of source range and soruce range must be multiple of 2
  constexpr output_iterator from_hex_ascii_(iterator sbeg, iterator send, output_iterator outit)
    {
    using input_value_type = std::remove_cv_t<typename std::iterator_traits<iterator>::value_type>;
    //using output_value_type = aq_typetraits::remove_cvref_t<decltype(*outit = std::declval<uint8_t>())>;
    static_assert( std::is_same_v<char,input_value_type>);
    //static_assert( std::is_same_v<uint8_t,output_value_type>);
    
    for(;sbeg != send; ) 
      {
      char c0{ *sbeg }; sbeg++;
      char c1{ *sbeg }; sbeg++;
      *outit = static_cast<uint8_t>(from_xdigit<int>(c1) + 16 * from_xdigit<int>(c0));
      ++outit;
      }
    return outit;
    }
    
  struct base_10_t
    {
    static constexpr unsigned base = 10u;
    static constexpr std::string_view output_prefix{};
    
    template<typename integral_type, typename char_type>
    static constexpr integral_type convert( char_type c ) noexcept 
      {
      return static_cast<integral_type>( unsigned(c) - unsigned('0'));
      }
      
    template<typename char_type>
    static constexpr bool is_number( char_type c ) noexcept { return stralgo::isdigit(c); }
    };
    
  struct base_16_t
    {
    static constexpr unsigned base = 16u;
    static constexpr std::string_view output_prefix{"0x"};
    
    template<typename integral_type, typename char_type>
    static constexpr integral_type convert( char_type c ) noexcept 
      {
      return detail::from_xdigit<integral_type>( c );
      }
      
    template<typename char_type>
    static constexpr bool is_number( char_type c ) noexcept { return stralgo::isxdigit(c); }
    };

  template<format_e fmt>
  struct base_conv_by_format { using type = void; };
  template<>
  struct base_conv_by_format<format_e::decimal>{ using type = base_10_t; };
  template<>
  struct base_conv_by_format<format_e::hexadecimal>{ using type = base_16_t; };
  
  template<format_e fmt>
  using base_conv_by_format_t = typename base_conv_by_format<fmt>::type;
  
  //--------------------------------------------------------------------------------------------------------
  inline constexpr unsigned integral_to_string_max_size = 22;
  //--------------------------------------------------------------------------------------------------------
  template<typename base_conv_type, typename output_iterator, typename value_type>
  constexpr output_iterator unsigned_to_str_rev_decimal( value_type value, output_iterator oit )
    {
    constexpr value_type base { static_cast<value_type>(base_conv_type::base) };
    do
      {
      value_type next_val{ static_cast<value_type>(value / base) };
      value_type decimal { static_cast<value_type>(value - (next_val * base)) };
      value = next_val;
      *oit =  value_to_hex_( static_cast<uint8_t>( decimal ) );
      ++oit;
      }
    while( value != 0 );
    return oit;
    }
  
  //--------------------------------------------------------------------------------------------------------
  template<typename base_conv_type, typename output_iterator, typename value_type,
    typename = std::enable_if_t<std::is_unsigned_v<value_type>
                            && strconcept::is_writable_iterator_v<output_iterator>>
           >
  [[nodiscard]]
  constexpr output_iterator unsigned_to_str_decimal( value_type value, output_iterator oit ) noexcept
    {
    using char_type = strconcept::remove_cvref_t<decltype(*oit)>;
    
    oit = stralgo::detail::transform( std::begin(base_conv_type::output_prefix), std::end(base_conv_type::output_prefix), oit,
                        [](char pxc) { return static_cast<char_type>(pxc); } );
    output_iterator new_end { unsigned_to_str_rev_decimal<base_conv_type>(value, oit) };
    std::reverse( oit, new_end );
    return new_end;
    }
  
  //--------------------------------------------------------------------------------------------------------
  template<typename base_conv_type, typename output_iterator, typename value_type,
    typename = std::enable_if_t<std::is_signed_v<value_type>
                             && strconcept::is_writable_iterator_v<output_iterator>>
    >
  [[nodiscard]]
  constexpr output_iterator signed_to_str_decimal( value_type value, output_iterator oit ) noexcept
    {
    using unsigned_type = std::make_unsigned_t<value_type>;
    using char_type = strconcept::remove_cvref_t<decltype(*oit)>;
    
    unsigned_type uvalue;
    if( value >= 0 )
      uvalue = static_cast<unsigned_type>( value );
    else
      {
      *oit = char_type('-');
      ++oit;
      uvalue = static_cast<unsigned_type>( -value );
      }
    return unsigned_to_str_decimal<base_conv_type>(uvalue, oit);
    }
  //--------------------------------------------------------------------------------------------------------
  template<format_e output_format = format_e::decimal,
           typename output_iterator, typename value_type,
           typename = std::enable_if_t<std::is_integral_v<value_type> 
                                    && strconcept::is_writable_iterator_v<output_iterator>>
    >
  [[nodiscard]]
  constexpr output_iterator integral_to_string_( value_type value, output_iterator oit ) noexcept
    {
    using base_conv_type = detail::base_conv_by_format_t<output_format>;
    if constexpr (std::is_unsigned_v<value_type>)
      return detail::unsigned_to_str_decimal<base_conv_type>(value, oit);
    else
      return detail::signed_to_str_decimal<base_conv_type>(value, oit);
    }
  //--------------------------------------------------------------------------------------------------------
  template<typename char_type = char, format_e output_format = format_e::decimal, typename value_type,
     typename string_type = strconcept::string_by_char_type_t<char_type>,
     typename = std::enable_if_t<std::is_integral_v<value_type> 
                              && strconcept::is_char_type_v<char_type>>
          >
  [[nodiscard]]
  auto integral_to_string_( value_type value ) noexcept
    {
    std::array<char_type,detail::integral_to_string_max_size> result;
    auto oit { std::begin(result) };
    return string_type{ oit, integral_to_string_<output_format>(value, oit) };
    }
  //--------------------------------------------------------------------------------------------------------
  inline constexpr uint32_t default_decimal_places = 6;
 
  template<uint32_t decimal_places, trailing_zeros_e trailing, typename output_iterator, typename float_type,
          typename = std::enable_if_t<std::is_floating_point_v<float_type> 
                                   && strconcept::is_writable_iterator_v<output_iterator>>
          >
  [[nodiscard]]
  output_iterator float_to_str_decimal( float_type value, output_iterator oit ) noexcept
    {
    using unsigned_type = uint64_t;
    using char_type = strconcept::remove_cvref_t<decltype(*oit)>;
    
    unsigned_type uvalue;
    auto it_beg{ oit };
    if( value >= float_type{0} )
      uvalue = static_cast<unsigned_type>( value );
    else
      {
      *it_beg = char_type('-');
      ++it_beg;
      value = -value;
      uvalue = static_cast<unsigned_type>( value );
      }
    value = value - static_cast<float_type>(uvalue);
    //store unsigned then fraction
    auto new_end{ unsigned_to_str_rev_decimal<base_10_t>(uvalue, it_beg) };
    std::reverse( it_beg, new_end );
    it_beg = new_end;
    auto it_last_nzero{ new_end };
    *it_beg = char_type('.');
    ++it_beg;
    //store fraction
    for(size_t dpl{decimal_places}; dpl !=0 ; --dpl)
      {
      float_type next_fraction{value * 10};
      unsigned ufraction { static_cast<unsigned>( next_fraction ) };
      *it_beg = value_to_hex_( static_cast<uint8_t>( ufraction ) );
      ++it_beg;
      
      if constexpr( trailing == trailing_zeros_e::skip )
        if(ufraction != 0)
          it_last_nzero = it_beg;
      
      value = next_fraction - static_cast<float_type>(ufraction);
      }
    if constexpr( trailing == trailing_zeros_e::skip )
      return it_last_nzero;
    else
      return it_beg;
    }
    
  //--------------------------------------------------------------------------------------------------------
  template<typename char_type = char, uint32_t decimal_places = detail::default_decimal_places,
            trailing_zeros_e trailing = trailing_zeros_e::skip,
            typename string_type = strconcept::string_by_char_type_t<char_type>,
            typename value_type,
            typename = std::enable_if_t<std::is_floating_point_v<value_type>
                                   && strconcept::is_char_type_v<char_type>>>
  [[nodiscard]]
  auto float_to_string_( value_type value ) noexcept
    {
    std::array<char_type,22 + decimal_places + 1> result;
    auto oit { std::begin(result) };
    return string_type{ oit, detail::float_to_str_decimal<decimal_places,trailing>(value, oit)};
    }
}
