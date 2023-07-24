#include <unit_test_core.h>
#include <stralgo/stralgo.h>
#include <coll/basic_fixed_string.h>

using namespace std::literals::string_view_literals;

  static_assert( stralgo::isspace(' ') == true );
  static_assert( stralgo::isspace('\r') == true );
  static_assert( stralgo::isspace('\n') == true );
  static_assert( stralgo::isspace('\t') == true );
  static_assert( stralgo::isspace('r') == false );
  static_assert( stralgo::isspace('b') == false );
  static_assert( stralgo::isspace('a') == false );

  static_assert( stralgo::isdigit(' ') == false );
  static_assert( stralgo::isdigit('\r') == false );
  static_assert( stralgo::isdigit('\n') == false );
  static_assert( stralgo::isdigit('\t') == false );
  static_assert( stralgo::isdigit('r') == false );
  static_assert( stralgo::isdigit('B') == false );
  static_assert( stralgo::isdigit('a') == false );
  static_assert( stralgo::isdigit('0') == true );
  static_assert( stralgo::isdigit('3') == true );
  static_assert( stralgo::isdigit('9') == true );
  static_assert( stralgo::isdigit(L'1') == true );
  static_assert( stralgo::isdigit(L'0') == true );
  static_assert( stralgo::isdigit(L'3') == true );
  static_assert( stralgo::isdigit(L'9') == true );
  static_assert( stralgo::isdigit(L'1') == true );
  
  static_assert( stralgo::isxdigit(' ') == false );
  static_assert( stralgo::isxdigit('\r') == false );
  static_assert( stralgo::isxdigit('\n') == false );
  static_assert( stralgo::isxdigit('\t') == false );
  static_assert( stralgo::isxdigit('r') == false );
  static_assert( stralgo::isxdigit('A') == true );
  static_assert( stralgo::isxdigit('B') == true );
  static_assert( stralgo::isxdigit('F') == true );
  static_assert( stralgo::isxdigit('G') == false );
  static_assert( stralgo::isxdigit('a') == true );
  static_assert( stralgo::isxdigit('f') == true );
  static_assert( stralgo::isxdigit('g') == false );
  static_assert( stralgo::isxdigit('0') == true );
  static_assert( stralgo::isxdigit('3') == true );
  static_assert( stralgo::isxdigit('9') == true );
  static_assert( stralgo::isxdigit(L'1') == true );
  static_assert( stralgo::isxdigit(L'0') == true );
  static_assert( stralgo::isxdigit(L'3') == true );
  static_assert( stralgo::isxdigit(L'9') == true );
  static_assert( stralgo::isxdigit(L'1') == true );
  static_assert( stralgo::isxdigit(L' ') == false );
  static_assert( stralgo::isxdigit(L'\r') == false );
  static_assert( stralgo::isxdigit(L'\n') == false );
  static_assert( stralgo::isxdigit(L'\t') == false );
  static_assert( stralgo::isxdigit(L'r') == false );
  static_assert( stralgo::isxdigit(L'A') == true );
  static_assert( stralgo::isxdigit(L'B') == true );
  static_assert( stralgo::isxdigit(L'F') == true );
  static_assert( stralgo::isxdigit(L'G') == false );
  static_assert( stralgo::isxdigit(L'a') == true );
  static_assert( stralgo::isxdigit(L'f') == true );
  static_assert( stralgo::isxdigit(L'g') == false );
  static_assert( stralgo::isxdigit(L'0') == true );
  static_assert( stralgo::isxdigit(L'3') == true );
  static_assert( stralgo::isxdigit(L'9') == true );
  static_assert( stralgo::isxdigit(L'1') == true );
 
  static_assert( stralgo::is_number( "0"sv ) == true );
  static_assert( stralgo::is_number( " "sv ) == false );
  static_assert( stralgo::is_number( "avde"sv ) == false );
  static_assert( stralgo::is_number( "9838320"sv ) == true );
  static_assert( stralgo::is_number( "9838a320"sv ) == false );
  static_assert( stralgo::is_number( "9838.320"sv ) == false );
  static_assert( stralgo::is_number( "9838,320"sv ) == false );
  static_assert( stralgo::is_number( ""sv ) == true );
  static_assert( stralgo::is_number( "9,"sv ) == false );
  static_assert( stralgo::is_number( ".9838320"sv ) == false );
  
  static_assert( stralgo::is_number( L"0"sv ) == true );
  static_assert( stralgo::is_number( L"9838320"sv ) == true );
  static_assert( stralgo::is_number( L"9838a320"sv ) == false );
  static_assert( stralgo::is_number( L"9838.320"sv ) == false );
  static_assert( stralgo::is_number( L"9838,320"sv ) == false );
  static_assert( stralgo::is_number( L""sv ) == true );
  static_assert( stralgo::is_number( L"9,"sv ) == false );
  static_assert( stralgo::is_number( L".9838320"sv ) == false );
  
  static_assert( stralgo::is_hexnumber( "0"sv ) == true );
  static_assert( stralgo::is_hexnumber( " "sv ) == false );
  static_assert( stralgo::is_hexnumber( "avde"sv ) == false );
  static_assert( stralgo::is_hexnumber( "9838320"sv ) == true );
  static_assert( stralgo::is_hexnumber( "9838a320"sv ) == true );
  static_assert( stralgo::is_hexnumber( "9838.320"sv ) == false );
  static_assert( stralgo::is_hexnumber( "9838,320"sv ) == false );
  static_assert( stralgo::is_hexnumber( ""sv ) == true );
  static_assert( stralgo::is_hexnumber( "9,"sv ) == false );
  static_assert( stralgo::is_hexnumber( ".9838320"sv ) == false );
  static_assert( stralgo::is_hexnumber( "afcbef"sv ) == true );
  static_assert( stralgo::is_hexnumber( "ABCDEF"sv ) == true );
  static_assert( stralgo::is_hexnumber( "AaBCcDEF"sv ) == true );
  static_assert( stralgo::is_hexnumber( "ABCDEFg"sv ) == false );
  static_assert( stralgo::is_hexnumber( "1AB3CDEF"sv ) == true );
  
  static_assert( stralgo::is_hexnumber( L"0"sv ) == true );
  static_assert( stralgo::is_hexnumber( L"9838320"sv ) == true );
  static_assert( stralgo::is_hexnumber( L"9838a320"sv ) == true );
  static_assert( stralgo::is_hexnumber( L"9838.320"sv ) == false );
  static_assert( stralgo::is_hexnumber( L"9838,320"sv ) == false );
  static_assert( stralgo::is_hexnumber( L""sv ) == true );
  static_assert( stralgo::is_hexnumber( L"9,"sv ) == false );
  static_assert( stralgo::is_hexnumber( L".9838320"sv ) == false );
  static_assert( stralgo::is_hexnumber( L"afcbef"sv ) == true );
  static_assert( stralgo::is_hexnumber( L"ABCDEF"sv ) == true );
  static_assert( stralgo::is_hexnumber( L"AaBCcDEF"sv ) == true );
  static_assert( stralgo::is_hexnumber( L"ABCDEFg"sv ) == false );
  static_assert( stralgo::is_hexnumber( L"1AB3CDEF"sv ) == true );
  
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_find_first_of
{
  auto constexpr t1a {"Lorem ipsum dolor sit amet, consectetur adipiscing elit. "sv};
  static_assert( stralgo::find_first_of(t1a, ""sv) == t1a.npos);
  static_assert( stralgo::find_first_of(t1a, " i"sv) == 5u);
  static_assert( stralgo::find_first_of(t1a, "i "sv) == 5u);
  static_assert( stralgo::find_first_of(t1a, "XZ"sv) == t1a.npos);
  static_assert( stralgo::find_first_of(std::string_view{}, "XZ"sv) == t1a.npos);
  
  auto constexpr t2a {L"Lorem ipsum dolor sit amet, consectetur adipiscing elit. "sv};
  static_assert( stralgo::find_first_of(t2a, L""sv) == t1a.npos);
  static_assert( stralgo::find_first_of(t2a, L" i"sv) == 5u);
  static_assert( stralgo::find_first_of(t2a, L"XZ"sv) == t1a.npos);
  static_assert( stralgo::find_first_of(std::wstring_view{}, L"XZ"sv) == t1a.npos);
}

//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_starts_with
  {
  auto constexpr t1a {" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n"sv};
  static_assert( stralgo::starts_with(t1a, ""sv) );
  static_assert( !stralgo::starts_with(t1a, "Lorem"sv) );
  static_assert( stralgo::starts_with(t1a, " \t\r\nLorem"sv) );
  
  auto constexpr t2a {L" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n"sv};
  static_assert( stralgo::starts_with(t2a, L""sv) );
  static_assert( !stralgo::starts_with(t2a, L"Lorem"sv) );
  static_assert( stralgo::starts_with(t2a, L" \t\r\nLorem"sv) );
  }

//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_trim_left
{
  constexpr auto t1a {" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n"sv};

  static_assert( stralgo::trim_left(t1a) == "Lorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n"sv );
  static_assert( stralgo::trim_left(t1a,std::string_view{}) == t1a );
  static_assert( stralgo::trim_left(t1a, ' ') == "\t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n"sv );
  static_assert( stralgo::trim_left(t1a, "Lorem  \t\r\n"sv) == "ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n"sv );
}

//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_trim_right
{
  constexpr auto t1a {" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n"sv};

  static_assert( stralgo::trim_right(t1a) == " \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit."sv );
  static_assert( stralgo::trim_right(t1a,std::string_view{}) == t1a );
  static_assert( stralgo::trim_right(t1a,'\n') == " \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r"sv );
  static_assert( stralgo::trim_right(t1a,"Lorem  \t\r\nelit."sv) == " \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing"sv );
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_trim
{
  
  constexpr auto t1a {" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n"sv};

    static_assert( stralgo::trim(t1a) == "Lorem ipsum dolor sit amet, consectetur adipiscing elit."sv );
    static_assert( std::is_same_v<decltype(stralgo::trim(t1a)),std::string_view> );
    static_assert( stralgo::trim(t1a,std::string_view{}) == t1a );
    static_assert( stralgo::trim(t1a,'\n') == " \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r"sv );
    static_assert( stralgo::trim(t1a,"Lorem  \t\r\nelit."sv) == "psum dolor sit amet, consectetur adipiscing"sv );
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_ends_with
{
  auto constexpr t1a {" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n"sv};
  static_assert( stralgo::ends_with(t1a, ""sv) == true );
  static_assert( stralgo::ends_with(t1a, std::string_view{}) == true );
  static_assert( stralgo::ends_with(t1a, "elit."sv) == false );
  static_assert( stralgo::ends_with(t1a, "elit. \t\r\n"sv) == true );
}
//----------------------------------------------------------------------------------------------------------------------

namespace stralgo_compare_no_case
{
  
  auto constexpr t1 {"Lorem ipsum"sv}; auto constexpr t1b {"Lorem ipsum"sv};
  auto constexpr t2 {"Lorem"sv};
  std::string_view constexpr t4 {};
  static_assert( stralgo::compare_no_case(t1,t2) == 1 );
  static_assert( stralgo::compare_no_case(t1,t1b) == 0 );
  static_assert( stralgo::compare_no_case(t1,t1) == 0 );
  static_assert( stralgo::compare_no_case(t2,t1) == -1 );
  static_assert( stralgo::compare_no_case(t4,t4) == 0 );
  static_assert( stralgo::compare_no_case(t1,t4) == 1 );
  static_assert( stralgo::compare_no_case(t4,t1) == -1 );
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_right
{
  constexpr auto t1 {"Lorem ipsum"sv};
  static_assert( stralgo::right( t1, 2u ) == "um"sv );
  static_assert( stralgo::right( t1, 0u ) == std::string_view{} );
  static_assert( stralgo::right( t1,  std::string_view::npos ) == t1 );
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_left
{
  
  constexpr auto t1 {"Lorem ipsum"sv};
  static_assert( stralgo::left( t1, 2 ) == "Lo"sv );
  static_assert( stralgo::left( t1, 0 ) == std::string_view{} );
  static_assert( stralgo::left( t1, std::string_view::npos ) == t1 );

}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_substr
{
  
  constexpr auto t1 {"Lorem ipsum"sv};
  static_assert( stralgo::substr( t1, 2u, 2u ) == "re"sv );
  static_assert( stralgo::substr( t1, 22u, 2u ).empty() );
  static_assert( stralgo::substr( t1, 2u, std::string_view::npos ) == "rem ipsum"sv );
  static_assert( stralgo::substr( t1, 0u, std::string_view::npos ) == t1 );
  static_assert( stralgo::substr( t1, std::string_view::npos, 1 ).empty() );
  static_assert( stralgo::substr( t1, std::string_view::npos, std::string_view::npos ).empty() );
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_to_lower
{
  static_assert( stralgo::to_lower('a') == 'a' );
  static_assert( stralgo::to_lower('A') == 'a' );
  static_assert( stralgo::to_lower('Z') == 'z' );
  static_assert( stralgo::to_lower('1') == '1' );
  static_assert( stralgo::to_lower(':') == ':' );
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_to_upper
{
  static_assert( stralgo::to_upper('a') == 'A' );
  static_assert( stralgo::to_upper('A') == 'A' );
  static_assert( stralgo::to_upper('z') == 'Z' );
  static_assert( stralgo::to_upper('1') == '1' );
  static_assert( stralgo::to_upper(':') == ':' );
}
//----------------------------------------------------------------------------------------------------------------------
using metatests::constexpr_test;
using metatests::run_consteval_test;
using metatests::run_constexpr_test;
namespace ut = boost::ut;
using ut::operator""_test;
using namespace ut::operators::terse;
using metatests::test_result;
using char_type_list = metatests::type_list<char,char8_t,char16_t,char32_t,wchar_t>;
using coll::cast_fixed_string;

static_assert( stralgo::detail::merge_concepts<char,char> );
static_assert( stralgo::detail::merge_concepts<wchar_t,wchar_t> );
template<typename decl_chr_type>
struct cfs_t
{
  template<typename char_type, std::size_t N>
  inline constexpr auto operator()(char_type const (&str)[N]) const noexcept
    { return coll::basic_fixed_string<decl_chr_type, N - 1>(str); }
};

int main()
{
  test_result result;
  "stralgo_merge"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      constexpr auto t1a { cfs("orem ipsum dolor sit amet, consectetur adipiscing elit. ")};
      std::basic_string_view<char_type> t2a { };
      constexpr auto t3a {cfs("Vestibulum") };
      char_type t4a{' '};
      char_type t0('L');
      constexpr auto t5a{ cfs("rutrum leo libero")};
      constexpr auto expected{ cfs("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum rutrum leo libero")};
      auto res{ stralgo::merge( t0,t1a,t2a,t3a,t4a,t5a)};
      
      constexpr_test( res.compare(expected) == 0 );
      
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };

  "stralgo_merge_range"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      std::array tst1 = { 
        coll::basic_string<char_type>{cfs("Lorem ipsum")},
        coll::basic_string<char_type>{cfs("Lorem ipsum dolor ")},
        coll::basic_string<char_type>{cfs("sit amet")},
        coll::basic_string<char_type>{},
        coll::basic_string<char_type>{cfs(",consectetur adipiscing elit.")}
      };
      auto constexpr expected{cfs("Lorem ipsumLorem ipsum dolor sit amet,consectetur adipiscing elit.")};
      {
      auto res { stralgo::merge_range(std::begin(tst1), std::end(tst1) ) };
      constexpr_test( res.compare(expected) == 0 );
      }
      {
      auto res { stralgo::merge_range(tst1) };
      constexpr_test( res.compare(expected) == 0 );
      }
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
}

