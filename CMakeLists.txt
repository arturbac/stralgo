cmake_minimum_required(VERSION 3.0)

project(stralgo)

add_executable(stralgo main.cpp)

install(TARGETS stralgo RUNTIME DESTINATION bin)
