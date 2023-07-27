#include <unit_test_core.h>
#include <stralgo/strconv_numeric.h>
#include <stralgo/detail/strong_type_compose.h>
#include <coll/basic_fixed_string.h>
#include <ctime>

using namespace std::literals::string_view_literals;
using stralgo::char_case_e;

template<typename decl_chr_type>
struct cfs_t
{
  template<typename char_type, std::size_t N>
  inline constexpr auto operator()(char_type const (&str)[N]) const noexcept
    { return coll::basic_fixed_string<decl_chr_type, N - 1>(str); }
};

using metatests::constexpr_test;
using metatests::run_consteval_test;
using metatests::run_constexpr_test;
namespace ut = boost::ut;
using ut::operator""_test;
using namespace ut::operators::terse;
using metatests::test_result;
using char_type_list = metatests::type_list<char,char8_t,char16_t,char32_t,wchar_t>;
using coll::cast_fixed_string;

namespace stralgo_value_to_hex
{
static void do_test(test_result &result)
  {
  "stralgo_value_to_hex"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr_test( stralgo::value_to_hex<char_case_e::uppercase,char_type>(uint8_t(0)) == char_type('0') );
      constexpr_test( stralgo::value_to_hex<char_case_e::uppercase,char_type>(uint8_t(5)) == char_type('5') );
      constexpr_test( stralgo::value_to_hex<char_case_e::uppercase,char_type>(uint8_t(15)) == char_type('F') );
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_from_hex_ascii
{
static void do_test(test_result &result)
  {
  "stralgo_from_hex_ascii"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      constexpr auto input { cfs("ff1a34b5f30027")};
      uint8_t res[7]{};
      stralgo::from_hex_ascii( std::begin(input), std::end(input),  std::begin(res) );
      constexpr std::array<uint8_t,7> expected { {0xff,0x1a,0x34,0xb5,0xf3,0x00,0x27} };
      constexpr_test(stralgo::detail::equal( expected.begin(), expected.end(), &res[0] ));
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_to_hex_ascii
{
static void do_test(test_result &result)
  {
  "stralgo_to_hex_ascii"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      constexpr std::array<uint8_t,7> tst{0u,1u,2u,0xffu,0x34,0x7f,10};
      {
      constexpr auto expected{ cfs("000102FF347F0A")};
      std::array<char_type,14> out{};
      std::basic_string_view res{ out.begin(), out.end()};
      stralgo::to_hex_ascii<stralgo::char_case_e::uppercase>( std::begin(tst), std::end(tst), std::begin(out) );
      constexpr_test(res == expected);
      }
      {
      constexpr auto expected{ cfs("000102ff347f0a")};
      std::array<char_type,14> out{};
      std::basic_string_view res{ out.begin(), out.end()};
      stralgo::to_hex_ascii<stralgo::char_case_e::lowercase>( std::begin(tst), std::end(tst), std::begin(out) );
      constexpr_test(res == expected);
      }
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_to_hex_ascii_string
{
static void do_test(test_result &result)
  {
  "stralgo_to_hex_ascii_string"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      {  
      constexpr std::array<uint8_t,7> tst{0u,1u,2u,0xffu,0x34,0x7f,10};
      constexpr auto expected{ cfs("000102FF347F0A")};
      auto res{stralgo::to_hex_ascii_string<stralgo::char_case_e::uppercase,char_type>( std::begin(tst), std::end(tst) )};
      constexpr_test(res == expected.view());
      }
      {  
      constexpr std::array<uint8_t,7> tst{0u,1u,2u,0xffu,0x34,0x7f,10};
      constexpr auto expected{ cfs("000102ff347f0a")};
      auto res{stralgo::to_hex_ascii_string<stralgo::char_case_e::lowercase,char_type>( std::begin(tst), std::end(tst) )};
      constexpr_test(res == expected.view());
      }
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_calculate_size_div_info
{
static void do_test(test_result &result)
  {
  "calculate_size_div_info"_test = [&]
    {
    auto fn_tmpl =
      []() -> metatests::test_result
      {
      constexpr_test( stralgo::detail::calculate_size_div_info<10>(0u).size_ == 0 );
      constexpr_test( stralgo::detail::calculate_size_div_info<10>(1u).size_ == 1 );
      constexpr_test( stralgo::detail::calculate_size_div_info<10>(10u).size_ == 2 );
      constexpr_test( stralgo::detail::calculate_size_div_info<10>(99u).size_ == 2 );
      constexpr_test( stralgo::detail::calculate_size_div_info<10>(100u).size_ == 3 );
      
      constexpr_test( stralgo::detail::calculate_size_div_info<10>(0u).divisor_ == 0 );
      constexpr_test( stralgo::detail::calculate_size_div_info<10>(1u).divisor_ == 1 );
      constexpr_test( stralgo::detail::calculate_size_div_info<10>(10u).divisor_ == 10 );
      constexpr_test( stralgo::detail::calculate_size_div_info<10>(99u).divisor_ == 10 );
      constexpr_test( stralgo::detail::calculate_size_div_info<10>(100u).divisor_ == 100 );
      constexpr_test( stralgo::detail::calculate_size_div_info<10>(1000u).divisor_ == 1000 );
      constexpr_test( stralgo::detail::calculate_size_div_info<10>(9999u).divisor_ == 1000 );
      
      constexpr_test( stralgo::detail::calculate_size_div_info<10>( 18446744073709551610ull).divisor_ == 10000000000000000000ull );
      //18446744073709551615ull
      //10000000000000000000
      
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(0u).size_ == 0 );
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(1u).size_ == 1 );
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(0xfu).size_ == 1 );
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(0x10u).size_ == 2 );
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(0xffu).size_ == 2 );
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(0xfffu).size_ == 3 );
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(0xffffu).size_ == 4 );
      
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(0u).divisor_ == 0 );
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(1u).divisor_ == 1 );
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(0xfu).divisor_ == 1 );
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(0x10u).divisor_ == 0x10 );
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(0xffu).divisor_ == 0x10 );
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(0xfffu).divisor_ == 0x100 );
      constexpr_test( stralgo::detail::calculate_size_div_info<16>(0xffffu).divisor_ == 0x1000 );
      return {};
      };
    result |= run_consteval_test(fn_tmpl);
    result |= run_constexpr_test(fn_tmpl);
    };
  }
}

static constexpr auto integral_to_string_max_size = 128;

namespace stralgo_integral_to_ascii
{
static void do_test(test_result &result)
  {
  using stralgo::format_e;
  using stralgo::char_case_e;
  using stralgo::prepend_sign_e;
  using stralgo::padd_with_e;
  using stralgo::include_prefix_e;
  using stralgo::alignment_e;

  "stralgo_integral_to_ascii_base10"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      using traits = stralgo::integral_format_traits;
      constexpr cfs_t<char_type> cfs;

      std::array<char_type,integral_to_string_max_size> buffer_{};
      {
      constexpr uint8_t value{ 0 };
      constexpr auto expected{ cfs("0") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{}>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr int8_t value{ 0 };
      constexpr auto expected{ cfs("") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{.precision = 0}>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr uint32_t value{ 0 };
      constexpr auto expected{ cfs("    0") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{.precision = 5}>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr int32_t value{ 0 };
      constexpr auto expected{ cfs("000000") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{.precision = 6,
                                                   .padd_with = padd_with_e::zeros}>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr int64_t value{ 1501900 };
      constexpr auto expected{ cfs("1501900") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{}>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr uint64_t value{ 1844674407370955161 };
      constexpr auto expected{ cfs("1844674407370955161") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{}>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr uint64_t value{ 1844674407370955161 };
      constexpr auto expected{ cfs("+1844674407370955161") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .sign = prepend_sign_e::always }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr int64_t value{ -1844674407370955161 };
      constexpr auto expected{ cfs("  -1844674407370955161") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 22 }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr int64_t value{ -1844674407370955161 };
      constexpr auto expected{ cfs("-001844674407370955161") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 22,
                                                    .padd_with = padd_with_e::zeros }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
    
  "stralgo_integral_to_ascii_base16"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      std::array<char_type,integral_to_string_max_size> buffer_{};
      using traits = stralgo::integral_format_traits;
      constexpr auto hex_lower = traits{ .format = format_e::hexadecimal,
                                         .char_case = char_case_e::lowercase };
      constexpr auto hex_upper = traits{ .format = format_e::hexadecimal,
                                         .char_case = char_case_e::uppercase };
      {
      constexpr uint8_t value{ 0 };
      constexpr auto expected{ cfs("0x0") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<hex_lower>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr uint16_t value{ 0 };
      constexpr auto expected{ cfs("") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 0, 
                                                   .format = format_e::hexadecimal
                                                  }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr uint32_t value{ 0 };
      constexpr auto expected{ cfs("   0x0") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 6, 
                                                   .format = format_e::hexadecimal
                                                  }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr int32_t value{ 0 };
      constexpr auto expected{ cfs("0x0000") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 6, 
                                                   .format = format_e::hexadecimal,
                                                   .padd_with = padd_with_e::zeros
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr int64_t value{ 0 };
      constexpr auto expected{ cfs("000000") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 6, 
                                                   .format = format_e::hexadecimal,
                                                   .padd_with = padd_with_e::zeros,
                                                   .include_prefix = include_prefix_e::no_prefix
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0x1ffeb3e };
      constexpr auto expected{ cfs("0X1FFEB3E") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<hex_upper>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0x1ffeb3e };
      constexpr auto expected{ cfs("+0X0001FFEB3E") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 13, 
                                                    .format = format_e::hexadecimal,
                                                    .char_case = char_case_e::uppercase,
                                                    .padd_with = padd_with_e::zeros,
                                                    .sign = prepend_sign_e::always
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0x1ffeb3e };
      constexpr auto expected{ cfs("  +0X1FFEB3E") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 12, 
                                                    .format = format_e::hexadecimal,
                                                    .char_case = char_case_e::uppercase,
                                                    .padd_with = padd_with_e::space,
                                                    .sign = prepend_sign_e::always
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0x1ffeb3e };
      constexpr auto expected{ cfs("+0X1FFEB3E  ") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 12, 
                                                    .format = format_e::hexadecimal,
                                                    .char_case = char_case_e::uppercase,
                                                    .padd_with = padd_with_e::space,
                                                    .sign = prepend_sign_e::always,
                                                    .alignment = alignment_e::left
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0x1ffeb3e };
      constexpr auto expected{ cfs(" +0x1ffeb3e ") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 12, 
                                                    .format = format_e::hexadecimal,
                                                    .char_case = char_case_e::lowercase,
                                                    .padd_with = padd_with_e::space,
                                                    .sign = prepend_sign_e::always,
                                                    .alignment = alignment_e::middle
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr uint64_t value{ 0x1ffeb3ef1ffeb3 };
      constexpr auto expected{ cfs("0x1ffeb3ef1ffeb3") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<hex_lower>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr uint64_t value{ 0x1ffeb3ef1ffeb3 };
      constexpr auto expected{ cfs("+0x1ffeb3ef1ffeb3") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .format = format_e::hexadecimal,
                                                     .char_case = char_case_e::lowercase,
                                                     .sign = prepend_sign_e::always
                                                    }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr int64_t value{ -0x1ffeb3ef1ffeb3 };
      constexpr auto expected{ cfs("-0x1ffeb3ef1ffeb3") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .format = format_e::hexadecimal,
                                                    .char_case = char_case_e::lowercase,
                                                    .sign = prepend_sign_e::only_negative
                                                    }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr int64_t value{ -0x1ffeb3ef1ffeb3 };
      constexpr auto expected{ cfs("  -0x1ffeb3ef1ffeb3   ") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 22,
                                                    .format = format_e::hexadecimal,
                                                    .char_case = char_case_e::lowercase,
                                                    .sign = prepend_sign_e::only_negative,
                                                    .alignment = alignment_e::middle
                                                    }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
    
  "stralgo_integral_to_ascii_base2"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      std::array<char_type,integral_to_string_max_size> buffer_{};
      using traits = stralgo::integral_format_traits;
      {
      constexpr unsigned value{ 0 };
      constexpr auto expected{ cfs("0b0") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .format = format_e::binary
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0 };
      constexpr auto expected{ cfs("") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 0,
                                                    .format = format_e::binary
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0 };
      constexpr auto expected{ cfs("   0b0") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 6,
                                                    .format = format_e::binary
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0 };
      constexpr auto expected{ cfs("000000") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 6,
                                                    .format = format_e::binary,
                                                    .padd_with = padd_with_e::zeros,
                                                    .include_prefix = include_prefix_e::no_prefix
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0b1010011 };
      constexpr auto expected{ cfs("0b1010011") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .format = format_e::binary
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0b1010011 };
      constexpr auto expected{ cfs("0B1010011") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .format = format_e::binary,
                                                    .char_case = char_case_e::uppercase
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0b1010011 };
      constexpr auto expected{ cfs("1010011") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .format = format_e::binary,
                                                    .include_prefix = include_prefix_e::no_prefix
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0b1010011 };
      constexpr auto expected{ cfs("   1010011") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 10,
                                                    .format = format_e::binary,
                                                    .include_prefix = include_prefix_e::no_prefix
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0b1010011 };
      constexpr auto expected{ cfs(" 0b1010011") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 10,
                                                    .format = format_e::binary,
                                                    .include_prefix = include_prefix_e::with_prefix
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0b1010011 };
      constexpr auto expected{ cfs("0B000001010011") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 14,
                                                    .format = format_e::binary,
                                                    .char_case = char_case_e::uppercase,
                                                    .padd_with = padd_with_e::zeros,
                                                    .include_prefix = include_prefix_e::with_prefix
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr unsigned value{ 0b1010011 };
      constexpr auto expected{ cfs("+0b00001010011") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 14,
                                                    .format = format_e::binary,
                                                    .char_case = char_case_e::lowercase,
                                                    .padd_with = padd_with_e::zeros,
                                                    .sign = prepend_sign_e::always,
                                                    .include_prefix = include_prefix_e::with_prefix
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr uint64_t value{ 0b1100110011001100110011001100110011001100110011001100110011001 };
      constexpr auto expected{ cfs("0b1100110011001100110011001100110011001100110011001100110011001") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .format = format_e::binary
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr int64_t value{ -0b1100110011001100110011001100110011001100110011001100110011001 };
      constexpr auto expected{ cfs("-0b00001100110011001100110011001100110011001100110011001100110011001") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 68,
                                                    .format = format_e::binary,
                                                    .padd_with = padd_with_e::zeros
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr int64_t value{ -0b1100110011001100110011001100110011001100110011001100110011001 };
      constexpr auto expected{ cfs("    -0b1100110011001100110011001100110011001100110011001100110011001") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 68,
                                                    .format = format_e::binary,
                                                    .padd_with = padd_with_e::space
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      {
      constexpr int64_t value{ -0b1100110011001100110011001100110011001100110011001100110011001 };
      constexpr auto expected{ cfs("      -1100110011001100110011001100110011001100110011001100110011001") };
      auto itbeg{ buffer_.begin() };
      auto oit = stralgo::integral_to_ascii<traits{ .precision = 68,
                                                    .format = format_e::binary,
                                                    .padd_with = padd_with_e::space,
                                                    .include_prefix = include_prefix_e::no_prefix
                                                   }>( value, itbeg );
      std::basic_string_view res{ itbeg, oit };
      constexpr_test(expected == res);
      }
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_integral_to_string
{
static void do_test(test_result &result)
  {
  using stralgo::format_e;
  using stralgo::char_case_e;
  using stralgo::prepend_sign_e;
  using stralgo::include_prefix_e;
  using stralgo::alignment_e;
  "stralgo_integral_to_string"_test = [&]
    {
    auto fn_tmpl = []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      using traits = stralgo::integral_format_traits;
      constexpr cfs_t<char_type> cfs;
        {
        constexpr int64_t value{ -0x1ffeb3ef1ffeb3 };
        constexpr auto expected{ cfs("  -0x1ffeb3ef1ffeb3   ") };
        auto res = stralgo::integral_to_string<char_type,traits{ .precision = 22,
                                                       .format = format_e::hexadecimal,
                                                       .char_case = char_case_e::lowercase,
                                                       .sign = prepend_sign_e::only_negative,
                                                       .alignment = alignment_e::middle
                                                      }>( value );
                                                      
        constexpr_test( expected.view() == res );
        }
        {
        constexpr int64_t value{ 0 };
        constexpr auto expected{ cfs("0") };
        auto res = stralgo::integral_to_string<char_type,traits{}>( value );
        constexpr_test( expected.view() == res );
        }
        {
        constexpr int8_t value{ 0b1010011 };
        constexpr auto expected{ cfs(" 0b1010011") };
        auto res = stralgo::integral_to_string<char_type,traits{ .precision = 10, 
                                                                 .format = format_e::binary,
                                                                 .include_prefix = include_prefix_e::with_prefix
                                                                  }>( value );
                                                      
        constexpr_test( expected.view() == res );
        }
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_estimate_float_to_string
{
static void do_test(test_result &result)
  {
  using stralgo::trailing_zeros_e;
  
  "stralgo_estimate_float_to_string"_test = [&]
    {
    auto fn_tmpl = []()-> metatests::test_result
      {
      using traits = stralgo::float_format_traits;
      {
      constexpr double value{ 0.0 };
      auto est_info = stralgo::detail::estimate_float_to_string<traits{
                                                    .decimal_places = 4,
                                                    .trailing_zeros = trailing_zeros_e::skip
                                                    }>( value);
      constexpr_test(est_info.size() == 1);
      }
      {
      constexpr double value{ 0.0 };
      auto est_info = stralgo::detail::estimate_float_to_string<traits{
                                                .precision = 0,
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value);
      constexpr_test(est_info.size() == 0);
      }
      {
      constexpr double value{ 202.5 };
      auto est_info = stralgo::detail::estimate_float_to_string<traits{
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value);
      constexpr_test(est_info.size() == 5);
      }
      {
      constexpr double value{ 202.5 };
      auto est_info = stralgo::detail::estimate_float_to_string<traits{
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::preserve
                                                }>(value);
      constexpr_test(est_info.size() == 8);
      }
      {
      constexpr double value{ 0.5 };
      auto est_info = stralgo::detail::estimate_float_to_string<traits{
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>(value);
      constexpr_test(est_info.size() == 3);
      }
      {
      constexpr double value{ 0.5 };
      auto est_info = stralgo::detail::estimate_float_to_string<traits{
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::preserve
                                                }>(value);
      constexpr_test(est_info.size() == 6);
      }
      return {};
      };
    result |= run_consteval_test(fn_tmpl);
    result |= run_constexpr_test(fn_tmpl);
    };
  }
}
namespace stralgo_float_to_ascii
{
static void do_test(test_result &result)
  {
  using stralgo::format_e;
  using stralgo::char_case_e;
  using stralgo::prepend_sign_e;
  using stralgo::padd_with_e;
  using stralgo::include_prefix_e;
  using stralgo::alignment_e;
  using stralgo::trailing_zeros_e;
  
  "stralgo_float_to_ascii"_test = [&]
    {
    auto fn_tmpl = []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      using traits = stralgo::float_format_traits;
      std::array<char_type,integral_to_string_max_size> buffer_{};
        {
        constexpr double value{ 0.0 };
        constexpr auto expected{ cfs("0") };
        auto oit = stralgo::float_to_ascii<traits{
                                                  .decimal_places = 4,
                                                  .trailing_zeros = trailing_zeros_e::skip
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
        {
        constexpr double value{ 0.0 };
        constexpr auto expected{ cfs("") };
        auto oit = stralgo::float_to_ascii<traits{ .precision = 0,
                                                   .decimal_places = 4,
                                                   .trailing_zeros = trailing_zeros_e::skip
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
        {
        constexpr float value{ 0.0 };
        constexpr auto expected{ cfs("+0.0000") };
        auto oit = stralgo::float_to_ascii<traits{ .precision = 1,
                                                   .decimal_places = 4,
                                                   .sign = prepend_sign_e::always,
                                                   .trailing_zeros = trailing_zeros_e::preserve
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
        {
        constexpr float value{ 202.5 };
        constexpr auto expected{ cfs("202.5") };
        auto oit = stralgo::float_to_ascii<traits{ .decimal_places = 4,
                                                   .trailing_zeros = trailing_zeros_e::skip
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
        {
        constexpr double value{ 2.5 };
        constexpr auto expected{ cfs("2.5000") };
        auto oit = stralgo::float_to_ascii<traits{ .decimal_places = 4,
                                                   .trailing_zeros = trailing_zeros_e::preserve
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
        {
        constexpr double value{ 0.5 };
        constexpr auto expected{ cfs("0.5") };
        auto oit = stralgo::float_to_ascii<traits{ .decimal_places = 4,
                                                   .trailing_zeros = trailing_zeros_e::skip
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
        {
        constexpr double value{ 0.543 };
        constexpr auto expected{ cfs("0.5430") };
        auto oit = stralgo::float_to_ascii<traits{ .decimal_places = 4,
                                                   .trailing_zeros = trailing_zeros_e::preserve
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
        {
        constexpr double value{ 0.5 };
        constexpr auto expected{ cfs("   0.5    ") };
        auto oit = stralgo::float_to_ascii<traits{  .precision = 10,
                                                    .decimal_places = 4,
                                                    .alignment = alignment_e::middle,
                                                    .trailing_zeros = trailing_zeros_e::skip
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
        {
        constexpr double value{ 0.5 };
        constexpr auto expected{ cfs("0.5       ") };
        auto oit = stralgo::float_to_ascii<traits{  .precision = 10,
                                                    .decimal_places = 4,
                                                    .alignment = alignment_e::left,
                                                    .trailing_zeros = trailing_zeros_e::skip
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
        {
        constexpr double value{ 120.5 };
        constexpr auto expected{ cfs("    120.50") };
        auto oit = stralgo::float_to_ascii<traits{  .precision = 10,
                                                    .decimal_places = 2,
                                                    .alignment = alignment_e::right,
                                                    .trailing_zeros = trailing_zeros_e::preserve
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
        {
        constexpr double value{ 120.5 };
        constexpr auto expected{ cfs("  120.5   ") };
        auto oit = stralgo::float_to_ascii<traits{  .precision = 10,
                                                    .decimal_places = 4,
                                                    .alignment = alignment_e::middle,
                                                    .trailing_zeros = trailing_zeros_e::skip
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
        {
        constexpr double value{ 120.5 };
        constexpr auto expected{ cfs("120.500   ") };
        auto oit = stralgo::float_to_ascii<traits{  .precision = 10,
                                                    .decimal_places = 3,
                                                    .alignment = alignment_e::left,
                                                    .trailing_zeros = trailing_zeros_e::preserve
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
        {
        constexpr double value{ 0.5 };
        constexpr auto expected{ cfs("00000000.5") };
        auto oit = stralgo::float_to_ascii<traits{  .precision = 10,
                                                    .decimal_places = 4,
                                                    .padd_with = padd_with_e::zeros,
                                                    .trailing_zeros = trailing_zeros_e::skip
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
        {
        constexpr double value{ 0.5 };
        constexpr auto expected{ cfs("00000.5000") };
        auto oit = stralgo::float_to_ascii<traits{  .precision = 10,
                                                    .decimal_places = 4,
                                                    .padd_with = padd_with_e::zeros,
                                                    .trailing_zeros = trailing_zeros_e::preserve
                                                  }>( value, buffer_.begin() );
        std::basic_string_view res{ buffer_.begin(), oit };
        constexpr_test(expected == res);
        }
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_float_to_string
{
  using stralgo::alignment_e;
  using stralgo::trailing_zeros_e;
  
static void do_test(test_result &result)
  {
  "stralgo_float_to_string"_test = [&]
    {
    auto fn_tmpl = []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      using traits = stralgo::float_format_traits;
      constexpr cfs_t<char_type> cfs;
        {
        constexpr double value{ 120.5 };
        constexpr auto expected{ cfs("  120.5   ") };
        auto res = stralgo::float_to_string<char_type,traits{  .precision = 10,
                                                    .decimal_places = 4,
                                                    .alignment = alignment_e::middle,
                                                    .trailing_zeros = trailing_zeros_e::skip
                                                  }>( value );
        constexpr_test(expected.view() == res);
        }
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}

namespace stralgo_string_to_integral_test
{
using stralgo::string_to_integral;
using stralgo::input_format_e;

template<std::integral integral_type, input_format_e input_format = input_format_e::undetermined,
        stralgo::concepts::char_range view_type, typename expected_type>
  constexpr auto test( view_type const & source, expected_type expected, int end_it_offset )
    {
    auto [result,end_it] = string_to_integral<integral_type,input_format>(source);
    constexpr_test(static_cast<integral_type>(expected) == result);
    constexpr_test(std::ranges::next(std::ranges::begin(source),end_it_offset) == end_it);
    }
 
  static void do_test(test_result &result)
    {

    "stralgo_string_to_integral_test"_test = [&]
      {
      auto fn_tmpl = []<typename char_type>
          ( char_type const *) -> metatests::test_result
        {
        constexpr cfs_t<char_type> cfs;
        using enum input_format_e;
        //format undetermined
        test<uint8_t>(cfs(""), 0u, 0 );
        test<uint8_t>(cfs("-"),0u,0);
        test<uint8_t>(cfs("+"),0u,1);
        test<uint8_t>(cfs("0x"),0u,2);
        test<uint8_t>(cfs("+0x"),0u,3);

        test<uint8_t>(cfs("-1"),0u,0);
        test<uint8_t>(cfs("255"),255u,3);
        test<uint8_t>(cfs("0xff"),255u,4);
 
        test<uint8_t>(cfs(" \t0xff 0xfe"),255u,6);
        test<uint8_t,hexadecimal>(cfs(" \t0xff 0xfe"),255u,6);
        test<uint8_t,hexadecimal>(cfs(" \tff 0xfe"),255u,4);
  
        test<int8_t>(cfs("-1"),-1,2);
        test<int8_t>(cfs("0x1f"), 0x1f,4);
        test<int8_t>(cfs("+0x1f"), 0x1f,5);
        test<int8_t>(cfs("-0x1f"),-0x1f,5);
        test<int8_t>(cfs("0x7f"), 0x7f,4);
        
  
        test<uint16_t>(cfs("-1"),0u,0);
        test<uint16_t>(cfs("255"),255u,3);
        test<uint16_t>(cfs("0xff"),255u,4);
        test<uint16_t>(cfs("0xFfFf"),0xffffu,6);

        test<int16_t>(cfs("-1"),-1,2);
        test<int16_t>(cfs("32767"),0x7fff,5);
        test<int16_t>(cfs("0x7fff"),0x7fff,6);
        test<int16_t>(cfs("-0x7fff"),-0x7fff,7);

        test<uint32_t>(cfs("0"),0u,1);
        test<uint32_t>(cfs("+0"),0u,2);
        test<uint32_t>(cfs("-0"),0u,0);
        test<uint32_t>(cfs("-1"),0u,0);
        test<uint32_t>(cfs("\t  \t-124566554 "),0u,0);
        test<uint32_t>(cfs("\t  \t124566554 "),124566554u,13);
        test<uint32_t>(cfs("\t  \t+124566554 "),124566554u,14);
        test<uint32_t>(cfs("\t  \t-124566554XSCD"),0u,0);
        test<uint32_t>(cfs("\t  \t-124566554 123432"),0u,0);
        test<uint32_t>(cfs("\t  \t+124566554XSCD"),124566554u,14);
        test<uint32_t>(cfs("\t  \t+124566554 123432"),124566554u,14);
        
        test<int32_t>(cfs("0"),0,1);
        test<int32_t>(cfs("+0"),0,2);
        test<int32_t>(cfs("-0"),0,2);
        test<int32_t>(cfs(" -0"),0,3);
        test<int32_t>(cfs("    -0"),0,6);
        test<int32_t>(cfs("   +0"),0,5);
        test<int32_t>(cfs(""),0,0);
        test<int32_t>(cfs("1"),1,1);
        test<int32_t>(cfs("1000"),1000,4);
        test<uint32_t>(cfs("1000"),1000u,4);
        test<int32_t>(cfs("12"),12,2);
        test<int32_t>(cfs("124566554"),124566554,9);
        test<int32_t>(cfs("+124566554"),124566554,10);
        test<int32_t>(cfs("-124566554"),-124566554,10);
        test<int32_t>(cfs("\t  \t-124566554"),-124566554,14);
        test<int32_t>(cfs("\t  \t+124566554"),124566554,14);
        test<int32_t>(cfs("\t  \t-124566554a"),-124566554,14);
        test<int32_t>(cfs("\t  \t+124566554a"),124566554,14);
        test<int32_t>(cfs("\t  \t-124566554 "),-124566554,14);
        test<int32_t>(cfs("\t  \t+124566554 "),124566554,14);
        test<int32_t>(cfs("\t  \t-124566554XSCD"),-124566554,14);
        test<int32_t>(cfs("\t  \t+124566554XSCD"),124566554,14);
        test<int32_t>(cfs("\t  \t-124566554 123432"),-124566554,14);
        test<int32_t>(cfs("\t  \t+124566554 123432"),124566554,14);
        
        test<int64_t>(cfs("+0xFF"),0xff,5);
        test<int64_t>(cfs("-0xFF3423"),-0xff3423,9);
        test<int64_t>(cfs("\t \t 0xFF3423 342fsdv"),0xff3423,12);
        test<int64_t>(cfs("\t \t 0x1aF3423R342fsdv"),0x1af3423,13);
        
        test<int64_t>(cfs("0xFF"),0xffu,4);
        test<int64_t>(cfs("0xFF3423"),0xff3423u,8);
        test<int64_t>(cfs("\t \t 0xFF3423 342fsdv"),0xff3423,12);
        test<int64_t>(cfs("\t \t +0xFF3423 342fsdv"),0xff3423,13);
        test<int64_t>(cfs("\t \t -0x1aF3423R342fsdv"),-0x1af3423,14);
        test<int64_t>(cfs("\t \t 0XFf3423 342fsdv"),0xff3423,12);
        test<int64_t>(cfs("\t \t +0XFF3423 342fsdv"),0xff3423,13);
        test<int64_t>(cfs("\t \t -0X1aF3423R342fsdv"),-0x1af3423,14);
        test<int64_t>(cfs("\t \t +0x7fffffffffffffff"),  0x7fffffffffffffffll,23);
        test<int64_t>(cfs("\t \t -0x7fffffffffffffff"),  -0x7fffffffffffffffll,23);
        
        test<uint64_t>(cfs("\t \t 0xffffffffffffffff"),  0xffffffffffffffffull,22);
        test<uint64_t>(cfs("\t \t +0xffffffffffffffff"),  0xffffffffffffffffull,23);
        
        //explicitly specified format decimal
        test<uint32_t,decimal>(cfs("\t  \t124566554 "),124566554u,13);
        test<uint32_t,decimal>(cfs("\t  \t124566554fa "),124566554u,13);
        test<uint32_t,decimal>(cfs("\t  \t124566554 54 "),124566554u,13);
        
        //explicitly specified format hexadecimal
        test<int64_t,hexadecimal>(cfs("+0xFF"),0xff,5);
        test<int64_t,hexadecimal>(cfs("-0xFF3423"),-0xff3423,9);
        test<int64_t,hexadecimal>(cfs("\t \t 0xFF3423 342fsdv"),0xff3423,12);
        
        test<int64_t,hexadecimal>(cfs("+FF"),0xff,3);
        test<int64_t,hexadecimal>(cfs("-FF3423"),-0xff3423,7);
        test<int64_t,hexadecimal>(cfs("\t \t FF3423 342fsdv"),0xff3423,10);
        return {};
        };
      result |= run_consteval_test<char_type_list>(fn_tmpl);
      result |= run_constexpr_test<char_type_list>(fn_tmpl);
      };
    }
}
namespace stralgo_string_to_float_test
  {
  using stralgo::string_to_float;
  using stralgo::input_format_e;


template<std::floating_point float_type,
        stralgo::concepts::char_range view_type, typename expected_type>
  constexpr auto test( view_type const & source, expected_type expected, int end_it_offset )
    {
    auto [result,end_it] = string_to_float<float_type>(source);
    constexpr_test(static_cast<float_type>(expected) == result);
    constexpr_test(std::ranges::next(std::ranges::begin(source),end_it_offset) == end_it);
    }
  static void do_test(test_result &result)
    {
    "stralgo_string_to_float_test"_test = [&]
      {
      auto fn_tmpl = []<typename char_type>
          ( char_type const *) -> metatests::test_result
        {
        constexpr cfs_t<char_type> cfs;

        test<float>(cfs(""), 0.f, 0 );
        test<double>(cfs(""), 0., 0 );
        
        test<float>(cfs("0"), 0.f, 1 );
        test<double>(cfs("0"), 0., 1 );
        
        test<float>(cfs("0."), 0.f, 2 );
        test<double>(cfs("0."), 0., 2 );
        
        test<float>(cfs("0.1"), 0.1f, 3 );
        test<double>(cfs("0.1"), 0.1, 3 );
        
        test<float>(cfs("0.1333"), 0.1333f, 6 );
        test<double>(cfs("0.1333"), 0.1333, 6 );
        
        test<double>(cfs("10.1333"), 10.1333, 7 );
        test<double>(cfs("+10.1333"), 10.1333, 8 );
        test<double>(cfs("-10.1333"), -10.1333, 8 );
        test<double>(cfs("-110.1333"), -110.1333, 9 );
        test<double>(cfs("1000.36"), 1000.36, 7 );
        return {};
        };
      result |= run_consteval_test<char_type_list>(fn_tmpl);
      result |= run_constexpr_test<char_type_list>(fn_tmpl);
      };
    }
  }
namespace stralgo_compose_test
{
  using stralgo::integral_format_traits;
  using stralgo::fmt;
  using stralgo::compose;
  using stralgo::format_e;
  using stralgo::char_case_e;
  using stralgo::padd_with_e;
  using stralgo::alignment_e;
  using stralgo::prepend_sign_e;
  using stralgo::trailing_zeros_e;
  using stralgo::include_prefix_e;
  using stralgo::detail::compose_arg_concept;
  
  namespace concepts = stralgo::concepts;
  
  static_assert( std::same_as<char, concepts::char_type_from_view_t<char>>);
  static_assert( std::same_as<wchar_t, concepts::char_type_from_view_t<std::wstring_view>>);
  
  static_assert( concepts::decl_chartype_from_args_has_valid_type_v<char> );
  static_assert( std::same_as<char, concepts::decl_chartype_from_args_t<char>>);
  
  static_assert( concepts::decl_chartype_from_args_has_valid_type_v<char, int> );
  static_assert( std::same_as<char, concepts::decl_chartype_from_args_t<char,int>>);
  static_assert( std::same_as<char, concepts::decl_chartype_from_args_t<int,char>>);
  static_assert( std::same_as<void, concepts::decl_chartype_from_args_t<int,double>>);
  static_assert( std::same_as<char16_t, concepts::decl_chartype_from_args_t<int,double,char16_t>>);
  
  static_assert( compose_arg_concept<char,char> );
  static_assert( !compose_arg_concept<wchar_t,char> );
  static_assert( compose_arg_concept<std::u8string_view,char8_t> );
  static_assert( compose_arg_concept<coll::u8string,char8_t> );
  static_assert( !compose_arg_concept<std::wstring_view,char8_t> );
  static_assert( compose_arg_concept<int,char8_t> );
  static_assert( compose_arg_concept<double,char8_t> );
  enum test_enum { x =1 };
  enum struct test_enum2 { x };
  static_assert( compose_arg_concept<test_enum,char8_t> );
  static_assert( compose_arg_concept<test_enum2,char8_t> );
  
  static_assert( !concepts::char_range<decltype(' ')> );
  static_assert( !concepts::char_range<decltype(" ")> );
  static_assert( concepts::char_range<std::string_view> );
  
  struct compose_not_supported{};
  
  static_assert(!compose_arg_concept<compose_not_supported,char>);
  static_assert(compose_arg_concept<int,char>);
  static_assert(compose_arg_concept<int64_t,char>);
  static_assert(compose_arg_concept<uint64_t,char16_t>);
  static_assert(compose_arg_concept<float,char>);
  static_assert(compose_arg_concept<std::string,char>);
  static_assert(compose_arg_concept<std::string_view,char>);
  static_assert(compose_arg_concept<std::u8string_view,char8_t>);
  static_assert(!compose_arg_concept<std::string_view,char8_t>);
  static_assert(compose_arg_concept<coll::u8string,char8_t>);
  static_assert(compose_arg_concept<test_enum2,char8_t>);
  static_assert(!compose_arg_concept<char const *,char>);
  static_assert(!compose_arg_concept<decltype(" "),char>);
  static_assert(compose_arg_concept<stralgo::detail::view_preconv_float_t<float, stralgo::float_format_traits{}>,char>);
  static_assert(compose_arg_concept<stralgo::detail::view_preconv_integral_t<int, stralgo::integral_format_traits{}>,char>);

  static void do_test(test_result &result)
    {
    "stralgo_compose_test"_test = [&]
      {
      auto fn_tmpl = []<typename char_type>
          ( char_type const *) -> metatests::test_result
        {
        constexpr cfs_t<char_type> cfs;
        //Lorem ipsum dolor sit amet, consectetur adipiscing elit.
        {
        auto res {compose( cfs(""), char_type(' '))};
        constexpr_test(res.view() == cfs(" "));
        }
        {
        auto res {compose( cfs("Lorem"), char_type(' '), cfs("ipsum"))};
        constexpr_test(res.view() == cfs("Lorem ipsum"));
        }
        {
        auto res {compose( -125, char_type(' '))};
        constexpr_test(res.view() == cfs("-125 "));
        }
        {
        auto res {compose( -125.5, char_type(' '))};
        constexpr_test(res.view() == cfs("-125.5 "));
        }
        {
        auto res {compose( -125.5, cfs(" "), 125, char_type(' '))};
        constexpr_test(res.view() == cfs("-125.5 125 "));
        }
        {
        test_enum2 a{};
        test_enum b{test_enum::x};
        auto res {compose( a, b, cfs(" "))};
        constexpr_test(res.view() == cfs("01 "));
        }
        {
        constexpr integral_format_traits ptrfmt 
          {
          .precision = 6,
          .format = format_e::hexadecimal,
          .char_case = char_case_e::uppercase,
          .padd_with = padd_with_e::zeros
          };
        auto res {compose( fmt<ptrfmt>(0x1ff56ef001), char_type(' '), fmt<ptrfmt>(0x0) )};
        constexpr_test(res.view() == cfs("0X1FF56EF001 0X0000"));
        }
        {
        constexpr integral_format_traits ptrfmt 
          {
          .precision = 6,
          .format = format_e::hexadecimal,
          .char_case = char_case_e::lowercase,
          .padd_with = padd_with_e::zeros
          };
        auto res {
          stralgo::compose(
              cfs(" some view ").view(),
              127.5f, //default formatted floating point 
              char_type(','), ///single char
              125, //default formatted integral number
              char_type('['),
              fmt<integral_format_traits{
                      .precision = 15,
                      .format = format_e::binary,
                      .char_case = char_case_e::lowercase,
                      .alignment = alignment_e::middle
                      }>(456), //custom formatted integral number with traits like in integral_to_string
              cfs("] ["),
              fmt<stralgo::float_format_traits{
                                                        .precision = 10,
                                                        .decimal_places = 2,
                                                        .padd_with = padd_with_e::space,
                                                        .sign = prepend_sign_e::only_negative,
                                                        .alignment = alignment_e::left,
                                                        .trailing_zeros = trailing_zeros_e::skip
                                                        }>(10.46713), //custom formatted floating point number with traits like in float_to_string
              cfs("] "),
              fmt<ptrfmt>(0x1ff56ef001), char_type(' '), //reuse custom constexpr formatting
              fmt<ptrfmt>(0x0)
            )
        };
        constexpr_test(res.view() == cfs(" some view 127.5,125[  0b111001000  ] [10.46     ] 0x1ff56ef001 0x0000"));
        }
        {
        auto res{stralgo::compose( cfs("output_filename"), 
                         char_type('_'), stralgo::fmt<stralgo::integral_format_traits{
                             .precision = 4,
                             .padd_with = padd_with_e::zeros
                          }>(2), 
                         char_type('_'), stralgo::fmt<stralgo::integral_format_traits{
                           .precision = 11,
                           .format = format_e::hexadecimal,
                           .char_case = char_case_e::lowercase,
                           .padd_with = padd_with_e::zeros,
                           .include_prefix = include_prefix_e::with_prefix
                            }>(0x1ffeb3e), cfs(".c") )};
        constexpr auto expected{ cfs("output_filename_0002_0x001ffeb3e.c") };
        constexpr_test( expected == res.view() );
        }
        {
        std::tm tm_value{};
        tm_value.tm_sec = 3;
        tm_value.tm_min = 1;
        tm_value.tm_hour = 5;
        tm_value.tm_yday = 125;
        using stralgo::fmt;
        constexpr stralgo::integral_format_traits _22dec
          {
          .precision = 2,
          .padd_with = stralgo::padd_with_e::zeros
          };
        auto res { stralgo::compose( tm_value.tm_yday, cfs(" days ") , fmt<_22dec>(tm_value.tm_hour), char_type(':'), fmt<_22dec>(tm_value.tm_min), char_type(':'), fmt<_22dec>(tm_value.tm_sec) ) };
        constexpr auto expected{ cfs("125 days 05:01:03") };
        constexpr_test( expected == res.view() );
        }
        return {};
        };
      result |= run_consteval_test<char_type_list>(fn_tmpl);
      result |= run_constexpr_test<char_type_list>(fn_tmpl);
      };
    }
}
int main()
  {
  test_result result;
  stralgo_value_to_hex::do_test(result);
  stralgo_from_hex_ascii::do_test(result);
  stralgo_to_hex_ascii::do_test(result);
  stralgo_to_hex_ascii_string::do_test(result);
  stralgo_calculate_size_div_info::do_test(result);
  stralgo_integral_to_ascii::do_test(result);
  stralgo_integral_to_string::do_test(result);
  stralgo_estimate_float_to_string::do_test(result);
  stralgo_float_to_ascii::do_test(result);
  stralgo_float_to_string::do_test(result);
  stralgo_string_to_integral_test::do_test(result);
  stralgo_string_to_float_test::do_test(result);
  stralgo_compose_test::do_test(result);
  
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
  }
