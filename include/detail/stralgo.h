#pragma once
#include <strconcept.h>
#include <functional>
#include <numeric>

namespace stralgo::detail
{
  template<typename possible_char_type,
    typename  = std::enable_if_t<strconcept::is_char_type_v<possible_char_type>>>
  [[nodiscard]]
  constexpr bool isblank_( possible_char_type src )
    {
    unsigned c{ static_cast<unsigned>(src) };
    return unsigned(' ') == c ||  unsigned('\t') == c;
    }
  
  template<typename possible_char_type,
    typename = std::enable_if_t<strconcept::is_char_type_v<possible_char_type>>>
  [[nodiscard]]
  constexpr bool isspace_( possible_char_type src )
    {
    unsigned c{ static_cast<unsigned>(src) };
    return isblank_(src) || unsigned('\n') == c || unsigned('\r') == c || unsigned('\v') == c || unsigned('\f') == c;
    }

  template<typename possible_char_type,
    typename = std::enable_if_t<strconcept::is_char_type_v<possible_char_type>>>
  [[nodiscard]]
  constexpr bool isdigit_( possible_char_type src )
    {
    unsigned c{ static_cast<unsigned>(src) };
    return c >= 48u && c <= 57u;
    }
    
  inline constexpr unsigned lower_char_begin_ = 97u;
  inline constexpr unsigned upper_char_begin_ = 65u;
  
  template<typename possible_char_type,
    typename = std::enable_if_t<strconcept::is_char_type_v<possible_char_type>>>
  [[nodiscard]]
  constexpr bool islower_( possible_char_type src )
    {
    unsigned c{ static_cast<unsigned>(src) };
    return c >= lower_char_begin_ && c <= 122u;
    }
    
  template<typename possible_char_type,
    typename = std::enable_if_t<strconcept::is_char_type_v<possible_char_type>>>
  [[nodiscard]]
  constexpr bool isxdigit_( possible_char_type src )
    {
    unsigned c{ static_cast<unsigned>(src) };
    return isdigit_(src) || (c >= upper_char_begin_ && c <= 70u) || (c >= lower_char_begin_ && c <= 102u);
    }
    
  template<typename possible_char_type,
    typename = std::enable_if_t<strconcept::is_char_type_v<possible_char_type>>>
  [[nodiscard]]
  constexpr bool isupper_( possible_char_type src )
    {
    unsigned c{ static_cast<unsigned>(src) };
    return c >= upper_char_begin_ && c <= 90u;
    }
    
  //--------------------------------------------------------------------------------------------------------
  
  template<typename possible_char_type,
    typename = std::enable_if_t<strconcept::is_char_type_v<possible_char_type>>>
  [[nodiscard]]
  constexpr auto to_lower_( possible_char_type c )
    {
    if( isupper_(c) )
      return static_cast<possible_char_type>( static_cast<unsigned>(c) - upper_char_begin_ + lower_char_begin_  );
    return c;
    }

  //--------------------------------------------------------------------------------------------------------
  
  template<typename possible_char_type,
    typename = std::enable_if_t<strconcept::is_char_type_v<possible_char_type>>>
  [[nodiscard]]
  constexpr auto to_upper_( possible_char_type c )
    {
    if( islower_(c) )
      return static_cast<possible_char_type>( static_cast<unsigned>(c) - lower_char_begin_ + upper_char_begin_ );
    return c;
    }
  //--------------------------------------------------------------------------------------------------------
  ///\brief same as stl one except that it is available as constexpr with c++17
  template <class forward_iterator, class predicate,
    typename = std::enable_if_t<strconcept::is_forward_iterator_v<forward_iterator>>>
  [[nodiscard]]
  constexpr forward_iterator find_if(forward_iterator first, const forward_iterator last, predicate pred)
    {
    for (; first != last; ++first) 
      if (pred(*first)) 
            break;

    return first;
    }
    
  //--------------------------------------------------------------------------------------------------------
  template <typename forward_iterator1, typename forward_iterator2,
    typename = std::enable_if_t<strconcept::is_forward_iterator_v<forward_iterator1>
                             && strconcept::is_forward_iterator_v<forward_iterator2>>
    >
  [[nodiscard]]
  constexpr bool equal(forward_iterator1 first, const forward_iterator1 last, forward_iterator2 first2 )
    {
    for (; first != last; ++first, ++first2) 
      if( *first != *first2 )
        return false;
    return true;
    }
    
  //--------------------------------------------------------------------------------------------------------
  
  template<typename forward_iterator, typename value_type,
    typename = std::enable_if_t<strconcept::is_forward_iterator_v<forward_iterator>>>
  constexpr forward_iterator find(forward_iterator first, forward_iterator last, value_type const & v)
    {
    for (; first != last; ++first) 
      if(v == (*first)) 
            break;
    return first;
    }
  struct identity_projection
    {
    template<typename input_value_type >
    constexpr auto operator()( input_value_type && value ) const
      { return std::forward<input_value_type>(value); }
    };
  //--------------------------------------------------------------------------------------------------------
    template<typename input_iterator, typename output_iterator, typename unary_operation = identity_projection,
      typename = std::enable_if_t<strconcept::is_input_iterator_v<input_iterator> 
                               && strconcept::is_writable_iterator_v<output_iterator>>
      >
    constexpr auto transform(input_iterator first, input_iterator last, output_iterator result, unary_operation unary_op = {})
      {
      for (; first != last; ++first, ++result)
        *result = std::invoke(unary_op, *first);
      return result;
      }
      
  //--------------------------------------------------------------------------------------------------------
  template<typename unary_operation = identity_projection, typename output_iterator, typename value_type,
      typename = std::enable_if_t<strconcept::is_writable_iterator_v<output_iterator>>
      >
    constexpr auto fill(output_iterator first, output_iterator last, value_type value, unary_operation unary_op = {} )
      {
      for (; first != last; ++first)
        *first = std::invoke(unary_op, value);
      return first;
      }
  //--------------------------------------------------------------------------------------------------------
  struct not_is_space_pred_t
    {
    template<typename char_type>
    constexpr bool operator()(char_type c) const noexcept { return !isspace_(c); }
    };
    
  template<typename char_type>
  struct not_is_char_pred_t
    {
    char_type value;
    constexpr bool operator()(char_type c) const noexcept { return value != c; }
    };
    
  template<typename iterator>
  struct not_is_any_of
    {
    using char_type = strconcept::remove_cvref_t<decltype(*iterator{})>;
    iterator any_of_beg, any_of_end;
    constexpr not_is_any_of( iterator b, iterator e ) : any_of_beg{b}, any_of_end{e} {}
    
    constexpr bool operator()(char_type c) const noexcept
      {
      auto it{ detail::find(any_of_beg, any_of_end, c) };
      return it == any_of_end;
      }
    };
    //ctad to disable warning about not intended ctad
    template<class iterator>
    not_is_any_of(iterator b, iterator e) -> not_is_any_of<iterator>;
  //--------------------------------------------------------------------------------------------------------
  template<typename string_view_type, typename string_view_type2,
    typename = std::enable_if_t<strconcept::view_value_type_equals_v<string_view_type,string_view_type2>>>
  [[nodiscard]]
  constexpr auto find_first_of_( string_view_type const & view, string_view_type2 const & one_of )
    {
    using size_type = typename string_view_type::size_type;
    
    auto it{std::begin(view)};
    for(; it != std::end(view); ++it)
      {
      if ( std::end(one_of) != find( std::begin(one_of),std::end(one_of), *it ) )
        break;
      }
    return it != std::end(view) ? static_cast<size_type>(std::distance(std::begin(view),it)) : string_view_type::npos;
    }
    
  //--------------------------------------------------------------------------------------------------------
  template<typename string_view_type, typename predicate_type>
  constexpr auto trim_left_predicate( string_view_type const & view, predicate_type pred ) noexcept
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    using view_type = std::basic_string_view<char_type>;
    using size_type = typename view_type::size_type;

    auto new_begin { detail::find_if( std::begin(view), std::end(view), pred ) };
    if( new_begin != std::end(view) )
      {
      char_type const * beg{ std::data(view) };
      auto beg_pos { static_cast<size_type>(new_begin - std::begin(view)) };
      return view_type{ beg + beg_pos, view.size() - beg_pos };
      }
    return view_type{};
    }
  //--------------------------------------------------------------------------------------------------------
  template<typename string_view_type, typename predicate_type>
  constexpr auto trim_right_predicate( string_view_type const & view, predicate_type pred ) noexcept
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    using view_type = std::basic_string_view<char_type>;
    using size_type = typename view_type::size_type;

    auto rbegin{ std::make_reverse_iterator(std::end(view)) };
    auto rend{ std::make_reverse_iterator(std::begin(view)) };
    auto new_rbegin { stralgo::detail::find_if( rbegin, rend, pred ) };

    if( new_rbegin != rend )
      {
      char_type const * beg{ std::data(view) };
      auto end_pos { static_cast<size_type>(new_rbegin.base() - std::begin(view)) };
      return view_type{ beg, end_pos };
      }
    return view_type{};
    }
  //--------------------------------------------------------------------------------------------------------
  template<typename string_view_type, typename predicate_type>
  constexpr auto trim_predicate( string_view_type const & view, predicate_type const & pred ) noexcept
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    using view_type = std::basic_string_view<char_type>;
    using size_type = typename view_type::size_type;

    auto new_begin { stralgo::detail::find_if( std::begin(view), view.end(), pred ) };
    auto rbegin{ std::make_reverse_iterator(view.end()) };
    auto rend{ std::make_reverse_iterator(new_begin) };
    auto new_rbegin { stralgo::detail::find_if( rbegin, rend, pred ) };

    if( new_begin != view.end() || new_rbegin != rend )
      {
      auto beg_pos{ new_begin - std::begin(view) };
      auto end_pos{ new_rbegin.base() - std::begin(view) };
      char_type const * ptr{ std::data(view) };
      return view_type{ ptr + beg_pos, static_cast<size_type>(end_pos - beg_pos) };
      }
    return view_type{};
    }
  //--------------------------------------------------------------------------------------------------------
  template<typename string_view_type,
    typename = std::enable_if_t<strconcept::is_convertible_to_string_view_v<string_view_type>>>
  [[nodiscard]]
  constexpr auto substr_( string_view_type const & view,
                         typename string_view_type::size_type pos,
                         typename string_view_type::size_type count = string_view_type::npos ) noexcept
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    using view_type = std::basic_string_view<char_type>;

    if( pos < view.size() )
      {
      count = std::min(count, view.size()-pos);
      char_type const * beg{ std::data(view) + pos };
      return view_type{ beg, count };
      }
    else
      return view_type{};
    }
  //--------------------------------------------------------------------------------------------------------
  template<typename string_view_type,
    typename = std::enable_if_t<strconcept::is_convertible_to_string_view_v<string_view_type>>>
  [[nodiscard]]
  constexpr auto left_( string_view_type const & view, typename string_view_type::size_type count )
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    using view_type = std::basic_string_view<char_type>;
    using size_type = typename string_view_type::size_type;
    size_type const trimed_count{ std::min(count, std::size(view)) };
    char_type const * beg{ std::data(view) };
    return view_type{ beg, trimed_count };
    }
  //--------------------------------------------------------------------------------------------------------
    
  template<typename string_view_type,
    typename = std::enable_if_t<strconcept::is_convertible_to_string_view_v<string_view_type>>>
  [[nodiscard]]
  constexpr auto right_( string_view_type const & view, typename string_view_type::size_type count )
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    using view_type = std::basic_string_view<char_type>;
    using size_type = typename string_view_type::size_type;

    size_type const trimed_count{ std::min(count, std::size(view)) };
    size_type const pos{ static_cast<size_type>(view.size() - trimed_count) };
    char_type const * beg{ std::data(view) + pos };
    return view_type{ beg, trimed_count };
    }
    
  //--------------------------------------------------------------------------------------------------------

  template<typename maybe_char_type,
    typename = std::enable_if_t<strconcept::is_char_type_v<maybe_char_type>>>
  constexpr size_t view_size( maybe_char_type ) noexcept
    { return 1u; }
  
  template<typename string_view_type,
    typename = std::enable_if_t<!strconcept::is_char_type_v<string_view_type> &&
      !std::is_array_v< strconcept::remove_cvref_t<string_view_type>> &&
      !std::is_pointer_v< strconcept::remove_cvref_t<string_view_type>>>
      >
  constexpr size_t view_size( string_view_type const & view ) noexcept
    { return std::size(view); }
    
  template<typename string_view_type, typename ... args_type >
  constexpr auto count_size(string_view_type const & viewl, args_type const & ... args) noexcept
    {
    if constexpr (sizeof...(args_type) != 0)
      return view_size(viewl) + count_size(args...);
    else
      return view_size(viewl);
    }
    
  template<typename iterator, typename string_view_type, typename ... args_type>
  void copy_views( iterator it, string_view_type const & view_or_char_value, args_type const & ... args ) noexcept
    {
    if constexpr (strconcept::is_char_type_v<string_view_type>)
      {
      *it = view_or_char_value;
      ++it;
      if constexpr (sizeof...(args_type) != 0)
        copy_views(it, args ... );
      }
    else
      {
      static_assert( 
        !std::is_array_v< strconcept::remove_cvref_t<string_view_type>> &&
        !std::is_pointer_v< strconcept::remove_cvref_t<string_view_type>>
        );
      iterator it_end { std::copy(std::begin(view_or_char_value),std::end(view_or_char_value), it)};
      if constexpr (sizeof...(args_type) != 0)
        copy_views(it_end, args ... );
      }
    }
    
  template<typename T>
  struct copy_views_t
    {};
  
  template<typename char_type>
  struct copy_views_t<std::basic_string<char_type>>
    {
    using string_type = std::basic_string<char_type>;
    using size_type = typename string_type::size_type;
    
    template<typename copy_type>
    auto operator()( size_type aggregated_size, copy_type const & copy_views) const noexcept
      {
      string_type result;
      result.resize(aggregated_size);
      copy_views( result.data() );
      return result;
      }
    };
    
  template<typename string_type, typename ... string_view_type_n>
  auto merge_impl_( string_view_type_n const & ... args  )
    {
    using size_type = typename string_type::size_type;
    size_type aggregated_size{ static_cast<size_type>(detail::count_size(args...))};
    return copy_views_t<string_type>{}( aggregated_size,
                                        [&args...]( auto out_iterator )
                                        {
                                        detail::copy_views( out_iterator, args...);
                                        } );
    }
    
  template<typename string_view_type, typename ... string_view_type_n>
  inline constexpr bool merge_concepts_ = 
      (sizeof...(string_view_type_n) != 0) //require at least 2 arguemnts
      && //support chars or views 
        (strconcept::is_char_type_v<string_view_type> //support char
          ||
        (!std::is_array_v< strconcept::remove_cvref_t<string_view_type>> &&
        !std::is_pointer_v< strconcept::remove_cvref_t<string_view_type>>) //dissallow C arrays aand pointers
        );
  //--------------------------------------------------------------------------------------------------------

  template<typename fwd_iterator>
  inline constexpr bool merge_range_concepts_ = 
      strconcept::is_convertible_to_string_view_v<decltype(*fwd_iterator{})>
     && (!std::is_array_v< strconcept::remove_cvref_t<decltype(*fwd_iterator{})>> &&
        !std::is_pointer_v< strconcept::remove_cvref_t<decltype(*fwd_iterator{})>>)
     && strconcept::is_forward_iterator_v<fwd_iterator>;
     
  template<typename fwd_iterator, typename string_type,
    typename = std::enable_if_t<detail::merge_range_concepts_<fwd_iterator>>
    >
  auto merge_range_( fwd_iterator itbeg, fwd_iterator itend )
    {
    using size_type = typename string_type::size_type;
    size_type aggregated_size{ std::accumulate(itbeg, itend, size_type{}, 
                                        [](size_type init, auto const & view )
                                        { return init + view.size(); }) };
    return copy_views_t<string_type>{}( aggregated_size,
                                        [itbeg, itend]( auto out_iterator )
                                        {
                                        static_assert( strconcept::is_writable_iterator_v<decltype(out_iterator)> );
                                        for(auto beg{itbeg}; beg !=itend; ++beg)
                                          out_iterator = std::copy(std::begin(*beg),std::end(*beg), out_iterator );
                                        } );
    }
    
  //--------------------------------------------------------------------------------------------------------
  template<typename string_view_type, typename string_view_type2,
    typename = std::enable_if_t<strconcept::view_value_type_equals_v<string_view_type,string_view_type2>>>
  [[nodiscard]]
  constexpr bool ends_with_( string_view_type const & str, string_view_type2 const & other ) noexcept
    {
    auto const str0_sz = std::size(str);
    auto const str1_sz = std::size(other);
    using difference_type = strconcept::iterator_difference_type<decltype(std::end(str))>;
    
    if(str0_sz >= str1_sz)
      return detail::equal(std::begin(other), std::end(other),
                        std::prev(std::end(str), static_cast<difference_type>(str1_sz)) );
    return false;
    }
  //--------------------------------------------------------------------------------------------------------
  template<typename string_view_type, typename string_view_type2,
    typename = std::enable_if_t<strconcept::view_value_type_equals_v<string_view_type,string_view_type2>>>
  [[nodiscard]]
  constexpr bool starts_with_( string_view_type const & str, string_view_type2 const & other ) noexcept
    {
    auto const str0_sz = str.size();
    auto const str1_sz = other.size();
    
    if(str0_sz >= str1_sz)
      return detail::equal(std::begin(other), std::end(other), std::begin(str) );
    return false;
    }
    
  //--------------------------------------------------------------------------------------------------------

  //TODO change to view
  template<typename target_char_type, typename string_view_type, typename function_obj,
    typename = std::enable_if_t<strconcept::is_convertible_to_string_view_v<string_view_type>>>
  auto make_transform_string_view_target( string_view_type const & str, function_obj const & fobj ) noexcept
    {
    using string_type = strconcept::string_by_char_type_t<target_char_type>;
    using char_type = strconcept::string_view_value_type<string_view_type>;

    string_type result;
    result.resize(str.size());
    stralgo::detail::transform(str.begin(), str.end(), result.begin(), [&fobj](char_type c){ return fobj(c); });
    return result;
    }
    
  template<typename string_view_type, typename function_obj,
    typename = std::enable_if_t<strconcept::is_convertible_to_string_view_v<string_view_type>>>
  auto make_transform_string_view( string_view_type const & str, function_obj const & fobj ) noexcept
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    return make_transform_string_view_target<char_type>(str, fobj);
    }
    
  template<typename target_char_type, typename function_obj >
  auto make_transform_string_view( strconcept::string_by_char_type_t<target_char_type> && str, function_obj const & fobj ) noexcept
    {
    using string_type = strconcept::string_by_char_type_t<target_char_type>;
    using char_type = target_char_type;
    using unique_buffer_type = typename string_type::unique_buffer_type;
      {
      auto const size{ str.size() };
      unique_buffer_type buff{str, size, true };
      stralgo::detail::transform(buff.begin(), buff.finish(), buff.begin(), [&fobj](char_type c){ return fobj(c); });
      }
    return std::move(str);
    }
    
  //--------------------------------------------------------------------------------------------------------
  
  template<typename string_view_type, typename string_view_type2,
    typename = std::enable_if_t<strconcept::view_value_type_equals_v<string_view_type,string_view_type2>>>
  [[nodiscard]]
  constexpr int compare_no_case_( string_view_type const & s1, string_view_type2 const & s2 )
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    using traits = std::char_traits<char_type>;
    
    auto s1_size{ std::size(s1) };
    auto s2_size{ std::size(s2) };
    
    size_t count{ std::min(s1_size,s2_size) };
    for (size_t i = 0; i != count; ++i)
      {
      char_type l1{ to_lower_(s1[i]) };
      char_type l2{ to_lower_(s2[i]) };
      if( !traits::eq(l1, l2) )
        return traits::lt(l1, l2) ? -1 : 1;
      }
    if( s1_size != s2_size )
      return s1_size < s2_size ? -1 : 1;
    else
      return 0;
    }
  //--------------------------------------------------------------------------------------------------------
  
  template<typename string_view_type,
    typename = std::enable_if_t<strconcept::is_convertible_to_string_view_v<string_view_type>>>
  constexpr bool is_number_(string_view_type const & str)
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    using view_type = std::basic_string_view<char_type>;
    
    view_type view{ static_cast<view_type>(str) };
    return view.end() == stralgo::detail::find_if( view.begin(), view.end(),
                                                   []( char_type c)
                                                   { return !isdigit_(c); } );
    }
    
  //--------------------------------------------------------------------------------------------------------
  
  template<typename string_view_type,
    typename = std::enable_if_t<strconcept::is_convertible_to_string_view_v<string_view_type>>>
  constexpr bool is_hexnumber(string_view_type const & str)
    {
    using char_type = strconcept::string_view_value_type<string_view_type>;
    using view_type = std::basic_string_view<char_type>;
    
    view_type view{ static_cast<view_type>(str) };
    return view.end() == stralgo::detail::find_if( view.begin(), view.end(),
                                                   []( char_type c)
                                                   { return !isxdigit_(c); } );
    }
}
