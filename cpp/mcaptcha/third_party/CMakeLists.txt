cmake_minimum_required(VERSION 3.10)
project(third_party VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard so we get `optional`
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_OSX_ARCHITECTURES "x86_64")

# Source files
set(SOURCE_FILES
    third_party.hpp
    impl.cpp
)

# Create shared library target
add_library(third_party SHARED ${SOURCE_FILES})

# Set library version
set_target_properties(third_party PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# Set include directories
target_include_directories(third_party
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Install rules
include(GNUInstallDirs)
install(TARGETS third_party
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES impl.cpp third_party.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/third_party)
