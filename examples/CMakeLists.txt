cmake_minimum_required(VERSION 3.6)
project(example VERSION 0.1.0.0 LANGUAGES C)

SET(X64_FILES ON CACHE BOOL "Build with 64-bit files support")

add_executable(files files.c)
add_executable(memory memory.c)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  target_link_libraries(memory Kernel32.lib)
  target_link_libraries(memory Ws2_32.lib)
endif()

if(X64_FILES)
  add_definitions(-DX64_FILES)
endif()
