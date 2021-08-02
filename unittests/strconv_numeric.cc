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

namespace integral_to_string_test
{
    constexpr bool test_unsigned_1()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "0" };
      char buffer_[strconv::detail::integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_1() );
    
    constexpr bool test_unsigned_2()
      {
      constexpr unsigned value{ 0 };
      constexpr std::string_view expected{ "0x0" };
      char buffer_[strconv::detail::integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<strconv::format_e::hexadecimal>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_2() );
    
    constexpr bool test_unsigned_3()
      {
      constexpr unsigned value{ 1501900 };
      constexpr std::string_view expected{ "1501900" };
      char buffer_[strconv::detail::integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<strconv::format_e::decimal>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_3() );
    
    constexpr bool test_unsigned_4()
      {
      constexpr unsigned value{ 0x1ffeb3e };
      constexpr std::string_view expected{ "0x1FFEB3E" };
      char buffer_[strconv::detail::integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<strconv::format_e::hexadecimal>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_4() );
    
    constexpr bool test_unsigned_5()
      {
      constexpr uint64_t value{ 18446744073709551615ull };
      constexpr std::string_view expected{ "18446744073709551615" };
      char buffer_[strconv::detail::integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<strconv::format_e::decimal>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_5() );
    
    constexpr bool test_unsigned_6()
      {
      constexpr uint64_t value{ 0x1ffeb3ef1ffeb3efull };
      constexpr auto expected{ L"0x1FFEB3EF1FFEB3EF"sv };
      wchar_t buffer_[strconv::detail::integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      auto oit = strconv::integral_to_string<strconv::format_e::hexadecimal>( value, itbeg );
      std::wstring_view result{ itbeg, static_cast<std::wstring_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_6() );
}
