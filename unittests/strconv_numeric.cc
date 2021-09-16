#include <strconv_numeric.h>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

using namespace std::literals::string_view_literals;

namespace strconcept_tests
  {
  constexpr bool test_0()
    {
    using type = std::vector<uint32_t>;
    return std::is_same_v<uint32_t, strconcept::iterator_value_type_t<type::iterator>>;
    }
    static_assert(test_0());
    
//   namespace test_1
//     {
//     using type = std::back_insert_iterator<std::vector<uint32_t>>;
//     static_assert( std::is_same_v<uint32_t, strconcept::iterator_value_type_t<type::iterator>>);
//     }
//     
  }

namespace strconv_value_to_hex
  {
  static_assert( strconv::value_to_hex(0) == '0' );
  static_assert( strconv::value_to_hex(5) == '5' );
  static_assert( strconv::value_to_hex(15) == 'F' );
  }
  
namespace from_hex_ascii_test
{
   constexpr bool test_1()
    {
    constexpr auto input = "ff1a34b5f30027"sv;
    uint8_t res[7]{};
    strconv::from_hex_ascii( std::begin(input), std::end(input),  std::begin(res) );
    constexpr std::array<uint8_t,7> expected { {0xff,0x1a,0x34,0xb5,0xf3,0x00,0x27} };
    return stralgo::detail::equal( expected.begin(), expected.end(), &res[0] );
    }
    
  static_assert( test_1() );
}

namespace strconv_to_hex_ascii
{
  constexpr bool test_1()
      {
      uint8_t tst[7]{0u,1u,2u,0xffu,0x34,0x7f,10};
      char res[14]{};
      std::string_view result{ &res[0], 14 };
      strconv::to_hex_ascii( std::begin(tst), std::end(tst), std::begin(res) );
      constexpr auto expected{"000102FF347F0A"sv};
      return result == expected;
      };
      
    static_assert( test_1() );
    
    constexpr bool test_2()
      {
      uint8_t tst[7]{0u,1u,2u,0xffu,0x34,0x7f,10};
      char res[14]{};
      std::string_view result{ &res[0], 14 };
      strconv::to_hex_ascii<strconv::char_case_e::lowercase>( std::begin(tst), std::end(tst), std::begin(res) );
      constexpr auto expected{"000102ff347f0a"sv};
      return result == expected;
      };
      
    static_assert( test_2() );
}

inline constexpr auto integral_to_string_max_size = 128;

namespace calculate_size_div_info_test
  {
   
    static_assert( strconv::detail::calculate_size_div_info<10>(0u).size_ == 0 );
    static_assert( strconv::detail::calculate_size_div_info<10>(1u).size_ == 1 );
    static_assert( strconv::detail::calculate_size_div_info<10>(10u).size_ == 2 );
    static_assert( strconv::detail::calculate_size_div_info<10>(99u).size_ == 2 );
    static_assert( strconv::detail::calculate_size_div_info<10>(100u).size_ == 3 );
    
    static_assert( strconv::detail::calculate_size_div_info<10>(0u).divisor_ == 0 );
    static_assert( strconv::detail::calculate_size_div_info<10>(1u).divisor_ == 1 );
    static_assert( strconv::detail::calculate_size_div_info<10>(10u).divisor_ == 10 );
    static_assert( strconv::detail::calculate_size_div_info<10>(99u).divisor_ == 10 );
    static_assert( strconv::detail::calculate_size_div_info<10>(100u).divisor_ == 100 );
    static_assert( strconv::detail::calculate_size_div_info<10>(1000u).divisor_ == 1000 );
    static_assert( strconv::detail::calculate_size_div_info<10>(9999u).divisor_ == 1000 );
    
    static_assert( strconv::detail::calculate_size_div_info<10>( 18446744073709551610ull).divisor_ == 10000000000000000000ull );
    //18446744073709551615ull
    //10000000000000000000
    
    static_assert( strconv::detail::calculate_size_div_info<16>(0u).size_ == 0 );
    static_assert( strconv::detail::calculate_size_div_info<16>(1u).size_ == 1 );
    static_assert( strconv::detail::calculate_size_div_info<16>(0xfu).size_ == 1 );
    static_assert( strconv::detail::calculate_size_div_info<16>(0x10u).size_ == 2 );
    static_assert( strconv::detail::calculate_size_div_info<16>(0xffu).size_ == 2 );
    static_assert( strconv::detail::calculate_size_div_info<16>(0xfffu).size_ == 3 );
    static_assert( strconv::detail::calculate_size_div_info<16>(0xffffu).size_ == 4 );
    
    static_assert( strconv::detail::calculate_size_div_info<16>(0u).divisor_ == 0 );
    static_assert( strconv::detail::calculate_size_div_info<16>(1u).divisor_ == 1 );
    static_assert( strconv::detail::calculate_size_div_info<16>(0xfu).divisor_ == 1 );
    static_assert( strconv::detail::calculate_size_div_info<16>(0x10u).divisor_ == 0x10 );
    static_assert( strconv::detail::calculate_size_div_info<16>(0xffu).divisor_ == 0x10 );
    static_assert( strconv::detail::calculate_size_div_info<16>(0xfffu).divisor_ == 0x100 );
    static_assert( strconv::detail::calculate_size_div_info<16>(0xffffu).divisor_ == 0x1000 );
    
  }
  
using traits = strconv::integral_format_traits;
using strconv::format_e;
using strconv::char_case_e;
using strconv::prepend_sign_e;
using strconv::padd_with_e;
using strconv::include_prefix_e;
using strconv::alignment_e;

namespace integral_to_string_test
{

  inline constexpr auto hex_lower = traits{ .format = format_e::hexadecimal,
                                            .char_case = char_case_e::lowercase };
                                            
  inline constexpr auto hex_upper = traits{ .format = format_e::hexadecimal,
                                            .char_case = char_case_e::uppercase };
                                            
  //--------------------------------------------------------------------------------------------------------
    constexpr bool test_unsigned_1()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "0" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_1() );
    
    constexpr bool test_unsigned_1a()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 0}>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_1a() );
    
    constexpr bool test_unsigned_1b()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "    0" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 5}>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_1b() );
    
    constexpr bool test_unsigned_1c()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "00000" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 5,
                                                     .padd_with = padd_with_e::zeros
                                                   }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_1c() );
  //--------------------------------------------------------------------------------------------------------
    constexpr bool test_unsigned_2()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "0x0" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<hex_lower>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_2() );
    
    constexpr bool test_unsigned_2a()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 0, 
                                                     .format = format_e::hexadecimal
                                                      }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_2a() );
    
    constexpr bool test_unsigned_2b()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "   0x0" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 6, 
                                                     .format = format_e::hexadecimal
                                                      }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_2b() );
    
    constexpr bool test_unsigned_2c()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "0x0000" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 6, 
                                                     .format = format_e::hexadecimal,
                                                     .padd_with = padd_with_e::zeros
                                                      }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_2c() );
    
    constexpr bool test_unsigned_2d()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "000000" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 6, 
                                                     .format = format_e::hexadecimal,
                                                     .padd_with = padd_with_e::zeros,
                                                     .include_prefix = include_prefix_e::no_prefix
                                                      }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_2d() );
  //--------------------------------------------------------------------------------------------------------
    constexpr bool test_unsigned_7()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "0b0" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .format = format_e::binary }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_7() );
    
    constexpr bool test_unsigned_7a()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 0, 
                                                     .format = format_e::binary
                                                      }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_7a() );
    
    constexpr bool test_unsigned_7b()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "   0b0" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 6, 
                                                     .format = format_e::binary
                                                      }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_7b() );
    
    constexpr bool test_unsigned_7c()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "0b0000" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 6, 
                                                     .format = format_e::binary,
                                                     .padd_with = padd_with_e::zeros
                                                      }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    constexpr bool test_unsigned_7d()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "000000" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 6, 
                                                     .format = format_e::binary,
                                                     .padd_with = padd_with_e::zeros,
                                                     .include_prefix = include_prefix_e::no_prefix
                                                      }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_7d() );
  //--------------------------------------------------------------------------------------------------------
    constexpr bool test_unsigned_3()
      {
      constexpr unsigned value{ 1501900 };
      constexpr std::string_view expected{ "1501900" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_3() );
  //--------------------------------------------------------------------------------------------------------
    constexpr bool test_unsigned_8()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr unsigned value{ 0b1010011 };
      constexpr std::string_view expected{ "0b1010011" };
      auto oit = strconv::integral_to_string<traits{ .format = format_e::binary}>( value, itbeg );
      
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_8() );
    
    constexpr bool test_unsigned_8a()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr unsigned value{ 0b1010011 };
      constexpr std::string_view expected{ "0B1010011" };
      auto oit = strconv::integral_to_string<traits{ .format = format_e::binary,
                                                     .char_case = char_case_e::uppercase
                                                   }>( value, itbeg );
      
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_8a() );
    
    constexpr bool test_unsigned_8b()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr unsigned value{ 0b1010011 };
      constexpr std::string_view expected{ "1010011" };
      auto oit = strconv::integral_to_string<traits{ .format = format_e::binary,
                                                     .include_prefix = include_prefix_e::no_prefix}
                                            >( value, itbeg );
      
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_8b() );
    
    constexpr bool test_unsigned_8c()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr unsigned value{ 0b1010011 };
      constexpr std::string_view expected{ "   1010011" };
      auto oit = strconv::integral_to_string<traits{ .precision = 10, 
                                                     .format = format_e::binary,
                                                     .include_prefix = include_prefix_e::no_prefix}
                                            >( value, itbeg );
      
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_8c() );
    constexpr bool test_unsigned_8d()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr unsigned value{ 0b1010011 };
      constexpr std::string_view expected{ " 0b1010011" };
      auto oit = strconv::integral_to_string<traits{ .precision = 10, 
                                                     .format = format_e::binary,
                                                     .include_prefix = include_prefix_e::with_prefix}
                                            >( value, itbeg );
      
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_8d() );
    
    constexpr bool test_unsigned_8e()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr unsigned value{ 0b1010011 };
      constexpr std::string_view expected{ "0B000001010011" };
      auto oit = strconv::integral_to_string<traits{ .precision = 14, 
                                                     .format = format_e::binary,
                                                     .char_case = char_case_e::uppercase,
                                                     .padd_with = padd_with_e::zeros,
                                                     .include_prefix = include_prefix_e::with_prefix}
                                            >( value, itbeg );
      
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_8e() );
    
    constexpr bool test_unsigned_8f()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr unsigned value{ 0b1010011 };
      constexpr std::string_view expected{ "+0b00001010011" };
      auto oit = strconv::integral_to_string<traits{ .precision = 14, 
                                                     .format = format_e::binary,
                                                     .char_case = char_case_e::lowercase,
                                                     .padd_with = padd_with_e::zeros,
                                                     .sign = prepend_sign_e::always,
                                                     .include_prefix = include_prefix_e::with_prefix
                                                      }
                                            >( value, itbeg );
      
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_8f() );
  //--------------------------------------------------------------------------------------------------------
    constexpr bool test_unsigned_4()
      {
      constexpr unsigned value{ 0x1ffeb3e };
      constexpr std::string_view expected{ "0X1FFEB3E" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<hex_upper>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_4() );
    
    constexpr bool test_unsigned_4a()
      {
      constexpr unsigned value{ 0x1ffeb3e };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr std::string_view expected{ "+0X0001FFEB3E" };
      auto oit = strconv::integral_to_string<traits{ .precision = 13, 
                                                     .format = format_e::hexadecimal,
                                                     .char_case = char_case_e::uppercase,
                                                     .padd_with = padd_with_e::zeros,
                                                     .sign = prepend_sign_e::always
                                                      }>( value, itbeg );
                                                      
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_4a() );
    
    constexpr bool test_unsigned_4b()
      {
      constexpr unsigned value{ 0x1ffeb3e };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr std::string_view expected{ "  +0X1FFEB3E" };
      auto oit = strconv::integral_to_string<traits{ .precision = 12, 
                                                     .format = format_e::hexadecimal,
                                                     .char_case = char_case_e::uppercase,
                                                     .padd_with = padd_with_e::space,
                                                     .sign = prepend_sign_e::always
                                                      }>( value, itbeg );
                                                      
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_4b() );
    
    constexpr bool test_unsigned_4c()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr unsigned value{ 0x1ffeb3e };
      constexpr std::string_view expected{ "+0X1FFEB3E  " };
      auto oit = strconv::integral_to_string<traits{ .precision = 12, 
                                                     .format = format_e::hexadecimal,
                                                     .char_case = char_case_e::uppercase,
                                                     .padd_with = padd_with_e::space,
                                                     .sign = prepend_sign_e::always,
                                                     .alignment = alignment_e::left
                                                      }>( value, itbeg );
                                                      
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_4c() );
    
    constexpr bool test_unsigned_4d()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr unsigned value{ 0x1ffeb3e };
      constexpr std::string_view expected{ " +0X1FFEB3E " };
      auto oit = strconv::integral_to_string<traits{ .precision = 12, 
                                                     .format = format_e::hexadecimal,
                                                     .char_case = char_case_e::uppercase,
                                                     .padd_with = padd_with_e::space,
                                                     .sign = prepend_sign_e::always,
                                                     .alignment = alignment_e::middle
                                                      }>( value, itbeg );
                                                      
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_4d() );
  //--------------------------------------------------------------------------------------------------------
    constexpr bool test_unsigned_5()
      {
      constexpr uint64_t value{ 1844674407370955161ull };
      constexpr std::string_view expected{ "1844674407370955161" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_5() );
    
    constexpr bool test_unsigned_5a()
      {
      constexpr uint64_t value{ 1844674407370955161ull };
      constexpr std::string_view expected{ "+1844674407370955161" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .sign = prepend_sign_e::always }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_5a() );
    
    constexpr bool test_unsigned_5b()
      {
      constexpr int64_t value{ -1844674407370955161ll };
      constexpr std::string_view expected{ "  -1844674407370955161" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 22 }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_5b() );
    
    constexpr bool test_unsigned_5c()
      {
      constexpr int64_t value{ -1844674407370955161ll };
      constexpr std::string_view expected{ "-001844674407370955161" };
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 22,
                                                     .padd_with = padd_with_e::zeros 
                                                   }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_5c() );
  //--------------------------------------------------------------------------------------------------------
    constexpr bool test_unsigned_6()
      {
      constexpr uint64_t value{ 0x1ffeb3ef1ffeb3ull };
      constexpr auto expected{ L"0x1ffeb3ef1ffeb3"sv };
      wchar_t buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<hex_lower>( value, itbeg );
      std::wstring_view result{ itbeg, static_cast<std::wstring_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_6() );
    
    constexpr bool test_unsigned_6a()
      {
      constexpr uint64_t value{ 0x1ffeb3ef1ffeb3ull };
      constexpr auto expected{ L"+0x1ffeb3ef1ffeb3"sv };
      wchar_t buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .format = format_e::hexadecimal,
                                                     .char_case = char_case_e::lowercase,
                                                     .sign = prepend_sign_e::always
                                                    }>( value, itbeg );
      std::wstring_view result{ itbeg, static_cast<std::wstring_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_6a() );
    
    constexpr bool test_unsigned_6b()
      {
      constexpr int64_t value{ -0x1ffeb3ef1ffeb3ll };
      constexpr auto expected{ L"-0x1ffeb3ef1ffeb3"sv };
      wchar_t buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .format = format_e::hexadecimal,
                                                     .char_case = char_case_e::lowercase,
                                                     .sign = prepend_sign_e::only_negative
                                                    }>( value, itbeg );
      std::wstring_view result{ itbeg, static_cast<std::wstring_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_6b() );
    
    constexpr bool test_unsigned_6c()
      {
      constexpr int64_t value{ -0x1ffeb3ef1ffeb3ll };
      constexpr auto expected{ L"  -0x1ffeb3ef1ffeb3   "sv };
      wchar_t buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<traits{ .precision = 22,
                                                     .format = format_e::hexadecimal,
                                                     .char_case = char_case_e::lowercase,
                                                     .sign = prepend_sign_e::only_negative,
                                                     .alignment = alignment_e::middle
                                                    }>( value, itbeg );
      std::wstring_view result{ itbeg, static_cast<std::wstring_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_6c() );
  //--------------------------------------------------------------------------------------------------------
    constexpr bool test_unsigned_9()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr uint64_t value{ 0b1100110011001100110011001100110011001100110011001100110011001ull };
      constexpr std::string_view expected{ "0b1100110011001100110011001100110011001100110011001100110011001" };
      auto oit = strconv::integral_to_string<traits{.format = format_e::binary}>( value, itbeg );
      
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_9() );
    constexpr bool test_unsigned_9a()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr int64_t value{ -0b1100110011001100110011001100110011001100110011001100110011001ll };
      constexpr std::string_view expected{ "-0b1100110011001100110011001100110011001100110011001100110011001" };
      auto oit = strconv::integral_to_string<traits{.format = format_e::binary}>( value, itbeg );
      
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_9a() );
    
    constexpr bool test_unsigned_9b()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr int64_t value{ -0b1100110011001100110011001100110011001100110011001100110011001ll };
      constexpr std::string_view expected{ "-0b00001100110011001100110011001100110011001100110011001100110011001" };
      auto oit = strconv::integral_to_string<traits{.precision = 68,
                                                    .format = format_e::binary,
                                                    .padd_with = padd_with_e::zeros
                                                    }>( value, itbeg );
                                                    
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_9b() );
    
    constexpr bool test_unsigned_9c()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr int64_t value{ -0b1100110011001100110011001100110011001100110011001100110011001ll };
      constexpr std::string_view expected{ "    -0b1100110011001100110011001100110011001100110011001100110011001" };
      auto oit = strconv::integral_to_string<traits{.precision = 68,
                                                    .format = format_e::binary,
                                                    .padd_with = padd_with_e::space
                                                    }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_9c() );
    
    constexpr bool test_unsigned_9d()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr int64_t value{ -0b1100110011001100110011001100110011001100110011001100110011001ll };
      constexpr std::string_view expected{ "      -1100110011001100110011001100110011001100110011001100110011001" };
      auto oit = strconv::integral_to_string<traits{.precision = 68,
                                                    .format = format_e::binary,
                                                    .padd_with = padd_with_e::space,
                                                    .include_prefix = include_prefix_e::no_prefix
                                                    }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_9d() );
  //--------------------------------------------------------------------------------------------------------
}
struct strconv_fixture_t {};

BOOST_FIXTURE_TEST_SUITE(strconv_fixture, strconv_fixture_t )
//----------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(strconv_integral_to_string)
{
  using traits = strconv::integral_format_traits;
    {
    constexpr int64_t value{ -0x1ffeb3ef1ffeb3ll };
    constexpr auto expected{ "  -0x1ffeb3ef1ffeb3   "sv };
    auto result = strconv::integral_to_string<traits{ .precision = 22,
                                                   .format = format_e::hexadecimal,
                                                   .char_case = char_case_e::lowercase,
                                                   .sign = prepend_sign_e::only_negative,
                                                   .alignment = alignment_e::middle
                                                  }>( value );
                                                  
    BOOST_TEST( expected == result );
    }
    {
    constexpr unsigned value{ 0 };
    constexpr std::string_view expected{ "0" };
    auto result = strconv::integral_to_string( value );
    BOOST_TEST( expected == result );
    }
    {
      constexpr unsigned value{ 0b1010011 };
      constexpr std::string_view expected{ " 0b1010011" };
      auto result = strconv::integral_to_string<traits{ .precision = 10, 
                                                     .format = format_e::binary,
                                                     .include_prefix = include_prefix_e::with_prefix}
                                            >( value );
    BOOST_TEST( expected == result );
    }
}
//----------------------------------------------------------------------------------------------------------------------
using strconv::trailing_zeros_e;
namespace estimate_float_to_string_test
  {
  using traits = strconv::float_format_traits;
  //----------------------------------------------------------------------------------------------------------------------
    constexpr bool test_estimate_float_0()
      {
      constexpr double value{ 0.0 };

      auto est_info = strconv::detail::estimate_float_to_string_<traits{
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value);
      return est_info.size() == 1;
      }
    static_assert( test_estimate_float_0() );
    
    constexpr bool test_estimate_float_0a()
      {
      constexpr double value{ 0.0 };

      auto est_info = strconv::detail::estimate_float_to_string_<traits{
                                                .precision = 0,
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value);
      return est_info.size() == 0;
      }
    static_assert( test_estimate_float_0a() );
    

  //----------------------------------------------------------------------------------------------------------------------
    constexpr bool test_estimate_float_1()
      {
      constexpr double value{ 202.5 };
      //constexpr std::string_view expected{ "202.5" };
      auto est_info = strconv::detail::estimate_float_to_string_<traits{
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value);
      return est_info.size() == 5;
      }
    static_assert( test_estimate_float_1() );
    constexpr bool test_estimate_float_1a()
      {
      constexpr double value{ 202.5 };
      auto est_info = strconv::detail::estimate_float_to_string_<traits{
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::preserve
                                                }>( value);
      return est_info.size() == 8;
      }
    static_assert( test_estimate_float_1a() );
  //----------------------------------------------------------------------------------------------------------------------
    constexpr bool test_estimate_float_2()
      {
      constexpr double value{ 0.5 };
      //constexpr std::string_view expected{ "202.5" };
      auto est_info = strconv::detail::estimate_float_to_string_<traits{
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value);
      return est_info.size() == 3;
      }
    static_assert( test_estimate_float_2() );
    
    constexpr bool test_estimate_float_2a()
      {
      constexpr double value{ 0.5 };
      //constexpr std::string_view expected{ "202.5" };
      auto est_info = strconv::detail::estimate_float_to_string_<traits{
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::preserve
                                                }>( value);
      return est_info.size() == 6;
      }
    static_assert( test_estimate_float_2a() );
  //----------------------------------------------------------------------------------------------------------------------
  }
namespace float_to_string_test
  {
  using traits = strconv::float_format_traits;
    //----------------------------------------------------------------------------------------------------------------------
    
    //----------------------------------------------------------------------------------------------------------------------
    constexpr bool test_float_0()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 0.0 };
      constexpr std::string_view expected{ "0" };
      auto oit = strconv::float_to_string<traits{
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_0() );
    
    constexpr bool test_float_0a()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 0.0 };
      constexpr std::string_view expected{ "" };
      auto oit = strconv::float_to_string<traits{
                                                .precision = 0,
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_0a() );
    
    constexpr bool test_float_0e()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 0.0 };
      constexpr std::string_view expected{ "+0.0000" };
      auto oit = strconv::float_to_string<traits{
                                                .precision = 1,
                                                .decimal_places = 4,
                                                .sign = prepend_sign_e::always,
                                                .include_prefix = include_prefix_e::no_prefix,
                                                .trailing_zeros = trailing_zeros_e::preserve
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_0e() );
    //----------------------------------------------------------------------------------------------------------------------
    
    constexpr bool test_float_1()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 202.5 };
      constexpr std::string_view expected{ "202.5" };
      auto oit = strconv::float_to_string<traits{
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_1() );
    
    constexpr bool test_float_1a()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 2.5 };
      constexpr std::string_view expected{ "2.5000" };
      auto oit = strconv::float_to_string<traits{
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::preserve
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_1a() );
    //----------------------------------------------------------------------------------------------------------------------
    
    constexpr bool test_float_2()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 0.5 };
      constexpr std::string_view expected{ "0.5" };
      auto oit = strconv::float_to_string<traits{
                                                .decimal_places = 4,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_2() );
    
    constexpr bool test_float_2a()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 0.5 };
      constexpr std::string_view expected{ "0.500" };
      auto oit = strconv::float_to_string<traits{
                                                .decimal_places = 3,
                                                .trailing_zeros = trailing_zeros_e::preserve
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_2a() );
    
    //----------------------------------------------------------------------------------------------------------------------
    
    constexpr bool test_float_3()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 0.5 };
      constexpr std::string_view expected{ "       0.5" };
      auto oit = strconv::float_to_string<traits{
                                                .precision = 10,
                                                .decimal_places = 4,
                                                .alignment = alignment_e::right,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_3() );
    
    constexpr bool test_float_3a()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 0.5 };
      constexpr std::string_view expected{ "   0.5    " };
      auto oit = strconv::float_to_string<traits{
                                                .precision = 10,
                                                .decimal_places = 4,
                                                .alignment = alignment_e::middle,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_3a() );
    
    constexpr bool test_float_3b()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 0.5 };
      constexpr std::string_view expected{ "0.5       " };
      auto oit = strconv::float_to_string<traits{
                                                .precision = 10,
                                                .decimal_places = 4,
                                                .alignment = alignment_e::left,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_3b() );
    
    constexpr bool test_float_3c()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 120.5 };
      constexpr std::string_view expected{ "     120.5" };
      auto oit = strconv::float_to_string<traits{
                                                .precision = 10,
                                                .decimal_places = 4,
                                                .alignment = alignment_e::right,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_3c() );
    
    constexpr bool test_float_3d()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 120.5 };
      constexpr std::string_view expected{ "  120.5   " };
      auto oit = strconv::float_to_string<traits{
                                                .precision = 10,
                                                .decimal_places = 4,
                                                .alignment = alignment_e::middle,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_3d() );
    
    constexpr bool test_float_3e()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 120.5 };
      constexpr std::string_view expected{ "120.5     " };
      auto oit = strconv::float_to_string<traits{
                                                .precision = 10,
                                                .decimal_places = 4,
                                                .alignment = alignment_e::left,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_3e() );
    //----------------------------------------------------------------------------------------------------------------------
    
    constexpr bool test_float_4()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 0.5 };
      constexpr std::string_view expected{ "00000000.5" };
      auto oit = strconv::float_to_string<traits{
                                                .precision = 10,
                                                .decimal_places = 4,
                                                .padd_with = padd_with_e::zeros,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_4() );
    
    constexpr bool test_float_4a()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 0.5 };
      constexpr std::string_view expected{ "00000.5000" };
      auto oit = strconv::float_to_string<traits{
                                                .precision = 10,
                                                .decimal_places = 4,
                                                .padd_with = padd_with_e::zeros,
                                                .trailing_zeros = trailing_zeros_e::preserve
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_4a() );
    //----------------------------------------------------------------------------------------------------------------------
    
    
  }
  
namespace string_to_integral_test
  {
  using strconv::string_to_integral;
  using namespace std::string_view_literals;
  using strconv::input_format_e;
  
  template<typename integral_type,
           input_format_e input_format = input_format_e::undetermined,
           typename expected_type>
  constexpr auto test( std::string_view source, expected_type expected, int end_it_offset )
    {
    auto [result,end_it] = string_to_integral<integral_type,input_format>(source);
    return std::next(std::begin(source),end_it_offset) == end_it
      && static_cast<integral_type>(expected) == result;
    }
  //format undetermined
  static_assert( test<uint8_t>({}, 0u, 0 ) );
  static_assert( test<uint8_t>("-"sv,0u,0) );
  static_assert( test<uint8_t>("+"sv,0u,1) );
  static_assert( test<uint8_t>("0x"sv,0u,2) );
  static_assert( test<uint8_t>("+0x"sv,0u,3) );
  
  static_assert( test<int8_t>({},0,0) );
  static_assert( test<int8_t>(""sv,0,0) );
  static_assert( test<int8_t>("-"sv,0,1) );
  static_assert( test<int8_t>("+"sv,0,1) );
  static_assert( test<int8_t>("0x"sv,0,2) );
  static_assert( test<int8_t>("+0x"sv,0,3) );
  
  static_assert( test<uint8_t>("-1"sv,0u,0) );
  static_assert( test<uint8_t>("255"sv,255u,3) );
  static_assert( test<uint8_t>("0xff"sv,255u,4) );
 
  static_assert( test<uint8_t>(" \t0xff 0xfe"sv,255u,6) );
  static_assert( test<uint8_t,input_format_e::hexadecimal>(" \t0xff 0xfe"sv,255u,6) );
  static_assert( test<uint8_t,input_format_e::hexadecimal>(" \tff 0xfe"sv,255u,4) );
  
  static_assert( test<int8_t>("-1"sv,-1,2) );
  static_assert( test<int8_t>("0x1f"sv, 0x1f,4) );
  static_assert( test<int8_t>("+0x1f"sv, 0x1f,5) );
  static_assert( test<int8_t>("-0x1f"sv,-0x1f,5) );
  static_assert( test<int8_t>("0x7f"sv, 0x7f,4) );
  
  static_assert( test<uint16_t>("-1"sv,0u,0) );
  static_assert( test<uint16_t>("255"sv,255u,3) );
  static_assert( test<uint16_t>("0xff"sv,255u,4) );
  static_assert( test<uint16_t>("0xFfFf"sv,0xffffu,6) );
  
  static_assert( test<int16_t>("-1"sv,-1,2) );
  static_assert( test<int16_t>("32767"sv,0x7fff,5) );
  static_assert( test<int16_t>("0x7fff"sv,0x7fff,6) );
  static_assert( test<int16_t>("-0x7fff"sv,-0x7fff,7) );

  static_assert( test<uint32_t>("0"sv,0u,1) );
  static_assert( test<uint32_t>("+0"sv,0u,2) );
  static_assert( test<uint32_t>("-0"sv,0u,0) );
  static_assert( test<uint32_t>("-1"sv,0u,0) );
  static_assert( test<uint32_t>("\t  \t-124566554 "sv,0u,0) );
  static_assert( test<uint32_t>("\t  \t124566554 "sv,124566554u,13) );
  static_assert( test<uint32_t>("\t  \t+124566554 "sv,124566554u,14) );
  static_assert( test<uint32_t>("\t  \t-124566554XSCD"sv,0u,0) );
  static_assert( test<uint32_t>("\t  \t-124566554 123432"sv,0u,0) );
  static_assert( test<uint32_t>("\t  \t+124566554XSCD"sv,124566554u,14) );
  static_assert( test<uint32_t>("\t  \t+124566554 123432"sv,124566554u,14) );
  
  static_assert( test<int32_t>("0"sv,0,1) );
  static_assert( test<int32_t>("+0"sv,0,2) );
  static_assert( test<int32_t>("-0"sv,0,2) );
  static_assert( test<int32_t>(" -0"sv,0,3) );
  static_assert( test<int32_t>("    -0"sv,0,6) );
  static_assert( test<int32_t>("   +0"sv,0,5) );
  static_assert( test<int32_t>( {},0,0) );
  static_assert( test<int32_t>("1"sv,1,1) );
  static_assert( test<int32_t>("12"sv,12,2) );
  static_assert( test<int32_t>("124566554"sv,124566554,9) );
  static_assert( test<int32_t>("+124566554"sv,124566554,10) );
  static_assert( test<int32_t>("-124566554"sv,-124566554,10) );
  static_assert( test<int32_t>("\t  \t-124566554"sv,-124566554,14) );
  static_assert( test<int32_t>("\t  \t+124566554"sv,124566554,14) );
  static_assert( test<int32_t>("\t  \t-124566554a"sv,-124566554,14) );
  static_assert( test<int32_t>("\t  \t+124566554a"sv,124566554,14) );
  static_assert( test<int32_t>("\t  \t-124566554 "sv,-124566554,14) );
  static_assert( test<int32_t>("\t  \t+124566554 "sv,124566554,14) );
  static_assert( test<int32_t>("\t  \t-124566554XSCD"sv,-124566554,14) );
  static_assert( test<int32_t>("\t  \t+124566554XSCD"sv,124566554,14) );
  static_assert( test<int32_t>("\t  \t-124566554 123432"sv,-124566554,14) );
  static_assert( test<int32_t>("\t  \t+124566554 123432"sv,124566554,14) );
  
  static_assert( test<int64_t>("+0xFF"sv,0xff,5) );
  static_assert( test<int64_t>("-0xFF3423"sv,-0xff3423,9) );
  static_assert( test<int64_t>("\t \t 0xFF3423 342fsdv"sv,0xff3423,12) );
  static_assert( test<int64_t>("\t \t 0x1aF3423R342fsdv"sv,0x1af3423,13) );
  
  static_assert( test<int64_t>("0xFF"sv,0xffu,4) );
  static_assert( test<int64_t>("0xFF3423"sv,0xff3423u,8) );
  static_assert( test<int64_t>("\t \t 0xFF3423 342fsdv"sv,0xff3423,12) );
  static_assert( test<int64_t>("\t \t +0xFF3423 342fsdv"sv,0xff3423,13) );
  static_assert( test<int64_t>("\t \t -0x1aF3423R342fsdv"sv,-0x1af3423,14) );
  static_assert( test<int64_t>("\t \t 0XFf3423 342fsdv"sv,0xff3423,12) );
  static_assert( test<int64_t>("\t \t +0XFF3423 342fsdv"sv,0xff3423,13) );
  static_assert( test<int64_t>( "\t \t -0X1aF3423R342fsdv"sv,-0x1af3423,14) );
  static_assert( test<int64_t>("\t \t +0x7fffffffffffffff"sv,  0x7fffffffffffffffll,23) );
  static_assert( test<int64_t>("\t \t -0x7fffffffffffffff"sv,  -0x7fffffffffffffffll,23) );
  
  static_assert( test<uint64_t>("\t \t 0xffffffffffffffff"sv,  0xffffffffffffffffull,22) );
  static_assert( test<uint64_t>("\t \t +0xffffffffffffffff"sv,  0xffffffffffffffffull,23) );
  
  //explicitly specified format decimal
  static_assert( test<uint32_t,input_format_e::decimal>("\t  \t124566554 "sv,124566554u,13) );
  static_assert( test<uint32_t,input_format_e::decimal>("\t  \t124566554fa "sv,124566554u,13) );
  static_assert( test<uint32_t,input_format_e::decimal>("\t  \t124566554 54 "sv,124566554u,13) );
  
  //explicitly specified format hexadecimal
  static_assert( test<int64_t,input_format_e::hexadecimal>("+0xFF"sv,0xff,5) );
  static_assert( test<int64_t,input_format_e::hexadecimal>("-0xFF3423"sv,-0xff3423,9) );
  static_assert( test<int64_t,input_format_e::hexadecimal>("\t \t 0xFF3423 342fsdv"sv,0xff3423,12) );
  
  static_assert( test<int64_t,input_format_e::hexadecimal>("+FF"sv,0xff,3) );
  static_assert( test<int64_t,input_format_e::hexadecimal>("-FF3423"sv,-0xff3423,7) );
  static_assert( test<int64_t,input_format_e::hexadecimal>("\t \t FF3423 342fsdv"sv,0xff3423,10) );
  }
namespace string_to_float_test
  {
  using strconv::string_to_float;
  using namespace std::string_view_literals;
  
  template<typename float_type, typename expected_type>
  constexpr auto test( std::string_view source, expected_type expected, int end_it_offset )
    {
    auto [result,end_it] = string_to_float<float_type>(source);
    return std::next(std::begin(source),end_it_offset) == end_it
      && static_cast<float_type>(expected) == result;
    }

  static_assert( test<float>({}, 0.f, 0 ) );
 
  static_assert( test<float>(""sv, 0.f, 0 ) );
  static_assert( test<double>(""sv, 0., 0 ) );
  
  static_assert( test<float>("0"sv, 0.f, 1 ) );
  static_assert( test<double>("0"sv, 0., 1 ) );
  
  static_assert( test<float>("0."sv, 0.f, 2 ) );
  static_assert( test<double>("0."sv, 0., 2 ) );
  
  static_assert( test<float>("0.1"sv, 0.1f, 3 ) );
  static_assert( test<double>("0.1"sv, 0.1, 3 ) );
  
  static_assert( test<float>("0.1333"sv, 0.1333f, 6 ) );
  static_assert( test<double>("0.1333"sv, 0.1333, 6 ) );
  
  static_assert( test<double>("10.1333"sv, 10.1333, 7 ) );
  static_assert( test<double>("+10.1333"sv, 10.1333, 8 ) );
  static_assert( test<double>("-10.1333"sv, -10.1333, 8 ) );
  static_assert( test<double>("-110.1333"sv, -110.1333, 9 ) );
  }
//----------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(strconv_compose)
{
  auto strres{ 
  strconv::compose<char>(
    " some view "sv,
    127.3f, //default formatted floating point 
    ',', ///single char
    125, //default formatted integral number
    '[',
    strconv::fmt<strconv::integral_format_traits{
            .precision = 10,
            .format = format_e::hexadecimal,
            .char_case = char_case_e::lowercase,
            .alignment = alignment_e::middle
            }>(456), //custom formatted integral number with traits like in integral_to_string
    "] ["sv,
    strconv::fmt<strconv::float_format_traits{
                                              .precision = 10,
                                              .decimal_places = 2,
                                              .alignment = alignment_e::left,
                                              .trailing_zeros = trailing_zeros_e::skip
                                              }>(10.46713), //custom formatted floating point number with traits like in float_to_string
    ']'
  ) };
  
  constexpr auto expected{ " some view 127.300003,125[  0x1c8   ] [10.46     ]"sv };
  
  BOOST_TEST( expected == strres );
}
BOOST_AUTO_TEST_SUITE_END()

