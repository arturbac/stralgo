//--------------------------------------------------------------------------
// string view manipulation algorithms without dependency on null termination
// {C} Artur Bac 2021
//--------------------------------------------------------------------------
#pragma once

#include <detail/stralgo.h>
#include <algorithm>
#include <numeric>
#include <cwctype>
#include <iterator>

namespace stralgo
{
  //--------------------------------------------------------------------------------------------------------
  ///\brief match https://en.cppreference.com/w/cpp/string/byte/isblank but is resistant to signed char UB
  template<strconcept::char_type possible_char_type>
  [[nodiscard]]
  constexpr bool isblank( possible_char_type src )
    { return detail::isblank_(src); }
  //--------------------------------------------------------------------------------------------------------
  ///\brief match https://en.cppreference.com/w/cpp/string/byte/isspace but is resistant to signed char UB  
  template<strconcept::char_type possible_char_type>
  [[nodiscard]]
  constexpr bool isspace( possible_char_type src )
    { return detail::isspace_(src); }
  //--------------------------------------------------------------------------------------------------------
  ///\brief match https://en.cppreference.com/w/cpp/string/byte/isdigit but is resistant to signed char UB
  template<strconcept::char_type possible_char_type>
  [[nodiscard]]
  constexpr bool isdigit( possible_char_type src )
    { return detail::isdigit_(src); }
  //--------------------------------------------------------------------------------------------------------
  ///\brief match https://en.cppreference.com/w/cpp/string/byte/isxdigit but is resistant to signed char UB
  template<strconcept::char_type possible_char_type>
  [[nodiscard]]
  constexpr bool isxdigit( possible_char_type src )
    { return detail::isxdigit_(src); }
  //--------------------------------------------------------------------------------------------------------
  ///\brief match https://en.cppreference.com/w/cpp/string/byte/islower but is resistant to signed char UB
  template<strconcept::char_type possible_char_type>
  [[nodiscard]]
  constexpr bool islower( possible_char_type src )
    { return detail::islower_(src); }
  //--------------------------------------------------------------------------------------------------------
  ///\brief match https://en.cppreference.com/w/cpp/string/byte/islower but is resistant to signed char UB
  template<strconcept::char_type possible_char_type>
  [[nodiscard]]
  constexpr bool isupper( possible_char_type src )
    { return detail::isupper_(src); }
  //--------------------------------------------------------------------------------------------------------
  template<strconcept::char_type possible_char_type>
  [[nodiscard]]
  constexpr auto to_lower( possible_char_type c )
    {
    return detail::to_lower_(c);
    }
  //--------------------------------------------------------------------------------------------------------
  template<strconcept::char_type possible_char_type>
  [[nodiscard]]
  constexpr auto to_upper( possible_char_type c )
    {
    return detail::to_upper_(c);
    }
  //--------------------------------------------------------------------------------------------------------
  ///\returns pos to the first occurrence in view of any of the characters that are part of one_of, or npos if there are no matches.
  ///\warning requires char_type of both views/strings to be same type
  template<typename string_view_type, typename string_view_type2>
    requires (
           strconcept::convertible_to_string_view<string_view_type>
        && strconcept::convertible_to_string_view<string_view_type2>
        && strconcept::view_value_type_equals<string_view_type,string_view_type2>
    )
  ///\param view and \param one_of any view_type or string_type
  [[nodiscard]]
  constexpr auto find_first_of( string_view_type const & view, string_view_type2 const & one_of )
    {
    return detail::find_first_of_(view,one_of);
    }

  //--------------------------------------------------------------------------------------------------------
  ///\returns trimmed left view of \param view from values that match isspace
  template<typename string_view_type>
    requires strconcept::convertible_to_string_view<string_view_type>
  [[nodiscard]]
  constexpr auto trim_left( string_view_type const & view) noexcept
    {
    return detail::trim_left_predicate( view, detail::not_is_space_pred_t{} );
    }
    
  ///\returns trimmed left view \param view from values that match value
  template<typename string_view_type, typename char_type>
    requires ( 
           strconcept::convertible_to_string_view<string_view_type>
        && strconcept::char_type<char_type>
        && strconcept::same_as<strconcept::string_view_value_type<string_view_type>,char_type> )
  [[nodiscard]]
  constexpr auto trim_left( string_view_type const & view, char_type value) noexcept
    {
    return detail::trim_left_predicate( view, detail::not_is_char_pred_t<char_type>{value} );
    }
    
  ///\returns trimmed left view \param view from values that match any chars in any_of
  template<typename string_view_type, typename string_view_type2>
    requires(
         strconcept::convertible_to_string_view<string_view_type>
      && strconcept::convertible_to_string_view<string_view_type2>
      && strconcept::view_value_type_equals<string_view_type,string_view_type2>
      )
  [[nodiscard]]
  constexpr auto trim_left( string_view_type const & view, string_view_type2 any_of) noexcept
    {
    return detail::trim_left_predicate( view, detail::not_is_any_of{ std::begin(any_of), std::end(any_of) });
    }

  //--------------------------------------------------------------------------------------------------------
  ///\returns trimmed right view of \param view from values that match isspace
  template<typename string_view_type>
    requires strconcept::convertible_to_string_view<string_view_type>
  [[nodiscard]]
  constexpr auto trim_right( string_view_type const & view) noexcept
    {
    return detail::trim_right_predicate( view, detail::not_is_space_pred_t{} );
    }
    
  ///\returns trimmed right view of \param view from values that match value
  template<typename string_view_type, typename char_type>
    requires ( 
           strconcept::convertible_to_string_view<string_view_type>
        && strconcept::char_type<char_type>
        && strconcept::same_as<strconcept::string_view_value_type<string_view_type>,char_type> )
  [[nodiscard]]
  constexpr auto trim_right( string_view_type const & view, char_type value) noexcept
    {
    return detail::trim_right_predicate( view, detail::not_is_char_pred_t<char_type>{value} );
    }
    
  ///\returns trimmed right view of \param view from values that match any chars in any_of
  template<typename string_view_type, typename string_view_type2>
    requires(
         strconcept::convertible_to_string_view<string_view_type>
      && strconcept::convertible_to_string_view<string_view_type2>
      && strconcept::view_value_type_equals<string_view_type,string_view_type2>
      )
  [[nodiscard]]
  constexpr auto trim_right( string_view_type const & view, string_view_type2 const & any_of) noexcept
    {
    return detail::trim_right_predicate( view, detail::not_is_any_of{ std::begin(any_of), std::end(any_of) });
    }
  //--------------------------------------------------------------------------------------------------------
  template<typename string_view_type>
    requires strconcept::convertible_to_string_view<string_view_type>
  [[nodiscard]]
  constexpr auto substr( string_view_type const & view,
                         typename string_view_type::size_type pos,
                         typename string_view_type::size_type count = string_view_type::npos ) noexcept
    {
    return detail::substr_(view, pos, count );
    }
  
  //--------------------------------------------------------------------------------------------------------
    
  template<typename string_view_type>
    requires strconcept::convertible_to_string_view<string_view_type>
  [[nodiscard]]
  constexpr auto left( string_view_type const & view, typename string_view_type::size_type count )
    {
    return detail::left_(view,count);
    }
  
  //--------------------------------------------------------------------------------------------------------
    
  template<typename string_view_type>
    requires strconcept::convertible_to_string_view<string_view_type>
  [[nodiscard]]
  constexpr auto right( string_view_type const & view, typename string_view_type::size_type count )
    {
    return detail::right_(view,count);
    }

  //--------------------------------------------------------------------------------------------------------
  ///\returns trimmed view of \param view from values that match isspace
  template<typename string_view_type>
    requires strconcept::convertible_to_string_view<string_view_type>
  [[nodiscard]]
  constexpr auto trim( string_view_type const & view) noexcept
    {
    return detail::trim_predicate( view, detail::not_is_space_pred_t{} );
    }
    
  ///\returns trimmed view of \param view from values that match value
  template<typename string_view_type, typename char_type>
      requires ( 
           strconcept::convertible_to_string_view<string_view_type>
        && strconcept::char_type<char_type>
        && strconcept::same_as<strconcept::string_view_value_type<string_view_type>,char_type> )
  [[nodiscard]]
  constexpr auto trim( string_view_type const & view, char_type value) noexcept
    {
    return detail::trim_predicate( view, detail::not_is_char_pred_t<char_type>{value} );
    }
    
  ///\returns trimmed view of \param view from values that match any chars in any_of
  template<typename string_view_type, typename string_view_type2>
    requires(
         strconcept::convertible_to_string_view<string_view_type>
      && strconcept::convertible_to_string_view<string_view_type2>
      && strconcept::view_value_type_equals<string_view_type,string_view_type2>
      )
  [[nodiscard]]
  constexpr auto trim( string_view_type const & view, string_view_type2 const & any_of) noexcept
    {
    static_assert(strconcept::view_value_type_equals_v<string_view_type,string_view_type2>);
    return detail::trim_predicate( view, detail::not_is_any_of{ std::begin(any_of), std::end(any_of) });
    }

  //--------------------------------------------------------------------------------------------------------
  ///\brief merges many strings views or chars into one with single string buffer allocation
  template<typename ... string_view_or_char_n,
    typename string_type = strconcept::string_by_char_type_t<strconcept::char_type_from_view_t<typename strconcept::unpack_first<string_view_or_char_n ...>::type> >,
    typename = std::enable_if_t<detail::merge_concepts_<string_view_or_char_n...>>
    >
  [[nodiscard]]
  auto merge( string_view_or_char_n const & ... args ) noexcept
    {
    return detail::merge_impl_<string_type>(args...);
    }
    
  //--------------------------------------------------------------------------------------------------------
  ///\brief merges range of string view with one buffer allocation
  template<typename fwd_iterator,
  typename string_type = strconcept::string_by_value_type_t<strconcept::remove_cvref_t<decltype(*std::declval<fwd_iterator>())>>,
    typename = std::enable_if_t<detail::merge_range_concepts_<fwd_iterator>>
    >
  [[nodiscard]]
  auto merge_range( fwd_iterator beg, fwd_iterator itend )
    {
    return detail::merge_range_<fwd_iterator,string_type>(beg,itend);
    }
  //--------------------------------------------------------------------------------------------------------
  template<typename string_view_type, typename string_view_type2>
    requires(
         strconcept::convertible_to_string_view<string_view_type>
      && strconcept::convertible_to_string_view<string_view_type2>
      && strconcept::view_value_type_equals<string_view_type,string_view_type2>
      )
  [[nodiscard]]
  constexpr bool ends_with( string_view_type const & str, string_view_type2 const & other ) noexcept
    {
    return detail::ends_with_(str,other);
    }
  //--------------------------------------------------------------------------------------------------------
  template<typename string_view_type, typename string_view_type2>
    requires(
         strconcept::convertible_to_string_view<string_view_type>
      && strconcept::convertible_to_string_view<string_view_type2>
      && strconcept::view_value_type_equals<string_view_type,string_view_type2>
      )
  [[nodiscard]]
  constexpr bool starts_with( string_view_type const & str, string_view_type2 const & other ) noexcept
    {
    return detail::starts_with_(str,other);
    }

  //--------------------------------------------------------------------------------------------------------
  template<typename string_view_type, typename string_view_type2>
    requires(
         strconcept::convertible_to_string_view<string_view_type>
      && strconcept::convertible_to_string_view<string_view_type2>
      && strconcept::view_value_type_equals<string_view_type,string_view_type2>
      )
  [[nodiscard]]
  constexpr int compare_no_case( string_view_type const & s1, string_view_type2 const & s2 )
    {
    return detail::compare_no_case_(s1,s2);
    }
  
  //--------------------------------------------------------------------------------------------------------
    
  ///\returns true when string contains only number chars.
  ///\param str any string or view convertible to std::basic_string_view
  template<typename string_view_type>
    requires strconcept::convertible_to_string_view<string_view_type>
  constexpr bool is_number(string_view_type const & str)
    {
    return detail::is_number_(str);
    }
  
  //--------------------------------------------------------------------------------------------------------
    
  ///\returns true when string contains only number chars.
  ///\param str any string or view convertible to std::basic_string_view
  template<typename string_view_type>
    requires strconcept::convertible_to_string_view<string_view_type>
  constexpr bool is_hexnumber(string_view_type const & str)
    {
    return detail::is_hexnumber(str);
    }

  }

  
