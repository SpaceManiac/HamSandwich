cmake_minimum_required(VERSION 3.15)
project(jtjson)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add double-conversion library
add_library(double-conversion STATIC
    double-conversion/bignum.cc
    double-conversion/bignum-dtoa.cc
    double-conversion/cached-powers.cc
    double-conversion/double-to-string.cc
    double-conversion/fast-dtoa.cc
    double-conversion/fixed-dtoa.cc
    double-conversion/string-to-double.cc
    double-conversion/strtod.cc
)
target_include_directories(double-conversion PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Main JSON library
add_library(jtjson STATIC
    src/json.cpp
)
target_link_libraries(jtjson PRIVATE double-conversion)
target_include_directories(jtjson PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
