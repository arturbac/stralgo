  #----------------------------------------------------------------
  # boost-ext/ut
  #----------------------------------------------------------------
  CPMAddPackage(
    ut
    GITHUB_REPOSITORY arturbac/ut-ext
    GIT_TAG        master
  )
  add_library(stralgo::stralgo ALIAS stralgo )
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      set(STRALGO_COMPILE_OPTIONS
        -Weverything
        -Werror
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-pre-c++14-compat
        -Wno-pre-c++17-compat
        -Wno-pre-c++20-compat-pedantic
        -Wno-c++20-compat
        -Wno-c++2b-extensions
        -Wno-unused-parameter
        -Wno-padded
        -Wno-unused-command-line-argument
        -Wunsafe-buffer-usage
        )
      # check_cxx_compiler_flag(-Wunsafe-buffer-usage WUNSAFE_BUFFER_USAGE)
      # if(WUNSAFE_BUFFER_USAGE)
        # list(APPEND STRALGO_COMPILE_OPTIONS -Wunsafe-buffer-usage)
      # endif()
  endif()
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set(STRALGO_COMPILE_OPTIONS
        -Wall
        -Wextra
        -Werror
      )
  endif()

add_subdirectory(ut_core)

function( stralgo_add_unittest name )
  add_executable(${name}_ut)
  target_sources(${name}_ut PRIVATE ${name}.cc)
  target_link_libraries( ${name}_ut PRIVATE stralgo_ut_core )
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
