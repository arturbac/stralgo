#pragma once

#include <stralgo/strconcept.h>
#include <utils/strong_type.h>

namespace stralgo::detail
{
  //--------------------------------------------------------------------------------------------------------------
  //
  // strong_type compose support
  //
  //--------------------------------------------------------------------------------------------------------------
  template<concepts::char_type char_type, typename value_type, typename tag>
  struct compose_preconv_t<char_type,strong_type<value_type,tag>>
    {
    [[nodiscard]]
    stralgo_static_call_operator
    constexpr auto operator()( strong_type<value_type,tag> const & value )
        stralgo_static_call_operator_const noexcept
       { return stralgo::detail::compose_preconv_t<char_type,value_type>{}(value.value()); }
    };
  namespace strong_type_compose_verification
    {
    struct test_tag_{};
    static_assert(compose_arg_concept<strong_type<uint64_t,test_tag_>,char>);
    } 
}
