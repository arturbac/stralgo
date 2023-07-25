cmake_minimum_required(VERSION 3.23)

project(stralgo 
        VERSION 0.1.5
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

include(cmake/CPM.cmake)
# ---- Add dependencies via CPM ----
# see https://github.com/cpm-cmake/CPM.cmake for more info
CPMAddPackage(
  NAME PackageProject.cmake
  GITHUB_REPOSITORY TheLartians/PackageProject.cmake
  VERSION 1.11.0
)
  
#----------------------------------------------------------------
# small_vectors
#----------------------------------------------------------------
CPMAddPackage(
  small_vectors
  GITHUB_REPOSITORY arturbac/small_vectors
  GIT_TAG        v1.1.28
  OPTIONS "SMALL_VECTORS_ENABLE_UNIT_TESTS=${STRALGO_ENABLE_UNIT_TESTS}"
  )
#----------------------------------------------------------------
# boost-ext/ut
#----------------------------------------------------------------
CPMAddPackage(
  ut
  GITHUB_REPOSITORY boost-ext/ut
  GIT_TAG        v1.1.9
)

if(NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 20 )
  message(STATUS "Assuming by default c++20 standard")
  set( CMAKE_CXX_STANDARD 20 )
endif()
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)




add_library(stralgo INTERFACE )
if(NOT DEFINED INCLUDE_INSTALL_DIR)
  set(INCLUDE_INSTALL_DIR include/${PROJECT_NAME})
endif()

target_include_directories(stralgo 
  INTERFACE   $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
)
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
      # option(SMALL_VECTORS_ENABLE_LLD_LINKER "enable lld linker for linking unit tests" ON )
      # add_feature_info("SMALL_VECTORS_ENABLE_LLD_LINKER" SMALL_VECTORS_ENABLE_LLD_LINKER "enable lld linker for linking unit tests")
      
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
        )
      check_cxx_compiler_flag(-Wunsafe-buffer-usage WUNSAFE_BUFFER_USAGE)
      if(WUNSAFE_BUFFER_USAGE)
        list(APPEND STRALGO_COMPILE_OPTIONS -Wno-unsafe-buffer-usage)
      endif()
      # if(SMALL_VECTORS_ENABLE_LLD_LINKER)
      #   add_link_options( -fuse-ld=lld )
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
  FetchContent_MakeAvailable(ut)
  add_subdirectory( unittests )
endif()
# 
# add_subdirectory(perf)
