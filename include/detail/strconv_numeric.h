#pragma once

#include <stralgo.h>
#include <strconv_numeric.h>
#include <cmath>
#include <optional>
#include <cassert>

namespace strconv::detail
{
  template<strconcept::integral value_type>
  constexpr uint8_t log2p1( value_type value ) noexcept
    {
    if( value != value_type{} )
      {
      constexpr auto Nd = std::numeric_limits<value_type>::digits;
      return static_cast<uint8_t>(Nd - std::countl_zero( value ));
      }
    return {};
    }
    
  template<char_case_e char_case = char_case_e::uppercase,
           strconcept::integral_uint8 value_type>
  constexpr char value_to_hex_( value_type value )
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
           strconcept::forward_iterator iterator,
           strconcept::writable_iterator output_iterator>
  ///\brief converts binary representation to hexadecimal representation
  ///\warning \param outit must have double size capacity of source range
  constexpr output_iterator to_hex_ascii_( iterator sbeg, iterator send, output_iterator outit )
    {
    using input_value_type = typename std::iterator_traits<iterator>::value_type;
    static_assert( std::is_same_v<uint8_t,input_value_type>);

    for(;sbeg != send; ++sbeg)
      {
      uint8_t const source_byte = static_cast<uint8_t>( *sbeg );
      char c0 = value_to_hex_<char_case>( static_cast<uint8_t>(source_byte & 0xFu) );
      char c1 = value_to_hex_<char_case>( static_cast<uint8_t>((source_byte>>4) & 0xFu) );
      *outit++ = c1;
      *outit++ = c0;
      }
    return outit;
    }
  //--------------------------------------------------------------------------------------------------------
  template<char_case_e char_case = char_case_e::uppercase,
           strconcept::char_type char_type = char,
           typename string_type = strconcept::string_by_value_type_t<char_type>,
           strconcept::forward_iterator iterator>
  constexpr auto to_hex_ascii_( iterator sbeg, iterator send )
    {
    using size_type = typename string_type::size_type;
    size_type output_size{ static_cast<size_type>(send - sbeg)*2};
    return stralgo::detail::copy_views_t<string_type>{}( output_size, [sbeg,send]
                                                    ( strconcept::writable_iterator auto out_iterator )
                                                     {
//                                                      static_assert( strconcept::is_writable_iterator_v<decltype(out_iterator)> );
                                                     to_hex_ascii_<char_case>( sbeg, send, out_iterator );
                                                    } );
    }
  //--------------------------------------------------------------------------------------------------------
  template<strconcept::integral integral_type, strconcept::char_type char_type>
  constexpr integral_type from_xdigit( char_type c )
    { 
    return static_cast<integral_type>( stralgo::isdigit(c) ? unsigned(c) - unsigned('0') 
                                                           : (stralgo::islower(c) ? (unsigned(c) - unsigned('a')) 
                                                                                  : (unsigned(c) - unsigned('A'))
                                                             ) + 10u );
    }
    
  //--------------------------------------------------------------------------------------------------------
  
  template<strconcept::forward_iterator iterator,
           strconcept::writable_iterator output_iterator>
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
    
    template<strconcept::integral integral_type, strconcept::char_type char_type>
    static constexpr integral_type convert( char_type c ) noexcept 
      {
      return static_cast<integral_type>( unsigned(c) - unsigned('0'));
      }
      
    template<strconcept::char_type char_type>
    static constexpr bool is_number( char_type c ) noexcept { return c== '0' || c == '1'; }
    };
    
  struct base_10_t
    {
    static constexpr unsigned base = 10u;
    static constexpr std::string_view output_prefix{};
    static constexpr unsigned integral_to_string_max_size = 22;
    
    template<strconcept::integral integral_type, strconcept::char_type char_type>
    static constexpr integral_type convert( char_type c ) noexcept 
      {
      return static_cast<integral_type>( unsigned(c) - unsigned('0'));
      }
      
    template<strconcept::char_type char_type>
    static constexpr bool is_number( char_type c ) noexcept { return stralgo::isdigit(c); }
    };
    
  struct base_16_t
    {
    static constexpr unsigned base = 16u;
    static constexpr std::string_view output_prefix{"0x"};
    static constexpr unsigned integral_to_string_max_size = 12;
    
    template<strconcept::integral integral_type, strconcept::char_type char_type>
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
  template<strconcept::char_type output_char_type>
  struct lower_projection
    {
    template<strconcept::char_type char_type>
    constexpr output_char_type operator()( char_type c )
      { return stralgo::to_lower(static_cast<output_char_type>(c)); }
    };
    
  template<strconcept::char_type output_char_type>
  struct upper_projection
    {
    template<strconcept::char_type char_type>
    constexpr output_char_type operator()( char_type c )
      { return stralgo::to_upper(static_cast<output_char_type>(c)); }
    };
    
  template<strconcept::char_type output_char_type, char_case_e char_case>
  constexpr auto char_case_projection() 
    {
    if constexpr( char_case == char_case_e::lowercase)
      return lower_projection<output_char_type>{};
    else
      return upper_projection<output_char_type>{};
    }
  //--------------------------------------------------------------------------------------------------------
  template<strconcept::unsigned_integral value_type>
  struct size_div_info_t
    {
    value_type divisor_ {};
    unsigned size_ {};
    };
  ///\brief main calculation of required output size for unsigned integral
  template<uint64_t base, strconcept::unsigned_integral value_type>
  constexpr auto calculate_size_div_info(value_type value)
    {
    size_div_info_t<value_type> result{};
//     if( std::is_constant_evaluated())
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
#if 0
    else
      {
      if(value != value_type{})
        {
        result.size_ = static_cast<unsigned>(1 + std::log(std::max<value_type>(1, value))/std::log(base) );
        result.divisor_ = static_cast<value_type>( std::pow(base, result.size_-1) );
        }
      }
#endif
    return result;
    }
    
  ///\brief pure convertion of unisgned integer to string at destination iterator
  template<typename base_conv_type,
           typename projection,
           strconcept::writable_iterator output_iterator,
           strconcept::unsigned_integral value_type>
  constexpr output_iterator unsigned_to_str_transform_( value_type value, projection prj,
                                                        size_div_info_t<value_type> size_div_info,
                                                        output_iterator oit )
    {
    constexpr value_type base { static_cast<value_type>(base_conv_type::base) };

    while( size_div_info.size_ != 0 )
      {
      value_type decimal { value / size_div_info.divisor_ };
      value = static_cast<value_type>( value - decimal * size_div_info.divisor_);
      size_div_info.divisor_ /= base;
      *oit = stralgo::detail::invoke( prj, value_to_hex_( static_cast<uint8_t>( decimal ) ));
      ++oit;
      --size_div_info.size_;
      }

    return oit;
    }
  ;
  //--------------------------------------------------------------------------------------------------------
  ///\brief preprocessed info for output string calculation and future formating
  template<strconcept::unsigned_integral value_type>
  struct estimate_info_t
    {
    size_div_info_t<value_type> size_div_info;
    value_type uvalue;
    unsigned output_prefix_size{};
    unsigned precision{};
    std::optional<char> sign;
    
    constexpr std::size_t number_size() const { return (sign ? 1u : 0u) +  output_prefix_size + size_div_info.size_; }
    constexpr std::size_t size() const { return std::max<std::size_t>(number_size(),precision); }
    };
    
  ///\brief calculate required output space for intergral to string convertion
  template<integral_format_traits traits,
           strconcept::unsigned_integral value_type>
  constexpr auto estimate_unsigned_to_str_( value_type value, std::optional<char> sign = {})
    {
    using base_conv_type = detail::base_conv_by_format_t<traits.format>;
    constexpr value_type base { static_cast<value_type>(base_conv_type::base) };
    
    estimate_info_t<value_type> result{};
    result.size_div_info = calculate_size_div_info<base>(value);
    result.uvalue = value;
    result.precision = traits.precision;
    
    if constexpr ( traits.precision != 0 )
      if( result.size_div_info.size_ == 0 )
        {
        result.size_div_info.size_ = 1;
        result.size_div_info.divisor_ = 1;
        }
        
    if( result.size_div_info.size_ != 0 )
      {
      
      if constexpr ( traits.sign == prepend_sign_e::always ) 
        {
        if(!sign )
          result.sign = '+';
        else
          result.sign = sign;
        }
      else
        result.sign = sign;
      
      if constexpr (traits.include_prefix == include_prefix_e::with_prefix && !base_conv_type::output_prefix.empty() )
        result.output_prefix_size = base_conv_type::output_prefix.size();
      }
    
    return result;
    }

  template<integral_format_traits traits,
           strconcept::unsigned_integral value_type,
           strconcept::writable_iterator output_iterator>
  [[nodiscard]]
  constexpr output_iterator unsigned_to_str_( estimate_info_t<value_type> const & est_info, output_iterator oit ) noexcept
    {
    using char_type = strconcept::remove_cvref_t<decltype(*oit)>;
    using iter_diff_type = strconcept::iterator_difference_type<output_iterator>;
    using base_conv_type = base_conv_by_format_t<traits.format>;
    
    if( est_info.size_div_info.size_ != 0 )
      {
      
      auto projection = char_case_projection<char_type,traits.char_case>();

      const auto estimated_number_size { est_info.number_size() };

      // ----------------------------
      if constexpr ( traits.padd_with == padd_with_e::space && traits.alignment != alignment_e::left )
        {
        if( estimated_number_size < traits.precision )
          {
          iter_diff_type fill_len = static_cast<iter_diff_type>(traits.precision - estimated_number_size);
          iter_diff_type left_fill;
          if constexpr( traits.alignment == alignment_e::right )
            left_fill = fill_len;
          else
            left_fill = fill_len>>1;
          oit = stralgo::detail::fill( oit, std::next(oit, left_fill), ' ');
          }
        }
        
      if(est_info.sign)
        {
        *oit = stralgo::detail::invoke(projection, *est_info.sign);
        ++oit;
        }

      if constexpr ( traits.include_prefix == include_prefix_e::with_prefix && !base_conv_type::output_prefix.empty())
        oit = stralgo::detail::transform( std::begin(base_conv_type::output_prefix), std::end(base_conv_type::output_prefix), oit, projection);
      
      if constexpr ( traits.padd_with == padd_with_e::zeros )
        {
        if( estimated_number_size < traits.precision )
          {
          iter_diff_type fill_len = static_cast<iter_diff_type>(traits.precision - estimated_number_size);
          oit = stralgo::detail::fill( oit, std::next(oit,fill_len), '0');
          }
        }
      
      oit = unsigned_to_str_transform_<base_conv_type>(est_info.uvalue, projection, est_info.size_div_info, oit);
      
      if constexpr ( traits.padd_with == padd_with_e::space && traits.alignment != alignment_e::right )
        if( estimated_number_size < traits.precision )
          {
          iter_diff_type fill_len = static_cast<iter_diff_type>(traits.precision - estimated_number_size);
          iter_diff_type right_fill;
          if constexpr( traits.alignment == alignment_e::left )
            right_fill = fill_len;
          else
            right_fill = fill_len -(fill_len>>1);
          oit = stralgo::detail::fill( oit, std::next(oit, right_fill), ' ');
          }
      }
    return oit;
    }
  
  //--------------------------------------------------------------------------------------------------------
  template<integral_format_traits traits,
           strconcept::signed_integral value_type>
  constexpr auto estimate_signed_to_str_( value_type value )
    {
    using unsigned_type = std::make_unsigned_t<value_type>;
    unsigned_type uvalue;
    std::optional<char> sign;
    if( value >= 0 )
      uvalue = static_cast<unsigned_type>( value );
    else
      {
      sign = '-';
      uvalue = static_cast<unsigned_type>( -value );
      }
    return estimate_unsigned_to_str_<traits>(uvalue, sign);
    }
    
  //--------------------------------------------------------------------------------------------------------
  template<integral_format_traits traits,
           strconcept::integral value_type>
  constexpr auto estimate_integral_to_str_( value_type value )
    {
    if constexpr (std::is_unsigned_v<value_type>)
      return estimate_unsigned_to_str_<traits>(value, {});
    else
      return estimate_signed_to_str_<traits>(value);
    }
  //--------------------------------------------------------------------------------------------------------
  template<integral_format_traits traits,
           strconcept::writable_iterator output_iterator,
           strconcept::unsigned_integral unsinged_value_type>
  constexpr output_iterator integral_to_string_( estimate_info_t<unsinged_value_type> const & est_info,
                                                 output_iterator oit ) noexcept
    {
    return unsigned_to_str_<traits>(est_info, oit);
    }
    
  //--------------------------------------------------------------------------------------------------------
  template<integral_format_traits traits,
           strconcept::writable_iterator output_iterator,
           strconcept::integral value_type>
  [[nodiscard]]
  constexpr output_iterator integral_to_string_( value_type value, output_iterator oit ) noexcept
    {
    auto est_info { estimate_integral_to_str_<traits>(value) };
    return integral_to_string_<traits>(est_info, oit);
    }

  //--------------------------------------------------------------------------------------------------------
  template<integral_format_traits traits,
           strconcept::char_type char_type = char,
           typename string_type = strconcept::string_by_char_type_t<char_type>,
           strconcept::integral value_type>
  [[nodiscard]]
  auto integral_to_string_( value_type value ) noexcept
    {
    auto est_info { estimate_integral_to_str_<traits>(value) };
    string_type result;
    result.resize(est_info.size());
    integral_to_string_<traits>(est_info, result.data());
    return result;
    }
  //--------------------------------------------------------------------------------------------------------
//   
  template<strconcept::floating_point value_type>
  struct float_estimate_info_t
    {
    size_div_info_t<uint64_t> size_div_info;
    uint64_t uvalue;
    value_type fraction;
    unsigned output_prefix_size{};
    unsigned precision{};
    unsigned decimal_places{};
    std::optional<char> sign;
    
    constexpr std::size_t number_size() const
       { return (sign ? 1u : 0u) + (decimal_places != 0 ? 1u :0u) +  output_prefix_size + size_div_info.size_ + decimal_places; }
    constexpr std::size_t size() const
       { return std::max<std::size_t>(number_size(),precision); }
    };
    
  template<float_format_traits traits,
           strconcept::floating_point float_type>
  [[nodiscard]]
  constexpr auto estimate_float_to_string_( float_type value ) noexcept
    {
    using base_conv_type = base_conv_by_format_t<format_e::decimal>;
    
    float_estimate_info_t<float_type> result{};
    result.precision = traits.precision;
    
    if( value >= float_type{0} )
      {
      if constexpr ( traits.sign == prepend_sign_e::always ) 
        result.sign = '+';
      }
    else
      {
      result.sign = '-';
      value = -value;
      }
    result.uvalue = static_cast<uint64_t>( value );
    
    //store unsigned then fraction
    result.size_div_info = calculate_size_div_info<base_conv_type::base>(result.uvalue);
    if constexpr ( traits.precision != 0 )
      if( result.size_div_info.size_ == 0 )
        {
        result.size_div_info.size_ = 1;
        result.size_div_info.divisor_ = 1;
        }
        
    result.fraction = value - static_cast<float_type>(result.uvalue);
    
    //calculate real decimal places
    auto decimal_places = traits.decimal_places;
    
    if constexpr( traits.trailing_zeros == trailing_zeros_e::skip )
      {
      auto tmp_value{ result.fraction };
      for( ; decimal_places !=0 && tmp_value != float_type{0}; --decimal_places)
        {
        float_type next_fraction{tmp_value * base_conv_type::base};
        unsigned ufraction { static_cast<unsigned>( next_fraction ) };
        tmp_value = next_fraction - static_cast<float_type>(ufraction);
        }
      decimal_places = traits.decimal_places - decimal_places;
      }
    else 
      {
      if( result.size_div_info.size_ == 0 )
        decimal_places = 0;
      }
    result.decimal_places = decimal_places;
    
//     if constexpr ( traits.include_prefix == include_prefix_e::with_prefix && !base_conv_type::output_prefix.empty())
//       if( result.size_div_info.size_ != 0 )
//         result.output_prefix_size = base_conv_type::output_prefix.size();
      
    return result;
    }
    
  template<float_format_traits traits,
           strconcept::writable_iterator output_iterator,
           strconcept::floating_point float_type>
  [[nodiscard]]
  constexpr output_iterator float_to_string_( float_estimate_info_t<float_type> const & est_info, output_iterator oit ) noexcept
    {
    using base_conv_type = base_conv_by_format_t<format_e::decimal>;
    using char_type = strconcept::remove_cvref_t<decltype(*oit)>;
    using iter_diff_type = strconcept::iterator_difference_type<output_iterator>;
    auto projection = char_case_projection<char_type,char_case_e::lowercase>();
        
    const auto estimated_number_size { est_info.number_size() };
    
    if constexpr ( traits.padd_with == padd_with_e::space && traits.alignment != alignment_e::left )
      {
      if( estimated_number_size < traits.precision )
        {
        iter_diff_type fill_len = static_cast<iter_diff_type>(traits.precision - estimated_number_size);
        iter_diff_type left_fill;
        if constexpr( traits.alignment == alignment_e::right )
          left_fill = fill_len;
        else
          left_fill = fill_len>>1;
        oit = stralgo::detail::fill( oit, std::next(oit, left_fill), ' ');
        }
      }
      
    if(est_info.sign)
      {
      *oit = stralgo::detail::invoke(projection, *est_info.sign);
      ++oit;
      }
      
//     if constexpr ( traits.include_prefix == include_prefix_e::with_prefix && !base_conv_type::output_prefix.empty())
//       if( est_info.size_div_info.size_ != 0 )
//         oit = stralgo::detail::transform( std::begin(base_conv_type::output_prefix), std::end(base_conv_type::output_prefix), oit, projection);
    
    if constexpr ( traits.padd_with == padd_with_e::zeros )
      {
      if( estimated_number_size < traits.precision )
        {
        iter_diff_type fill_len = static_cast<iter_diff_type>(traits.precision - estimated_number_size);
        oit = stralgo::detail::fill( oit, std::next(oit,fill_len), '0');
        }
      }
        
    oit = unsigned_to_str_transform_<base_conv_type>(est_info.uvalue, projection, est_info.size_div_info, oit);

    if(est_info.decimal_places != 0 )
      {
      *oit = stralgo::detail::invoke(projection, char_type('.'));
      ++oit;
      
      auto fraction { est_info.fraction };
      //store fraction
      for(uint32_t dpl{est_info.decimal_places}; dpl !=0 ; --dpl)
        {
        float_type next_fraction{fraction * base_conv_type::base};
        unsigned ufraction { static_cast<unsigned>( next_fraction ) };
        *oit = value_to_hex_( static_cast<uint8_t>( ufraction ) );
        ++oit;
        fraction = next_fraction - static_cast<float_type>(ufraction);
        }
      }
      
    if constexpr ( traits.padd_with == padd_with_e::space && traits.alignment != alignment_e::right )
      if( estimated_number_size < traits.precision )
        {
        iter_diff_type fill_len = static_cast<iter_diff_type>(traits.precision - estimated_number_size);
        iter_diff_type right_fill;
        if constexpr( traits.alignment == alignment_e::left )
          right_fill = fill_len;
        else
          right_fill = fill_len -(fill_len>>1);
        oit = stralgo::detail::fill( oit, std::next(oit, right_fill), ' ');
        }
          
    return oit;
    }
    
  //--------------------------------------------------------------------------------------------------------
  template<float_format_traits traits,
            strconcept::char_type char_type = char,
            typename string_type = strconcept::string_by_char_type_t<char_type>,
            strconcept::floating_point value_type>
  [[nodiscard]]
  auto float_to_string_( value_type value ) noexcept
    {
    auto est_info{ detail::estimate_float_to_string_<traits>(value) };
    string_type result;
    result.resize(est_info.size());
    auto oit = detail::float_to_string_<traits>(est_info, std::data(result));
    result.resize( static_cast<size_t>(oit - std::data(result)) );
    return result;
    }
    
  //--------------------------------------------------------------------------------------------------------
  //workaround for LIBCPP < 12 and no constexpr on tuple
  template<strconcept::unsigned_integral integral_type,
           strconcept::forward_iterator iterator>
  struct tstoui_result_t
    {
    integral_type result;
    iterator ret_it;
    };

  template<strconcept::unsigned_integral integral_type,
           typename base_conv_t,
           strconcept::forward_iterator iterator>
  constexpr auto trimed_string_to_unsigned_integral( iterator beg, iterator end) 
    {
    using char_type = strconcept::remove_cvref_t<decltype(*beg)>;
    using result_type = tstoui_result_t<integral_type,iterator>;

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
    return result_type{ total, it };
    }

  template<strconcept::char_type char_type>
  constexpr bool is_hex_prefix( char_type c0, char_type c1 )
    {
    return c0 == char_type('0') && ( c1 == char_type('x') || c1 == char_type('X') );
    }

  //--------------------------------------------------------------------------------------------------------
  template<strconcept::unsigned_integral integral_type,
           input_format_e input_format,
           strconcept::convertible_to_string_view string_view_type>
  constexpr auto string_to_integral_( string_view_type str_number ) 
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    using view_type = std::basic_string_view<char_type>;


    view_type view_str_number{ static_cast<view_type>(str_number) };
    
    auto snumber{ stralgo::trim_left(view_str_number) };

    using oit_type = decltype(std::begin(view_str_number));
    using tstoui_result_type = tstoui_result_t<integral_type,oit_type>;

    tstoui_result_type data{ {}, std::begin(view_str_number) };
//     auto ret_it{ std::begin(view_str_number) };
//     integral_type result{};
    
    if( !snumber.empty() )
      {
      auto it{ std::begin(snumber) };
      char_type c { *it };
      if (c != char_type('-') )
        {
        if ( c == char_type('+'))
          ++it;
        
        if constexpr (input_format == input_format_e::undetermined)
          {
          //if contains hex preffix assume hexadecimal
          auto next_it{ it+1 };
          if( next_it < std::end( snumber ) && detail::is_hex_prefix(*it, *next_it) )
            {
            it += 2;
            data = detail::trimed_string_to_unsigned_integral<
                      integral_type, detail::base_16_t>( it, std::end( snumber ) );
            }
          else
            data = detail::trimed_string_to_unsigned_integral<
                      integral_type, detail::base_10_t>( it, std::end( snumber ) );
          }
        else if constexpr( input_format == input_format_e::hexadecimal )
          {
          //skip hex prefix if exists
          auto next_it{ it+1 };
          if( next_it < std::end( snumber ) && detail::is_hex_prefix(*it, *next_it) )
            it += 2;

          data = detail::trimed_string_to_unsigned_integral<
                      integral_type, detail::base_16_t>( it, std::end( snumber ) );
          }
        else 
          {
          data = detail::trimed_string_to_unsigned_integral<
                      integral_type, detail::base_10_t>( it, std::end( snumber ) );
          }
        }
      }
    //for a case when number is negative return begin of untrimed value
    return std::make_pair( data.result, std::next( std::begin(str_number), std::distance(std::begin(view_str_number), data.ret_it)) );
    }
    
  //--------------------------------------------------------------------------------------------------------
  ///\brief signed integral convertion from string supports untrimed strings of decimal [+/-]d[n] and hexadecimal lower and uppercase [+/-]0xh[n] numbers
  template<strconcept::signed_integral integral_type,
           input_format_e input_format,
           strconcept::convertible_to_string_view string_view_type>
  constexpr auto string_to_integral_( string_view_type str_number ) 
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    using view_type = std::basic_string_view<char_type>;
    using unsigned_itegral_type = strconcept::make_unsigned_t<integral_type>;
    
    view_type view_str_number{ static_cast<view_type>(str_number) };
    auto snumber{ stralgo::trim_left(view_str_number) };

    auto ret_it{ std::begin(view_str_number) };
    integral_type total{};

    char_type sign{};
    
    if( !snumber.empty() )
      {
      using oit_type = decltype(std::begin(snumber));
      using tstoui_result_type = tstoui_result_t<unsigned_itegral_type,oit_type>;
      tstoui_result_type data{ {}, std::begin(snumber) };

      char_type c { *data.ret_it };
      sign = c;          // save sign indication if '-', then negative, otherwise positive 
      if (c == char_type('-') || c == char_type('+'))
        ++data.ret_it;
      
      if constexpr (input_format == input_format_e::undetermined)
        {
        auto next_it{ data.ret_it+1 };
        if( next_it < std::end( snumber ) && detail::is_hex_prefix(*data.ret_it, *next_it) )
          {
          data.ret_it += 2;
          data =
                detail::trimed_string_to_unsigned_integral<
                    unsigned_itegral_type, detail::base_16_t>( data.ret_it, std::end( snumber ) );
          }
        else
          data =
              detail::trimed_string_to_unsigned_integral<
                  unsigned_itegral_type, detail::base_10_t>( data.ret_it, std::end( snumber ) );
        }
      else if constexpr( input_format == input_format_e::hexadecimal )
        {
        //skip hex prefix if exists
        auto next_it{ data.ret_it+1 };
        if( next_it < std::end( snumber ) && detail::is_hex_prefix(*data.ret_it, *next_it) )
          data.ret_it += 2;

        data = detail::trimed_string_to_unsigned_integral<
                    unsigned_itegral_type, detail::base_16_t>( data.ret_it, std::end( snumber ) );
        }
      else 
        {
        data = detail::trimed_string_to_unsigned_integral<
                    unsigned_itegral_type, detail::base_10_t>( data.ret_it, std::end( snumber ) );
        }
      total = static_cast<integral_type>(data.result);
      if (sign == '-')
        total = static_cast<integral_type>(-total);
      ret_it = data.ret_it;
      }
    //nothing to convert return begin iterator
    return std::make_pair(total, std::next( std::begin(str_number), std::distance(std::begin(view_str_number), ret_it)) );
    }
    
  //--------------------------------------------------------------------------------------------------------
  //workaround for LIBCPP < 12 and no constexpr on tuple
  template<strconcept::floating_point float_type,
           strconcept::forward_iterator iterator>
  struct tstof_result_t
    {
    float_type result;
    iterator ret_it;
    };

  template<strconcept::floating_point float_type,
           typename base_conv_t,
           strconcept::forward_iterator iterator>
  constexpr auto trimed_string_to_float( iterator beg, iterator end) 
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
        total = base_conv_t::base * total + static_cast<float_type>(base_conv_t:: template convert<int>(c));
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
          fraction = fraction + static_cast<float_type>(base_conv_t:: template convert<int>(c)) * divider;
        else
          break;
        divider = divider * ibase;
        }
      }
    return tstof_result_t<float_type,iterator>{ total + fraction, it };
    }
    
  //--------------------------------------------------------------------------------------------------------
  template<strconcept::floating_point float_type,
           strconcept::convertible_to_string_view string_view_type>
  constexpr auto string_to_float_( string_view_type str_number ) 
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    using view_type = std::basic_string_view<char_type>;

    view_type view_str_number{ static_cast<view_type>(str_number) };
    auto snumber{ stralgo::trim_left(view_str_number) };

    if( !snumber.empty() )
      {
      using oit_type = decltype(std::begin(snumber));
      tstof_result_t<float_type,oit_type> result;

      char_type sign{};
      auto it{ std::begin(snumber) };

      char_type c { *it };
      sign = c;          // save sign indication if '-', then negative, otherwise positive 
      if (c == char_type('-') || c == char_type('+'))
        ++it;
      auto next_it{ it+1 };
      if( next_it != std::end( snumber ) && detail::is_hex_prefix(*it, *next_it) )
        {
        it += 2;
        result = detail::trimed_string_to_float<float_type, detail::base_16_t>( it, std::end( snumber ));
        }
      else
        result = detail::trimed_string_to_float<float_type, detail::base_10_t>( it, std::end( snumber ));
      
      auto oit = std::next( std::begin(str_number), std::distance(std::begin(view_str_number), result.ret_it));
      if (sign == '-')
        result.result = -result.result;
      return std::make_pair(result.result, oit);
      }
    else //nothing to convert return begin iterator
      return std::make_pair(float_type{}, std::begin(str_number));
    }
    
  //--------------------------------------------------------------------------------------------------------
  //compose
  //--------------------------------------------------------------------------------------------------------

  //preconvertion char type
  template<strconcept::char_type char_type>
  struct view_preconv_char_t
    {
    char_type value_;
    constexpr std::size_t view_size() const noexcept { return 1; }
    
    template<strconcept::forward_iterator iterator>
    constexpr iterator transform( iterator it ) const
      {
      *it = value_;
      ++it;
      return it;
      }
    };
    
  template<strconcept::char_type char_type, strconcept::char_type maybe_char_type>
    requires strconcept::same_as<char_type, maybe_char_type>
  constexpr auto compose_preconv( maybe_char_type value )
    {
    return view_preconv_char_t<char_type>{ value };
    }
      
  //--------------------------------------------------------------------------------------------------------
  ///preconv string_view
  template<strconcept::char_type char_type>
  struct view_preconv_string_view_t
    {
    std::basic_string_view<char_type> view_;
    
    constexpr std::size_t view_size() const noexcept { return view_.size(); }
    
    template<strconcept::forward_iterator iterator>
    constexpr iterator transform( iterator it ) const
      {
      it = std::copy(std::begin(view_),std::end(view_), it);
      return it;
      }
    };
    
  template<strconcept::char_type char_type, strconcept::convertible_to_string_view string_view_type
  /*,std::enable_if_t<std::is_convertible_v<string_view_type, std::basic_string_view<char_type>> &&
    !strconcept::is_char_type_v<string_view_type> &&
    !std::is_array_v< strconcept::remove_cvref_t<string_view_type>> &&
    !std::is_pointer_v< strconcept::remove_cvref_t<string_view_type>>
        ,bool> = true */>
  constexpr auto compose_preconv( string_view_type const & value )
    {
    return view_preconv_string_view_t<char_type>{ static_cast<std::basic_string_view<char_type>>(value) };
    }
      
  template<strconcept::char_type char_type, size_t size>
  constexpr auto compose_preconv( std::array<char_type,size> const & data ) noexcept
    {
    return compose_preconv<char_type>(std::basic_string_view<char_type>{ data.data(), size });
    }
  //--------------------------------------------------------------------------------------------------------
  //preconv integral
  template<strconcept::integral integral_type, integral_format_traits traits = integral_format_traits{}>
    requires ( !strconcept::char_type<integral_type> )
  struct view_preconv_integral_t
    {
    using unsigned_integral_type = std::make_unsigned_t<integral_type>;
    estimate_info_t<unsigned_integral_type> est_info_;

    constexpr view_preconv_integral_t( integral_type value ) 
        : est_info_{ detail::estimate_integral_to_str_<traits>(value) }
      {}
    
    constexpr std::size_t view_size() const noexcept { return est_info_.size(); }
    
    template<strconcept::forward_iterator iterator>
    constexpr iterator transform( iterator oit ) const
      {
      return integral_to_string_<traits>(est_info_, oit);
      }
    };
    
  template<integral_format_traits traits, strconcept::integral integral_type>
    requires ( !strconcept::char_type<integral_type> )
  constexpr auto fmt( integral_type value )
    {
    return view_preconv_integral_t<integral_type,traits>{ value };
    }
    //
  template<strconcept::char_type char_type, strconcept::integral integral_type>
    requires ( !strconcept::char_type<integral_type> )
  constexpr auto compose_preconv( integral_type value )
    {
    return view_preconv_integral_t<integral_type>{ value };
    }
    
  template<strconcept::char_type char_type, strconcept::integral integral_type, integral_format_traits traits>
    requires ( !strconcept::char_type<integral_type> )
  constexpr auto compose_preconv( view_preconv_integral_t<integral_type,traits> value )
    {
    return value;
    }
    
  template<strconcept::char_type char_type, typename maybe_enum_type,
    std::enable_if_t<std::is_enum_v<maybe_enum_type>,bool> = true >
  constexpr auto compose_preconv( maybe_enum_type value )
    {
    using underlying_type = std::underlying_type_t<maybe_enum_type>;
    return compose_preconv<char_type>( static_cast<underlying_type>(value));
    }
  //--------------------------------------------------------------------------------------------------------
  //preconv floating point
  template<strconcept::floating_point float_type, float_format_traits traits = float_format_traits{} >
  struct view_preconv_float_t
    {
    float_estimate_info_t<float_type> est_info_;

    constexpr view_preconv_float_t( float_type value ) 
        : est_info_{ detail::estimate_float_to_string_<traits>(value) }
      {}
      
    constexpr std::size_t view_size() const noexcept { return est_info_.size(); }
    
    template<strconcept::forward_iterator iterator>
    constexpr iterator transform( iterator oit ) const
      {
      return detail::float_to_string_<traits>(est_info_, oit );
      }
    };
    
  template<float_format_traits traits, strconcept::floating_point float_type>
  constexpr auto fmt( float_type value )
    {
    return view_preconv_float_t<float_type,traits>{ value };
    }
    
  template<strconcept::char_type char_type, strconcept::floating_point float_type>
  constexpr auto compose_preconv( float_type value )
    {
    return view_preconv_float_t<float_type>{ value };
    }
    
  template<strconcept::char_type char_type, strconcept::floating_point float_type, float_format_traits traits>
  constexpr auto compose_preconv( view_preconv_float_t<float_type,traits> value )
    {
    return value;
    }
    
  //--------------------------------------------------------------------------------------------------------
  // main composing
  template<strconcept::char_type char_type, typename ... args_type >
  constexpr auto compose_preprocess( args_type const & ... args )
    { return std::make_tuple(compose_preconv<char_type>(args) ...);}
    
  template<typename input_argument_type, typename ... args_type >
  constexpr auto preprocessed_count_size(input_argument_type const & preparg, args_type const & ... args) noexcept
    {
    if constexpr (sizeof...(args_type) != 0)
      return preparg.view_size() + preprocessed_count_size(args...);
    else
      return preparg.view_size();
    }
    
  template<strconcept::char_type char_type, typename iterator, typename input_argument_type, typename ... args_type>
  constexpr iterator preprocessed_transform_views( iterator it, input_argument_type const & preparg, args_type const & ... args ) noexcept
    {
    it = preparg.transform(it);
    if constexpr (sizeof...(args_type) != 0)
      it = preprocessed_transform_views<char_type>(it, args ... );
    return it;
    }
  
  template<strconcept::char_type char_type = char, typename ... input_argument_type_n>
  [[nodiscard]]
  auto compose_(input_argument_type_n const & ... args) noexcept
    {
    auto preprocessed{ detail::compose_preprocess<char_type>(args ...) };
    static_assert(sizeof...(input_argument_type_n) > 1);
    using string_type = strconcept::string_by_char_type_t<char_type>;
    using size_type = typename string_type::size_type;
    
    size_type aprox_size{ static_cast<size_type>(
        std::apply( [](auto const&... tuple_args)
          { return preprocessed_count_size(tuple_args ...); },
                   preprocessed))};
    string_type result;
    result.resize(aprox_size);
    char_type * buff{ result.data() };
    auto new_end {std::apply( [buff](auto const & ... tuple_args)
          { return preprocessed_transform_views<char_type>(buff, tuple_args ...); },
          preprocessed)};
    
    auto new_size{ static_cast<size_type>(std::distance(result.data(),new_end)) };
    assert(new_size==aprox_size);
//     assert(new_size<=aprox_size);
//     result.resize( new_size );
    return result;
    }
}
