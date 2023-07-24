function( stralgo_add_unittest name )
  add_executable(${name}_ut)
  target_sources(${name}_ut PRIVATE ${name}.cc)
  target_link_libraries( ${name}_ut PRIVATE stralgo )
  target_link_libraries( ${name}_ut PRIVATE small_vectors_ut_core )
  # add_dependencies(unit_tests ${name}_ut )
  add_test( NAME ${name}_test COMMAND ${name}_ut )
  target_compile_options( ${name}_ut PRIVATE ${STRALGO_COMPILE_OPTIONS} )
endfunction()

stralgo_add_unittest( stralgo)
stralgo_add_unittest( strconv_numeric)
target_compile_options(strconv_numeric_ut PRIVATE -Wno-float-equal)
# add_executable( stralgo_test )
# target_sources( stralgo_test PRIVATE stralgo.cc )
# target_link_libraries( stralgo_test
#     PRIVATE
#      stralgo
#      small_vectors_ut_core
#      )
#      
# add_test( stralgo_test ${CMAKE_CURRENT_BINARY_DIR}/stralgo_test )

# add_executable( strconv_numeric_test )
# target_sources( strconv_numeric_test PRIVATE strconv_numeric.cc )
# target_link_libraries( strconv_numeric_test
#     PRIVATE
#      stralgo
#      small_vectors_ut_core
#      )
# add_test( strconv_numeric_test ${CMAKE_CURRENT_BINARY_DIR}/strconv_numeric_test )
