#pragma once

#include <stralgo/stralgo.h>
#include <cmath>
#include <optional>
#include <cassert>
#include <bit>

namespace stralgo::detail
  {
struct log2p1_t
  {
  template<std::integral value_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr uint8_t
    operator()(value_type value) stralgo_static_call_operator_const noexcept
    {
    if(value != value_type{})
      {
      constexpr auto Nd = std::numeric_limits<value_type>::digits;
      return static_cast<uint8_t>(Nd - std::countl_zero(value));
      }
    return {};
    }
  };

inline constexpr log2p1_t log2p1;

//--------------------------------------------------------------------------------------------------------
template<char_case_e char_case, concepts::char_type char_type>
struct value_to_hex_t
  {
  ///\returns char representation of single decimal value, value must be in range 0..15
  [[nodiscard]]
  stralgo_static_call_operator constexpr char_type
    operator()(concepts::integral_uint8 auto value) stralgo_static_call_operator_const noexcept
    {
    if(value < 10)
      return static_cast<char_type>('0' + value);
    else
      {
      if constexpr(char_case_e::uppercase == char_case)
        return static_cast<char_type>('A' + value - 10);
      else
        return static_cast<char_type>('a' + value - 10);
      }
    }
  };

template<char_case_e char_case, concepts::char_type char_type>
inline constexpr value_to_hex_t<char_case, char_type> value_to_hex;

//--------------------------------------------------------------------------------------------------------
///\brief converts binary representation to hexadecimal representation
template<char_case_e char_case>
struct to_hex_ascii_t
  {
  template<
    concepts::ui8_iterator iterator,
    std::sentinel_for<iterator> sentinel,
    concepts::typed_char_output_iterator output_iterator>
  stralgo_static_call_operator constexpr output_iterator
    operator()(iterator sbeg, sentinel send, output_iterator outit) stralgo_static_call_operator_const noexcept
    {
    using char_type = std::iter_value_t<output_iterator>;
    for(; sbeg != send; ranges::advance(sbeg, 1))
      {
      uint8_t const source_byte = static_cast<uint8_t>(*sbeg);
      char_type c0 = value_to_hex<char_case, char_type>(static_cast<uint8_t>(source_byte & 0xFu));
      char_type c1 = value_to_hex<char_case, char_type>(static_cast<uint8_t>((source_byte >> 4) & 0xFu));
      *outit = c1;
      ranges::advance(outit, 1);
      *outit = c0;
      ranges::advance(outit, 1);
      }
    return outit;
    }

  template<ranges::forward_range forward_range, concepts::typed_char_output_iterator output_iterator>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(forward_range const & range, output_iterator outit) stralgo_static_call_operator_const
    {
    return operator()(ranges::begin(range), ranges::end(range), outit);
    }
  };

template<char_case_e char_case>
inline constexpr to_hex_ascii_t<char_case> to_hex_ascii;

//--------------------------------------------------------------------------------------------------------
template<template<typename> typename basic_string_type, char_case_e char_case, concepts::char_type char_type>
struct to_hex_ascii_string_t
  {
  template<concepts::ui8_iterator iterator, std::sentinel_for<iterator> sentinel>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(iterator sbeg, sentinel send) stralgo_static_call_operator_const
    {
    using string_type = basic_string_type<char_type>;
    using size_type = typename string_type::size_type;
    constexpr bool supports_resize_and_overwrite = string_supports_resize_and_overwrite_t<basic_string_type>::value;
    size_type output_size{static_cast<size_type>(send - sbeg) * 2};
    return stralgo::detail::copy_views_t<string_type, supports_resize_and_overwrite>{
    }(output_size, [sbeg, send](auto out_iterator) { return to_hex_ascii<char_case>(sbeg, send, out_iterator); });
    }

  template<ranges::forward_range forward_range>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(forward_range const & range) stralgo_static_call_operator_const
    {
    return operator()(ranges::begin(range), ranges::end(range));
    }
  };

template<char_case_e char_case = char_case_e::uppercase, concepts::char_type char_type = char>
inline constexpr to_hex_ascii_string_t<small_vectors::basic_string, char_case, char_type> to_hex_ascii_string;

namespace stl
  {
  template<char_case_e char_case = char_case_e::uppercase, concepts::char_type char_type = char>
  inline constexpr to_hex_ascii_string_t<basic_string, char_case, char_type> to_hex_ascii_string;
  }

//--------------------------------------------------------------------------------------------------------
struct from_xdigit_t
  {
  template<concepts::char_type char_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr uint32_t
    operator()(char_type c) stralgo_static_call_operator_const noexcept
    {
    return static_cast<uint32_t>(
      stralgo::isdigit(c) ? uint32_t(c) - uint32_t('0')
                          : (stralgo::islower(c) ? (uint32_t(c) - uint32_t('a')) : (uint32_t(c) - uint32_t('A'))) + 10u
    );
    }
  };

inline constexpr from_xdigit_t from_xdigit;

//--------------------------------------------------------------------------------------------------------
struct from_hex_ascii_t
  {
  ///\warning \ref outit must have half the size capacity of source range and source range must be multiple of 2
  template<
    concepts::char_iterator iterator,
    std::sentinel_for<iterator> sentinel,
    std::output_iterator<std::uint8_t> output_iterator>
  stralgo_static_call_operator constexpr output_iterator
    operator()(iterator sbeg, sentinel send, output_iterator outit) stralgo_static_call_operator_const noexcept
    {
    using char_type = std::iter_value_t<iterator>;
    while(sbeg != send)
      {
      char_type c0{*sbeg};
      ranges::advance(sbeg, 1);
      char_type c1{*sbeg};
      ranges::advance(sbeg, 1);
      *outit = static_cast<uint8_t>(from_xdigit(c1) + 16u * from_xdigit(c0));
      ranges::advance(outit, 1);
      }
    return outit;
    }

  template<ranges::forward_range forward_range, std::output_iterator<std::uint8_t> output_iterator>
  stralgo_static_call_operator constexpr auto
    operator()(forward_range const & range, output_iterator outit) stralgo_static_call_operator_const
    {
    return operator()(ranges::begin(range), ranges::end(range), outit);
    }
  };

inline constexpr from_hex_ascii_t from_hex_ascii;

struct base_2_t
  {
  static constexpr unsigned base = 2;
  static constexpr std::string_view output_prefix{"0b"};
  static constexpr unsigned integral_to_string_max_size = 70;

  template<std::integral integral_type, concepts::char_type char_type>
  [[nodiscard]]
  static constexpr integral_type convert(char_type c) noexcept
    {
    return static_cast<integral_type>(unsigned(c) - unsigned('0'));
    }

  template<concepts::char_type char_type>
  [[nodiscard]]
  static constexpr bool is_number(char_type c) noexcept
    {
    return c == '0' || c == '1';
    }
  };

struct base_10_t
  {
  static constexpr unsigned base = 10u;
  static constexpr std::string_view output_prefix{};
  static constexpr unsigned integral_to_string_max_size = 22;

  template<std::integral integral_type, concepts::char_type char_type>
  [[nodiscard]]
  static constexpr integral_type convert(char_type c) noexcept
    {
    return static_cast<integral_type>(unsigned(c) - unsigned('0'));
    }

  template<concepts::char_type char_type>
  [[nodiscard]]
  static constexpr bool is_number(char_type c) noexcept
    {
    return stralgo::isdigit(c);
    }
  };

struct base_16_t
  {
  static constexpr unsigned base = 16u;
  static constexpr std::string_view output_prefix{"0x"};
  static constexpr unsigned integral_to_string_max_size = 12;

  template<std::integral integral_type, concepts::char_type char_type>
  [[nodiscard]]
  static constexpr integral_type convert(char_type c) noexcept
    {
    return static_cast<integral_type>(detail::from_xdigit(c));
    }

  template<typename char_type>
  [[nodiscard]]
  static constexpr bool is_number(char_type c) noexcept
    {
    return stralgo::isxdigit(c);
    }
  };

template<format_e fmt>
struct base_conv_by_format
  {
  using type = void;
  };

template<>
struct base_conv_by_format<format_e::decimal>
  {
  using type = base_10_t;
  };

template<>
struct base_conv_by_format<format_e::hexadecimal>
  {
  using type = base_16_t;
  };

template<>
struct base_conv_by_format<format_e::binary>
  {
  using type = base_2_t;
  };

template<format_e fmt>
using base_conv_by_format_t = typename base_conv_by_format<fmt>::type;

//--------------------------------------------------------------------------------------------------------

struct lower_projection_t
  {
  template<concepts::char_type char_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr char_type
    operator()(char_type c) stralgo_static_call_operator_const noexcept
    {
    return stralgo::to_lower(c);
    }
  };

inline constexpr lower_projection_t lower_projection;

struct upper_projection_t
  {
  template<concepts::char_type char_type>
  stralgo_static_call_operator constexpr char_type operator()(char_type c) stralgo_static_call_operator_const noexcept
    {
    return stralgo::to_upper(c);
    }
  };

inline constexpr upper_projection_t upper_projection;

template<char_case_e char_case>
constexpr auto char_case_projection()
  {
  if constexpr(char_case == char_case_e::lowercase)
    return lower_projection;
  else
    return upper_projection;
  }

//--------------------------------------------------------------------------------------------------------
template<std::unsigned_integral value_type>
struct size_div_info_t
  {
  value_type divisor_{};
  unsigned size_{};
  };

///\brief main calculation of required output size for unsigned integral
template<uint64_t base>
struct calculate_size_div_info_t
  {
  template<std::unsigned_integral value_type>
  stralgo_static_call_operator constexpr auto operator()(value_type value) stralgo_static_call_operator_const noexcept
    -> size_div_info_t<value_type>
    {
    size_div_info_t<value_type> result{};
      {
      while(value != value_type{})
        {
        result.size_ += 1;
        if(result.divisor_ != 0u)
          result.divisor_ *= base;
        else
          result.divisor_ = 1;
        value /= base;
        }
      }
    return result;
    }
  };

template<uint64_t base>
inline constexpr calculate_size_div_info_t<base> calculate_size_div_info;

///\brief pure convertion of unisgned integer to string at destination iterator
template<typename base_conv_type, char_case_e char_case, concepts::char_type char_type>
struct unsigned_to_str_transform_t
  {
  template<concepts::char_output_iterator<char_type> output_iterator, std::unsigned_integral value_type>
  stralgo_static_call_operator constexpr auto operator()(
    value_type value, size_div_info_t<value_type> size_div_info, output_iterator oit
  ) stralgo_static_call_operator_const noexcept -> output_iterator
    {
    constexpr value_type base{static_cast<value_type>(base_conv_type::base)};

    while(size_div_info.size_ != 0)
      {
      value_type decimal{static_cast<value_type>(value / size_div_info.divisor_)};
      value = static_cast<value_type>(value - decimal * size_div_info.divisor_);
      size_div_info.divisor_ /= base;
      *oit = value_to_hex<char_case, char_type>(static_cast<uint8_t>(decimal));
      ranges::advance(oit, 1);
      --size_div_info.size_;
      }

    return oit;
    }
  };

template<typename base_conv_type, char_case_e char_case, concepts::char_type char_type>
inline constexpr unsigned_to_str_transform_t<base_conv_type, char_case, char_type> unsigned_to_str_transform;

//--------------------------------------------------------------------------------------------------------
///\brief preprocessed info for output string calculation and future formating
template<std::unsigned_integral value_type>
struct estimate_info_t
  {
  size_div_info_t<value_type> size_div_info;
  value_type uvalue;
  unsigned output_prefix_size{};
  unsigned precision{};
  std::optional<char> sign;

  constexpr std::size_t number_size() const noexcept
    {
    return (sign ? 1u : 0u) + output_prefix_size + size_div_info.size_;
    }

  constexpr std::size_t size() const noexcept { return std::max<std::size_t>(number_size(), precision); }
  };

///\brief calculate required output space for intergral to string convertion
template<integral_format_traits traits>
struct estimate_integral_to_str_t
  {
  template<std::unsigned_integral value_type>
  stralgo_static_call_operator constexpr auto
    operator()(value_type value, std::optional<char> sign = {}) stralgo_static_call_operator_const noexcept
    {
    using base_conv_type = detail::base_conv_by_format_t<traits.format>;
    constexpr value_type base{static_cast<value_type>(base_conv_type::base)};

    estimate_info_t<value_type> result{};
    result.size_div_info = calculate_size_div_info<base>(value);
    result.uvalue = value;
    result.precision = traits.precision;

    if constexpr(traits.precision != 0)
      if(result.size_div_info.size_ == 0)
        {
        result.size_div_info.size_ = 1;
        result.size_div_info.divisor_ = 1;
        }

    if(result.size_div_info.size_ != 0)
      {
      if constexpr(traits.sign == prepend_sign_e::always)
        {
        if(!sign)
          result.sign = '+';
        else
          result.sign = sign;
        }
      else
        result.sign = sign;

      if constexpr(traits.include_prefix == include_prefix_e::with_prefix && !base_conv_type::output_prefix.empty())
        result.output_prefix_size = base_conv_type::output_prefix.size();
      }
    return result;
    }

  template<std::signed_integral value_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(value_type value) stralgo_static_call_operator_const noexcept
    {
    using unsigned_type = std::make_unsigned_t<value_type>;
    unsigned_type uvalue;
    std::optional<char> sign;
    if(value >= 0)
      uvalue = static_cast<unsigned_type>(value);
    else
      {
      sign = '-';
      uvalue = static_cast<unsigned_type>(-value);
      }
    return operator()(uvalue, sign);
    }
  };

template<integral_format_traits traits>
inline constexpr estimate_integral_to_str_t<traits> estimate_integral_to_str;

//--------------------------------------------------------------------------------------------------------

template<integral_format_traits traits>
struct unsigned_to_str_t
  {
  template<std::unsigned_integral value_type, concepts::typed_char_output_iterator output_iterator>
  stralgo_static_call_operator constexpr auto operator()(
    estimate_info_t<value_type> const & est_info, output_iterator oit
  ) stralgo_static_call_operator_const noexcept -> output_iterator
    {
    using char_type = std::iter_value_t<output_iterator>;
    using iter_diff_type = std::iter_difference_t<output_iterator>;
    using base_conv_type = base_conv_by_format_t<traits.format>;

    if(est_info.size_div_info.size_ != 0)
      {
      auto projection = char_case_projection<traits.char_case>();

      auto const estimated_number_size{est_info.number_size()};

      // ----------------------------
      if constexpr(traits.padd_with == padd_with_e::space && traits.alignment != alignment_e::left)
        {
        if(estimated_number_size < traits.precision)
          {
          iter_diff_type fill_len = static_cast<iter_diff_type>(traits.precision - estimated_number_size);
          iter_diff_type left_fill;
          if constexpr(traits.alignment == alignment_e::right)
            left_fill = fill_len;
          else
            left_fill = fill_len >> 1;
          oit = stralgo::detail::fill(oit, ranges::next(oit, left_fill), char_type(' '));
          }
        }

      if(est_info.sign)
        {
        *oit = char_type(*est_info.sign);
        ranges::advance(oit, 1);
        }

      if constexpr(traits.include_prefix == include_prefix_e::with_prefix && !base_conv_type::output_prefix.empty())
        oit = stralgo::detail::transform(
          ranges::begin(base_conv_type::output_prefix),
          ranges::end(base_conv_type::output_prefix),
          oit,
          [projection](auto c) noexcept { return static_cast<char_type>(projection(c)); }
        );

      if constexpr(traits.padd_with == padd_with_e::zeros)
        {
        if(estimated_number_size < traits.precision)
          {
          iter_diff_type fill_len = static_cast<iter_diff_type>(traits.precision - estimated_number_size);
          oit = stralgo::detail::fill(oit, ranges::next(oit, fill_len), char_type('0'));
          }
        }

      oit = unsigned_to_str_transform<base_conv_type, traits.char_case, char_type>(
        est_info.uvalue, est_info.size_div_info, oit
      );

      if constexpr(traits.padd_with == padd_with_e::space && traits.alignment != alignment_e::right)
        if(estimated_number_size < traits.precision)
          {
          iter_diff_type fill_len = static_cast<iter_diff_type>(traits.precision - estimated_number_size);
          iter_diff_type right_fill;
          if constexpr(traits.alignment == alignment_e::left)
            right_fill = fill_len;
          else
            right_fill = fill_len - (fill_len >> 1);
          oit = stralgo::detail::fill(oit, ranges::next(oit, right_fill), char_type(' '));
          }
      }
    return oit;
    }
  };

template<integral_format_traits traits>
inline constexpr unsigned_to_str_t<traits> unsigned_to_str;

//--------------------------------------------------------------------------------------------------------
// template<integral_format_traits traits,
//          typename output_iterator,
//          std::unsigned_integral unsinged_value_type>
// constexpr output_iterator integral_to_string_( estimate_info_t<unsinged_value_type> const & est_info,
//                                                output_iterator oit ) noexcept
//   {
//   return unsigned_to_str<traits>(est_info, oit);
//   }

//--------------------------------------------------------------------------------------------------------
template<integral_format_traits traits>
struct integral_to_ascii_t
  {
  template<concepts::typed_char_output_iterator output_iterator, std::integral value_type>
  stralgo_static_call_operator constexpr auto
    operator()(value_type value, output_iterator oit) stralgo_static_call_operator_const noexcept -> output_iterator
    {
    auto est_info{estimate_integral_to_str<traits>(value)};
    return unsigned_to_str<traits>(est_info, oit);
    }
  };

template<integral_format_traits traits>
inline constexpr integral_to_ascii_t<traits> integral_to_ascii;

//--------------------------------------------------------------------------------------------------------
template<template<typename> typename basic_string_type, concepts::char_type char_type, integral_format_traits traits>
struct integral_to_string_t
  {
  template<std::integral value_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(value_type value) stralgo_static_call_operator_const->basic_string_type<char_type>
    {
    using string_type = basic_string_type<char_type>;
    using size_type = typename string_type::size_type;
    constexpr bool supports_resize_and_overwrite = supports_resize_and_overwrite_v<basic_string_type>;

    auto est_info{estimate_integral_to_str<traits>(value)};

    return copy_views_t<string_type, supports_resize_and_overwrite>{
    }(static_cast<size_type>(est_info.size()),
      [&est_info](auto out_iterator) noexcept { return unsigned_to_str<traits>(est_info, out_iterator); });
    }
  };

template<concepts::char_type char_type, integral_format_traits traits>
inline constexpr integral_to_string_t<small_vectors::basic_string, char_type, traits> integral_to_string;

namespace stl
  {
  template<concepts::char_type char_type, integral_format_traits traits>
  inline constexpr integral_to_string_t<basic_string, char_type, traits> integral_to_string;
  }

//--------------------------------------------------------------------------------------------------------

template<std::floating_point value_type>
struct float_estimate_info_t
  {
  size_div_info_t<uint64_t> size_div_info;
  uint64_t uvalue;
  value_type fraction;
  unsigned output_prefix_size{};
  unsigned precision{};
  unsigned decimal_places{};
  std::optional<char> sign;

  constexpr std::size_t number_size() const noexcept
    {
    return (sign ? 1u : 0u) + (decimal_places != 0 ? 1u : 0u) + output_prefix_size + size_div_info.size_
           + decimal_places;
    }

  constexpr std::size_t size() const noexcept { return std::max<std::size_t>(number_size(), precision); }
  };

//--------------------------------------------------------------------------------------------------------
template<float_format_traits traits>
struct estimate_float_to_string_t
  {
  template<std::floating_point float_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(float_type value) stralgo_static_call_operator_const noexcept
    {
    using base_conv_type = base_conv_by_format_t<format_e::decimal>;

    float_estimate_info_t<float_type> result{};
    result.precision = traits.precision;

    if(value >= float_type{0})
      {
      if constexpr(traits.sign == prepend_sign_e::always)
        result.sign = '+';
      }
    else
      {
      result.sign = '-';
      value = -value;
      }
    result.uvalue = static_cast<uint64_t>(value);

    // store unsigned then fraction
    result.size_div_info = calculate_size_div_info<base_conv_type::base>(result.uvalue);
    if constexpr(traits.precision != 0)
      if(result.size_div_info.size_ == 0)
        {
        result.size_div_info.size_ = 1;
        result.size_div_info.divisor_ = 1;
        }

    result.fraction = value - static_cast<float_type>(result.uvalue);

    // calculate real decimal places
    auto decimal_places = traits.decimal_places;

    if constexpr(traits.trailing_zeros == trailing_zeros_e::skip)
      {
      auto tmp_value{result.fraction};
      for(; decimal_places != 0 && tmp_value != float_type{0}; --decimal_places)
        {
        float_type next_fraction{tmp_value * base_conv_type::base};
        unsigned ufraction{static_cast<unsigned>(next_fraction)};
        tmp_value = next_fraction - static_cast<float_type>(ufraction);
        }
      decimal_places = traits.decimal_places - decimal_places;
      }
    else
      {
      if(result.size_div_info.size_ == 0)
        decimal_places = 0;
      }
    result.decimal_places = decimal_places;

    //     if constexpr ( traits.include_prefix == include_prefix_e::with_prefix &&
    //     !base_conv_type::output_prefix.empty())
    //       if( result.size_div_info.size_ != 0 )
    //         result.output_prefix_size = base_conv_type::output_prefix.size();

    return result;
    }
  };

template<float_format_traits traits>
inline constexpr estimate_float_to_string_t<traits> estimate_float_to_string;

template<float_format_traits traits>
struct float_to_ascii_t
  {
  template<std::floating_point float_type, concepts::typed_char_output_iterator output_iterator>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(float_estimate_info_t<float_type> const & est_info, output_iterator oit)
      stralgo_static_call_operator_const noexcept -> output_iterator
    {
    using base_conv_type = base_conv_by_format_t<format_e::decimal>;
    using char_type = std::iter_value_t<output_iterator>;
    using iter_diff_type = std::iter_difference_t<output_iterator>;
    // auto projection = char_case_projection<char_case_e::lowercase>();

    auto const estimated_number_size{est_info.number_size()};

    if constexpr(traits.padd_with == padd_with_e::space && traits.alignment != alignment_e::left)
      {
      if(estimated_number_size < traits.precision)
        {
        iter_diff_type fill_len = static_cast<iter_diff_type>(traits.precision - estimated_number_size);
        iter_diff_type left_fill;
        if constexpr(traits.alignment == alignment_e::right)
          left_fill = fill_len;
        else
          left_fill = fill_len >> 1;
        oit = stralgo::detail::fill(oit, ranges::next(oit, left_fill), char_type(' '));
        }
      }

    if(est_info.sign)
      {
      *oit = char_type(*est_info.sign);
      ranges::advance(oit, 1);
      }

    //     if constexpr ( traits.include_prefix == include_prefix_e::with_prefix &&
    //     !base_conv_type::output_prefix.empty())
    //       if( est_info.size_div_info.size_ != 0 )
    //         oit = stralgo::detail::transform( std::begin(base_conv_type::output_prefix),
    //         std::end(base_conv_type::output_prefix), oit, projection);

    if constexpr(traits.padd_with == padd_with_e::zeros)
      {
      if(estimated_number_size < traits.precision)
        {
        iter_diff_type fill_len = static_cast<iter_diff_type>(traits.precision - estimated_number_size);
        oit = stralgo::detail::fill(oit, ranges::next(oit, fill_len), char_type('0'));
        }
      }

    oit = unsigned_to_str_transform<base_conv_type, char_case_e::lowercase, char_type>(
      est_info.uvalue, est_info.size_div_info, oit
    );

    if(est_info.decimal_places != 0)
      {
      *oit = char_type('.');
      ranges::advance(oit, 1);

      auto fraction{est_info.fraction};
      // store fraction
      for(uint32_t dpl{est_info.decimal_places}; dpl != 0; --dpl)
        {
        float_type next_fraction{fraction * base_conv_type::base};
        unsigned ufraction{static_cast<unsigned>(next_fraction)};
        *oit = value_to_hex<char_case_e::lowercase, char_type>(static_cast<uint8_t>(ufraction));
        stralgo_clang_unsafe_buffer_usage_begin //
        ++oit;
        stralgo_clang_unsafe_buffer_usage_end //
        fraction = next_fraction - static_cast<float_type>(ufraction);
        }
      }

    if constexpr(traits.padd_with == padd_with_e::space && traits.alignment != alignment_e::right)
      if(estimated_number_size < traits.precision)
        {
        iter_diff_type fill_len = static_cast<iter_diff_type>(traits.precision - estimated_number_size);
        iter_diff_type right_fill;
        if constexpr(traits.alignment == alignment_e::left)
          right_fill = fill_len;
        else
          right_fill = fill_len - (fill_len >> 1);
        oit = stralgo::detail::fill(oit, ranges::next(oit, right_fill), char_type(' '));
        }

    return oit;
    }

  template<std::floating_point float_type, typename output_iterator>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(float_type value, output_iterator oit) stralgo_static_call_operator_const noexcept -> output_iterator
    {
    float_estimate_info_t<float_type> est_info{estimate_float_to_string<traits>(value)};
    return operator()(est_info, oit);
    }
  };

template<float_format_traits traits>
inline constexpr float_to_ascii_t<traits> float_to_ascii;

//--------------------------------------------------------------------------------------------------------
template<template<typename> typename basic_string_type, concepts::char_type char_type, float_format_traits traits>
struct float_to_string_t
  {
  template<std::floating_point value_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(value_type value) stralgo_static_call_operator_const->basic_string_type<char_type>
    {
    using string_type = basic_string_type<char_type>;
    using size_type = typename string_type::size_type;
    constexpr bool supports_resize_and_overwrite = supports_resize_and_overwrite_v<basic_string_type>;

    auto est_info{detail::estimate_float_to_string<traits>(value)};

    return copy_views_t<string_type, supports_resize_and_overwrite>{
    }(static_cast<size_type>(est_info.size()),
      [&est_info](auto out_iterator) noexcept { return float_to_ascii<traits>(est_info, out_iterator); });
    }
  };

template<concepts::char_type char_type, float_format_traits traits>
inline constexpr float_to_string_t<small_vectors::basic_string, char_type, traits> float_to_string;

namespace stl
  {
  template<concepts::char_type char_type, float_format_traits traits>
  inline constexpr float_to_string_t<basic_string, char_type, traits> float_to_string;
  }

//--------------------------------------------------------------------------------------------------------
template<std::unsigned_integral integral_type, concepts::char_iterator iterator>
struct tstoui_result_t
  {
  integral_type result;
  iterator out;
  };

template<std::unsigned_integral integral_type, typename base_conv>
struct trimed_string_to_unsigned_integral_t
  {
  template<concepts::char_iterator iterator, std::sentinel_for<iterator> sentinel>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(iterator beg, sentinel end) stralgo_static_call_operator_const noexcept
    -> tstoui_result_t<integral_type, iterator>
    {
    using char_type = std::iter_value_t<iterator>;
    using result_type = tstoui_result_t<integral_type, iterator>;

    integral_type total{};
    while(beg != end)
      {
      char_type const c{*beg};
      if(base_conv::is_number(c))
        {
        total = base_conv::base * total + base_conv::template convert<integral_type>(c);
        ranges::advance(beg, 1);
        }
      else
        break;
      }
    return result_type{.result = total, .out = beg};
    }
  };

template<std::unsigned_integral integral_type, typename base_conv>
inline constexpr trimed_string_to_unsigned_integral_t<integral_type, base_conv> trimed_string_to_unsigned_integral;

//--------------------------------------------------------------------------------------------------------

struct is_hex_prefix_t
  {
  template<concepts::char_type char_type>
  stralgo_static_call_operator constexpr bool
    operator()(char_type c0, char_type c1) stralgo_static_call_operator_const noexcept
    {
    return c0 == char_type('0') && (c1 == char_type('x') || c1 == char_type('X'));
    }
  };

inline constexpr is_hex_prefix_t is_hex_prefix;

//--------------------------------------------------------------------------------------------------------
template<std::unsigned_integral integral_type, input_format_e input_format>
struct string_to_unsigned_integral_t
  {
  template<concepts::char_range string_view_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(string_view_type const & str_number) stralgo_static_call_operator_const noexcept
    {
    using ranges::begin;
    using ranges::end;
    using char_type = ranges::range_value_t<string_view_type>;

    auto snumber{trim_left(str_number)};
    auto out_it{begin(str_number)};
    integral_type result{};
    if(!ranges::empty(snumber))
      {
      using oit_type = decltype(begin(snumber));
      using tstoui_result_type = tstoui_result_t<integral_type, oit_type>;
      tstoui_result_type data{{}, begin(snumber)};
      auto it{begin(snumber)};
      if(char_type c{*it}; c != char_type('-'))
        {
        if(c == char_type('+'))
          ranges::advance(it, 1);

        if constexpr(input_format == input_format_e::undetermined)
          {
          // if contains hex prefix assume hexadecimal
          stralgo_clang_unsafe_buffer_usage_begin //
          if(auto next_it{it + 1}; next_it < end(snumber) && is_hex_prefix(*it, *next_it))
            {
            it += 2;
            data = trimed_string_to_unsigned_integral<integral_type, base_16_t>(it, end(snumber));
            }
          stralgo_clang_unsafe_buffer_usage_end //
          else
            data = trimed_string_to_unsigned_integral<integral_type, base_10_t>(it, end(snumber));
          }
        else if constexpr(input_format == input_format_e::hexadecimal)
          {
          stralgo_clang_unsafe_buffer_usage_begin //
          // skip hex prefix if exists
          if(auto next_it{it + 1}; next_it < end(snumber) && is_hex_prefix(*it, *next_it))
            it += 2;
          stralgo_clang_unsafe_buffer_usage_end //
          data = trimed_string_to_unsigned_integral<integral_type, base_16_t>(it, end(snumber));
          }
        else
          data = trimed_string_to_unsigned_integral<integral_type, base_10_t>(it, end(snumber));
        using ranges::size;
        result = data.result;
        out_it = ranges::next(
          begin(str_number),
          static_cast<std::ptrdiff_t>(size(str_number) - size(snumber)) + ranges::distance(begin(snumber), data.out)
        );
        }
      }

    return std::make_pair(result, out_it);
    }
  };

template<std::unsigned_integral integral_type, input_format_e input_format>
inline constexpr string_to_unsigned_integral_t<integral_type, input_format> string_to_unsigned_integral;

//--------------------------------------------------------------------------------------------------------
///\brief signed integral convertion from string supports untrimmed strings of decimal [+/-]d[n] and hexadecimal lower
///and uppercase [+/-]0xh[n] numbers
template<std::signed_integral integral_type, input_format_e input_format>
struct string_to_signed_integral_t
  {
  template<concepts::char_range string_view_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(string_view_type const & str_number) stralgo_static_call_operator_const noexcept
    {
    using char_type = ranges::range_value_t<string_view_type>;
    using unsigned_itegral_type = std::make_unsigned_t<integral_type>;
    using ranges::begin;
    using ranges::end;

    integral_type total{};
    char_type sign{};

    auto snumber{trim_left(str_number)};
    auto ret_it{ranges::begin(str_number)};
    if(!ranges::empty(snumber))
      {
      using oit_type = decltype(begin(snumber));
      using tstoui_result_type = tstoui_result_t<unsigned_itegral_type, oit_type>;
      tstoui_result_type data{{}, begin(snumber)};

      char_type const c{*data.out};
      sign = c;  // save sign indication if '-', then negative, otherwise positive
      if(c == char_type('-') || c == char_type('+'))
        ranges::advance(data.out, 1);

      if constexpr(input_format == input_format_e::undetermined)
        {
        if(auto next_it{ranges::next(data.out, 1)}; next_it < end(snumber) && is_hex_prefix(*data.out, *next_it))
          {
          ranges::advance(data.out, 2);
          data = trimed_string_to_unsigned_integral<unsigned_itegral_type, base_16_t>(data.out, end(snumber));
          }
        else
          data = trimed_string_to_unsigned_integral<unsigned_itegral_type, base_10_t>(data.out, end(snumber));
        }
      else if constexpr(input_format == input_format_e::hexadecimal)
        {
        // skip hex prefix if exists
        if(auto next_it{ranges::next(data.out, 1)}; next_it < end(snumber) && is_hex_prefix(*data.out, *next_it))
          ranges::advance(data.out, 2);

        data = trimed_string_to_unsigned_integral<unsigned_itegral_type, base_16_t>(data.out, end(snumber));
        }
      else
        data = trimed_string_to_unsigned_integral<unsigned_itegral_type, base_10_t>(data.out, end(snumber));

      total = static_cast<integral_type>(data.result);
      if(sign == '-')
        total = static_cast<integral_type>(-total);

      using ranges::size;

      ret_it = ranges::next(
        begin(str_number),
        static_cast<std::ptrdiff_t>(size(str_number) - size(snumber)) + ranges::distance(begin(snumber), data.out)
      );
      }
    return std::make_pair(total, ret_it);
    }
  };

template<std::signed_integral integral_type, input_format_e input_format>
inline constexpr string_to_signed_integral_t<integral_type, input_format> string_to_signed_integral;

template<std::integral integral_type, input_format_e input_format>
struct string_to_integral_t
  {
  template<concepts::char_range string_view_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(string_view_type const & str_number) stralgo_static_call_operator_const noexcept
    {
    if constexpr(std::is_signed_v<integral_type>)
      return string_to_signed_integral<integral_type, input_format>(str_number);
    else
      return string_to_unsigned_integral<integral_type, input_format>(str_number);
    }
  };

template<std::integral integral_type, input_format_e input_format>
inline constexpr string_to_integral_t<integral_type, input_format> string_to_integral;

//--------------------------------------------------------------------------------------------------------
template<std::floating_point float_type, concepts::char_iterator iterator>
struct tstof_result_t
  {
  float_type result;
  iterator out;
  };

template<std::floating_point float_type, typename base_conv_t>
struct trimed_string_to_float_t
  {
  template<concepts::char_iterator iterator, std::sentinel_for<iterator> sentinel>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(iterator first, sentinel last) stralgo_static_call_operator_const noexcept

    {
    using ranges::advance;
    using char_type = std::iter_value_t<iterator>;

    float_type total{};
    float_type fraction{};
    constexpr float_type ibase{float_type(1) / base_conv_t::base};

    auto it{first};

    for(; it != last; advance(it, 1))
      {
      char_type c{*it};
      if(base_conv_t::is_number(c))
        total = base_conv_t::base * total + static_cast<float_type>(base_conv_t::template convert<int>(c));
      else
        break;
      }
    if(it != last && *it == char_type('.'))
      {
      advance(it, 1);
      float_type divider{ibase};
      for(; it != last; advance(it, 1))
        {
        char_type const c{*it};
        if(base_conv_t::is_number(c))
          fraction = fraction + static_cast<float_type>(base_conv_t::template convert<int>(c)) * divider;
        else
          break;
        divider = divider * ibase;
        }
      }
    return tstof_result_t<float_type, iterator>{total + fraction, it};
    }
  };

//--------------------------------------------------------------------------------------------------------
template<std::floating_point float_type>
struct string_to_float_t
  {
  template<concepts::char_range string_view_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(string_view_type const & str_number) stralgo_static_call_operator_const noexcept
    {
    using char_type = ranges::range_value_t<string_view_type>;
    using ranges::advance;
    using ranges::begin;
    using ranges::distance;
    using ranges::end;
    using ranges::next;

    auto snumber{stralgo::trim_left(str_number)};
    if(!snumber.empty())
      {
      using oit_type = decltype(begin(snumber));
      tstof_result_t<float_type, oit_type> result;

      char_type sign{};
      auto it{begin(snumber)};

      char_type c{*it};
      sign = c;  // save sign indication if '-', then negative, otherwise positive
      if(c == char_type('-') || c == char_type('+'))
        advance(it, 1);
      auto next_it{next(it, 1)};
      if(next_it != end(snumber) && is_hex_prefix(*it, *next_it))
        {
        advance(it, 2);
        result = trimed_string_to_float_t<float_type, base_16_t>{}(it, end(snumber));
        }
      else
        result = trimed_string_to_float_t<float_type, base_10_t>{}(it, end(snumber));

      auto oit = next(begin(str_number), distance(begin(snumber), result.out));
      if(sign == char_type('-'))
        result.result = -result.result;
      return std::make_pair(result.result, oit);
      }
    else  // nothing to convert return begin iterator
      return std::make_pair(float_type{}, begin(str_number));
    }
  };

template<std::floating_point float_type>
inline constexpr string_to_float_t<float_type> string_to_float;

//--------------------------------------------------------------------------------------------------------
// compose
//--------------------------------------------------------------------------------------------------------

// preconvertion char type
template<concepts::char_type char_type>
struct view_preconv_char_t
  {
  char_type value_;

  constexpr std::size_t view_size() const noexcept { return 1; }

  template<concepts::char_iterator iterator>
    requires std::same_as<std::iter_value_t<iterator>, char_type>
  constexpr iterator transform(iterator it) const noexcept
    {
    *it = value_;
    ranges::advance(it, 1);
    return it;
    }
  };

template<concepts::char_type char_type, typename T>
struct compose_preconv_t
  {
  };

template<concepts::char_type char_type, concepts::char_type maybe_char_type>
struct compose_preconv_t<char_type, maybe_char_type>
  {
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(maybe_char_type value) stralgo_static_call_operator_const noexcept
    requires std::same_as<char_type, maybe_char_type>
    {
    return view_preconv_char_t<char_type>{value};
    }
  };

//--------------------------------------------------------------------------------------------------------
/// preconv string_view
template<concepts::char_type char_type>
struct view_preconv_string_view_t
  {
  std::basic_string_view<char_type> view_;

  constexpr std::size_t view_size() const noexcept { return view_.size(); }

  template<concepts::char_iterator iterator>
    requires std::same_as<std::iter_value_t<iterator>, char_type>
  constexpr iterator transform(iterator it) const noexcept
    {
    return ranges::copy(ranges::begin(view_), ranges::end(view_), it).out;
    }
  };

template<concepts::char_type char_type, concepts::char_range string_view_type>
  requires std::same_as<char_type, ranges::range_value_t<string_view_type>>
struct compose_preconv_t<char_type, string_view_type>
  {
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(string_view_type const & value) stralgo_static_call_operator_const noexcept
    {
    return view_preconv_string_view_t<char_type>{
      std::basic_string_view<char_type>(ranges::begin(value), ranges::end(value))
    };
    }
  };

//--------------------------------------------------------------------------------------------------------
// preconv integral
template<std::integral integral_type, integral_format_traits traits = integral_format_traits{}>
  requires(!concepts::char_type<integral_type>)
struct view_preconv_integral_t
  {
  using unsigned_integral_type = std::make_unsigned_t<integral_type>;
  estimate_info_t<unsigned_integral_type> est_info_;

  constexpr view_preconv_integral_t(integral_type value) noexcept : est_info_{estimate_integral_to_str<traits>(value)}
    {
    }

  constexpr std::size_t view_size() const noexcept { return est_info_.size(); }

  template<concepts::char_iterator iterator>
  constexpr iterator transform(iterator oit) const
    {
    return unsigned_to_str<traits>(est_info_, oit);
    }
  };

template<integral_format_traits traits, std::integral integral_type>
  requires(!concepts::char_type<integral_type>)
constexpr auto fmt(integral_type value)
  {
  return view_preconv_integral_t<integral_type, traits>{value};
  }

//
template<concepts::char_type char_type, std::integral integral_type>
  requires(!concepts::char_type<integral_type>)
struct compose_preconv_t<char_type, integral_type>
  {
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(integral_type value) stralgo_static_call_operator_const noexcept
    {
    return view_preconv_integral_t<integral_type>{value};
    }
  };

template<concepts::char_type char_type, std::integral integral_type, integral_format_traits traits>
  requires(!concepts::char_type<integral_type>)
struct compose_preconv_t<char_type, view_preconv_integral_t<integral_type, traits>>
  {
  stralgo_static_call_operator constexpr auto operator()(view_preconv_integral_t<integral_type, traits> value
  ) stralgo_static_call_operator_const noexcept
    {
    return value;
    }
  };

template<concepts::char_type char_type, concepts::enumeration maybe_enum_type>
struct compose_preconv_t<char_type, maybe_enum_type>
  {
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(maybe_enum_type value) stralgo_static_call_operator_const noexcept
    {
    using underlying_type = std::underlying_type_t<maybe_enum_type>;
    return compose_preconv_t<char_type, underlying_type>{}(static_cast<underlying_type>(value));
    }
  };

//--------------------------------------------------------------------------------------------------------
// preconv floating point
template<std::floating_point float_type, float_format_traits traits = float_format_traits{}>
struct view_preconv_float_t
  {
  float_estimate_info_t<float_type> est_info_;

  constexpr view_preconv_float_t(float_type value) : est_info_{estimate_float_to_string<traits>(value)} {}

  constexpr std::size_t view_size() const noexcept { return est_info_.size(); }

  template<concepts::char_iterator iterator>
  constexpr iterator transform(iterator oit) const
    {
    // using char_type = std::iter_value_t<iterator>;
    return float_to_ascii<traits>(est_info_, oit);
    }
  };

template<float_format_traits traits, std::floating_point float_type>
constexpr auto fmt(float_type value)
  {
  return view_preconv_float_t<float_type, traits>{value};
  }

template<concepts::char_type char_type, std::floating_point float_type>
struct compose_preconv_t<char_type, float_type>
  {
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(float_type value) stralgo_static_call_operator_const noexcept
    {
    return view_preconv_float_t<float_type>{value};
    }
  };

template<concepts::char_type char_type, std::floating_point float_type, float_format_traits traits>
struct compose_preconv_t<char_type, view_preconv_float_t<float_type, traits>>
  {
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(view_preconv_float_t<float_type, traits> value) stralgo_static_call_operator_const noexcept
    {
    return value;
    }
  };

//--------------------------------------------------------------------------------------------------------
// main composing
template<concepts::char_type char_type, typename... args_type>
constexpr auto compose_preprocess(args_type const &... args)
  {
  return std::make_tuple(compose_preconv_t<char_type, args_type>{}(args)...);
  }

template<typename input_argument_type, typename... args_type>
constexpr auto preprocessed_count_size(input_argument_type const & preparg, args_type const &... args) noexcept
  {
  if constexpr(sizeof...(args_type) != 0)
    return preparg.view_size() + preprocessed_count_size(args...);
  else
    return preparg.view_size();
  }

template<
  concepts::char_type char_type,
  concepts::char_iterator iterator,
  typename input_argument_type,
  typename... args_type>
constexpr iterator
  preprocessed_transform_views(iterator it, input_argument_type const & preparg, args_type const &... args) noexcept
  {
  it = preparg.transform(it);
  if constexpr(sizeof...(args_type) != 0)
    it = preprocessed_transform_views<char_type>(it, args...);
  return it;
  }

// validate argument types so constraint substitution failure will produce more human friendly results and not dive into
// function calls
template<typename T, typename char_type>
concept compose_arg_concept = requires(T const & arg) {
  requires concepts::char_type<char_type>;
  stralgo::detail::compose_preconv_t<char_type, T>{}(arg);
  // all deduced char types must match
  requires concepts::match_char_type_or_void<char_type, T>;
};

template<template<typename> typename basic_string_type, concepts::char_type char_type>
struct compose_t
  {
  template<compose_arg_concept<char_type>... input_argument_type_n>
    requires(sizeof...(input_argument_type_n) > 1)
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(input_argument_type_n const &... args) stralgo_static_call_operator_const
    {
    auto preprocessed{compose_preprocess<char_type>(args...)};
    using string_type = basic_string_type<char_type>;
    using size_type = typename string_type::size_type;
    constexpr bool supports_resize_and_overwrite = supports_resize_and_overwrite_v<basic_string_type>;
    using copy_view_to_str = copy_views_t<string_type, supports_resize_and_overwrite>;

    size_type aprox_size{static_cast<size_type>(
      std::apply([](auto const &... tuple_args) { return preprocessed_count_size(tuple_args...); }, preprocessed)
    )};
    return copy_view_to_str{
    }(aprox_size,
      [&](auto out_iterator)
      {
        auto new_end{std::apply(
          [out_iterator](auto const &... tuple_args)
          { return preprocessed_transform_views<char_type>(out_iterator, tuple_args...); },
          preprocessed
        )};
        return new_end;
      });
    }
  };

template<concepts::char_type char_type>
inline constexpr compose_t<small_vectors::basic_string, char_type> compose;

namespace stl
  {
  template<concepts::char_type char_type>
  inline constexpr compose_t<basic_string, char_type> compose;
  }
  }  // namespace stralgo::detail
