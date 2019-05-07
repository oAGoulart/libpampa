cmake_minimum_required(VERSION 3.6)
project(example VERSION 0.1.0.0 LANGUAGES C)

SET(X64_FILES ON CACHE BOOL "Build with 64-bit files support")

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
 add_definitions(-DWINDOWS)
endif()

if(X64_FILES)
 add_definitions(-DX64)
endif()

add_executable(example example.c)
