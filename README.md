# stralgo ![MIT](https://img.shields.io/badge/license-MIT-blue.svg) ![language](https://img.shields.io/badge/language-C%2B%2B20-blue.svg)
constexpr number &lt;-> string composition, formatting and utf/number conversions with full support of unterminated string views and forward ranges.

## features

This code solves those problems:

* stralgo::utf - fully constant evaluated utf8,16,32 string manipulation with formatters for char format
* formatting information is attached to arguments and prevents mismatching formatting with argument type errors at compile time
* every conversion, composing, formatting is constexpr except allocations
* full support for basic_string_view and forward_ranges with not null terminated strings
* formatting traits can be declared constexpr constant and reused consistently across project
* minimum c++20 compiler required (stl at least c++17)
* fully constexpr string to number and number to string conversion for functions that doesn't allocate (using output iterators)
* extensive number formating with constexpr traits
* merging string_views and composing any data (string_views, numbers) with one variadic template, one allocation
* compose, merge doesn't allow using directly pointers to chars and char tables (usable only with basic_string_view), to disallow working with pointers
* supports std::basic_string and my own small_vectors::basic_string

## installation

* header only, except unit tests

## c++ compilers
At this point of development I work with clang 18,19 and gcc-14

## small_vectors integration
* this header only library integrates with cpm my other repository small_vectors and uses from it fully constexpr small_vectors::basic_string and small_vectors::basic_fixed_string for unit tests

## examples
### stralgo::compose
```C++
    {
    //main feature compose with constexpr formatting except final allocation
  using stralgo::integral_format_traits;
  using stralgo::format_e;
  using stralgo::char_case_e;
  using stralgo::prepend_sign_e;
  using stralgo::padd_with_e;
  using stralgo::include_prefix_e;
  using stralgo::alignment_e;

  constexpr integral_format_traits ptrfmt //reusable formatting traits
    {
    .precision = 6, //minimum precision/size of output text
    .format = format_e::hexadecimal,
    .char_case = char_case_e::uppercase,
    .padd_with = padd_with_e::zeros //fill space with zeros
    };
  //output string value type is deduced as char
  auto strres { 
   stralgo::stl::compose(
    " some view "sv,
    127.3f, //default formatted floating point see float_format_traits{}
    ',', ///single char
    125, //default formatted integral number see integral_format_traits{}
    '[',
    stralgo::fmt<integral_format_traits{
            .precision = 15, //minimum number of characters
            .format = format_e::binary, //output format encoding of digit numbers
            .char_case = char_case_e::lowercase, //char case when using hexadecimal format
            .padd_with = padd_with_e::space, //pad value with zeros or space when precision is higher than value representation
            .sign = prepend_sign_e::only_negative, //prepend sign to result string
            .include_prefix = include_prefix_e::with_prefix, //include prefix when defined for format ex: for hexadecimal 0x
            .alignment = alignment_e::middle //alignment when padding with space
            }>(456), //custom formatted integral number with traits like in integral_to_string
    "] ["sv,
    stralgo::fmt<stralgo::float_format_traits{
                                      .precision = 10,
                                      .decimal_places = 2,
                                      .alignment = stralgo::alignment_e::left,
                                      .trailing_zeros = stralgo::trailing_zeros_e::skip
                                      }>(10.46713), //custom formatted floating point number with traits like in float_to_string
    "] "sv,
    fmt<ptrfmt>(0x1ff56ef001), ' ', //reuse custom constexpr formatting
    fmt<ptrfmt>(0x0)
  ) };
  
  constexpr auto expected{ " some view 127.300003,125[  0b111001000  ] [10.46     ] 0X1FF56EF001 0X0000"sv };
    }
    
    ```
### stralgo::merge

  merge does same as compose except that it i simpler form limited to views and chars
  
```C++
  string_view t1a {"orem ipsum dolor sit amet, consectetur adipiscing elit. "sv}, t2a { }, t3a {"Vestibulum"sv }; char t4a{' '}; string_view t5a{"rutrum leo libero"};
  //stralgo::merge uses my own coll::basic_string_view
  auto resa{ stralgo::stl::merge('L',t1a,t2a,t3a,t4a,t5a)};
  
  constexpr string_view expected{"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum rutrum leo libero"};
  
```
    
### stralgo::integral_to_string

Converting and formatting integral numbers.
Building block of compose that can be use separately

There are 2 variants available
* one returning string
* second constexpr storing result at output iterator returning iterator pass the end last stored char

```C++
    //non constexpr returning string
    using traits = stralgo::integral_format_traits;
    {
    constexpr int64_t value{ -0x1ffeb3ef1ffeb3ll };
    constexpr auto expected{ "  -0x1ffeb3ef1ffeb3   "sv };
    auto result = stralgo::integral_to_string<traits{ .precision = 22,
                                                   .format = format_e::hexadecimal,
                                                   .char_case = char_case_e::lowercase,
                                                   .sign = prepend_sign_e::only_negative,
                                                   .alignment = alignment_e::middle
                                                  }>( value );
                                                  
    static_assert( expected == result );
    }
    //example constexpr use without buffer allocation
    
    constexpr bool test_unsigned_9d()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr int64_t value{ -0b1100110011001100110011001100110011001100110011001100110011001ll };
      constexpr std::string_view expected{ "      -1100110011001100110011001100110011001100110011001100110011001" };
      auto oit = stralgo::integral_to_string<traits{.precision = 68,
                                                    .format = format_e::binary,
                                                    .padd_with = padd_with_e::space,
                                                    .include_prefix = include_prefix_e::no_prefix
                                                    }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_9d() );
    
    ```
### stralgo::string_to_integral

Converting string representation of integral numbers.
Returns converted number value and iterator pass the end of source string view where conversion stopped
Example as constexpr test function

```C++
  using stralgo::string_to_integral;
  using namespace std::string_view_literals;
  using stralgo::input_format_e;
  
  template<typename integral_type,
           input_format_e input_format = input_format_e::undetermined,
           typename expected_type>
  constexpr auto test( std::string_view source, expected_type expected, int end_it_offset )
    {
    
    auto [result,end_it] = string_to_integral<integral_type,input_format>(source);
    
    return std::next(std::begin(source),end_it_offset) == end_it
      && static_cast<integral_type>(expected) == result;
    }
    
  //format determined at function based on prefix
  static_assert( test<uint16_t>("255"sv,255u,3) );
  static_assert( test<uint8_t>(" \t0xff 0xfe"sv,255u,6) );
  static_assert( test<int64_t>("\t \t -0x1aF3423R342fsdv"sv,-0x1af3423,14) );
  
  //format explicitly specified, prefix is not required
  static_assert( test<uint8_t,input_format_e::hexadecimal>(" \t0xff 0xfe"sv,255u,6) );
  static_assert( test<uint8_t,input_format_e::hexadecimal>(" \tff 0xfe"sv,255u,4) );
    ```
    
### stralgo::string_to_float

Converting string representation of float numbers.

```C++
  using stralgo::string_to_float;
  using namespace std::string_view_literals;
  
  template<typename float_type, typename expected_type>
  constexpr auto test( std::string_view source, expected_type expected, int end_it_offset )
    {
    auto [result,end_it] = string_to_float<float_type>(source);
    return std::next(std::begin(source),end_it_offset) == end_it
      && static_cast<float_type>(expected) == result;
    }
    
  static_assert( test<double>("10.1333"sv, 10.1333, 7 ) );
  static_assert( test<double>("+10.1333"sv, 10.1333, 8 ) );
  static_assert( test<double>("-10.1333"sv, -10.1333, 8 ) );
    ```

### stralgo::float_to_string

Converts floating point number into string representation
There are 2 variants available
* one returning string
* second constexpr storing result at output iterator returning iterator pass the end last stored char

Example using output iterator
```C++
    constexpr bool test_float_4a()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 0.5 };
      constexpr std::string_view expected{ "00000.5000" };
      auto oit = stralgo::float_to_string<traits{
                                                .precision = 10,
                                                .decimal_places = 4,
                                                .padd_with = padd_with_e::zeros,
                                                .trailing_zeros = trailing_zeros_e::preserve
                                                }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_float_4a() );
```
#### stralgo::utf convertion namespace

* utf_input_view_t - deducing utf view over range
* deducing utf_forward_iterator_t
* deducing utf_output_iterator_t with typed output iterators
* utf_explicit_output_iterator_t for use with untyped output iterators
* formatters of utf char16_t,char32_t,wchar_t for std::format char
* length - number of code points in range
* capacity_t<char_type> - number of bytes required to encode range into given char type, ie char8_t, char16_t, char32_t, wchar_t ...
* convert - convert range into output iterator with other utf encoding
* to_string_t<char_type> - convert utf range into coll::basic_string<> or std::basic_string<> with other utf encoding
* verify - verification of utf range
```C++
namespace utf = stralgo::utf;

// any range (string string_view, array, vector ...) to output iterator with tpe deduction
constexpr auto view( auto const & a ) noexcept
  { return std::basic_string_view{ std::begin(a), std::end(a)}; }

constexpr std::basic_string_view u8test{u8"𐃆𐃇𐃈𐃉𐃊𐃋𐃌𐃍𐃎𐃏𐃐𐃑𐃒𐃓𐃔"};
constexpr std::basic_string_view u16test{u16"𐃆𐃇𐃈𐃉𐃊𐃋𐃌𐃍𐃎𐃏𐃐𐃑𐃒𐃓𐃔"};
std::array<char16_t, u16test.size()> storage;
utf::convert( u8test, std::begin(storage));
constexpr_test( view(storage) == u16test );

//any range to own string
constexpr_test( utf::to_u16string(u8test) == u16test );

//any range to std string
std::u16string str2 = utf::stl::to_u16string(u8test);
constexpr_test( str2 == u16test );
```

## Performance
Performance comparison of compose vs snprintf for bellow test functions

**Ryzen 9 - 5900X**
compiler | function | stralgo | legacy
---------|----------|---------|-------
clang-16 |  compose | 17 ms   | 42 ms
gcc-12   |  compose | 23 ms   | 42 ms

full implementation performance comparison can be seen in perf/perf.cc

```C++
  //example compose test
  stralgo::compose<char>( 'T',
                         data.example_string, ' ',
                         data.emaple_float,
                         ' ',
                         stralgo::fmt<stralgo::integral_format_traits{
                                 .precision = 11,
                                 .format = format_e::hexadecimal,
                                 .char_case = char_case_e::lowercase,
                                 .padd_with = padd_with_e::zeros,
                                 .include_prefix = include_prefix_e::with_prefix,
                                        }>(data.example_int) );
```
```C++
 //example snprintf test
  char const * const fmt{ "T%s %f %11.11x" };
  int sz = snprintf(nullptr,0,fmt, data.example_string.c_str(), data.emaple_float, data.example_int );
  string result;
  result.resize(static_cast<size_t>(sz+1));
  sz = snprintf(result.data(), result.size(),fmt, data.example_string.c_str(), data.emaple_float, data.example_int );
  result.resize(static_cast<size_t>(sz));
```
### tested compilers

there are predefined cmake workflows to test
* cmake --workflow --preset="clang-19-release"
* cmake --workflow --preset="clang-19-libc++release"
* cmake --workflow --preset="clang-18-release"
* cmake --workflow --preset="clang-18-libc++release"
* cmake --workflow --preset="gcc-14-release"
* cmake --workflow --preset="gcc-13-release"

## Feedback

If you think you have found a bug, please file an issue via issue submission [form](https://github.com/arturbac/stralgo/issues). Include the relevant information to reproduce the bug for example as static_assert( expression ), if it is important also the compiler version and target architecture. Feature requests and contributions can be filed as issues or pull requests.

## License

This library is available to anybody free of charge, under the terms of BSL-1.0 License (see LICENSE.md).
