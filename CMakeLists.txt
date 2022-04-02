# HamSandwich root CMake project file.
# Minimum version is based on what is available in VS 2019.
cmake_minimum_required(VERSION 3.19.0)
project("HamSandwich")

cmake_policy(SET CMP0116 NEW)  # https://cmake.org/cmake/help/latest/policy/CMP0116.html

# Global settings pertaining to installation.
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

# Include the external/ and source/ directories to get most of the build.
add_subdirectory("external")
add_subdirectory("source")

# On MinGW, install the requisite .dll files.
if(WIN32 AND CMAKE_COMPILER_IS_GNUCC)
	function(install_dll dll_name)
		find_program("${dll_name}" "${dll_name}" REQUIRED NO_CACHE)
		install(FILES "${${dll_name}}" TYPE BIN COMPONENT generic/executables)
	endfunction()
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		install_dll("libgcc_s_dw2-1.dll")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		install_dll("libgcc_s_seh-1.dll")
	endif()
	install_dll("libstdc++-6.dll")
	install_dll("libwinpthread-1.dll")
endif()

# Install Itch metadata.
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT EMSCRIPTEN)
	set(pathext "")
	if(APPLE)
		set(itch_platform "osx")
	elseif(WIN32)
		set(itch_platform "windows")
		set(pathext ".bat")
	else()
		set(itch_platform "linux")
	endif()

	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(itch_arch "386")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(itch_arch "amd64")
	endif()

	install(FILES "${CMAKE_SOURCE_DIR}/tools/itch/${itch_platform}.itch.toml" RENAME ".itch.toml" DESTINATION "${CMAKE_INSTALL_PREFIX}")
	# Work around bug.
	install(FILES "${CMAKE_SOURCE_DIR}/tools/itch/distraction.sh" RENAME "itch-bug-workaround-1.sh" DESTINATION "${CMAKE_INSTALL_PREFIX}/installers")
	install(FILES "${CMAKE_SOURCE_DIR}/tools/itch/distraction.sh" RENAME "itch-bug-workaround-2.sh" DESTINATION "${CMAKE_INSTALL_PREFIX}/installers")

	# Run butler-validate.
	install(CODE "
		execute_process(COMMAND
			${CMAKE_SOURCE_DIR}/tools/bootstrap/butler${pathext} validate
			--platform=${itch_platform}
			--arch=${itch_arch}
			\${CMAKE_INSTALL_PREFIX}
			COMMAND_ERROR_IS_FATAL ANY
		)
	")
endif()
