# HamSandwich root CMake project file.
# Minimum version is based on what is available in VS 2019.
cmake_minimum_required(VERSION 3.19.0)
project("HamSandwich")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_MESSAGE LAZY)
set(CMAKE_INSTALL_RPATH "\$ORIGIN")
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/build/install")
set(CMAKE_INSTALL_DATADIR ".")
set(CMAKE_INSTALL_BINDIR ".")
set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_BINDIR}")

add_subdirectory("external" EXCLUDE_FROM_ALL)
add_subdirectory("source")

install(TARGETS libcurl supreme sleepless loonyland loonyland2 mystic launcher COMPONENT Executables)

set_property(TARGET launcher PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
