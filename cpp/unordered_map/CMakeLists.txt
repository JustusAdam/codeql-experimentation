cmake_minimum_required(VERSION 3.15)

project(Tutorial VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(testlib main.cpp)
