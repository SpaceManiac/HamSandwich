file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(ham SHARED ${SOURCES})
target_link_libraries(ham vanilla_extract SDL2 SDL2_image SDL2_mixer)
target_include_directories(ham INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

install(TARGETS ham COMPONENT Executables)

function(HamSandwich_icon SOURCES_VAR RC_FILENAME)
	set(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/${RC_FILENAME}")
	if(WIN32)
		list(APPEND "${SOURCES_VAR}" "${INPUT}")
		set("${SOURCES_VAR}" "${${SOURCES_VAR}}" PARENT_SCOPE)
	else()
		set(RESCOMP "${CMAKE_SOURCE_DIR}/tools/build/rescomp.py")
		set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${RC_FILENAME}.cpp")
		add_custom_command(
			OUTPUT "${OUTPUT}"
			COMMAND python3 "${RESCOMP}" "${INPUT}" "${OUTPUT}"
			MAIN_DEPENDENCY "${INPUT}"
			DEPENDS "${RESCOMP}"
		)
		list(APPEND "${SOURCES_VAR}" "${OUTPUT}")
		set("${SOURCES_VAR}" "${${SOURCES_VAR}}" PARENT_SCOPE)
	endif()
endfunction()

function(HamSandwich_metadata TARGET_NAME SOURCES_VAR JSON_FILENAME)
	set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${JSON_FILENAME}")
	file(READ "${JSON_FILENAME}" JSON_BLOB)

	get_property(all_metadata GLOBAL PROPERTY HamSandwich_all_metadata)
	if("${all_metadata}" STREQUAL "")
		set(all_metadata "{}")
	endif()
	string(JSON all_metadata SET "${all_metadata}" "${TARGET_NAME}" "${JSON_BLOB}")
	set_property(GLOBAL PROPERTY HamSandwich_all_metadata "${all_metadata}")

	set(metadata_cpp "")
	string(APPEND metadata_cpp [[
#include "metadata.h"

static const char* default_asset_specs[] = {
]])

	string(JSON len LENGTH "${JSON_BLOB}" installers)
	math(EXPR len "${len} - 1")
	foreach(i RANGE "${len}")
		string(JSON optional ERROR_VARIABLE optional_err GET "${JSON_BLOB}" installers ${i} optional)
		if("${optional_err}" STREQUAL "NOTFOUND" AND "${optional}" STREQUAL "ON")  # "error not found" means "ok"
			continue()
		endif()

		string(JSON mountpoint ERROR_VARIABLE mountpoint_err GET "${JSON_BLOB}" installers ${i} mountpoint)
		if(NOT "${mountpoint_err}" EQUAL NOTFOUND)
			set(mountpoint "")
		endif()
		string(JSON kind GET "${JSON_BLOB}" installers ${i} kind)
		string(JSON filename GET "${JSON_BLOB}" installers ${i} filename)
		string(APPEND metadata_cpp "\t\"${mountpoint}@${kind}@installers/${filename}\",\n")
	endforeach()

	string(APPEND metadata_cpp [[
	nullptr
};

static const HamSandwichMetadata metadata = {
	"]] "${TARGET_NAME}" [[",
	default_asset_specs
};

const HamSandwichMetadata* GetHamSandwichMetadata()
{
	return &metadata;
}
]])

	file(GENERATE OUTPUT metadata.cpp CONTENT "${metadata_cpp}")
	list(APPEND "${SOURCES_VAR}" "${CMAKE_CURRENT_BINARY_DIR}/metadata.cpp")
	set("${SOURCES_VAR}" "${${SOURCES_VAR}}" PARENT_SCOPE)
endfunction()

function(HamSandwich_install TARGET_NAME)
	if(EMSCRIPTEN)
		install(FILES
			"${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.js"
			"${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.wasm"
			DESTINATION "${CMAKE_INSTALL_PREFIX}/${TARGET_NAME}")
	else()
		install(TARGETS "${TARGET_NAME}" COMPONENT Executables)
	endif()
endfunction()
