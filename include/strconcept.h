//--------------------------------------------------------------------------
// string view manipulation algorithms without dependency on null termination
// {C} Artur Bac 2021
//--------------------------------------------------------------------------
#pragma once

#include <type_traits>
#include <string_view>
#include <string>
#include <ranges_config.h>

namespace strconcept
{
  template<typename type>
  inline constexpr bool is_const = !std::is_same_v<std::remove_const_t<type>,type>;
  
  template<typename iterator>
  using dereferenced_type = decltype(*std::declval<iterator>());
 
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
  template<typename T>
  inline constexpr bool equality_comaprable_v { detail::has_operator_equal<T>::value };
  
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
  template<typename T>
  inline constexpr bool indexable_v { detail::has_operator_index<T>::value };
  
  template<typename iterator>
  using iterator_category_t = typename std::iterator_traits<iterator>::iterator_category;
  
  template<typename iterator>
  inline constexpr bool is_forward_iterator_v = 
      std::is_convertible_v<iterator_category_t<iterator>, std::forward_iterator_tag>;
                            
  template<typename iterator>
  inline constexpr bool is_input_iterator_v = 
      std::is_convertible_v<iterator_category_t<iterator>, std::input_iterator_tag>;

  template<typename iterator>
  inline constexpr bool is_output_iterator_v = 
      std::is_convertible_v<iterator_category_t<iterator>, std::input_iterator_tag>;
                            
  template<typename iterator>
  inline constexpr bool is_iterator_v = is_input_iterator_v<iterator> || is_output_iterator_v<iterator>;

  template<typename iterator>
  inline constexpr bool is_writable_iterator_v = 
    (is_input_iterator_v<iterator> && !is_const<dereferenced_type<iterator>>)
    || is_output_iterator_v<iterator>;

  template<typename iterator>
  inline constexpr bool is_bidirectional_iterator_v = 
      std::is_convertible_v<iterator_category_t<iterator>, std::bidirectional_iterator_tag>;

  template<typename iterator>
  inline constexpr bool is_random_access_iterator_v = 
      std::is_convertible_v<iterator_category_t<iterator>, std::random_access_iterator_tag>;

  template< class T >
  using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

  template<typename iterator,
    typename = std::enable_if_t<is_iterator_v<iterator>>>
  using iterator_difference_type = typename std::iterator_traits<iterator>::difference_type;
  
  template<typename value_type>
  inline constexpr bool is_char_1b_type_v = std::is_same_v<remove_cvref_t<value_type>,char>
                                      /*|| std::is_same_v<aq_typetraits::remove_cvref_t<value_type>,char8_t>*/;
  template<typename value_type>
  inline constexpr bool is_char_type_v = is_char_1b_type_v<value_type>
                                      || std::is_same_v<remove_cvref_t<value_type>,char16_t>
                                      || std::is_same_v<remove_cvref_t<value_type>,char32_t>
                                      || std::is_same_v<remove_cvref_t<value_type>,wchar_t>;
                                      
  
  template<typename string_view_type>
  ///\returns string or view char_type
  using string_view_value_type = remove_cvref_t<decltype(std::declval<string_view_type>()[0])>;
  
  template<typename string_view_type, typename string_view_type2>
  ///\returns true when both views have same base char_type but views may be different types (strings or views)
  inline constexpr bool view_value_type_equals_v 
    = std::is_same_v<string_view_value_type<string_view_type>,string_view_value_type<string_view_type2>>;

  template<typename string_view_type>
  ///\returns aqurat string type based on view base char_type
  using string_by_value_type_t = std::basic_string<string_view_value_type<string_view_type>>;
  
  template<typename char_type>
  ///\brief returns aqurat string type based on char_type
  using string_by_char_type_t = std::basic_string<char_type>;
  
  
  template<typename string_view_type>
  ///\returns true when string_view_type is convertible to basic_string_view
  inline constexpr bool is_convertible_to_string_view_v = 
      std::is_convertible_v<string_view_type,std::basic_string_view<string_view_value_type<string_view_type>>
                                                                               >;
  template<typename string_view_type>
  ///\returns true when string_view_type is convertible to single character
  inline constexpr bool is_convertible_to_char_type_v = std::is_convertible_v<string_view_type,
                                           string_view_value_type<string_view_type>
                                           >;
                                           
   namespace detail
     {
    template<typename value_type, bool >
    struct unsigned_selctor
      {
      using type = std::make_unsigned_t<value_type>;
      };
      
    template<typename value_type>
    struct unsigned_selctor<value_type,false>
      {
      using type = value_type;
      };
     }
  template<typename value_type>
  using make_unsigned_t = typename  detail::unsigned_selctor<value_type, std::is_integral_v<value_type>>::type;
  
//   template<typename float_type>
//   inline constexpr bool is_floating_point_or_fixed_v = std::is_floating_point_v<float_type> || fixedmath::detail::is_fixed_v<float_type>;
  
  namespace detail
    {
    
    template<typename templ_type, bool>
    struct char_type_from_view
      {
      using type = templ_type;
      };
      
    template<typename templ_type>
    struct char_type_from_view<templ_type,false>
      {
      using type = string_view_value_type<templ_type>;
      };
    }
  template<typename templ_type>
  using char_type_from_view_t = typename detail::char_type_from_view<templ_type, is_char_type_v<templ_type>>::type;
  
  template<typename templ_type, typename ... args>
  struct unpack_first { using type = templ_type ; };
  
  template<typename templ_type, typename ... args>
  using unpack_first_t = typename unpack_first<templ_type,args...>::type;
}
