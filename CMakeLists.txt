cmake_minimum_required(VERSION 3.0)

project(stralgo LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20 )
set(CMAKE_CXX_STANDARD_REQUIRED 20 )

find_package(Boost 1.70.0 COMPONENTS unit_test_framework)

add_library(stralgo STATIC
  include/strconcept.h
  include/stralgo.h
  include/strconv_numeric.h
  )
set_target_properties(stralgo PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(stralgo PUBLIC include )

#add_executable(main main.cpp)
#target_link_libraries( main PRIVATE stralgo )

#install(TARGETS stralgo RUNTIME DESTINATION bin)

add_subdirectory( unittests )
add_subdirectory(perf)
