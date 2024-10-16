# CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

project(
  LibraryTest
  DESCRIPTION "Test CodeQL library build support."
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(some_library)
