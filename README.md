# stralgo
constexpr number &lt;-> string composition, formating and convertions with full support of unterminated string views.
I started my jurney with C++ in the 2004 with compilers like embedded visual c++ 4, gcc 2.95. Thru years of participation in big projects i have had to fix many errors that source was in string formating like vnprintf.

The source of problems i've many time found was:
* there was no consistency between formating string and arguments, that was source of errors detected at runtime.
* string composition was always done at runtime even if some operations and arguments were known and could be used at compile time
* all that C library functions require null terminated C strings
* formating information for example in vsnprintf could not be easly shared/reused across project.
* working on pointers to null terminated strings often causes a lot of redudant strlen and dangling pointer errors

## features

This code solves thise problems:

* foramting information is attached to arguments and prevents mismatching formating with argument type errors at compile time
* every convertion, composing, formatting is constexpr except allocations
* full suport for std::string_view with not null terminated strings
* formating traits can be declared constexpr constant and reused consistently across project
* minimum c++20 compiler required (in development at this point constexpr unit tests require c++20 and std::is_constant_evaluated() required for float size calculation)
* fully constexpr string to number and number to string convetions for functions that doesn't allocate (using output interators)
* extensive number formating with constexpr traits
* merging string_views and composing any data (string_views, numbers) with one variadic template, one allocation
* compose, merge doesn't allow using directly pointers to chars and char tables (usable only with std::basic_string_view), to dissallow working with pointers

## installation

* header only, except some of unit tests

## c++ compilers
At this point of early development I work with clang 12.
I'm planing checking and porting to gcc and msvc in near future.

* clang version 12.0.1 x86_64-portbld-freebsd13.0 with libcpp 11


## examples
### strconv::compose
```C++
    {
    //main feature compose with constexpr formating except final allocation
  using strconv::integral_format_traits;
  using strconv::format_e;
  using strconv::char_case_e;
  using strconv::prepend_sign_e;
  using strconv::padd_with_e;
  using strconv::include_prefix_e;
  using strconv::alignment_e;

  constexpr integral_format_traits ptrfmt //reusable formating traits
    {
    .precision = 6, //minimum precision/size of output text
    .format = format_e::hexadecimal,
    .char_case = char_case_e::uppercase,
    .padd_with = padd_with_e::zeros //fill space with zeros
    };
    
  auto strres { 
   strconv::compose<char>(
    " some view "sv,
    127.3f, //default formatted floating point see float_format_traits{}
    ',', ///single char
    125, //default formatted integral number see integral_format_traits{}
    '[',
    strconv::fmt<integral_format_traits{
            .precision = 15, //minimum number of characters
            .format = format_e::binary, //output format encoding of digit numbers
            .char_case = char_case_e::lowercase, //char case whenusing hexadecimal format
            .padd_with = padd_with_e::space, //pad value with zeros or space when precision is higher than value representation
            .sign = prepend_sign_e::only_negative, //prepend sign to result string
            .include_prefix = include_prefix_e::with_prefix, //include prefix when defined for format ex: for hexadecimal 0x
            .alignment = alignment_e::middle //alignment when padding with space
            }>(456), //custom formatted integral number with traits like in integral_to_string
    "] ["sv,
    strconv::fmt<strconv::float_format_traits{
                                      .precision = 10,
                                      .decimal_places = 2,
                                      .alignment = strconv::alignment_e::left,
                                      .trailing_zeros = strconv::trailing_zeros_e::skip
                                      }>(10.46713), //custom formatted floating point number with traits like in float_to_string
    "] "sv,
    fmt<ptrfmt>(0x1ff56ef001), ' ', //reuse custom constexpr formatting
    fmt<ptrfmt>(0x0)
  ) };
  
  constexpr auto expected{ " some view 127.300003,125[  0b111001000  ] [10.46     ] 0X1FF56EF001 0X0000"sv };
    }
    
    ```
### stralgo::merge

  merge doeas same as compose except that it i simpler form limited to views and chars
  
```C++
  string_view t1a {"orem ipsum dolor sit amet, consectetur adipiscing elit. "sv}, t2a { }, t3a {"Vestibulum"sv }; char t4a{' '}; string_view t5a{"rutrum leo libero"};
  auto resa{ stralgo::merge('L',t1a,t2a,t3a,t4a,t5a)};
  
  constexpr string_view expected{"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum rutrum leo libero"};
  
```
    
### strconv::integral_to_string

Converting and formating integral numbers.
Building block of compose that can be use separatly

There are 2 variants available
* one returning string
* second cosntexpr storing result at output iterator returning iterator pass the end last stored char

```C++
    //non constexpr returning string
    using traits = strconv::integral_format_traits;
    {
    constexpr int64_t value{ -0x1ffeb3ef1ffeb3ll };
    constexpr auto expected{ "  -0x1ffeb3ef1ffeb3   "sv };
    auto result = strconv::integral_to_string<traits{ .precision = 22,
                                                   .format = format_e::hexadecimal,
                                                   .char_case = char_case_e::lowercase,
                                                   .sign = prepend_sign_e::only_negative,
                                                   .alignment = alignment_e::middle
                                                  }>( value );
                                                  
    BOOST_TEST( expected == result );
    }
    //exmple constexpr use without buffer allocation
    
    constexpr bool test_unsigned_9d()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr int64_t value{ -0b1100110011001100110011001100110011001100110011001100110011001ll };
      constexpr std::string_view expected{ "      -1100110011001100110011001100110011001100110011001100110011001" };
      auto oit = strconv::integral_to_string<traits{.precision = 68,
                                                    .format = format_e::binary,
                                                    .padd_with = padd_with_e::space,
                                                    .include_prefix = include_prefix_e::no_prefix
                                                    }>( value, itbeg );
      std::string_view result{ itbeg, static_cast<std::string_view::size_type>(oit-itbeg) };
      return expected == result;
      }
    static_assert( test_unsigned_9d() );
    
    ```
### strconv::string_to_integral

Converting string represetation of integral numbers.
Returns converted number value and iterator pass the end of source string view where conversion stopped
Example as constexpr test function

```C++
  using strconv::string_to_integral;
  using namespace std::string_view_literals;
  using strconv::input_format_e;
  
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
    
### strconv::string_to_float

Converting string represetation of float numbers.

```C++
  using strconv::string_to_float;
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

### strconv::float_to_string

Converts floating point number into string representation
There are 2 variants available
* one returning string
* second cosntexpr storing result at output iterator returning iterator pass the end last stored char

Example using output iterator
```C++
    constexpr bool test_float_4a()
      {
      char buffer_[integral_to_string_max_size]{};
      auto itbeg{ &buffer_[0] };
      
      constexpr double value{ 0.5 };
      constexpr std::string_view expected{ "00000.5000" };
      auto oit = strconv::float_to_string<traits{
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
## Prformance
Performance comparision of compose vs snprintf for bellow test functions

**Ryzen 9 - 5900X**
compiler | function | stralgo | legacy
---------|----------|---------|-------
clang-13 | compose  | 14 ms   | 85 ms 

full implementation performance comparision can be seen in perf/perf.cc

```C++
  //example compose test
  strconv::compose<char>( 'T',
                         data.example_string, ' ',
                         data.emaple_float,
                         ' ',
                         strconv::fmt<strconv::integral_format_traits{
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
## Feedback

If you think you have found a bug, please file an issue via issue submission [form](https://github.com/arturbac/stralgo/issues). Include the relevant information to reproduce the bug for example as static_assert( expression ), if it is important also the compiler version and target architecture. Feature requests and contributions can be filed as issues or pull requests.

## License

This library is available to anybody free of charge, under the terms of MIT License (see LICENSE.md).
