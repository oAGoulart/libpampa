include_directories("acutest/include")

add_executable(test_file test_file.c)
add_executable(test_memory test_memory.c)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  target_link_libraries(test_memory Kernel32.lib)
  target_link_libraries(test_memory Ws2_32.lib)
endif()
