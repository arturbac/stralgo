add_executable( stralgo_test )
target_sources( stralgo_test PRIVATE stralgo.cc )
target_link_libraries( stralgo_test
     stralgo
     Boost::unit_test_framework
     )
add_executable( strconv_numeric_test )
target_sources( strconv_numeric_test PRIVATE strconv_numeric.cc )
target_link_libraries( strconv_numeric_test
     stralgo
     Boost::unit_test_framework
     )
