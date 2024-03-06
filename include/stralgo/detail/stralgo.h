#pragma once
#include <stralgo/strconcept.h>
#include <small_vectors/ranges/accumulate.h>
#include <small_vectors/basic_string.h>
#include <functional>
#include <numeric>
#include <algorithm>
#include <ranges>

namespace stralgo::detail
  {
#if (defined(_LIBCPP_VERSION) && _LIBCPP_VERSION >= 12000) || (defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE > 10)
#define STRALGO_USE_STD_INVOKE
#endif
#if defined(STRALGO_USE_STD_INVOKE)
template<typename F, typename... Args>
constexpr auto invoke(F && f, Args &&... args) noexcept(std::is_nothrow_invocable_v<F, Args...>)
  {
  return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
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
  stralgo_static_call_operator constexpr bool
    operator()(char_type src) stralgo_static_call_operator_const noexcept
    {
    unsigned c{static_cast<unsigned>(src)};
    return unsigned(' ') == c || unsigned('\t') == c;
    }
  };

inline constexpr isblank_t isblank;

///\brief match https://en.cppreference.com/w/cpp/string/byte/isspace but is resistant to signed char UB
struct isspace_t
  {
  [[nodiscard]]
  stralgo_static_call_operator constexpr bool
    operator()(concepts::char_type auto src) stralgo_static_call_operator_const noexcept
    {
    unsigned c{static_cast<unsigned>(src)};
    return isblank(src) || unsigned('\n') == c || unsigned('\r') == c || unsigned('\v') == c || unsigned('\f') == c;
    }
  };

inline constexpr isspace_t isspace;

///\brief match https://en.cppreference.com/w/cpp/string/byte/isdigit but is resistant to signed char UB
struct isdigit_t
  {
  [[nodiscard]]
  stralgo_static_call_operator constexpr bool
    operator()(concepts::char_type auto src) stralgo_static_call_operator_const noexcept
    {
    unsigned c{static_cast<unsigned>(src)};
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
  stralgo_static_call_operator constexpr bool
    operator()(concepts::char_type auto src) stralgo_static_call_operator_const noexcept
    {
    unsigned c{static_cast<unsigned>(src)};
    return c >= lower_char_begin_ && c <= 122u;
    }
  };

inline constexpr islower_t islower;

///\brief match https://en.cppreference.com/w/cpp/string/byte/isxdigit but is resistant to signed char UB
struct isxdigit_t
  {
  [[nodiscard]]
  stralgo_static_call_operator constexpr bool
    operator()(concepts::char_type auto src) stralgo_static_call_operator_const noexcept
    {
    unsigned c{static_cast<unsigned>(src)};
    return isdigit(src) || (c >= upper_char_begin_ && c <= 70u) || (c >= lower_char_begin_ && c <= 102u);
    }
  };

inline constexpr isxdigit_t isxdigit;

///\brief match https://en.cppreference.com/w/cpp/string/byte/islower but is resistant to signed char UB
struct isupper_t
  {
  [[nodiscard]]
  stralgo_static_call_operator constexpr bool
    operator()(concepts::char_type auto src) stralgo_static_call_operator_const noexcept
    {
    unsigned c{static_cast<unsigned>(src)};
    return c >= upper_char_begin_ && c <= 90u;
    }
  };

inline constexpr isupper_t isupper;

//--------------------------------------------------------------------------------------------------------
struct to_lower_t
  {
  template<concepts::char_type char_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(char_type c) stralgo_static_call_operator_const noexcept -> char_type
    {
    if(isupper(c))
      return static_cast<char_type>(static_cast<unsigned>(c) - upper_char_begin_ + lower_char_begin_);
    return c;
    }
  };

inline constexpr to_lower_t to_lower;

//--------------------------------------------------------------------------------------------------------
struct to_upper_t
  {
  template<concepts::char_type char_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(char_type c) stralgo_static_call_operator_const noexcept -> char_type
    {
    if(islower(c))
      return static_cast<char_type>(static_cast<unsigned>(c) - lower_char_begin_ + upper_char_begin_);
    return c;
    }
  };

inline constexpr to_upper_t to_upper;

//--------------------------------------------------------------------------------------------------------
struct rewind_iterator_t
  {
  template<std::random_access_iterator iterator, std::sentinel_for<iterator> sentinel>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(iterator first, sentinel last, std::size_t pos) stralgo_static_call_operator_const noexcept -> iterator
    {
    using difference_type = std::iter_difference_t<iterator>;
    auto count{std::min<std::size_t>(static_cast<std::size_t>(std::distance(first, last)), pos)};
    std::ranges::advance(first, static_cast<difference_type>(count));
    return first;
    }

  template<std::forward_iterator iterator, std::sentinel_for<iterator> sentinel>
    requires(!std::random_access_iterator<iterator>)
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(iterator first, sentinel last, std::size_t pos) stralgo_static_call_operator_const noexcept -> iterator
    {
    while(pos != 0u && first != last)
      {
      --pos;
      std::ranges::advance(first, 1);
      }
    return first;
    }
  };

inline constexpr rewind_iterator_t rewind_iterator;

//--------------------------------------------------------------------------------------------------------
struct identity_projection_t
  {
  template<typename input_value_type>
  stralgo_static_call_operator constexpr auto operator()(input_value_type && value) stralgo_static_call_operator_const
    noexcept(noexcept(std::forward<input_value_type>(value))) -> input_value_type &&
    {
    return std::forward<input_value_type>(value);
    }
  };

inline constexpr identity_projection_t identity_projection;

//--------------------------------------------------------------------------------------------------------
struct transform_t
  {
  template<
    std::forward_iterator input_iterator,
    std::sentinel_for<input_iterator> sentinel,
    typename output_iterator,
    typename unary_operation = identity_projection_t>
  stralgo_static_call_operator constexpr auto operator()(
    input_iterator first, sentinel last, output_iterator result, unary_operation unary_op = {}
  ) stralgo_static_call_operator_const noexcept(noexcept(invoke(unary_op, *first))) -> output_iterator
    {
    for(; first != last; ranges::advance(first, 1), ranges::advance(result, 1))
      *result = invoke(unary_op, *first);
    return result;
    }
  };

inline constexpr transform_t transform;

//--------------------------------------------------------------------------------------------------------
struct fill_t
  {
  template<
    typename unary_operation = identity_projection_t,
    std::forward_iterator output_iterator,
    std::sentinel_for<output_iterator> sentinel,
    typename value_type>
  stralgo_static_call_operator constexpr auto operator()(
    output_iterator first, sentinel last, value_type value, unary_operation unary_op = {}
  ) stralgo_static_call_operator_const noexcept(noexcept(invoke(unary_op, *first)))
    {
    for(; first != last; ranges::advance(first, 1))
      *first = invoke(unary_op, value);
    return first;
    }
  };

inline constexpr fill_t fill;

//--------------------------------------------------------------------------------------------------------
struct not_is_space_pred_t
  {
  [[nodiscard]]
  stralgo_static_call_operator constexpr bool
    operator()(concepts::char_type auto c) stralgo_static_call_operator_const noexcept
    {
    return !isspace(c);
    }
  };

inline constexpr not_is_space_pred_t not_is_space;

template<concepts::char_type char_type>
struct not_is_char_pred_t
  {
  char_type value;

  [[nodiscard]]
  constexpr bool
    operator()(char_type c) const noexcept
    {
    return value != c;
    }
  };

template<concepts::char_iterator iterator>
struct not_is_any_of
  {
  using char_type = std::iter_value_t<iterator>;
  iterator any_of_beg, any_of_end;

  constexpr not_is_any_of(iterator b, iterator e) noexcept : any_of_beg{b}, any_of_end{e} {}

  [[nodiscard]]
  constexpr bool
    operator()(char_type c) const noexcept
    {
    return any_of_end == ranges::find(any_of_beg, any_of_end, c);
    }
  };

// ctad to disable warning about not intended ctad
template<concepts::char_iterator iterator>
not_is_any_of(iterator b, iterator e) -> not_is_any_of<iterator>;

//--------------------------------------------------------------------------------------------------------
struct find_first_of_t
  {
  ///\returns iterator to the first occurrence in view of any of the characters that are part of one_of, or end if there
  ///are no matches.
  template<concepts::char_range string_view_type, concepts::char_range string_view_type2>
    requires concepts::same_range_type<string_view_type, string_view_type2>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(string_view_type const & view, string_view_type2 const & one_of)
      stralgo_static_call_operator_const noexcept
    {
    return ranges::find_if(
      view, [&one_of](auto cp) noexcept { return ranges::end(one_of) != ranges::find(one_of, cp); }
    );
    }
  };

inline constexpr find_first_of_t find_first_of;

//--------------------------------------------------------------------------------------------------------
struct trim_left_with_pred_t
  {
  template<concepts::char_range string_view_type, typename predicate_type>
  stralgo_static_call_operator constexpr auto
    operator()(string_view_type const & view, predicate_type const & pred) stralgo_static_call_operator_const noexcept
    {
    auto first{ranges::find_if(view, pred)};
    auto last{ranges::end(view)};
    if constexpr(concepts::char_contiguous_range<string_view_type>)
      return std::basic_string_view{first, last};
    else
      return ranges::subrange(first, last);
    }
  };

inline constexpr trim_left_with_pred_t trim_left_with_pred;

//--------------------------------------------------------------------------------------------------------
struct trim_right_with_pred_t
  {
  template<concepts::char_range string_view_type, typename predicate_type>
  stralgo_static_call_operator constexpr auto
    operator()(string_view_type const & view, predicate_type const & pred) stralgo_static_call_operator_const noexcept
    {
#if defined(__clang__) && !defined(_LIBCPP_STD_VER) && __clang_major__ <= 15
    // clang 15 with libstdc++ has broken unusable views
    auto last{ranges::find_if(
                std::make_reverse_iterator(ranges::end(view)), std::make_reverse_iterator(ranges::begin(view)), pred
    )
                .base()};
#else
    auto last{ranges::find_if(ranges::reverse_view{view}, pred).base()};
#endif
    if constexpr(concepts::char_contiguous_range<string_view_type>)
      return std::basic_string_view{ranges::begin(view), last};
    else
      return ranges::subrange{ranges::begin(view), last};
    }
  };

inline constexpr trim_right_with_pred_t trim_right_with_pred;

//--------------------------------------------------------------------------------------------------------
struct trim_pred_t
  {
  template<concepts::char_range string_view_type, typename predicate_type>
  stralgo_static_call_operator constexpr auto
    operator()(string_view_type const & view, predicate_type const & pred) stralgo_static_call_operator_const noexcept
    {
    return trim_left_with_pred(trim_right_with_pred(view, pred), pred);
    }
  };

inline constexpr trim_pred_t trim_pred;
//--------------------------------------------------------------------------------------------------------
inline constexpr auto npos = std::numeric_limits<std::size_t>::max();

//--------------------------------------------------------------------------------------------------------
struct substr_t
  {
  template<concepts::char_iterator iterator, std::sentinel_for<iterator> sentinel>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(iterator first, sentinel last, std::size_t pos, std::size_t count = npos)
      stralgo_static_call_operator_const noexcept
    {
    first = rewind_iterator(first, last, pos);
    last = rewind_iterator(first, last, count);
    if constexpr(std::contiguous_iterator<iterator>)
      return std::basic_string_view{first, last};
    else
      return ranges::subrange{first, last};
    }

  template<concepts::char_range string_view_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(string_view_type const & view, std::size_t pos, std::size_t count = npos)
      stralgo_static_call_operator_const noexcept
    {
    return operator()(ranges::begin(view), ranges::end(view), pos, count);
    }
  };

inline constexpr substr_t substr;

//--------------------------------------------------------------------------------------------------------
struct left_t
  {
  template<concepts::char_iterator iterator, std::sentinel_for<iterator> sentinel>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(iterator first, sentinel last, std::size_t count) stralgo_static_call_operator_const noexcept
    {
    last = rewind_iterator(first, last, count);
    if constexpr(std::contiguous_iterator<iterator>)
      return std::basic_string_view{first, last};
    else
      return ranges::subrange{first, last};
    }

  template<concepts::char_range string_view_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(string_view_type const & view, std::size_t count) stralgo_static_call_operator_const noexcept
    {
    return operator()(ranges::begin(view), ranges::end(view), count);
    }
  };

inline constexpr left_t left;

//--------------------------------------------------------------------------------------------------------
struct right_t
  {
  template<concepts::char_iterator iterator, std::sentinel_for<iterator> sentinel>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(iterator first, sentinel last, std::size_t count) stralgo_static_call_operator_const noexcept
    {
    auto size{static_cast<std::size_t>(ranges::distance(first, last))};
    std::size_t const trimed_count{std::min(count, size)};
    std::size_t const pos{size - trimed_count};
    first = rewind_iterator(first, last, pos);
    if constexpr(std::contiguous_iterator<iterator>)
      return std::basic_string_view{first, last};
    else
      return ranges::subrange{first, last};
    }

  template<concepts::char_range string_view_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(string_view_type const & view, std::size_t count) stralgo_static_call_operator_const noexcept
    {
    return operator()(ranges::begin(view), ranges::end(view), count);
    }
  };

inline constexpr right_t right;

//--------------------------------------------------------------------------------------------------------

template<concepts::char_type maybe_char_type>
constexpr size_t view_size(maybe_char_type) noexcept
  {
  return 1u;
  }

template<concepts::char_range string_view_type>
  requires(!concepts::char_type<string_view_type>)
constexpr size_t view_size(string_view_type const & view) noexcept
  {
  return ranges::size(view);
  }

template<typename string_view_type, typename... args_type>
constexpr auto count_size(string_view_type const & viewl, args_type const &... args) noexcept
  {
  if constexpr(sizeof...(args_type) != 0)
    return view_size(viewl) + count_size(args...);
  else
    return view_size(viewl);
  }

template<typename T>
struct copy_view_data_t
  {
  };

template<concepts::char_type char_type>
struct copy_view_data_t<char_type>
  {
  template<concepts::char_output_iterator<char_type> iterator, typename... args_type>
  stralgo_static_call_operator constexpr iterator operator()(
    iterator it, char_type const & char_value, args_type const &... args
  ) stralgo_static_call_operator_const noexcept
    {
    *it = char_value;
    ranges::advance(it, 1);
    if constexpr(sizeof...(args_type) != 0)
      {
      using first_arg_type = typename concepts::unpack_first<args_type...>::type;
      it = copy_view_data_t<first_arg_type>{}(std::move(it), args...);
      }
    return it;
    }
  };

template<concepts::char_range char_range>
struct copy_view_data_t<char_range>
  {
  using char_type = std::ranges::range_value_t<char_range>;

  template<concepts::char_output_iterator<char_type> iterator, typename... args_type>
  stralgo_static_call_operator constexpr iterator operator()(
    iterator it, char_range const & view, args_type const &... args
  ) stralgo_static_call_operator_const noexcept
    {
    it = ranges::copy(view, it).out;
    if constexpr(sizeof...(args_type) != 0)
      {
      using first_arg_type = typename concepts::unpack_first<args_type...>::type;
      it = copy_view_data_t<first_arg_type>{}(std::move(it), args...);
      }
    return it;
    }
  };

template<template<typename> typename basic_string_type>
struct string_supports_resize_and_overwrite_t
  {
  static constexpr bool value = false;
  };

template<template<typename> typename basic_string_type>
inline constexpr bool supports_resize_and_overwrite_v
  = string_supports_resize_and_overwrite_t<basic_string_type>::value;

template<typename string_type, bool supports_resize_and_overwrite>
struct copy_views_t
  {
  using size_type = typename string_type::size_type;

  template<typename copy_type>
  stralgo_static_call_operator constexpr auto
    operator()(size_type aggregated_size, copy_type const & copy_views_op) stralgo_static_call_operator_const noexcept
    {
    string_type result;
    if constexpr(supports_resize_and_overwrite)
      result.resize_and_overwrite(
        aggregated_size,
        [&copy_views_op](auto * data, size_type)
        {
          auto oit{copy_views_op(data)};
          return static_cast<size_type>(ranges::distance(data, oit));
        }
      );
    else
      {
      result.resize(aggregated_size);
      auto oit{copy_views_op(result.data())};
      result.resize(static_cast<size_type>(ranges::distance(result.data(), oit)));
      }
    return result;
    }
  };

//--------------------------------------------------------------------------------------------------------

template<>
struct string_supports_resize_and_overwrite_t<small_vectors::basic_string>
  {
  static constexpr bool value = true;
  };

template<typename T, typename... args_n>
concept merge_concepts = requires {
  // requires (sizeof...(args_n) > 0); //require at least 2 arguments
  // support chars or views
  requires concepts::char_type<T> || concepts::char_range<T>;
};

template<template<typename> typename basic_string_type>
struct merge_t
  {
  ///\brief merges many strings views or chars into one with single string buffer allocation
  template<merge_concepts... string_view_or_char_n>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(string_view_or_char_n const &... args) stralgo_static_call_operator_const
    {
    using first_arg_type = typename concepts::unpack_first<string_view_or_char_n...>::type;
    using char_type = concepts::char_type_from_view_t<first_arg_type>;

    using string_type = basic_string_type<char_type>;
    using size_type = typename string_type::size_type;
    auto const aggregated_size{static_cast<size_type>(detail::count_size(args...))};
    constexpr bool supports_resize_and_overwrite = supports_resize_and_overwrite_v<basic_string_type>;
    return copy_views_t<string_type, supports_resize_and_overwrite>{
    }(aggregated_size,
      [&args...](auto out_iterator) noexcept
      {
        // return detail::copy_views( out_iterator, args...);
        return copy_view_data_t<first_arg_type>{}(std::move(out_iterator), args...);
      });
    }
  };

inline constexpr merge_t<small_vectors::basic_string> merge;

namespace stl
  {
  template<concepts::char_type char_type>
  using basic_string = std::basic_string<char_type>;
  inline constexpr merge_t<basic_string> merge;
  }  // namespace stl

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
template<template<typename> typename basic_string_type>
struct merge_range_t
  {
  ///\brief merges range of string view with one buffer allocation
  template<concepts::iterator_over_char_ranges fwd_iterator, std::sentinel_for<fwd_iterator> sentinel>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(fwd_iterator itbeg, sentinel itend) stralgo_static_call_operator_const
    {
    using range_type = std::iter_value_t<fwd_iterator>;
    using char_type = ranges::range_value_t<range_type>;
    using string_type = basic_string_type<char_type>;
    using size_type = typename string_type::size_type;
    constexpr bool supports_resize_and_overwrite = supports_resize_and_overwrite_v<basic_string_type>;
    size_type aggregated_size{small_vectors::ranges::accumulate(
      itbeg,
      itend,
      size_type{},
      [](size_type init, auto const & view) noexcept -> size_type
      { return init + static_cast<size_type>(ranges::size(view)); }
    )};
    return copy_views_t<string_type, supports_resize_and_overwrite>{
    }(aggregated_size,
      [itbeg, itend](auto out_iterator)
      {
        for(auto beg{itbeg}; beg != itend; ranges::advance(beg, 1))
          out_iterator = std::copy(std::ranges::begin(*beg), ranges::end(*beg), out_iterator);
        return out_iterator;
      });
    }

  template<ranges::forward_range forward_range>
  [[nodiscard]]
  stralgo_static_call_operator constexpr auto
    operator()(forward_range const & range) stralgo_static_call_operator_const
    {
    return operator()(ranges::begin(range), ranges::end(range));
    }
  };

inline constexpr merge_range_t<small_vectors::basic_string> merge_range;

namespace stl
  {
  inline constexpr merge_range_t<basic_string> merge_range;
  }

//--------------------------------------------------------------------------------------------------------
struct ends_with_t
  {
  template<concepts::char_range string_view_type, concepts::char_range string_view_type2>
    requires concepts::same_range_type<string_view_type, string_view_type2>
  [[nodiscard]]
  stralgo_static_call_operator constexpr bool
    operator()(string_view_type const & str, string_view_type2 const & other)
      stralgo_static_call_operator_const noexcept
    {
    auto const str0_sz = ranges::size(str);
    auto const str1_sz = ranges::size(other);
    using difference_type = ranges::range_difference_t<string_view_type>;
    using ranges::begin;
    using ranges::end;

    if(str0_sz >= str1_sz) [[likely]]
      return ranges::equal(
        begin(other), end(other), ranges::prev(end(str), static_cast<difference_type>(str1_sz)), end(str)
      );
    return false;
    }
  };

inline constexpr ends_with_t ends_with;

//--------------------------------------------------------------------------------------------------------
struct starts_with_t
  {
  template<concepts::char_range string_view_type, concepts::char_range string_view_type2>
    requires concepts::same_range_type<string_view_type, string_view_type2>
  [[nodiscard]]
  stralgo_static_call_operator constexpr bool
    operator()(string_view_type const & str, string_view_type2 const & other)
      stralgo_static_call_operator_const noexcept
    {
    using ranges::begin;
    using ranges::size;

    auto const str0_sz = size(str);
    auto const str1_sz = size(other);
    using difference_type = ranges::range_difference_t<string_view_type>;
    return str0_sz >= str1_sz
           && ranges::equal(
             begin(other),
             ranges::end(other),
             begin(str),
             ranges::next(begin(str), static_cast<difference_type>(str1_sz))
           );
    }
  };

inline constexpr starts_with_t starts_with;

//--------------------------------------------------------------------------------------------------------
struct compare_no_case_t
  {
  template<concepts::char_range string_view_type, concepts::char_range string_view_type2>
    requires concepts::same_range_type<string_view_type, string_view_type2>
  [[nodiscard]]
  stralgo_static_call_operator constexpr int
    operator()(string_view_type const & s1, string_view_type2 const & s2) stralgo_static_call_operator_const noexcept
    {
    using difference_type = ranges::range_difference_t<string_view_type>;
    using char_type = ranges::range_value_t<string_view_type>;
    using traits = std::char_traits<char_type>;

    auto const s1_size{ranges::size(s1)};
    auto const s2_size{ranges::size(s2)};

    auto const cmp_count{std::min(s1_size, s2_size)};
    auto first{ranges::begin(s1)};
    auto const last{ranges::next(first, static_cast<difference_type>(cmp_count))};

    for(auto first2{ranges::begin(s2)}; first != last;)
      {
      auto const l1{to_lower(*first)};
      auto const l2{to_lower(*first2)};
      if(!traits::eq(l1, l2))
        return traits::lt(l1, l2) ? -1 : 1;
      ranges::advance(first, 1);
      ranges::advance(first2, 1);
      }
    if(s1_size != s2_size)
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
  stralgo_static_call_operator constexpr bool
    operator()(string_view_type const & str) stralgo_static_call_operator_const noexcept
    {
    using char_type = ranges::range_value_t<string_view_type>;
    return ranges::end(str) == ranges::find_if(str, [](char_type c) noexcept { return !isdigit(c); });
    }
  };

inline constexpr is_number_t is_number;

//--------------------------------------------------------------------------------------------------------
struct is_hexnumber_t
  {
  template<concepts::char_range string_view_type>
  [[nodiscard]]
  stralgo_static_call_operator constexpr bool
    operator()(string_view_type const & str) stralgo_static_call_operator_const noexcept
    {
    using char_type = ranges::range_value_t<string_view_type>;
    return ranges::end(str) == ranges::find_if(str, [](char_type c) noexcept { return !isxdigit(c); });
    }
  };

inline constexpr is_hexnumber_t is_hexnumber;
  }  // namespace stralgo::detail
