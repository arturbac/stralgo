//--------------------------------------------------------------------------
// string view manipulation algorithms without dependency on null termination
// {C} Artur Bac 2021
//--------------------------------------------------------------------------
#pragma once

#include <type_traits>
#include <string_view>
#include <string>

namespace strconcept
{
  template<typename type>
  inline constexpr bool is_const = !std::is_same_v<std::remove_const_t<type>,type>;
  
  template<typename iterator>
  using dereferenced_type = decltype(*std::declval<iterator>());
 
  template<typename integral_type>
  concept integral = std::is_integral_v<integral_type>;
  
  template<typename integral_type>
  concept signed_integral = std::is_signed_v<integral_type>;
  
  template<typename integral_type>
  concept unsigned_integral = std::is_unsigned_v<integral_type>;
  
  template<typename float_type>
  concept floating_point = std::is_floating_point_v<float_type>;
  
  namespace detail 
    {
    template<typename T, typename U>
    concept same_as = std::is_same_v<T, U>;
    }
   
  template<typename T, typename U >
  concept same_as = detail::same_as<T, U> && detail::same_as<U, T>;

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

  template<typename value_type>
  concept indexable = detail::has_operator_index<value_type>::value;
  
  template<typename iterator>
  using iterator_category_t = typename std::iterator_traits<iterator>::iterator_category;
  
  template<typename iterator>
  concept forward_iterator = convertible<iterator_category_t<iterator>, std::forward_iterator_tag>;
  
  template<typename iterator>
  concept input_iterator = convertible<iterator_category_t<iterator>, std::input_iterator_tag> ;

  template<typename iterator>
  concept output_iterator = convertible<iterator_category_t<iterator>, std::input_iterator_tag>;

  template<typename iterator_type>
  concept iterator = input_iterator<iterator_type> || output_iterator<iterator_type>;
  
  template<typename iterator>
  concept writable_iterator =  (input_iterator<iterator> && !is_const<dereferenced_type<iterator>>) || output_iterator<iterator>;
  
  template<typename iterator>
  concept bidirectional_iterator = convertible<iterator_category_t<iterator>, std::bidirectional_iterator_tag>;
  
  template<typename iterator>
  concept random_access_iterator = convertible<iterator_category_t<iterator>, std::random_access_iterator_tag>;
  
  template <typename iter, typename = void>
  struct iterator_trait 
    : std::iterator_traits<iter> {};
    
  template <typename iter>
  struct iterator_trait<iter, std::void_t<typename iter::container_type>> 
    : std::iterator_traits<typename iter::container_type::iterator> {};

  template <typename iterator>
  using iterator_value_type_t = typename iterator_trait<iterator>::value_type;
  
  template< class T >
  using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

  template<iterator iterator_type>
  using iterator_difference_type = typename std::iterator_traits<iterator_type>::difference_type;
  
  template<typename value_type>
  concept char_1b_type = same_as<remove_cvref_t<value_type>,char>
                                      /*|| std::is_same_v<aq_typetraits::remove_cvref_t<value_type>,char8_t>*/;

  template<typename value_type>
  concept char_type = char_1b_type<value_type>
                    || same_as<remove_cvref_t<value_type>,char16_t>
                    || same_as<remove_cvref_t<value_type>,char32_t>
                    || same_as<remove_cvref_t<value_type>,wchar_t>;
                                      
  
  template<typename string_view_type>
  ///\returns string or view char_type
  using string_view_value_type = remove_cvref_t<decltype(std::declval<string_view_type>()[0])>;
  
  template<typename string_view_type, typename string_view_type2>
  ///\brief true when both views have same base char_type but views may be different types (strings or views)
  concept view_value_type_equals = same_as<string_view_value_type<string_view_type>, string_view_value_type<string_view_type2>>;
  
  template<typename string_view_type>
  ///\returns aqurat string type based on view base char_type
  using string_by_value_type_t = std::basic_string<string_view_value_type<string_view_type>>;
  
  template<typename char_type>
  ///\brief returns aqurat string type based on char_type
  using string_by_char_type_t = std::basic_string<char_type>;

  template<typename string_view_type>
  concept convertible_to_string_view = 
        !char_type<string_view_type>
     && convertible<string_view_type,std::basic_string_view<string_view_value_type<string_view_type>>>
     && char_type<string_view_value_type<string_view_type>>;
  
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
  using char_type_from_view_t = typename detail::char_type_from_view<templ_type, char_type<templ_type>>::type;
  
  template<typename templ_type, typename ... args>
  struct unpack_first { using type = templ_type ; };
  
  template<typename templ_type, typename ... args>
  using unpack_first_t = typename unpack_first<templ_type,args...>::type;
}
