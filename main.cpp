#include <strconv_numeric.h>
#include <iostream>
#include <ranges_config.h>

using namespace std::literals::string_view_literals;
using std::cout;



template<typename IntegralType>
struct int2str_view 
  : public ranges::view_interface<int2str_view<IntegralType>>
  {
  static_assert(std::is_integral_v<IntegralType>);
  
public:
  using value_type = IntegralType;
  
private:
    struct sentinel;
    
  struct iterator 
    {
    using value_type = IntegralType;
    
  private:
    friend struct sentinel;
    value_type value_;
    value_type divisor_;
    unsigned size_;
    char current_char_;
    
  public:
    using iterator_category = ranges::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    // Extension: legacy typedefs
    using pointer = void;
    using reference = char;

    iterator() = default;

    constexpr explicit iterator(value_type value, unsigned s, value_type d) 
      : value_{value}, divisor_{d}, size_{s}
      {
      eval();
      }
    
    constexpr char operator*() const noexcept
      { return current_char_; }
    
    void eval() noexcept
      {
      value_type decimal { value_ / divisor_ };
      value_ = static_cast<value_type>( value_ - decimal * divisor_);
      divisor_ /= 10;
      current_char_ =  strconv::value_to_hex( static_cast<uint8_t>( decimal ) );
      --size_;
      }
    constexpr iterator& operator++()
      {
      if( size_ != 0 )
        eval();
      else
        current_char_ = '\0';
      return *this;
      }

    constexpr auto operator++(int)
      {
      auto tmp = *this;
      ++*this;
      return tmp;
      }
    };
    
  struct sentinel 
    {
    private:
      static constexpr char bound_ = '\0';

    public:
      sentinel() = default;

    friend constexpr bool operator==(const iterator& i, const sentinel& s)
      { return i.current_char_ == s.bound_; }

    friend constexpr bool operator==(const sentinel& s, const iterator& i)
      { return i == s; }

    friend constexpr bool operator!=(const iterator& i, const sentinel& s)
      { return !(i == s); }

    friend constexpr bool operator!=(const sentinel& s, const iterator& i)
      { return !(i == s); }
    };
private:
  value_type value_;
  unsigned size_;
  value_type divisor_;
  
  void calculate_size()
    {
    size_ = static_cast<unsigned>(1+std::log10(std::max<value_type>(1,std::abs(value_))));
    divisor_ = static_cast<value_type>( std::pow(10,size_-1) );
    }
public:

    constexpr explicit int2str_view(value_type value) 
      : value_(value) 
    {calculate_size();}

    constexpr int2str_view(iterator first, sentinel)
        : int2str_view{ first->value_,first->size_, first->divisor_ } 
      {}
        
    constexpr iterator begin() const
      {
      return iterator{value_,size_,divisor_};
      }
      
    constexpr auto end() const
      {
      return sentinel{};
      }
      
    [[nodiscard]]
    std::size_t size() const noexcept { return size_; }
  };

template <typename I,
            std::enable_if_t<std::is_integral_v<I>, bool> = true>
int2str_view(I) -> int2str_view<I>;

template <typename I>
inline constexpr bool ranges::enable_borrowed_range<int2str_view<I>> = true;

namespace v
{
namespace detail 
{

struct int2str_view_fn {
    template <typename I>
    constexpr auto operator()(I&& value) const
        noexcept(noexcept(int2str_view{std::forward<I>(value)}))
        -> decltype(int2str_view(std::forward<I>(value)))
    {
        return int2str_view(std::forward<I>(value));
    }

    template <typename W, typename Bound>
    constexpr auto operator()(W&& value, Bound&& bound) const
        noexcept(noexcept(int2str_view{std::forward<W>(value), std::forward<Bound>(bound)}))
        -> decltype(int2str_view(std::forward<W>(value), std::forward<Bound>(bound)))
    {
        return int2str_view{std::forward<W>(value), std::forward<Bound>(bound)};
    }
};

} // namespace detail
inline namespace function_objects 
  {
  inline constexpr detail::int2str_view_fn int2str{};
  }
}


template< strconv::integral_format_traits traits = strconv::integral_format_traits{} >
constexpr double bar(double x) 
  {
  if constexpr( traits.padd_with == strconv::padd_with_e::zeros ) 
    return x+ 1;
  return x;
  }
  
using strconv::string_to_float;
using namespace std::string_view_literals;

template<typename float_type, typename expected_type>
constexpr auto test( std::string_view source, expected_type expected, int end_it_offset )
  {
  auto [result,end_it] = string_to_float<float_type>(source);
  auto expit =std::next(std::begin(source),end_it_offset);
  return  expit == end_it
    && static_cast<float_type>(expected) == result;
  }
  
int main(int , char **) 
  {
  auto tres = test<float>(""sv, 0.f, 0 );
  
    
  auto res{ strconv::detail::compose_preprocess<char>('a',"sv"sv, 127.3f, 125)};
  
  using traits = strconv::float_format_traits;
  using strconv::format_e;
  using strconv::char_case_e;
  using strconv::prepend_sign_e;
  using strconv::padd_with_e;
  using strconv::include_prefix_e;
  using strconv::alignment_e;
  using strconv::trailing_zeros_e;
  
  auto strres{ 
    strconv::compose<char>(
      " sv "sv,
      127.3f,
      ',',
      125,
      '[',
      strconv::fmt<strconv::integral_format_traits{
              .precision = 10,
              .format = format_e::hexadecimal,
              .char_case = char_case_e::lowercase,
              .alignment = alignment_e::middle
              }>(456),
      "] ["sv,
      strconv::fmt<strconv::float_format_traits{
                                                .precision = 10,
                                                .decimal_places = 2,
                                                .alignment = alignment_e::left,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>(10.46713),
      ']'
    ) };
  std::cout << strres << std::endl;
  bar<strconv::integral_format_traits{
      .precision = 10,
      .format = strconv::format_e::hexadecimal,
      .char_case = strconv::char_case_e::uppercase
    }>(3);


  
  constexpr auto hex_lower = traits{  .char_case = char_case_e::lowercase };
  constexpr auto hex_upper = traits{ .char_case = char_case_e::uppercase };
  {
      constexpr double value{ 0.0 };
      //constexpr std::string_view expected{ "0x0.0000" };
      auto est_info = strconv::detail::estimate_float_to_string_<traits{
                                                .precision = 1,
                                                .decimal_places = 4,
             
                                                .trailing_zeros = trailing_zeros_e::preserve
                                                }>( value);
  std::cout << est_info.size() << std::endl;
  }
      {

      char buffer_[128]{};
      auto itbeg{ &buffer_[0] };
      
     constexpr double value{ 0.5 };
      constexpr std::string_view expected{ "       0.5" };
      auto oit = strconv::float_to_string<traits{
                                                .precision = 10,
                                                .decimal_places = 4,
                                                .alignment = alignment_e::left,
                                                .trailing_zeros = trailing_zeros_e::skip
                                                }>( value, itbeg );
                                                
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };

      std::cout << result << std::endl;
      }
  auto view1 = v::int2str(124);
  
  for (char i : view1 )
    std::cout << i << ' ';
  std::cout  << std::endl;
  
  std::vector<char> vec;
  vec.resize(view1.size());

  ranges::copy( view1.begin(), view1.end(), vec.begin() );
  
  auto fix_char{ [](auto c){ return c-1;} };
  ranges::copy(view1 | views::transform(fix_char), vec.begin() );
  
  for (char i : vec )
    std::cout << i << ' ';
  std::cout  << std::endl;
  
  using type = std::iterator_traits<char *>::iterator_category;
  
  std::string_view s1{"12"}, s2{"fg"};
  auto v1 = views::all(s1);
  auto v2 = views::all(s2);
//   auto v3 = views::single('A');
  auto bits  = {v1,v2};
  auto x = bits | views::join;
//   auto v{ ranges::join_view(v1,v2);
//   std::string res{ strconv::compose('T', " tr "sv, 1235, ':', 345.5)};
//   cout << res;
//   std::to_string(45.4);
  return 0;
  }
