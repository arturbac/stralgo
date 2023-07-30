#pragma once

#include <stralgo/strconcept.h>
#include <concepts>
#include <iterator>
#include <ranges>

#if !defined(__cpp_unicode_characters)
#error "missing __cpp_unicode_characters support"
#endif
#if !defined(__cpp_char8_t)
#error "missing __cpp_char8_t support"
#endif

namespace stralgo::utf
{
  using u8 = std::uint8_t;
  using u16 = std::uint16_t;
  using u32 = std::uint32_t;
  
  namespace concepts
    {
    using stralgo::concepts::char_type;
    
    template<typename value_type, size_t req_char_size>
    concept char_size = char_type<value_type> && sizeof(value_type) == req_char_size;
    
    using stralgo::concepts::char_iterator;
    
    template<typename iterator_type, size_t req_char_size>
    concept sized_char_iterator = char_iterator<iterator_type> && char_size<std::iter_value_t<iterator_type>,req_char_size>;
    
    template<typename iterator_type>
    concept octet_iterator = sized_char_iterator<iterator_type,1>;
    
    template<typename iterator_type>
    concept u16bit_iterator = sized_char_iterator<iterator_type,2>;
    
    template<typename iterator_type>
    concept u32bit_iterator = sized_char_iterator<iterator_type,4>;
    
    using stralgo::concepts::char_range;
    }
}
