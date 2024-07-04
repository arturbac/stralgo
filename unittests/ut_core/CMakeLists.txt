#----------------------------------------------------------------
# boost-ext/ut
#----------------------------------------------------------------
CPMAddPackage(
  ut
  GITHUB_REPOSITORY arturbac/ut-ext
  GIT_TAG        master
)

add_library( stralgo_ut_core )
target_sources( stralgo_ut_core PRIVATE unit_test_core.cc )
target_link_libraries( stralgo_ut_core
  PUBLIC
    stralgo
    Boost::ut
    )
target_include_directories( stralgo_ut_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
target_compile_options(stralgo_ut_core
  PUBLIC
    ${SMALL_VECTORS_COMPILE_OPTIONS}
    )
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  target_compile_options(stralgo_ut_core
    PUBLIC
      -Wno-disabled-macro-expansion
      -Wno-used-but-marked-unused
      -Wno-global-constructors
      -Wno-exit-time-destructors
      -Wno-ctad-maybe-unsupported
      -Wno-weak-vtables
      -fconstexpr-backtrace-limit=0
      -Wno-misleading-indentation
      -Wno-switch-default
      )
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(stralgo_ut_core
    PUBLIC
      -Wno-misleading-indentation
      -Wno-attributes
      -Wno-unused-parameter
      )
endif()
