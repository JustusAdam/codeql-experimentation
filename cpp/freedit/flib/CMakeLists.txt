cmake_minimum_required(VERSION 3.10)
project(flib VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard so we get `optional`
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_OSX_ARCHITECTURES "x86_64")

# Source files
set(SOURCE_FILES
    flib.hpp
    flib.cpp
)

# Create shared library target
add_library(flib SHARED ${SOURCE_FILES})

# Set library version
set_target_properties(flib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# Set include directories
target_include_directories(flib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Install rules
include(GNUInstallDirs)
install(TARGETS flib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES main.cpp flib.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/flib)
