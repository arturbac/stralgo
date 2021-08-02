//--------------------------------------------------------------------------
// string view manipulation algorithms without dependency on null termination
// {C} Artur Bac 2021
//--------------------------------------------------------------------------
#pragma once
namespace strconv
{
  enum struct char_case_e : bool
    {
    uppercase,
    lowercase
    };
    
  enum struct trailing_zeros_e : bool
    {
    preserve,
    skip
    };
    
  enum struct format_e : uint8_t
    {
    decimal,
    hexadecimal
    };
    
  ///\brief traits for custom integral_to_string formating
  struct integral_format_traits
    {
    format_e format      {format_e::decimal};
    ///\brief minimum number of characters
    uint8_t precision    {1};
    char_case_e char_case { char_case_e::lowercase };
    bool padd_with_zeros {};
    };
}
