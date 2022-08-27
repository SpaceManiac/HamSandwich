# HamSandwich tools directory CMake project file.
# Tools must be built for the host platform rather than the target platform.
# Minimum version is based on what is available in VS 2019.
cmake_minimum_required(VERSION 3.19.0)
project("HamSandwich tools")

set(export_cmake "${CMAKE_CURRENT_BINARY_DIR}/host_tools_exports.cmake")
set(custom_cmake "${CMAKE_CURRENT_BINARY_DIR}/host_tools.cmake")
file(WRITE "${export_cmake}" "")
file(WRITE "${custom_cmake}" "include(${export_cmake})")
function (export_tool target)
	export(TARGETS ${target} APPEND FILE "${export_cmake}")
	file(APPEND "${custom_cmake}" "
		add_custom_target(
			hostbuild_${target}
			COMMAND
				${CMAKE_COMMAND}
				--build ${CMAKE_BINARY_DIR}
				--target ${target}
				--config ${CMAKE_BUILD_TYPE}
			COMMENT \"Building host tool: ${target}\"
		)
		add_dependencies(${target} hostbuild_${target})
	")
endfunction()
function (export_script scriptname)
	include("${scriptname}")
	file(APPEND "${custom_cmake}" "include(${CMAKE_CURRENT_SOURCE_DIR}/${scriptname})")
endfunction()

add_subdirectory(mini_xxd)
