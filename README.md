# stralgo
constexpr number &lt;-> string convertions with full support of unterminated string views

## features

* minimum c++17 compiler required (in development at this point constexpr unit tests require c++20)
* full suport for std::string_view with not null terminated strings
* fully constexpr string to number and number to string convetions for functions that doesn't allocate (using output interators)
* extensive number formating with traits
* merging string_views and composing any data (string_views, numbers) with one variadic template, one allocation


## installation

* header only except some of unit tests

## examples

### integral_to_string

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
