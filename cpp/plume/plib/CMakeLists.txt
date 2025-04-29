cmake_minimum_required(VERSION 3.18)
project(Plib VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_OSX_ARCHITECTURES "x86_64")

add_library(
    plib SHARED lib.cpp lib.hpp
)

target_include_directories(plib PUBLIC ".")