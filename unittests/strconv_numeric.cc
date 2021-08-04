#include <strconv_numeric.h>

using namespace std::literals::string_view_literals;


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
    return ranges::equal( expected, res );
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
namespace integral_to_string_test
{
  using traits = strconv::integral_format_traits;
  using strconv::format_e;
  using strconv::char_case_e;
  using strconv::prepend_sign_e;
  using strconv::padd_with_e;
  using strconv::include_prefix_e;
  using strconv::alignment_e;
  
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
