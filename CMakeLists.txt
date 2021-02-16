cmake_minimum_required(VERSION 3.6)
project(libpampa VERSION 0.1.11.0 LANGUAGES C)

include_directories("include" "lib")

add_subdirectory("examples")
add_subdirectory("tests")
