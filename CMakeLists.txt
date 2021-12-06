cmake_minimum_required(VERSION 3.19.0)
project("HamSandwich")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_MESSAGE LAZY)
set(CMAKE_INSTALL_RPATH "\$ORIGIN")
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/build/install")
set(CMAKE_INSTALL_BINDIR ".")
set(CMAKE_INSTALL_DATADIR ".")

add_subdirectory("external")
add_subdirectory("source")
