//--------------------------------------------------------------------------
// string view manipulation algorithms without dependency on null termination
// {C} Artur Bac 2021
//--------------------------------------------------------------------------
#pragma once

// #include "constexpr_string.h"
#include <coll/basic_string.h>
#include <ranges>
#include <type_traits>
#include <concepts>
#include <iterator>
#include <string>

namespace stralgo::concepts
{
  template<typename type>
  inline constexpr bool is_const = !std::is_same_v<std::remove_const_t<type>,type>;
  
  template<typename iterator>
  using dereferenced_type = decltype(*std::declval<iterator>());
 
  using std::integral;
  using std::signed_integral;
  using std::unsigned_integral;
  using std::floating_point;
  using std::same_as;

  template<typename from, typename to>
  concept convertible = std::is_convertible_v<from,to>;
  
  template< typename maybe_uint8_t>
  concept integral_uint8 = same_as<maybe_uint8_t,uint8_t>;
  
  namespace detail
    {
    //solution for lack of <concepts> equality_comaprable_v with c++17
    //source https://stackoverflow.com/questions/6534041/how-to-check-whether-operator-exists
    template<class T, class EqualTo = T>
    struct has_operator_equal
      {
      template<class U, class V>
      static auto test(U*) -> decltype(std::declval<U>() == std::declval<V>());
      template<typename, typename>
      static auto test(...) -> std::false_type;

      static constexpr bool value = std::is_same<bool, decltype(test<T, EqualTo>(nullptr))>::value;
      };
    }
    
  using std::equality_comparable;
  
  namespace detail
    {
    template<class T>
    struct has_operator_index
      {
      template<class U>
      static auto test(U*) -> decltype(std::declval<U>()[0]);
      template<typename>
      static auto test(...) -> void;

      static constexpr bool value = !std::is_same<void, decltype(test<T>(nullptr))>::value;
      };
    }

  template<typename value_type>
  concept indexable = detail::has_operator_index<value_type>::value;
  
  template<typename iterator>
  using iterator_category_t = typename std::iterator_traits<iterator>::iterator_category;
  
  using std::forward_iterator;
  using std::input_iterator;
  using std::output_iterator;
  using std::bidirectional_iterator;
  using std::random_access_iterator;
  
  // template<typename iterator_type>
  // concept iterator = input_iterator<iterator_type> || output_iterator<iterator_type>;
  
  // template<typename iterator>
  // concept writable_iterator =  (input_iterator<iterator> && !is_const<dereferenced_type<iterator>>) || output_iterator<iterator>;
  using std::iter_value_t;

  template <typename iterator>
  using iterator_value_type_t = iter_value_t<iterator>;
  
  template< class T >
  using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

  // using std::iter_difference_t;
  
  // template<typename iterator_type>
  // using iterator_difference_type = std::iter_difference_t<iterator_type>;
  
  template<typename value_type>
  concept char_1b_type = same_as<remove_cvref_t<value_type>,char>
                                      || same_as<remove_cvref_t<value_type>,char8_t>;

  template<typename value_type>
  concept char_type = char_1b_type<value_type>
                    || same_as<remove_cvref_t<value_type>,char16_t>
                    || same_as<remove_cvref_t<value_type>,char32_t>
                    || same_as<remove_cvref_t<value_type>,wchar_t>;

  template<typename iterator_type>
  concept char_iterator = std::forward_iterator<iterator_type> && char_type<std::iter_value_t<iterator_type>>;
    
  template<typename range>
  concept char_range = std::ranges::forward_range<range> && char_type<std::ranges::range_value_t<range>>;
    
  template<typename iterator_type>
  concept ui8_iterator = std::forward_iterator<iterator_type> && std::same_as<uint8_t,std::iter_value_t<iterator_type>>;
  
  template<typename iterator_type>
  concept iterator_over_char_ranges = std::forward_iterator<iterator_type> && char_range<std::iter_value_t<iterator_type>>;
  
  template<char_range string_view_type>
  ///\returns string or view char_type
  using string_view_value_type = std::ranges::range_value_t<string_view_type>;

  template<typename string_view_type, typename string_view_type2>
  ///\brief true when both views have same base char_type but views may be different types (strings or views)
  concept same_range_type = 
    requires {
      requires char_range<string_view_type>;
      requires char_range<string_view_type2>;
      requires std::same_as<std::ranges::range_value_t<string_view_type>, std::ranges::range_value_t<string_view_type2>>;
      };
      
  template<concepts::char_range string_view_type>
  ///\returns string type based on view base char_type
  using string_by_value_type_t = std::basic_string<std::ranges::range_value_t<string_view_type>>;
  
  template<concepts::char_type char_type>
  ///\brief returns string type based on char_type
  using string_by_char_type_t = std::basic_string<char_type>;

  using std::make_unsigned_t;
  
  namespace detail
    {
    enum struct arg_type { invalid, single, range };
    template<typename T>
    consteval arg_type decl_arg_type() noexcept 
      {
      if constexpr( concepts::char_type<T> )
        return arg_type::single;
      else if constexpr( concepts::char_range<T> )
        return arg_type::range;
      else return arg_type::invalid;
      }
      
    template<typename templ_type, arg_type>
    struct char_type_from_view
      {
      using type = void;
      };
      
    template<typename templ_type>
    struct char_type_from_view<templ_type,arg_type::single>
      {
      using type = templ_type;
      };
      
    template<typename templ_type>
    struct char_type_from_view<templ_type,arg_type::range>
      {
      using type = std::ranges::range_value_t<templ_type>;
      };
    }
  template<typename templ_type>
  using char_type_from_view_t = typename detail::char_type_from_view<templ_type, detail::decl_arg_type<templ_type>()>::type;
  
  template<typename templ_type, typename ... args>
  struct unpack_first { using type = templ_type ; };
  
  template<typename templ_type, typename ... args>
  using unpack_first_t = typename unpack_first<templ_type,args...>::type;
  
  namespace detail
    {
    template<typename T, typename ... args>
    consteval arg_type guess_arg_type() noexcept 
      {
      constexpr arg_type my_value {decl_arg_type<T>()};
      if constexpr( my_value != arg_type::invalid || sizeof...(args) == 0)
        return my_value;
      else
        return guess_arg_type<args...>();
      }

    template<typename T, typename ... args>
    struct decl_chartype_from_args
      {
      static constexpr bool is_valid_type  = decl_arg_type<T>() != detail::arg_type::invalid;
      using type = std::conditional_t<is_valid_type,
                                    char_type_from_view_t<T>,
                                    typename decl_chartype_from_args<args...>::type
                                    >;
      };
    template<typename T>
    struct decl_chartype_from_args<T>
      {
      static constexpr bool is_valid_type  = decl_arg_type<T>() != detail::arg_type::invalid;
      using type = char_type_from_view_t<T>; //maybe void if no char_type or char_range
      };
    }
  
  template<typename ... args>
  inline constexpr bool decl_chartype_from_args_has_valid_type_v = detail::decl_chartype_from_args<args...>::is_valid_type;
  
  template<typename ... args>
  using decl_chartype_from_args_t = typename detail::decl_chartype_from_args<args...>::type;
}

#if defined(__cpp_static_call_operator)
  #define stralgo_static_call_operator static
  #define stralgo_static_call_operator_const
#else
  #define stralgo_static_call_operator
  #define stralgo_static_call_operator_const const
#endif
