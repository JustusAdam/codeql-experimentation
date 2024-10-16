set(NAME "SomeLibrary")

add_library(${NAME} STATIC lib.cpp lib.hpp)

target_include_directories(
  ${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${NAME} PRIVATE cxx_std_20)
