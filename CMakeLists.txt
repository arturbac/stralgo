cmake_minimum_required(VERSION 3.23)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/extract_version.cmake)
project(
  stralgo
  VERSION ${STRALGO_VERSION}
  LANGUAGES CXX
  HOMEPAGE_URL "https://github.com/arturbac/stralgo")
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

if(PROJECT_IS_TOP_LEVEL)
  include(FeatureSummary)
endif()

if(PROJECT_IS_TOP_LEVEL)
  option(STRALGO_ENABLE_UNIT_TESTS "unit tests available from CTest"
         ${PROJECT_IS_TOP_LEVEL})
  add_feature_info("STRALGO_ENABLE_UNIT_TESTS" STRALGO_ENABLE_UNIT_TESTS
                   "unit test available from CTest")

  if(STRALGO_ENABLE_UNIT_TESTS)
    set(SMALL_VECTORS_ENABLE_UNIT_TESTS ON)
    enable_testing()
  endif()
endif()
option(STRALGO_EXPORT_CMAKE_TARGETS "Enable cmake targets" ON)

if(NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 23)
  message(STATUS "Assuming by default c++23 standard")
  set(CMAKE_CXX_STANDARD 23)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----------------------------------------------------------------
# small_vectors
# ----------------------------------------------------------------
find_package(small_vectors QUIET)
if(NOT small_vectors_FOUND)
  include(cmake/get_cpm.cmake)
  cpmaddpackage(small_vectors GITHUB_REPOSITORY arturbac/small_vectors GIT_TAG
                v3.3.5
                )
  add_library(small_vectors::small_vectors ALIAS small_vectors )
endif()

add_library(stralgo INTERFACE)

target_sources(
  stralgo
  INTERFACE FILE_SET HEADERS BASE_DIRS
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)
target_link_libraries(stralgo INTERFACE small_vectors::small_vectors)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)
install(
  TARGETS stralgo
  EXPORT stralgo_targets
  INCLUDES
  DESTINATION include
  FILE_SET HEADERS)
  
configure_package_config_file(
  cmake/stralgoConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/stralgoConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stralgo)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/stralgoConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/stralgoConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/stralgoConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stralgo)

if(STRALGO_EXPORT_CMAKE_TARGETS)
  install(
    EXPORT stralgo_targets
    FILE stralgoTargets.cmake
    NAMESPACE stralgo::
    DESTINATION lib/cmake/stralgo)
endif()

if(STRALGO_ENABLE_UNIT_TESTS)
  add_subdirectory(unittests)
endif()
#
# add_subdirectory(perf)
