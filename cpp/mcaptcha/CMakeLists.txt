cmake_minimum_required(VERSION 3.10)
project(mCaptchaFreeditLib VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard so we get `optional`
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_OSX_ARCHITECTURES "x86_64")

set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/build)
find_library(THIRD_PARTY NAMES third_party PATHS ${THIRD_PARTY_DIR})

# Source files
set(SOURCE_FILES
    main.cpp
)

add_library(mCaptcha SHARED ${SOURCE_FILES})
target_link_libraries(mCaptcha PRIVATE ${THIRD_PARTY})

# Set include directories
target_include_directories(mCaptcha
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${THIRD_PARTY_DIR}
)

