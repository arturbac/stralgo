#pragma once
#include <strconcept.h>
// #include <constexpr_string.h>
#include<coll/ranges/accumulate.h>
#include <functional>
#include <numeric>
#include <algorithm>

namespace stralgo::detail
{
#if (defined(_LIBCPP_VERSION) && _LIBCPP_VERSION >= 12000) || (defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE > 10)
  #define STRALGO_USE_STD_INVOKE
#endif
#if defined(STRALGO_USE_STD_INVOKE)
  template<typename F, typename... Args>
  constexpr auto invoke(F && f, Args &&... args) noexcept(std::is_nothrow_invocable_v<F, Args...>)
    {
    return std::invoke( std::forward<F>(f), std::forward<Args>(args)...);
    }
#else
  template<typename F, typename... Args>
  constexpr auto invoke(F && f, Args &&... args) noexcept(std::is_nothrow_invocable_v<F, Args...>)
    {
    return std::forward<F>(f)(std::forward<Args>(args)...);
    }
#endif

  ///\brief match https://en.cppreference.com/w/cpp/string/byte/isblank but is resistant to signed char UB
  struct isblank_t
    {
    template<concepts::char_type char_type>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr bool operator()( char_type src ) 
      stralgo_static_call_operator_const noexcept
      {
      unsigned c{ static_cast<unsigned>(src) };
      return unsigned(' ') == c ||  unsigned('\t') == c;
      }
    };
  inline constexpr isblank_t isblank;
  
  ///\brief match https://en.cppreference.com/w/cpp/string/byte/isspace but is resistant to signed char UB  
  struct isspace_t
    {
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr bool operator()(concepts::char_type auto src )
        stralgo_static_call_operator_const noexcept
      {
      unsigned c{ static_cast<unsigned>(src) };
      return isblank(src) || unsigned('\n') == c || unsigned('\r') == c || unsigned('\v') == c || unsigned('\f') == c;
      }
    };
  inline constexpr isspace_t isspace;
  
  ///\brief match https://en.cppreference.com/w/cpp/string/byte/isdigit but is resistant to signed char UB
  struct isdigit_t
    {
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr bool operator()( concepts::char_type auto src )
        stralgo_static_call_operator_const noexcept
      {
      unsigned c{ static_cast<unsigned>(src) };
      return c >= 48u && c <= 57u;
      }
    };
  inline constexpr isdigit_t isdigit;
  
  inline constexpr unsigned lower_char_begin_ = 97u;
  inline constexpr unsigned upper_char_begin_ = 65u;
  
  ///\brief match https://en.cppreference.com/w/cpp/string/byte/islower but is resistant to signed char UB
  struct islower_t
    {
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr bool operator()( concepts::char_type auto src )
        stralgo_static_call_operator_const noexcept
      {
      unsigned c{ static_cast<unsigned>(src) };
      return c >= lower_char_begin_ && c <= 122u;
      }
    };
  inline constexpr islower_t islower;
  
  ///\brief match https://en.cppreference.com/w/cpp/string/byte/isxdigit but is resistant to signed char UB
  struct isxdigit_t
    {
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr bool operator()( concepts::char_type auto src )
        stralgo_static_call_operator_const noexcept
      {
      unsigned c{ static_cast<unsigned>(src) };
      return isdigit(src) || (c >= upper_char_begin_ && c <= 70u) || (c >= lower_char_begin_ && c <= 102u);
      }
    };
  inline constexpr isxdigit_t isxdigit;

  ///\brief match https://en.cppreference.com/w/cpp/string/byte/islower but is resistant to signed char UB
  struct isupper_t
    {
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr bool operator()( concepts::char_type auto src )
        stralgo_static_call_operator_const noexcept
      {
      unsigned c{ static_cast<unsigned>(src) };
      return c >= upper_char_begin_ && c <= 90u;
      }
    };
  inline constexpr isupper_t isupper;
  //--------------------------------------------------------------------------------------------------------
  struct to_lower_t
    {
    template<concepts::char_type char_type>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( char_type c )
        stralgo_static_call_operator_const noexcept
        -> char_type
      {
      if( isupper(c) )
        return static_cast<char_type>( static_cast<unsigned>(c) - upper_char_begin_ + lower_char_begin_  );
      return c;
      }
    };
  inline constexpr to_lower_t to_lower;
  //--------------------------------------------------------------------------------------------------------
  struct to_upper_t
    {
    template<concepts::char_type char_type>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( char_type c )
        stralgo_static_call_operator_const noexcept
        -> char_type
      {
      if( islower(c) )
        return static_cast<char_type>( static_cast<unsigned>(c) - lower_char_begin_ + upper_char_begin_ );
      return c;
      }
    };
  inline constexpr to_upper_t to_upper;
  //--------------------------------------------------------------------------------------------------------
  using std::find_if;
  using std::equal;
  using std::find;
    
  struct identity_projection_t
    {
    template<typename input_value_type >
    stralgo_static_call_operator
    constexpr auto operator()( input_value_type && value ) 
        stralgo_static_call_operator_const 
            noexcept(noexcept(std::forward<input_value_type>(value)))
            -> input_value_type &&
      { return std::forward<input_value_type>(value); }
    };
  inline constexpr identity_projection_t identity_projection;
  //--------------------------------------------------------------------------------------------------------
    template<std::forward_iterator input_iterator,
             typename output_iterator,
             typename unary_operation = identity_projection_t>
    constexpr auto transform(input_iterator first, input_iterator last, output_iterator result, unary_operation unary_op = {})
      {
      for (; first != last; ++first, ++result)
        *result = invoke(unary_op, *first);
      return result;
      }
      
  //--------------------------------------------------------------------------------------------------------
  template<typename unary_operation = identity_projection_t,
           typename output_iterator,
           typename value_type>
    constexpr auto fill(output_iterator first, output_iterator last, value_type value, unary_operation unary_op = {} )
      {
      for (; first != last; ++first)
        *first = invoke(unary_op, value);
      return first;
      }
  //--------------------------------------------------------------------------------------------------------
  struct not_is_space_pred_t
    {
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr bool operator()(concepts::char_type auto c)
        stralgo_static_call_operator_const noexcept
      { return !isspace(c); }
    };
  inline constexpr not_is_space_pred_t not_is_space;
    
  template<concepts::char_type char_type>
  struct not_is_char_pred_t
    {
    char_type value;
    [[nodiscard]]
    constexpr bool operator()(char_type c) const noexcept { return value != c; }
    };

  template<std::input_iterator iterator>
  struct not_is_any_of
    {
    using char_type = std::iter_value_t<iterator>;
    iterator any_of_beg, any_of_end;
    constexpr not_is_any_of( iterator b, iterator e ) : any_of_beg{b}, any_of_end{e} {}
    [[nodiscard]]
    constexpr bool operator()(char_type c) const noexcept
      {
      auto it{ std::find(any_of_beg, any_of_end, c) };
      return it == any_of_end;
      }
    };
    //ctad to disable warning about not intended ctad
    template<std::input_iterator iterator>
    not_is_any_of(iterator b, iterator e) -> not_is_any_of<iterator>;
  //--------------------------------------------------------------------------------------------------------
  struct find_first_of_t
    {
    ///\returns pos to the first occurrence in view of any of the characters that are part of one_of, or npos if there are no matches.
    template<concepts::char_range string_view_type,
             concepts::char_range string_view_type2>
      requires concepts::same_range_type<string_view_type,string_view_type2>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( string_view_type const & view, string_view_type2 const & one_of )
        stralgo_static_call_operator_const noexcept
          -> typename string_view_type::size_type
      {
      using size_type = typename string_view_type::size_type;
      
      auto it{std::ranges::begin(view)};
      for(; it != std::ranges::end(view); ++it)
        {
        if ( std::ranges::end(one_of) != std::find( std::ranges::begin(one_of),std::ranges::end(one_of), *it ) )
          break;
        }
      return it != std::ranges::end(view) 
        ? static_cast<size_type>(std::ranges::distance(std::ranges::begin(view),it)) 
        : string_view_type::npos;
      }
    };
  inline constexpr find_first_of_t find_first_of;
  
  //--------------------------------------------------------------------------------------------------------
  struct trim_left_with_pred_t
    {
    template<concepts::char_range string_view_type, typename predicate_type>
    stralgo_static_call_operator
    constexpr auto operator()( string_view_type const & view, predicate_type pred )
        stralgo_static_call_operator_const noexcept
      {
      using char_type = std::ranges::range_value_t<string_view_type>;
      using view_type = std::basic_string_view<char_type>;
      using size_type = typename view_type::size_type;

      auto new_begin { std::ranges::find_if( std::ranges::begin(view), std::ranges::end(view), pred ) };
      if( new_begin != std::ranges::end(view) )
        {
        char_type const * beg{ std::ranges::data(view) };
        auto beg_pos { static_cast<size_type>(new_begin - std::ranges::begin(view)) };
        return view_type{ beg + beg_pos, view.size() - beg_pos };
        }
      return view_type{};
      }
    };
  inline constexpr trim_left_with_pred_t trim_left_with_pred;
  
  //--------------------------------------------------------------------------------------------------------
  struct trim_right_with_pred_t
    {
    template<concepts::char_range string_view_type, typename predicate_type>
    stralgo_static_call_operator
    constexpr auto operator()( string_view_type const & view, predicate_type pred ) 
        stralgo_static_call_operator_const noexcept
      {
      using char_type = std::ranges::range_value_t<string_view_type>;
      using view_type = std::basic_string_view<char_type>;
      using size_type = typename view_type::size_type;

      auto rbegin{ std::make_reverse_iterator(std::end(view)) };
      auto rend{ std::make_reverse_iterator(std::begin(view)) };
      auto new_rbegin { std::ranges::find_if( rbegin, rend, pred ) };

      if( new_rbegin != rend )
        {
        char_type const * beg{ std::ranges::data(view) };
        auto end_pos { static_cast<size_type>(new_rbegin.base() - std::ranges::begin(view)) };
        return view_type{ beg, end_pos };
        }
      return view_type{};
      }
    };
  inline constexpr trim_right_with_pred_t trim_right_with_pred;
  //--------------------------------------------------------------------------------------------------------
  struct trim_pred_t
    {
    template<concepts::char_range string_view_type, typename predicate_type>
    stralgo_static_call_operator
    constexpr auto operator()( string_view_type const & view, predicate_type const & pred )
      stralgo_static_call_operator_const noexcept
      {
      using char_type = std::ranges::range_value_t<string_view_type>;
      using view_type = std::basic_string_view<char_type>;
      using size_type = typename view_type::size_type;

      auto new_begin { std::ranges::find_if( std::ranges::begin(view), std::ranges::end(view), pred ) };
      auto rbegin{ std::make_reverse_iterator( std::ranges::end(view)) };
      auto rend{ std::make_reverse_iterator(new_begin) };
      auto new_rbegin { std::ranges::find_if( rbegin, rend, pred ) };

      if( new_begin != view.end() || new_rbegin != rend )
        {
        auto beg_pos{ new_begin - std::ranges::begin(view) };
        auto end_pos{ new_rbegin.base() - std::ranges::begin(view) };
        char_type const * ptr{ std::ranges::data(view) };
        return view_type{ ptr + beg_pos, static_cast<size_type>(end_pos - beg_pos) };
        }
      return view_type{};
      }
    };
  inline constexpr trim_pred_t trim_pred;
  //--------------------------------------------------------------------------------------------------------
  struct substr_t
    {
    template<concepts::char_range string_view_type>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( string_view_type const & view,
                           std::size_t pos,
                           std::size_t count = std::numeric_limits<std::size_t>::max() )
        stralgo_static_call_operator_const noexcept
      {
      using char_type = std::ranges::range_value_t<string_view_type>;
      using view_type = std::basic_string_view<char_type>;

      if( pos < std::ranges::size(view) )
        {
        count = std::min(count, std::ranges::size(view) - pos);
        char_type const * beg{ std::ranges::data(view) + pos };
        return view_type{ beg, count };
        }
      else
        return view_type{};
      }
    };
  inline constexpr substr_t substr;
  //--------------------------------------------------------------------------------------------------------
  struct left_t
    {
    template<concepts::char_range string_view_type>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( string_view_type const & view, std::size_t count )
        stralgo_static_call_operator_const noexcept
      {
      using char_type = std::ranges::range_value_t<string_view_type>;
      using view_type = std::basic_string_view<char_type>;
      std::size_t const trimed_count{ std::min(count, std::ranges::size(view)) };
      char_type const * beg{ std::ranges::data(view) };
      return view_type{ beg, trimed_count };
      }
    };
  inline constexpr left_t left;
  //--------------------------------------------------------------------------------------------------------
  struct right_t
    {
    template<concepts::char_range string_view_type>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( string_view_type const & view, std::size_t count )
      stralgo_static_call_operator_const noexcept
      {
      using char_type = std::ranges::range_value_t<string_view_type>;
      using view_type = std::basic_string_view<char_type>;

      std::size_t const trimed_count{ std::min(count, std::ranges::size(view)) };
      std::size_t const pos{ std::ranges::size(view) - trimed_count };
      char_type const * beg{ std::data(view) + pos };
      return view_type{ beg, trimed_count };
      }
    };
  inline constexpr right_t right;
  //--------------------------------------------------------------------------------------------------------

  template<concepts::char_type maybe_char_type>
  constexpr size_t view_size( maybe_char_type ) noexcept
    { return 1u; }
  
  template<concepts::char_range string_view_type>
    requires (!concepts::char_type<string_view_type>)
  constexpr size_t view_size( string_view_type const & view ) noexcept
    { return std::ranges::size(view); }
    
  template<typename string_view_type, typename ... args_type >
  constexpr auto count_size(string_view_type const & viewl, args_type const & ... args) noexcept
    {
    if constexpr (sizeof...(args_type) != 0)
      return view_size(viewl) + count_size(args...);
    else
      return view_size(viewl);
    }
    
  template<typename iterator, typename string_view_type, typename ... args_type>
  constexpr iterator copy_views( iterator it, string_view_type const & view_or_char_value, args_type const & ... args ) noexcept
    {
    if constexpr (concepts::char_type<string_view_type>)
      {
      *it = view_or_char_value;
      ++it;
      if constexpr (sizeof...(args_type) != 0)
        it = copy_views(it, args ... );
      }
    else
      {
      it = std::ranges::copy( std::ranges::begin(view_or_char_value),
                                           std::ranges::end(view_or_char_value), it).out;
      if constexpr (sizeof...(args_type) != 0)
        it = copy_views(it, args ... );
      }
    return it;
    }
  template<template<typename > typename basic_string_type>
  struct string_supports_resize_and_overwrite_t
    {
    static constexpr bool value = false;
    };
  template<template<typename > typename basic_string_type>
  inline constexpr bool supports_resize_and_overwrite_v = string_supports_resize_and_overwrite_t<basic_string_type>::value;
    
  template<typename string_type, bool supports_resize_and_overwrite>
  struct copy_views_t
    {
    using size_type = typename string_type::size_type;
    
    template<typename copy_type>
    stralgo_static_call_operator
    constexpr auto operator()( size_type aggregated_size, copy_type const & copy_views_op) 
      stralgo_static_call_operator_const noexcept
      {
      string_type result;
      if constexpr(supports_resize_and_overwrite)
        result.resize_and_overwrite(aggregated_size, [&copy_views_op]( auto * data, size_type )
          {
          auto oit {copy_views_op( data )};
          return static_cast<size_type>(std::distance(data, oit));
          });
      else
        {
        result.resize(aggregated_size);
        auto oit {copy_views_op(result.data())};
        result.resize(static_cast<size_type>(std::distance(result.data(), oit)));
        }
      return result;
      }
    };

  //--------------------------------------------------------------------------------------------------------


  template<>
  struct string_supports_resize_and_overwrite_t<coll::basic_string>
    {
    static constexpr bool value = true;
    };

    
    
  template<typename T, typename ... args_n>
  concept merge_concepts =
   requires
     {
      // requires (sizeof...(args_n) > 0); //require at least 2 arguments
      //support chars or views 
      requires concepts::char_type<T> || concepts::char_range<T>;
    };
  template<template<typename > typename basic_string_type>
  struct merge_t
    {
    ///\brief merges many strings views or chars into one with single string buffer allocation
    template<merge_concepts ... string_view_or_char_n>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( string_view_or_char_n const & ... args  )
        stralgo_static_call_operator_const
      {
      using first_arg_type = typename concepts::unpack_first<string_view_or_char_n ...>::type;
      using char_type = concepts::char_type_from_view_t<first_arg_type>;

      using string_type = basic_string_type<char_type>;
      using size_type = typename string_type::size_type;
      auto const aggregated_size{ static_cast<size_type>(detail::count_size(args...)) };
      constexpr bool supports_resize_and_overwrite = supports_resize_and_overwrite_v<basic_string_type>;
      return copy_views_t<string_type,supports_resize_and_overwrite>{}( aggregated_size,
                                          [&args...]( auto out_iterator ) noexcept
                                          {
                                          return detail::copy_views( out_iterator, args...);
                                          } );
  
      }
    };
  inline constexpr merge_t<coll::basic_string> merge;
  namespace stl
    {
    template<concepts::char_type char_type>
    using basic_string = std::basic_string<char_type>;
    inline constexpr merge_t<basic_string> merge;
    }
  template<>
  struct string_supports_resize_and_overwrite_t<stl::basic_string>
    {
#if defined(__cpp_lib_string_resize_and_overwrite)
    static constexpr bool value = true;
#else
    static constexpr bool value = false;
#endif
    };
  //--------------------------------------------------------------------------------------------------------
  template<template<typename > typename basic_string_type>
  struct merge_range_t
    {
    ///\brief merges range of string view with one buffer allocation
    template<concepts::iterator_over_char_ranges fwd_iterator, std::sentinel_for<fwd_iterator> sentinel>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( fwd_iterator itbeg, sentinel itend )
        stralgo_static_call_operator_const
      {
      using range_type = std::iter_value_t<fwd_iterator>;
      using char_type = std::ranges::range_value_t<range_type>;
      using string_type = basic_string_type<char_type>;
      using size_type = typename string_type::size_type;
      constexpr bool supports_resize_and_overwrite = supports_resize_and_overwrite_v<basic_string_type>;
      size_type aggregated_size{ coll::ranges::accumulate(itbeg, itend, size_type{}, 
                                          [](size_type init, auto const & view ) noexcept -> size_type
                                          { return init + static_cast<size_type>(std::ranges::size(view)); }) };
      return copy_views_t<string_type,supports_resize_and_overwrite>{}( aggregated_size,
                                          [itbeg, itend]( auto out_iterator )
                                          {
                                          for(auto beg{itbeg}; beg !=itend; ++beg)
                                            out_iterator = std::copy(std::begin(*beg),std::end(*beg), out_iterator );
                                          return out_iterator;
                                          } );
      }
    template<std::ranges::forward_range forward_range>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( forward_range const & range )
        stralgo_static_call_operator_const
      {
      return operator()(std::ranges::begin(range), std::ranges::end(range) );
      }
    };
  inline constexpr merge_range_t<coll::basic_string> merge_range;
  namespace stl
    {
    inline constexpr merge_range_t<basic_string> merge_range;
    }
  //--------------------------------------------------------------------------------------------------------
  struct ends_with_t
    {
    template<concepts::char_range string_view_type,
             concepts::char_range string_view_type2>
      requires concepts::same_range_type<string_view_type,string_view_type2>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr bool operator()( string_view_type const & str, string_view_type2 const & other )
        stralgo_static_call_operator_const noexcept
      {
      auto const str0_sz = std::ranges::size(str);
      auto const str1_sz = std::ranges::size(other);
      using difference_type = std::ranges::range_difference_t<string_view_type>;
      
      if(str0_sz >= str1_sz) [[likely]]
        return std::ranges::equal(std::ranges::begin(other), std::ranges::end(other),
                          std::ranges::prev(std::ranges::end(str), static_cast<difference_type>(str1_sz)), std::ranges::end(str) );
      return false;
      }
    };
  inline constexpr ends_with_t ends_with;
  //--------------------------------------------------------------------------------------------------------
  struct starts_with_t
    {
    template<concepts::char_range string_view_type,
             concepts::char_range string_view_type2>
      requires concepts::same_range_type<string_view_type,string_view_type2>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr bool operator()( string_view_type const & str, string_view_type2 const & other )
        stralgo_static_call_operator_const noexcept
      {
      auto const str0_sz = std::ranges::size(str);
      auto const str1_sz = std::ranges::size(other);
      using difference_type = std::ranges::range_difference_t<string_view_type>;
      
      if(str0_sz >= str1_sz)
        return std::ranges::equal(std::ranges::begin(other), std::ranges::end(other),
                                  std::ranges::begin(str),
                                  std::ranges::next(std::ranges::begin(str),static_cast<difference_type>(str1_sz)) );
      return false;
      }
    };
  inline constexpr starts_with_t starts_with;
  //--------------------------------------------------------------------------------------------------------
#if 0
  //TODO change to view
  template<concepts::char_type target_char_type,
           concepts::convertible_to_string_view string_view_type,
           typename function_obj>
  auto make_transform_string_view_target( string_view_type const & str, function_obj const & fobj ) noexcept
    {
    using string_type = concepts::string_by_char_type_t<target_char_type>;
    using char_type = std::ranges::range_value_t<string_view_type>;

    string_type result;
    result.resize(str.size());
    stralgo::detail::transform(str.begin(), str.end(), result.begin(), [&fobj](char_type c){ return fobj(c); });
    return result;
    }
    
  template<concepts::convertible_to_string_view string_view_type, typename function_obj>
  auto make_transform_string_view( string_view_type const & str, function_obj const & fobj ) noexcept
    {
    using char_type = std::ranges::range_value_t<string_view_type>;
    return make_transform_string_view_target<char_type>(str, fobj);
    }
    
  template<concepts::char_type target_char_type, typename function_obj>
  auto make_transform_string_view( concepts::string_by_char_type_t<target_char_type> && str,
                                   function_obj const & fobj ) noexcept
    {
    using string_type = concepts::string_by_char_type_t<target_char_type>;
    using char_type = target_char_type;
    using unique_buffer_type = typename string_type::unique_buffer_type;
      {
      auto const size{ str.size() };
      unique_buffer_type buff{str, size, true };
      stralgo::detail::transform(buff.begin(), buff.finish(), buff.begin(), [&fobj](char_type c){ return fobj(c); });
      }
    return std::move(str);
    }
#endif
  //--------------------------------------------------------------------------------------------------------
  struct compare_no_case_t
    {
    template<concepts::char_range string_view_type,
             concepts::char_range string_view_type2>
      requires concepts::same_range_type<string_view_type,string_view_type2>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr int operator()( string_view_type const & s1, string_view_type2 const & s2 )
        stralgo_static_call_operator_const noexcept
      {
      using char_type = std::ranges::range_value_t<string_view_type>;
      using traits = std::char_traits<char_type>;
      
      auto s1_size{ std::ranges::size(s1) };
      auto s2_size{ std::ranges::size(s2) };
      
      std::size_t count{ std::min(s1_size,s2_size) };
      for (std::size_t i = 0; i != count; ++i)
        {
        char_type l1{ to_lower(s1[i]) };
        char_type l2{ to_lower(s2[i]) };
        if( !traits::eq(l1, l2) )
          return traits::lt(l1, l2) ? -1 : 1;
        }
      if( s1_size != s2_size )
        return s1_size < s2_size ? -1 : 1;
      else
        return 0;
      }
    };
  inline constexpr compare_no_case_t compare_no_case;
  //--------------------------------------------------------------------------------------------------------
  struct is_number_t
    {
    ///\returns true when string contains only number chars.
    template<concepts::char_range string_view_type>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr bool operator()(string_view_type const & str)
        stralgo_static_call_operator_const noexcept
      {
      using char_type = std::ranges::range_value_t<string_view_type>;
      return std::ranges::end(str) == std::ranges::find_if( str,
                                                     []( char_type c) noexcept
                                                     { return !isdigit(c); } );
      }
    };
  inline constexpr is_number_t is_number;
  //--------------------------------------------------------------------------------------------------------
  struct is_hexnumber_t
    {
    template<concepts::char_range string_view_type>
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr bool operator()(string_view_type const & str)
        stralgo_static_call_operator_const noexcept
      {
      using char_type = std::ranges::range_value_t<string_view_type>;
      return std::ranges::end(str) == std::ranges::find_if( str,
                                                     []( char_type c) noexcept
                                                     { return !isxdigit(c); } );
      }
    };
  inline constexpr is_hexnumber_t is_hexnumber;
}
