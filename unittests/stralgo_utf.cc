#include <unit_test_core.h>
#include <stralgo/stralgo.h>
#include <coll/basic_fixed_string.h>
#include <stralgo/utf/utf.h>

using namespace std::literals::string_view_literals;
//----------------------------------------------------------------------------------------------------------------------
using metatests::constexpr_test;
using metatests::run_consteval_test;
using metatests::run_constexpr_test;
namespace ut = boost::ut;
using ut::operator""_test;
using namespace ut::operators::terse;
using metatests::test_result;
using char_type_list = metatests::type_list<char,char8_t,char16_t>;
// using char_type_list = metatests::type_list<char8_t>;
// using char_type_list = metatests::type_list<char16_t>;
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
namespace ranges = std::ranges;
namespace utf = stralgo::utf;

namespace stralgo_utf_left
{
static void do_test(test_result &result)
  {
  "stralgo_utf_left"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr auto source_u32{ coll::basic_fixed_string(U"🯀🯁🯂🯃🯄🯅🯆🯇🯈🯉🯊🯰🯱🯲🯳🯴🯵🯶🯷🯸🯹")};
      auto source{ utf::to_string_t<char_type>{}(source_u32)};
      constexpr auto expected_u32{coll::basic_fixed_string(U"🯀🯁🯂🯃")};
      auto expected{ utf::to_string_t<char_type>{}(expected_u32)};
      utf::utf_input_view_t view{source};
      
      auto res{stralgo::left(view, 4u )};
      std::basic_string_view x{ res.begin().iter_, res.end().iter_ };
      
      constexpr_test( x == expected );
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_utf_right
{
static void do_test(test_result &result)
  {
  "stralgo_utf_right"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr auto source_u32{ coll::basic_fixed_string(U"🯀🯁🯂🯃🯄🯅🯆🯇🯈🯉🯊🯰🯱🯲🯳🯴🯵🯶🯷🯸🯹")};
      auto source{ utf::to_string_t<char_type>{}(source_u32)};
      constexpr auto expected_u32{coll::basic_fixed_string(U"🯶🯷🯸🯹")};
      auto expected{ utf::to_string_t<char_type>{}(expected_u32)};
      utf::utf_input_view_t view{source};
      
      auto res{stralgo::right(view, 4u )};
      std::basic_string_view x{ res.begin().iter_, res.end().iter_ };
      
      constexpr_test( x == expected );
      return {};
      };
    result |= run_consteval_test<char_type_list>(fn_tmpl);
    result |= run_constexpr_test<char_type_list>(fn_tmpl);
    };
  }
}
namespace stralgo_utf_substr
{
static void do_test(test_result &result)
  {
  "stralgo_utf_substr"_test = [&]
    {
    auto fn_tmpl =
      []<typename char_type>
        ( char_type const *) -> metatests::test_result
      {
      constexpr auto source_u32{ coll::basic_fixed_string(U"🯀🯁🯂🯃🯄🯅🯆🯇🯈🯉🯊🯰🯱🯲🯳🯴🯵🯶🯷🯸🯹")};
      auto source{ utf::to_string_t<char_type>{}(source_u32)};
      constexpr auto expected_u32{coll::basic_fixed_string(U"🯁🯂🯃")};
      auto expected{ utf::to_string_t<char_type>{}(expected_u32)};
      utf::utf_input_view_t view{source};
      auto res{stralgo::substr(view, 1, 3 )};
      std::basic_string_view x{ res.begin().iter_, res.end().iter_ };
      
      constexpr_test( x == expected );
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
  stralgo_utf_left::do_test(result);
  stralgo_utf_right::do_test(result);
  stralgo_utf_substr::do_test(result);
  
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
