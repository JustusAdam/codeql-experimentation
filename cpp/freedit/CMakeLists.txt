cmake_minimum_required(VERSION 3.10)
project(FreeditLib VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard so we get `optional`
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(FLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/flib/build)
find_library(FLIB_LIBRARY NAMES flib PATHS ${FLIB_DIR})

# Source files
set(SOURCE_FILES
    main.cpp
)

# Create shared library target
add_library(FreeditLib SHARED ${SOURCE_FILES})
target_link_libraries(FreeditLib PRIVATE ${FLIB_LIBRARY})

# Set library version
set_target_properties(FreeditLib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${FLIB_DIR}"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# Set include directories
target_include_directories(FreeditLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${FLIB_DIR}
)

# Install rules
include(GNUInstallDirs)
install(TARGETS FreeditLib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES main.cpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/FreeditLib)
