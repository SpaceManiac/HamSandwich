# HamSandwich root CMake project file.
# Minimum version is based on what is available in VS 2019.
cmake_minimum_required(VERSION 3.19.0)
project("HamSandwich")

cmake_policy(SET CMP0116 NEW)  # https://cmake.org/cmake/help/latest/policy/CMP0116.html

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_MESSAGE LAZY)
set(CMAKE_INSTALL_RPATH "\$ORIGIN")
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/build/install")
set(CMAKE_INSTALL_DATADIR ".")
set(CMAKE_INSTALL_BINDIR ".")
set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_BINDIR}")

if(EMSCRIPTEN)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/build/webroot")
endif()

if(APPLE)
	set(CMAKE_INSTALL_RPATH "@executable_path")
endif()

add_subdirectory("external")
add_subdirectory("source")

if(WIN32 AND CMAKE_COMPILER_IS_GNUCC)
	function(install_dll dll_name)
		find_program("${dll_name}" "${dll_name}" REQUIRED NO_CACHE)
		install(FILES "${${dll_name}}" TYPE BIN COMPONENT Executables)
	endfunction()
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		install_dll("libgcc_s_dw2-1.dll")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		install_dll("libgcc_s_seh-1.dll")
	endif()
	install_dll("libstdc++-6.dll")
	install_dll("libwinpthread-1.dll")
endif()
