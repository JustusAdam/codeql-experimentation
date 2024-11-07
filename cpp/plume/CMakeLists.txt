cmake_minimum_required(VERSION 3.15)

project(Tutorial VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_OSX_ARCHITECTURES "x86_64")

add_executable(testbin main.cpp)

find_library(plib NAMES plib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/plib/build)

target_link_libraries(testbin PRIVATE ${plib})