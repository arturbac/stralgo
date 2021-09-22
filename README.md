# stralgo
constexpr number &lt;-> string composition, formating and convertions with full support of unterminated string views
Thru years of participating in projects, string formating like vnprintf had always a downsides
* there was no coherence between formating string and arguments, that was source of errors
* string composition was always done at runtime even if some operations and arguments was known and could be known at compile time
* all that operations required null terminated C strings
* formating information for ex in vsnprintf could be easly shared/reused

## features

This code solves thise problems
* foramting information is attached to arguments and prevents mismatching formating with number type errors at compile time
* every convertion, composing, formatting is constexpr except allocations
* full suport for std::string_view with not null terminated strings
* formating traits can be declared constexpr constant and reused
* minimum c++20 compiler required (in development at this point constexpr unit tests require c++20 and std::is_constant_evaluated() )
* fully constexpr string to number and number to string convetions for functions that doesn't allocate (using output interators)
* extensive number formating with traits
* merging string_views and composing any data (string_views, numbers) with one variadic template, one allocation

## installation

* header only except some of unit tests

## examples
### strconv::compose
```C++
    {
    //main feature compose with constexpr formating except final allocation
  
  std::string strres{ 
   strconv::compose<char>(
    " some view "sv,
    127.3f, //default formatted floating point see float_format_traits{}
    ',', ///single char
    125, //default formatted integral number see integral_format_traits{}
    '[',
    strconv::fmt<integral_format_traits{
            .precision = 10, //minimum number of characters
            .format = format_e::hexadecimal, //output format encoding of digit numbers
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
    ']'
  ) };
  
  constexpr auto expected{ " some view 127.300003,125[  0x1c8   ] [10.46     ]"sv };
    }
    
    ```
### stralgo::merge

### strconv::integral_to_string

Converting and formating integral numbers.
Building block of compose that can be use separatly

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

### strconv::float_to_integral
