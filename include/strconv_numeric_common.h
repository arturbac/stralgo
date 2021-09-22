//--------------------------------------------------------------------------
// string view manipulation algorithms without dependency on null termination
// {C} Artur Bac 2021
//--------------------------------------------------------------------------
#pragma once
namespace strconv
{
  enum struct char_case_e : bool { lowercase, uppercase };
    
  enum struct trailing_zeros_e : bool { preserve, skip };
    
  enum struct format_e : uint8_t { decimal, hexadecimal, binary };
    
  enum struct input_format_e : uint8_t { undetermined, decimal, hexadecimal };
  
  enum struct padd_with_e : bool { space, zeros };
    
  enum struct prepend_sign_e : bool { only_negative, always };
    
  enum struct include_prefix_e : bool { no_prefix, with_prefix };
  
  enum struct alignment_e : uint8_t { left, middle, right};
  
  ///\brief traits for custom integral_to_string formating
  struct integral_format_traits
    {
    ///\brief minimum number of characters
    uint16_t precision              { 1 };
    ///\brief output format encoding of digit numbers
    format_e format                 { format_e::decimal };
    ///\brief char case whenusing hexadecimal format
    char_case_e char_case           { char_case_e::lowercase };
    ///\brief pad value with zeros or space when precision is higher than value representation
    padd_with_e padd_with           { padd_with_e::space };
    ///\brief prepend sign to result string
    prepend_sign_e sign             { prepend_sign_e::only_negative };
    ///\brief include prefix when defined for format ex: for hexadecimal 0x
    include_prefix_e include_prefix { include_prefix_e::with_prefix };
    ///\brief alignment when padding with space
    alignment_e alignment           { alignment_e::right };
    };
    
  namespace detail
    {
    inline constexpr uint32_t default_decimal_places = 6;
    }
  struct float_format_traits 
    {
    ///\brief minimum number of characters
    uint16_t precision              { 1 };
    
    uint16_t decimal_places { detail::default_decimal_places };

    ///\brief pad value with zeros or space when precision is higher than value representation
    padd_with_e padd_with           { padd_with_e::space };
    ///\brief prepend sign to result string
    prepend_sign_e sign             { prepend_sign_e::only_negative };

    ///\brief alignment when padding with space
    alignment_e alignment           { alignment_e::right };
    
    trailing_zeros_e trailing_zeros { trailing_zeros_e::skip };
    
    };
}
