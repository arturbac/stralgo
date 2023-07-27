#include <unit_test_core.h>
#include <stralgo/stralgo.h>
#include <coll/basic_fixed_string.h>

using namespace std::literals::string_view_literals;
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


namespace stralgo_isspace
{
static void do_test(test_result &result)
  {
  "stralgo_isspace"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr_test( stralgo::isspace(char_type(' ')) == true );
      constexpr_test( stralgo::isspace(char_type('\r')) == true );
      constexpr_test( stralgo::isspace(char_type('\n')) == true );
      constexpr_test( stralgo::isspace(char_type('\t')) == true );
      constexpr_test( stralgo::isspace(char_type('r')) == false );
      constexpr_test( stralgo::isspace(char_type('b')) == false );
      constexpr_test( stralgo::isspace(char_type('a')) == false );
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_isdigit
{
static void do_test(test_result &result)
  {
  "stralgo_isdigit"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr_test( stralgo::isdigit(char_type(' ')) == false );
      constexpr_test( stralgo::isdigit(char_type('\r')) == false );
      constexpr_test( stralgo::isdigit(char_type('\n')) == false );
      constexpr_test( stralgo::isdigit(char_type('\t')) == false );
      constexpr_test( stralgo::isdigit(char_type('r')) == false );
      constexpr_test( stralgo::isdigit(char_type('B')) == false );
      constexpr_test( stralgo::isdigit(char_type('a')) == false );
      constexpr_test( stralgo::isdigit(char_type('0')) == true );
      constexpr_test( stralgo::isdigit(char_type('3')) == true );
      constexpr_test( stralgo::isdigit(char_type('9')) == true );
      constexpr_test( stralgo::isdigit(char_type('1')) == true );
      constexpr_test( stralgo::isdigit(char_type('0')) == true );
      constexpr_test( stralgo::isdigit(char_type('3')) == true );
      constexpr_test( stralgo::isdigit(char_type('9')) == true );
      constexpr_test( stralgo::isdigit(char_type('1')) == true );
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_isxdigit
{
static void do_test(test_result &result)
  {
  "stralgo_isxdigit"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr_test( stralgo::isxdigit(char_type(' ')) == false );
      constexpr_test( stralgo::isxdigit(char_type('\r')) == false );
      constexpr_test( stralgo::isxdigit(char_type('\n')) == false );
      constexpr_test( stralgo::isxdigit(char_type('\t')) == false );
      constexpr_test( stralgo::isxdigit(char_type('r')) == false );
      constexpr_test( stralgo::isxdigit(char_type('A')) == true );
      constexpr_test( stralgo::isxdigit(char_type('B')) == true );
      constexpr_test( stralgo::isxdigit(char_type('F')) == true );
      constexpr_test( stralgo::isxdigit(char_type('G')) == false );
      constexpr_test( stralgo::isxdigit(char_type('a')) == true );
      constexpr_test( stralgo::isxdigit(char_type('f')) == true );
      constexpr_test( stralgo::isxdigit(char_type('g')) == false );
      constexpr_test( stralgo::isxdigit(char_type('0')) == true );
      constexpr_test( stralgo::isxdigit(char_type('3')) == true );
      constexpr_test( stralgo::isxdigit(char_type('9')) == true );
      constexpr_test( stralgo::isxdigit(char_type('1')) == true );
      constexpr_test( stralgo::isxdigit(char_type('b')) == true );

       return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_is_number
{
static void do_test(test_result &result)
  {
  "stralgo_is_number"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      constexpr_test( stralgo::is_number( cfs("0") ) == true );
      constexpr_test( stralgo::is_number( cfs(" ") ) == false );
      constexpr_test( stralgo::is_number( cfs("avde") ) == false );
      constexpr_test( stralgo::is_number( cfs("9838320") ) == true );
      constexpr_test( stralgo::is_number( cfs("9838a320") ) == false );
      constexpr_test( stralgo::is_number( cfs("9838.320") ) == false );
      constexpr_test( stralgo::is_number( cfs("9838,320") ) == false );
      constexpr_test( stralgo::is_number( cfs("") ) == true );
      constexpr_test( stralgo::is_number( cfs("9,") ) == false );
      constexpr_test( stralgo::is_number( cfs(".9838320") ) == false );

       return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_is_hexnumber
{
static void do_test(test_result &result)
  {
  "stralgo_is_hexnumber"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      constexpr_test( stralgo::is_hexnumber( cfs("0") ) == true );
      constexpr_test( stralgo::is_hexnumber( cfs(" ") ) == false );
      constexpr_test( stralgo::is_hexnumber( cfs("avde") ) == false );
      constexpr_test( stralgo::is_hexnumber( cfs("9838320") ) == true );
      constexpr_test( stralgo::is_hexnumber( cfs("9838a320") ) == true );
      constexpr_test( stralgo::is_hexnumber( cfs("9838.320") ) == false );
      constexpr_test( stralgo::is_hexnumber( cfs("9838,320") ) == false );
      constexpr_test( stralgo::is_hexnumber( cfs("") ) == true );
      constexpr_test( stralgo::is_hexnumber( cfs("9,") ) == false );
      constexpr_test( stralgo::is_hexnumber( cfs(".9838320") ) == false );
      constexpr_test( stralgo::is_hexnumber( cfs("afcbef") ) == true );
      constexpr_test( stralgo::is_hexnumber( cfs("ABCDEF") ) == true );
      constexpr_test( stralgo::is_hexnumber( cfs("AaBCcDEF") ) == true );
      constexpr_test( stralgo::is_hexnumber( cfs("ABCDEFg") ) == false );
      constexpr_test( stralgo::is_hexnumber( cfs("1AB3CDEF") ) == true );
       return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_find_first_of
{
static void do_test(test_result &result)
  {
  "stralgo_find_first_of"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      auto constexpr t1a {cfs("Lorem ipsum dolor sit amet, consectetur adipiscing elit. ")};
      constexpr_test( stralgo::find_first_of(t1a, cfs("")) == t1a.end());
      constexpr_test( stralgo::find_first_of(t1a, cfs(" i")) == std::ranges::next(t1a.begin(),5));
      constexpr_test( stralgo::find_first_of(t1a, cfs("i ")) == std::ranges::next(t1a.begin(),5));
      constexpr_test( stralgo::find_first_of(t1a, cfs("XZ")) == t1a.end());
      auto constexpr t2{cfs("")};
      constexpr_test( stralgo::find_first_of(t2, cfs("XZ")) == t2.end());
       return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }

}

//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_starts_with
{
static void do_test(test_result &result)
  {
  "stralgo_starts_with"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      auto constexpr t1a { cfs(" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n")};
      constexpr_test( stralgo::starts_with(t1a, cfs("")) );
      constexpr_test( !stralgo::starts_with(t1a, cfs("Lorem")) );
      constexpr_test( stralgo::starts_with(t1a, cfs(" \t\r\nLorem")) );
       return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_ends_with
{
static void do_test(test_result &result)
  {
  "stralgo_trim"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      auto constexpr t1a {cfs(" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n")};
      constexpr_test( stralgo::ends_with(t1a, std::basic_string_view<char_type>{}) == true );
      constexpr_test( stralgo::ends_with(t1a, cfs("")) == true );
      constexpr_test( stralgo::ends_with(t1a, cfs("elit.")) == false );
      constexpr_test( stralgo::ends_with(t1a, cfs("elit. \t\r\n")) == true );
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_trim_left
{
static void do_test(test_result &result)
  {
  "stralgo_trim_left"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      constexpr auto t1a {cfs(" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n")};
      using std::ranges::equal;
      
      constexpr_test(equal(stralgo::trim_left(t1a), cfs("Lorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n") ));
      constexpr_test(stralgo::trim_left(t1a) == cfs("Lorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n") );
      
      constexpr_test( stralgo::trim_left(t1a,std::basic_string_view<char_type>{}) == t1a );
      constexpr_test( stralgo::trim_left(t1a, char_type(' ')) == cfs("\t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n") );
      constexpr_test( stralgo::trim_left(t1a, cfs("Lorem  \t\r\n")) == cfs("ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n"));
      constexpr_test(std::ranges::empty(stralgo::trim_left(cfs("\t\r\n"), cfs("\t\r\n"))));
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}

//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_trim_right
{
static void do_test(test_result &result)
  {
  "stralgo_trim_right"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      constexpr auto t1a {cfs(" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n")};

      constexpr_test( stralgo::trim_right(t1a) == cfs(" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit.") );
      constexpr_test( stralgo::trim_right(t1a,std::basic_string_view<char_type>{}) == t1a );
      constexpr_test( stralgo::trim_right(t1a,char_type('\n')) == cfs(" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r") );
      constexpr_test( stralgo::trim_right(t1a, cfs("Lorem  \t\r\nelit.")) == cfs(" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing") );
      constexpr_test(stralgo::trim_right(cfs("\t\r\n"), cfs("\t\r\n")) == cfs(""));
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_trim
{
static void do_test(test_result &result)
  {
  "stralgo_trim"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      constexpr auto t1a {cfs(" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r\n")};

      constexpr_test( stralgo::trim(t1a) == cfs("Lorem ipsum dolor sit amet, consectetur adipiscing elit.") );
      constexpr_test( stralgo::trim(t1a,std::basic_string_view<char_type>{}) == t1a );
      constexpr_test( stralgo::trim(t1a, char_type('\n')) == cfs(" \t\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. \t\r") );
      constexpr_test( stralgo::trim(t1a, cfs("Lorem  \t\r\nelit.")) == cfs("psum dolor sit amet, consectetur adipiscing") );
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}

//----------------------------------------------------------------------------------------------------------------------

namespace stralgo_compare_no_case
{
static void do_test(test_result &result)
  {
  "stralgo_trim"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      auto constexpr t1 {cfs("Lorem ipsum")}; auto constexpr t1b {cfs("Lorem ipsum")};
      auto constexpr t2 {cfs("Lorem")};
      std::basic_string_view<char_type> constexpr t4 {};
      constexpr_test( stralgo::compare_no_case(t1,t2) == 1 );
      constexpr_test( stralgo::compare_no_case(t1,t1b) == 0 );
      constexpr_test( stralgo::compare_no_case(t1,t1) == 0 );
      constexpr_test( stralgo::compare_no_case(t2,t1) == -1 );
      constexpr_test( stralgo::compare_no_case(t4,t4) == 0 );
      constexpr_test( stralgo::compare_no_case(t1,t4) == 1 );
      constexpr_test( stralgo::compare_no_case(t4,t1) == -1 );
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_left
{
static void do_test(test_result &result)
  {
  "stralgo_left"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      constexpr auto t1 { cfs("Lorem ipsum")};
      constexpr_test( stralgo::left( t1, 2 ) == cfs("Lo") );
      constexpr_test( stralgo::left( t1, 0 ) == std::basic_string_view<char_type>{} );
      constexpr_test( stralgo::left( t1, stralgo::npos ) == t1 );
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_right
{
static void do_test(test_result &result)
  {
  "stralgo_right"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      constexpr auto t1 {cfs("Lorem ipsum")};
      constexpr_test( stralgo::right( t1, 2u ) == cfs("um") );
      constexpr_test( stralgo::right( t1, 0u ) == std::basic_string_view<char_type>{} );
      constexpr_test( stralgo::right( t1,  stralgo::npos ) == t1 );
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}

//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_substr
{
static void do_test(test_result &result)
  {
  "stralgo_substr"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr cfs_t<char_type> cfs;
      constexpr auto t1 {cfs("Lorem ipsum")};
      constexpr_test( stralgo::substr( t1, 2u, 2u ) == cfs("re") );
      constexpr_test( std::ranges::empty(stralgo::substr( t1, 22u, 2u )) );
      constexpr_test( stralgo::substr( t1, 2u, stralgo::npos ) == cfs("rem ipsum") );
      constexpr_test( stralgo::substr( t1, 0u, stralgo::npos ) == t1 );
      constexpr_test( stralgo::substr( t1, stralgo::npos, 1 ).empty() );
      constexpr_test( std::ranges::empty(stralgo::substr( t1, stralgo::npos, stralgo::npos )) );
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_to_lower
{
static void do_test(test_result &result)
  {
  "stralgo_to_lower"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr_test( stralgo::to_lower(char_type('a')) == char_type('a') );
      constexpr_test( stralgo::to_lower(char_type('A')) == char_type('a') );
      constexpr_test( stralgo::to_lower(char_type('Z')) == char_type('z') );
      constexpr_test( stralgo::to_lower(char_type('1')) == char_type('1') );
      constexpr_test( stralgo::to_lower(char_type(':')) == char_type(':') );
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
//----------------------------------------------------------------------------------------------------------------------
namespace stralgo_to_upper
{
static void do_test(test_result &result)
  {
  "stralgo_to_upper"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr_test( stralgo::to_upper(char_type('a')) == char_type('A') );
      constexpr_test( stralgo::to_upper(char_type('A')) == char_type('A') );
      constexpr_test( stralgo::to_upper(char_type('z')) == char_type('Z') );
      constexpr_test( stralgo::to_upper(char_type('1')) == char_type('1') );
      constexpr_test( stralgo::to_upper(char_type(':')) == char_type(':') );
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_merge
{
static void do_test(test_result &result)
  {
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
  }
}
namespace stralgo_merge_range
{
static void do_test(test_result &result)
  {
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
}

int main()
{
  test_result result;
  stralgo_isspace::do_test(result);
  stralgo_isdigit::do_test(result);
  stralgo_isxdigit::do_test(result);
  stralgo_is_number::do_test(result);
  stralgo_is_hexnumber::do_test(result);
  stralgo_find_first_of::do_test(result);
  stralgo_starts_with::do_test(result);
  stralgo_ends_with::do_test(result);
  stralgo_trim_left::do_test(result);
  stralgo_trim_right::do_test(result);
  stralgo_trim::do_test(result);
  stralgo_compare_no_case::do_test(result);
  stralgo_left::do_test(result);
  stralgo_right::do_test(result);
  stralgo_substr::do_test(result);
  stralgo_to_lower::do_test(result);
  stralgo_to_upper::do_test(result);
  stralgo_merge::do_test(result);
  stralgo_merge_range::do_test(result);
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

