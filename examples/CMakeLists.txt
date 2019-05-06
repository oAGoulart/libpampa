cmake_minimum_required(VERSION 3.6)
project(example VERSION 0.1.0.0 LANGUAGES C)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
 add_definitions(-DWINDOWS)
endif()

if(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
 add_definitions(-DX64)
endif()

add_executable(example example.c)
