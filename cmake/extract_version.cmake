# extract_version.cmake

file(READ include/stralgo/version.h stralgo_version_h_content)
string(REGEX MATCH "STRALGO_VERSION \"([^\"]*)\"" _ ${stralgo_version_h_content})
set(STRALGO_VERSION ${CMAKE_MATCH_1})
