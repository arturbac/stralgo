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
    
  struct base_2_t
    {
    static constexpr unsigned base = 2;
    static constexpr std::string_view output_prefix{"0b"};
    static constexpr unsigned integral_to_string_max_size = 70;
    
    template<typename integral_type, typename char_type>
    static constexpr integral_type convert( char_type c ) noexcept 
      {
      return static_cast<integral_type>( unsigned(c) - unsigned('0'));
      }
      
    template<typename char_type>
    static constexpr bool is_number( char_type c ) noexcept { return c== '0' || c == '1'; }
    };
    
  struct base_10_t
    {
    static constexpr unsigned base = 10u;
    static constexpr std::string_view output_prefix{};
    static constexpr unsigned integral_to_string_max_size = 22;
    
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
    static constexpr unsigned integral_to_string_max_size = 12;
    
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
  template<>
  struct base_conv_by_format<format_e::binary>{ using type = base_2_t; };
  
  template<format_e fmt>
  using base_conv_by_format_t = typename base_conv_by_format<fmt>::type;
  
  //--------------------------------------------------------------------------------------------------------
  template<typename output_char_type>
  struct lower_projection
    {
    template<typename char_type>
    constexpr output_char_type operator()( char_type c )
      { return stralgo::to_lower(static_cast<output_char_type>(c)); }
    };
    
  template<typename output_char_type>
  struct upper_projection
    {
    template<typename char_type>
    constexpr output_char_type operator()( char_type c )
      { return stralgo::to_upper(static_cast<output_char_type>(c)); }
    };
    
  template<typename output_char_type, char_case_e char_case>
  constexpr auto char_case_projection() 
    {
    if constexpr( char_case == char_case_e::lowercase)
      return lower_projection<output_char_type>{};
    else
      return upper_projection<output_char_type>{};
    }
  //--------------------------------------------------------------------------------------------------------
  template<typename value_type>
  struct size_div_info_t
    {
    value_type divisor_ {};
    unsigned size_ {};
    };
  ///\brief main calculation of required output size for unsigned integral
  template<uint64_t base, typename value_type,
           typename = std::enable_if_t<std::is_unsigned_v<value_type>>>
  constexpr auto calculate_size_div_info(value_type value)
    {
    size_div_info_t<value_type> result{};
    if( std::is_constant_evaluated())
      {
      while(value != value_type{} )
        {
        result.size_ += 1;
        if( result.divisor_ != 0u)
          result.divisor_ *= base;
        else
          result.divisor_ =  1;
        value /= base;
        }
      }
    else
      {
      if(value != value_type{})
        {
        result.size_ = static_cast<unsigned>(1 + std::log(std::max<value_type>(1, value))/std::log(base) );
        result.divisor_ = static_cast<value_type>( std::pow(base, result.size_-1) );
        }
      }
    
    return result;
    }
    
  ///\brief pure convertion of unisgned integer to string at destination iterator
  template<typename base_conv_type, typename projection, typename output_iterator, typename value_type>
  constexpr output_iterator unsigned_to_str_transform_( value_type value, projection prj, size_div_info_t<value_type> size_div_info, output_iterator oit )
    {
    constexpr value_type base { static_cast<value_type>(base_conv_type::base) };

    while( size_div_info.size_ != 0 )
      {
      value_type decimal { value / size_div_info.divisor_ };
      value = static_cast<value_type>( value - decimal * size_div_info.divisor_);
      size_div_info.divisor_ /= base;
      *oit = std::invoke( prj, value_to_hex_( static_cast<uint8_t>( decimal ) ));
      ++oit;
      --size_div_info.size_;
      }

    return oit;
    }
  
  //--------------------------------------------------------------------------------------------------------
  template<typename value_type>
  struct estimate_info_t
    {
    size_div_info_t<value_type> size_div_info;
    unsigned sign_prefix_size{};
    unsigned output_prefix_size{};
    
    constexpr std::size_t size() const { return sign_prefix_size +  output_prefix_size + size_div_info.size_; }
    };
    
  ///\brief calculate required output space for intergral to string convertion
  template<integral_format_traits traits, typename value_type,
           typename = std::enable_if_t<std::is_unsigned_v<value_type>>>
  constexpr auto estimate_unsigned_to_str_( value_type value, unsigned sign_prefix_size = 0)
    {
    using base_conv_type = detail::base_conv_by_format_t<traits.format>;
    constexpr value_type base { static_cast<value_type>(base_conv_type::base) };
    
    estimate_info_t<value_type> result{};
    result.size_div_info = calculate_size_div_info<base>(value);
    
    if constexpr ( traits.precision != 0 )
      if( result.size_div_info.size_ == 0 )
        {
        result.size_div_info.size_ = 1;
        result.size_div_info.divisor_ = 1;
        }
        
    if( result.size_div_info.size_ != 0 )
      {
      if constexpr ( traits.sign == prepend_sign_e::always ) 
        if(sign_prefix_size == 0 )
           sign_prefix_size = 1;
        
      result.sign_prefix_size = sign_prefix_size;
    
      if constexpr (traits.include_prefix == include_prefix_e::with_prefix && !base_conv_type::output_prefix.empty() )
        result.output_prefix_size = base_conv_type::output_prefix.size();
      }
      
    return result;
    }

  template<integral_format_traits traits,
           typename base_conv_type, typename value_type, typename output_iterator, 
           typename = std::enable_if_t<std::is_unsigned_v<value_type>
                                    && strconcept::is_writable_iterator_v<output_iterator>>
           >
  [[nodiscard]]
  constexpr output_iterator unsigned_to_str_( value_type value, std::basic_string_view<char> sign_prefix, output_iterator oit ) noexcept
    {
    using char_type = strconcept::remove_cvref_t<decltype(*oit)>;
    using iter_diff_type = strconcept::iterator_difference_type<output_iterator>;
    
    constexpr value_type base { static_cast<value_type>(base_conv_type::base) };
    auto size_div_info = calculate_size_div_info<base>(value);
    
    if constexpr ( traits.precision != 0 )
      if( size_div_info.size_ == 0 )
        {
        size_div_info.size_ = 1;
        size_div_info.divisor_ = 1;
        }
    
    if( size_div_info.size_ != 0 )
      {
      constexpr char pos_sign ='+';
      if constexpr ( traits.sign == prepend_sign_e::always ) 
        if(sign_prefix.empty())
           sign_prefix = std::basic_string_view<char>{ &pos_sign , 1 };
      
      auto projection = char_case_projection<char_type,traits.char_case>();
      
      auto output_prefix_size{ 0u };
      if constexpr (traits.include_prefix == include_prefix_e::with_prefix && !base_conv_type::output_prefix.empty() )
        output_prefix_size = base_conv_type::output_prefix.size();

      //exact output size after transform
      const auto estimated_size { sign_prefix.size() +  output_prefix_size + size_div_info.size_ };
    
      // ----------------------------
      if constexpr ( traits.padd_with == padd_with_e::space )
        if( estimated_size < traits.precision )
          {
          const auto fill_len = static_cast<iter_diff_type>(traits.precision - estimated_size);
          oit = stralgo::detail::fill( oit, std::next(oit,fill_len), ' ');
          }
    
      oit = stralgo::detail::transform( std::begin(sign_prefix), std::end(sign_prefix), oit, projection );

      if constexpr ( traits.include_prefix == include_prefix_e::with_prefix && !base_conv_type::output_prefix.empty())
        oit = stralgo::detail::transform( std::begin(base_conv_type::output_prefix), std::end(base_conv_type::output_prefix), oit, projection);
      
      if constexpr ( traits.padd_with == padd_with_e::zeros )
        if( estimated_size < traits.precision )
          {
          const auto fill_len = static_cast<iter_diff_type>(traits.precision - estimated_size);
          oit = stralgo::detail::fill( oit, std::next(oit,fill_len), '0');
          }
      return unsigned_to_str_transform_<base_conv_type>(value, projection, size_div_info, oit);
      }
    else
      return oit;
    }
  
  //--------------------------------------------------------------------------------------------------------
  template<integral_format_traits traits, typename value_type,
           typename = std::enable_if_t<std::is_signed_v<value_type>>>
  constexpr auto estimate_signed_to_str_( value_type value )
    {
    using unsigned_type = std::make_unsigned_t<value_type>;
    unsigned_type uvalue;
    unsigned sign_prefix_size{};
    if( value >= 0 )
      uvalue = static_cast<unsigned_type>( value );
    else
      {
      sign_prefix_size = 1;
      uvalue = static_cast<unsigned_type>( -value );
      }
    return estimate_unsigned_to_str_<traits>(uvalue, sign_prefix_size);
    }
    
  //--------------------------------------------------------------------------------------------------------
  template<integral_format_traits traits,
           typename base_conv_type, typename output_iterator, typename value_type,
           typename = std::enable_if_t<std::is_signed_v<value_type>
                                    && strconcept::is_writable_iterator_v<output_iterator>>
    >
  [[nodiscard]]
  constexpr output_iterator signed_to_str_( value_type value, output_iterator oit ) noexcept
    {
    using unsigned_type = std::make_unsigned_t<value_type>;

    unsigned_type uvalue;
    constexpr char neg_sign ='-';
    
    std::basic_string_view<char> sign_prefix {};
    if( value >= 0 )
      uvalue = static_cast<unsigned_type>( value );
    else
      {
      sign_prefix = std::basic_string_view<char>{ &neg_sign , 1 };
      uvalue = static_cast<unsigned_type>( -value );
      }
    return unsigned_to_str_<traits,base_conv_type>(uvalue, sign_prefix, oit);
    }
  //--------------------------------------------------------------------------------------------------------
  template<integral_format_traits traits, typename value_type,
           typename = std::enable_if_t<std::is_integral_v<value_type>>
           >
  auto estimate_integral_to_str_( value_type value )
    {
    if constexpr (std::is_unsigned_v<value_type>)
      return estimate_unsigned_to_str_<traits>(value, {});
    else
      return estimate_signed_to_str_<traits>(value);
    }
  //--------------------------------------------------------------------------------------------------------
  template<integral_format_traits traits,
           typename output_iterator, typename value_type,
           typename = std::enable_if_t<std::is_integral_v<value_type> 
                                    && strconcept::is_writable_iterator_v<output_iterator>>
    >
  [[nodiscard]]
  constexpr output_iterator integral_to_string_( value_type value, output_iterator oit ) noexcept
    {
    using base_conv_type = base_conv_by_format_t<traits.format>;
    if constexpr (std::is_unsigned_v<value_type>)
      return unsigned_to_str_<traits,base_conv_type>(value, {}, oit);
    else
      return signed_to_str_<traits,base_conv_type>(value, oit);
    }
  //--------------------------------------------------------------------------------------------------------
  template<typename char_type = char, integral_format_traits traits, typename value_type,
     typename string_type = strconcept::string_by_char_type_t<char_type>,
     typename = std::enable_if_t<std::is_integral_v<value_type> 
                              && strconcept::is_char_type_v<char_type>>
          >
  [[nodiscard]]
  auto integral_to_string_( value_type value ) noexcept
    {
    using base_conv_type = base_conv_by_format_t<traits.format>;
    
    std::array<char_type,base_conv_type::integral_to_string_max_size> result;
    auto oit { std::begin(result) };
    return string_type{ oit, integral_to_string_<traits>(value, oit) };
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
    auto size_div_info = calculate_size_div_info<base_10_t::base>(uvalue);
    auto new_end{ unsigned_to_str_transform_<base_10_t>(uvalue, size_div_info, it_beg) };

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
