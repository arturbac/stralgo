function( stralgo_add_unittest name )
  add_executable(${name}_ut)
  target_sources(${name}_ut PRIVATE ${name}.cc)
  target_link_libraries( ${name}_ut PRIVATE stralgo )
  target_link_libraries( ${name}_ut PRIVATE small_vectors_ut_core )
  # add_dependencies(unit_tests ${name}_ut )
  add_test( NAME ${name}_test COMMAND ${name}_ut )
  target_compile_options( ${name}_ut PRIVATE ${STRALGO_COMPILE_OPTIONS} )
endfunction()

stralgo_add_unittest( stralgo )
stralgo_add_unittest( strconv_numeric )
target_compile_options(strconv_numeric_ut PRIVATE -Wno-float-equal)

if( NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR ${CMAKE_CXX_COMPILER_VERSION} STRGREATER_EQUAL "16.0.0")
 # clang 15 with stdc++ has broken views
  stralgo_add_unittest( stralgo_utf )
endif()
stralgo_add_unittest( utf_ )
