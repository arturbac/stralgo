cmake_minimum_required(VERSION 3.23)

project(stralgo 
        VERSION 1.3.4
        LANGUAGES CXX
        HOMEPAGE_URL "https://github.com/arturbac/stralgo"
        )
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

include(FeatureSummary)
option(STRALGO_ENABLE_UNIT_TESTS "unit tests available from CTest"  ${PROJECT_IS_TOP_LEVEL} )
add_feature_info("STRALGO_ENABLE_UNIT_TESTS" STRALGO_ENABLE_UNIT_TESTS "unit test available from CTest")

if( PROJECT_IS_TOP_LEVEL AND STRALGO_ENABLE_UNIT_TESTS)
  set(SMALL_VECTORS_ENABLE_UNIT_TESTS ON)
  enable_testing()
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 23 )
  message(STATUS "Assuming by default c++23 standard")
  set( CMAKE_CXX_STANDARD 23 )
endif()
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)

include(cmake/get_cpm.cmake)
# ---- Add dependencies via CPM ----
# see https://github.com/cpm-cmake/CPM.cmake for more info
CPMAddPackage(
  NAME PackageProject.cmake
  GITHUB_REPOSITORY TheLartians/PackageProject.cmake
  VERSION 1.12.0
)

#----------------------------------------------------------------
# small_vectors
#----------------------------------------------------------------
CPMAddPackage(
  small_vectors
  GITHUB_REPOSITORY arturbac/small_vectors
  GIT_TAG        v3.2.0
  )
  
if( PROJECT_IS_TOP_LEVEL )
  #----------------------------------------------------------------
  # boost-ext/ut
  #----------------------------------------------------------------
  CPMAddPackage(
    ut
    GITHUB_REPOSITORY arturbac/ut-ext
    GIT_TAG        master
  )
endif()




add_library(stralgo INTERFACE )

if(PROJECT_IS_TOP_LEVEL)
  add_library(stralgo::stralgo ALIAS stralgo )
endif()

if(NOT DEFINED INCLUDE_INSTALL_DIR)
  set(INCLUDE_INSTALL_DIR include/${PROJECT_NAME})
endif()

target_include_directories(stralgo 
  INTERFACE   $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
)
target_link_libraries(stralgo INTERFACE small_vectors::small_vectors)

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  NAMESPACE stralgo
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION ${INCLUDE_INSTALL_DIR}
  COMPATIBILITY SameMajorVersion
)

if( PROJECT_IS_TOP_LEVEL )

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
endif()

if( STRALGO_ENABLE_UNIT_TESTS)
  add_subdirectory( unittests )
endif()
# 
# add_subdirectory(perf)
