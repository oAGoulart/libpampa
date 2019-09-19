SET(X64_FILES ON CACHE BOOL "Build with 64-bit file support")

add_executable(using_files main.c)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  target_link_libraries(using_files Kernel32.lib)
endif()

if(X64_FILES)
  add_definitions(-DX64_FILES)
endif()
