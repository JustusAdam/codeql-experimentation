cmake_minimum_required(VERSION 3.10)
project(LemmyLib VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard so we get `optional`
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source files
set(SOURCE_FILES
    main.cpp
)

# Create shared library target
add_library(LemmyLib SHARED ${SOURCE_FILES})

# Set library version
set_target_properties(LemmyLib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# Set include directories
target_include_directories(LemmyLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Link against bcrypt library
list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/bcrypt")

# Install rules
include(GNUInstallDirs)
install(TARGETS LemmyLib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES main.cpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/LemmyLib)
