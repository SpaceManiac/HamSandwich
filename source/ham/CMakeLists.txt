file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
if(WIN32 OR EMSCRIPTEN)
	# TODO: Deal with Ham importing various functions from each game.
	set(HAM_LIBRARY_TYPE STATIC)
else()
	set(HAM_LIBRARY_TYPE SHARED)
endif()
add_library(ham ${HAM_LIBRARY_TYPE} ${sources})
target_link_libraries(ham minizip vanilla_extract SDL2 SDL2_image SDL2_mixer)
if(ANDROID)
	target_link_libraries(ham log)
endif()
target_include_directories(ham INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

if("${HAM_LIBRARY_TYPE}" STREQUAL "SHARED")
	install(TARGETS ham COMPONENT Executables)
endif()

function(HamSandwich_add_executable TARGET_NAME)
	if(ANDROID)
		add_library("${TARGET_NAME}" SHARED ${ARGN})
	else()
		add_executable("${TARGET_NAME}" ${ARGN})
	endif()
endfunction()

function(HamSandwich_embed_file target_name filename symbol_name)
	set(embed_py "${CMAKE_SOURCE_DIR}/tools/build/embed.py")
	set(cpp "${CMAKE_CURRENT_BINARY_DIR}/${symbol_name}.cpp")
	add_custom_command(
		OUTPUT "${cpp}"
		COMMAND "${CMAKE_SOURCE_DIR}/tools/bootstrap/python" "${embed_py}" "${filename}" "${cpp}" "${symbol_name}"
		MAIN_DEPENDENCY "${filename}"
		DEPENDS
			"${CMAKE_SOURCE_DIR}/dependencies.sh"
			"${CMAKE_SOURCE_DIR}/tools/bootstrap/_common.sh"
			"${CMAKE_SOURCE_DIR}/tools/bootstrap/python"
			"${embed_py}"
		VERBATIM
	)
	target_sources("${target_name}" PRIVATE "${cpp}")
endfunction()

function(HamSandwich_icon target_name ico_filename)
	set(ico "${CMAKE_CURRENT_SOURCE_DIR}/${ico_filename}")
	# Convert the icon to .png for embedding.
	set(ico2png_py "${CMAKE_SOURCE_DIR}/tools/build/ico2png.py")
	set(png "${CMAKE_CURRENT_BINARY_DIR}/${ico_filename}.png")
	add_custom_command(
		OUTPUT "${png}"
		COMMAND "${CMAKE_SOURCE_DIR}/tools/bootstrap/python" "${ico2png_py}" "${ico}" "${png}"
		MAIN_DEPENDENCY "${ico}"
		DEPENDS "${ico2png_py}"
		VERBATIM
	)

	if(WIN32)
		# On Windows, generate a simple .rc file that includes the icon.
		file(GENERATE OUTPUT icon.rc CONTENT "allegro_icon ICON \"${ico}\"")
		target_sources("${target_name}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/icon.rc")
	else()
		# On other platforms, embed the .png version as a document.
		HamSandwich_embed_file("${target_name}" "${png}" embed_game_icon)
	endif()

	if(EMSCRIPTEN)
		# On web, include the icon as a favicon.
		add_custom_command(
			OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/favicon.ico"
			COMMAND cmake -E copy "${ico}" "${CMAKE_CURRENT_BINARY_DIR}/favicon.ico"
			MAIN_DEPENDENCY "${ico}"
			VERBATIM
		)
	endif()

	# Include the .png icon in the launcher metadata.
	get_property(is_excluded DIRECTORY PROPERTY EXCLUDE_FROM_ALL)
	if(NOT "${is_excluded}")
		get_property(launcher_icons GLOBAL PROPERTY HamSandwich_launcher_icons)
		list(APPEND launcher_icons "${target_name}")
		set_property(GLOBAL PROPERTY HamSandwich_launcher_icons "${launcher_icons}")
		# The reason this has to be its own target created at this precise moment is:
		# > If any dependency is an OUTPUT of another custom command in the same directory (CMakeLists.txt file), CMake automatically brings the other custom command into the target in which this command is built.
		add_library("${target_name}_icon" STATIC)
		HamSandwich_embed_file("${target_name}_icon" "${png}" "embed_icon_${target_name}")
	endif()
endfunction()

function(HamSandwich_metadata TARGET_NAME JSON_FILENAME)
	set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${JSON_FILENAME}")
	file(READ "${JSON_FILENAME}" JSON_BLOB)

	string(JSON JSON_BLOB SET "${JSON_BLOB}" "projectName" "\"${TARGET_NAME}\"")
	string(JSON JSON_BLOB SET "${JSON_BLOB}" "appdataName" "\"${TARGET_NAME}\"")
	string(JSON JSON_BLOB SET "${JSON_BLOB}" "hasAssets" false)

	# If the current directory is marked EXCLUDE_FROM_ALL, say so in the metadata.
	get_property(is_excluded DIRECTORY PROPERTY EXCLUDE_FROM_ALL)
	if("${is_excluded}")
		string(JSON JSON_BLOB SET "${JSON_BLOB}" "excluded" true)
	endif()

	# Include this project in the launcher metadata.
	get_property(launcher_metadata GLOBAL PROPERTY HamSandwich_launcher_metadata)
	if("${launcher_metadata}" STREQUAL "")
		set(launcher_metadata [[ { "project_list": [], "project_metadata": {} } ]])
	endif()
	if(NOT "${is_excluded}")
		string(JSON num_projects LENGTH "${launcher_metadata}" project_list)
		string(JSON launcher_metadata SET "${launcher_metadata}" project_list ${num_projects} "\"${TARGET_NAME}\"")
	endif()
	string(JSON launcher_metadata SET "${launcher_metadata}" project_metadata "${TARGET_NAME}" "${JSON_BLOB}")
	set_property(GLOBAL PROPERTY HamSandwich_launcher_metadata "${launcher_metadata}")

	# Generate metadata.cpp
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
	target_sources("${TARGET_NAME}" PRIVATE metadata.cpp)

	if(EMSCRIPTEN)
		file(READ "${CMAKE_SOURCE_DIR}/assets/emscripten/index.html" INDEX_HTML)
		string(REPLACE "__HAMSANDWICH_METADATA__" "${JSON_BLOB}" INDEX_HTML "${INDEX_HTML}")
		file(GENERATE OUTPUT index.html CONTENT "${INDEX_HTML}")
	endif()
endfunction()

function(HamSandwich_install TARGET_NAME)
	if(WIN32 AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set_target_properties("${TARGET_NAME}" PROPERTIES WIN32_EXECUTABLE TRUE)
	endif()

	if(ANDROID)
		set(hamsandwich_assets "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/../../../../hamsandwich-assets")
		# Use a GLOB to reconfigure if a new asset directory is created.
		file(GLOB fake_sources LIST_DIRECTORIES true CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/assets/*")
		if(EXISTS "${CMAKE_SOURCE_DIR}/assets/${TARGET_NAME}")
			add_custom_command(
				TARGET "${TARGET_NAME}" POST_BUILD
				COMMAND
					# Clear out old assets.
					"${CMAKE_COMMAND}" -E rm -rf "${hamsandwich_assets}/assets/${TARGET_NAME}"
				COMMAND
					# Copy assets into a place where Android will see them.
					"${CMAKE_COMMAND}" -E copy_directory
					"${CMAKE_SOURCE_DIR}/assets/${TARGET_NAME}"
					"${hamsandwich_assets}/assets/${TARGET_NAME}"
			)
		else()
			add_custom_command(
				TARGET "${TARGET_NAME}" POST_BUILD
				# Clear out old assets.
				COMMAND "${CMAKE_COMMAND}" -E rm -rf "${hamsandwich_assets}/assets/${TARGET_NAME}"
			)
		endif()
	elseif(EMSCRIPTEN)
		target_link_libraries("${TARGET_NAME}" idbfs.js)
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			target_link_options("${TARGET_NAME}" PRIVATE --emrun)
		endif()

		install(
			FILES "${CMAKE_SOURCE_DIR}/assets/splashes/${TARGET_NAME}.jpg"
			RENAME "splash.jpg"
			DESTINATION "${CMAKE_INSTALL_PREFIX}/${TARGET_NAME}"
		)
		install(
			FILES
			"${CMAKE_SOURCE_DIR}/assets/emscripten/ham.css"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/ham.js"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/jszip.min.js"
			"${CMAKE_CURRENT_BINARY_DIR}/index.html"
			"${CMAKE_CURRENT_BINARY_DIR}/favicon.ico"
			"${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.js"
			"${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.wasm"
			DESTINATION "${CMAKE_INSTALL_PREFIX}/${TARGET_NAME}"
		)

		# Use a GLOB to reconfigure if a new asset directory is created.
		file(GLOB fake_sources LIST_DIRECTORIES true CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/assets/*")
		if(EXISTS "${CMAKE_SOURCE_DIR}/assets/${TARGET_NAME}")
			set(data "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.data")
			set(data_js "${data}.js")
			set(data_d "${data_js}.d")
			add_custom_command(
				OUTPUT "${data}" "${data_js}"
				COMMAND
					"${CMAKE_COMMAND}" -E env
					"EMSCRIPTEN_ROOT_PATH=${EMSCRIPTEN_ROOT_PATH}"
					"${CMAKE_SOURCE_DIR}/tools/bootstrap/python"
					"${CMAKE_SOURCE_DIR}/tools/emscripten/file_packager_deps.py"
					"${data}"
					"--js-output=${data_js}"
					"--from-emcc"
					"--preload"
					"${CMAKE_SOURCE_DIR}/assets/${TARGET_NAME}@"
				DEPENDS
					"${CMAKE_SOURCE_DIR}/dependencies.sh"
					"${CMAKE_SOURCE_DIR}/tools/bootstrap/_common.sh"
					"${CMAKE_SOURCE_DIR}/tools/bootstrap/python"
					"${CMAKE_SOURCE_DIR}/tools/emscripten/file_packager_deps.py"
				DEPFILE "${data_d}"
				VERBATIM
			)
			target_link_options("${TARGET_NAME}" PRIVATE --pre-js "${data_js}" -s FORCE_FILESYSTEM=1)
			target_sources("${TARGET_NAME}" PRIVATE "${data}")

			install(
				FILES "${data}"
				DESTINATION "${CMAKE_INSTALL_PREFIX}/${TARGET_NAME}"
				COMPONENT Assets
			)
		endif()
	else()
		install(TARGETS "${TARGET_NAME}" COMPONENT Executables)
		install(
			DIRECTORY
			"${CMAKE_SOURCE_DIR}/assets/${TARGET_NAME}"
			OPTIONAL
			DESTINATION "${CMAKE_INSTALL_PREFIX}/assets"
			COMPONENT Assets
		)
	endif()
endfunction()
