cmake_minimum_required(VERSION 3.6)
project(example VERSION 0.1.0.0 LANGUAGES C)

add_definitions(-Wall -g)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
 add_definitions(-DWINDOWS)
endif()

add_executable(example example.c)
