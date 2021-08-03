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
    
  enum struct padd_with_e : bool { space, zeros };
    
  enum struct prepend_sign_e : bool { only_negative, always };
    
  enum struct include_prefix_e : bool { no_prefix, with_prefix };
  
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
    };
}
