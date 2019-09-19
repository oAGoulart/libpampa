add_executable(patching_memory main.c)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  target_link_libraries(patching_memory Kernel32.lib)
  target_link_libraries(patching_memory Ws2_32.lib)
endif()
