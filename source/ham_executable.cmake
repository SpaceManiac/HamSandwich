function(HamSandwich_embed_file target_name filename symbol_name)
	set(embed_cmake "${CMAKE_SOURCE_DIR}/tools/build/embed.cmake")
	set(cpp "${CMAKE_CURRENT_BINARY_DIR}/${symbol_name}.cpp")
	MiniXxd_add_command("${filename}" "${cpp}" "${symbol_name}")
	target_sources("${target_name}" PRIVATE "${cpp}")
endfunction()

function(HamSandwich_executable_icon target_name ico)
	if(WIN32)
		# On Windows, generate a simple .rc file that includes the icon.
		file(GENERATE OUTPUT icon.rc CONTENT "allegro_icon ICON \"${ico}\"")
		target_sources("${target_name}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/icon.rc")
	else()
		# On other platforms, embed the .ico as a document.
		HamSandwich_embed_file("${target_name}" "${ico}" embed_game_icon)
	endif()
endfunction()

function(HamSandwich_add_executable target_name)
	# target_name
	# opt: ICON whatever.ico, default: ${target_name}.ico
	# opt: JSON whatever.json, default: ${target_name}.json
	# opt: APPDATA_NAME whatever, default: ${target_name}
	# opt: SOURCES foo bar baz, default: none
	cmake_parse_arguments(arg "" "ICON;JSON;APPDATA_NAME" "SOURCES" ${ARGN})

	# Create a shared library on Android or an executable on everything else.
	if(ANDROID)
		add_library("${target_name}" SHARED ${arg_SOURCES})
	else()
		add_executable("${target_name}" ${arg_SOURCES})
	endif()

	# Link libham by default.
	target_link_libraries("${target_name}" ham)

	# On Windows release builds, disable the console.
	if(WIN32 AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set_target_properties("${target_name}" PROPERTIES WIN32_EXECUTABLE TRUE)
	endif()

	# -------------------------------------------------------------------------
	# Include the game's icon.
	if(NOT arg_ICON AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${target_name}.ico")
		set(arg_ICON "${target_name}.ico")
	endif()
	if(arg_ICON)
		# Embed the .ico.
		set(ico "${CMAKE_CURRENT_SOURCE_DIR}/${arg_ICON}")

		if(WIN32)
			# On Windows, generate a simple .rc file that includes the icon.
			file(GENERATE OUTPUT icon.rc CONTENT "allegro_icon ICON \"${ico}\"")
			target_sources("${target_name}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/icon.rc")
		else()
			# On other platforms, embed the .ico as a document.
			HamSandwich_embed_file("${target_name}" "${ico}" embed_game_icon)
		endif()

		set_target_properties("${target_name}" PROPERTIES HamSandwich_ico "${ico}")

		# Include the icon in the launcher metadata.
		get_property(is_excluded DIRECTORY PROPERTY EXCLUDE_FROM_ALL)
		if(NOT "${is_excluded}")
			get_property(launcher_icons GLOBAL PROPERTY HamSandwich_launcher_icons)
			list(APPEND launcher_icons "${target_name}")
			set_property(GLOBAL PROPERTY HamSandwich_launcher_icons "${launcher_icons}")
			# The reason this has to be its own target created at this precise moment is:
			# > If any dependency is an OUTPUT of another custom command in the same directory (CMakeLists.txt file), CMake automatically brings the other custom command into the target in which this command is built.
			add_library("${target_name}_icon" STATIC)
			HamSandwich_embed_file("${target_name}_icon" "${ico}" "embed_icon_${target_name}")
		endif()
	endif()

	# -------------------------------------------------------------------------
	# Include the game's json metadata.
	if(NOT arg_JSON AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${target_name}.json")
		set(arg_JSON "${target_name}.json")
	endif()
	if(arg_JSON)
		set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${arg_JSON}")
		file(READ "${arg_JSON}" json_blob)
	else()
		set(json_blob [[ {} ]])
	endif()

	if(NOT arg_APPDATA_NAME)
		set(arg_APPDATA_NAME "${target_name}")
	endif()
	string(JSON json_blob SET "${json_blob}" "projectName" "\"${target_name}\"")
	string(JSON json_blob SET "${json_blob}" "appdataName" "\"${arg_APPDATA_NAME}\"")

	# If the current directory is marked EXCLUDE_FROM_ALL, say so in the metadata.
	get_property(is_excluded DIRECTORY PROPERTY EXCLUDE_FROM_ALL)
	if("${is_excluded}")
		string(JSON json_blob SET "${json_blob}" "excluded" true)
	endif()

	# Include this project in the launcher metadata.
	get_property(launcher_metadata GLOBAL PROPERTY HamSandwich_launcher_metadata)
	if("${launcher_metadata}" STREQUAL "")
		set(launcher_metadata [[ { "project_list": [], "project_metadata": {} } ]])
	endif()
	if(NOT "${is_excluded}")
		string(JSON num_projects LENGTH "${launcher_metadata}" project_list)
		string(JSON launcher_metadata SET "${launcher_metadata}" project_list ${num_projects} "\"${target_name}\"")
	endif()
	string(JSON launcher_metadata SET "${launcher_metadata}" project_metadata "${target_name}" "${json_blob}")
	set_property(GLOBAL PROPERTY HamSandwich_launcher_metadata "${launcher_metadata}")

	# Generate metadata.cpp
	set(metadata_cpp "")
	string(APPEND metadata_cpp [[
		#include "metadata.h"

		static const AssetSpec default_asset_specs[] = {
	]])

	string(JSON len LENGTH "${json_blob}" installers)
	math(EXPR len "${len} - 1")
	foreach(i RANGE "${len}")
		string(JSON optional ERROR_VARIABLE optional_err GET "${json_blob}" installers ${i} optional)
		if("${optional_err}" STREQUAL "NOTFOUND" AND "${optional}" STREQUAL "ON")  # "error not found" means "ok"
			set(optional "true")
		else()
			set(optional "false")
		endif()

		string(JSON mountpoint ERROR_VARIABLE mountpoint_err GET "${json_blob}" installers ${i} mountpoint)
		if(NOT "${mountpoint_err}" STREQUAL "NOTFOUND")
			set(mountpoint "")
		endif()
		string(JSON kind GET "${json_blob}" installers ${i} kind)
		string(JSON filename GET "${json_blob}" installers ${i} filename)
		string(APPEND metadata_cpp "\t{\"${mountpoint}\", \"${kind}\", \"installers/${filename}\", ${optional} },\n")
	endforeach()

	string(APPEND metadata_cpp [[
			{ nullptr, nullptr, nullptr, false }
		};

		static const HamSandwichMetadata metadata = {
			"]] "${arg_APPDATA_NAME}" [[",
			default_asset_specs
		};

		const HamSandwichMetadata* GetHamSandwichMetadata()
		{
			return &metadata;
		}
	]])

	file(GENERATE OUTPUT metadata.cpp CONTENT "${metadata_cpp}")
	target_sources("${target_name}" PRIVATE metadata.cpp)

	# -------------------------------------------------------------------------
	if(ANDROID)
		set(hamsandwich_assets "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/../../../../hamsandwich-assets")
		# Use a GLOB to reconfigure if a new asset directory is created.
		file(GLOB fake_sources LIST_DIRECTORIES true CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/assets/*")
		if(EXISTS "${CMAKE_SOURCE_DIR}/assets/${target_name}")
			add_custom_command(
				TARGET "${target_name}" POST_BUILD
				COMMAND
					# Clear out old assets.
					"${CMAKE_COMMAND}" -E rm -rf "${hamsandwich_assets}/assets/${target_name}"
				COMMAND
					# Copy assets into a place where Android will see them.
					"${CMAKE_COMMAND}" -E copy_directory
					"${CMAKE_SOURCE_DIR}/assets/${target_name}"
					"${hamsandwich_assets}/assets/${target_name}"
			)
		else()
			add_custom_command(
				TARGET "${target_name}" POST_BUILD
				# Clear out old assets.
				COMMAND "${CMAKE_COMMAND}" -E rm -rf "${hamsandwich_assets}/assets/${target_name}"
			)
		endif()
	elseif(EMSCRIPTEN)
		# Generate index.html from the json metadata.
		set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/assets/emscripten/game.html")
		file(READ "${CMAKE_SOURCE_DIR}/assets/emscripten/game.html" index_html)
		string(REPLACE "__HAMSANDWICH_METADATA__" "${json_blob}" index_html "${index_html}")
		string(REPLACE "__HAMSANDWICH_ID__" "${target_name}" index_html "${index_html}")
		file(GENERATE OUTPUT "${target_name}.html" CONTENT "${index_html}")

		target_link_libraries("${target_name}" idbfs.js)
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			target_link_options("${target_name}" PRIVATE --emrun)
		endif()

		install(
			FILES "${CMAKE_SOURCE_DIR}/assets/splashes/${target_name}.jpg"
			RENAME "${target_name}.splash.jpg"
			DESTINATION "${CMAKE_INSTALL_PREFIX}"
			OPTIONAL
		)
		get_target_property(ico "${target_name}" HamSandwich_ico)
		install(
			FILES
			"${CMAKE_SOURCE_DIR}/assets/emscripten/98/98.css"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/98/98.license.txt"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/98/ms_sans_serif.readme.txt"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/98/ms_sans_serif.license.txt"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/98/ms_sans_serif.woff"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/98/ms_sans_serif.woff2"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/98/ms_sans_serif_bold.readme.txt"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/98/ms_sans_serif_bold.license.txt"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/98/ms_sans_serif_bold.woff"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/98/ms_sans_serif_bold.woff2"
			DESTINATION "98"
			COMPONENT "${target_name}/web"
		)
		install(
			FILES
			"${CMAKE_SOURCE_DIR}/assets/emscripten/ham.css"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/ham.js"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/jszip.min.js"
			"${CMAKE_CURRENT_BINARY_DIR}/${target_name}.html"
			"${CMAKE_CURRENT_BINARY_DIR}/${target_name}.js"
			"${CMAKE_CURRENT_BINARY_DIR}/${target_name}.wasm"
			DESTINATION "."
			COMPONENT "${target_name}/web"
		)
		install(
			FILES "${ico}"
			RENAME "${target_name}.ico"
			DESTINATION "."
			COMPONENT "${target_name}/web"
		)

		# Use a GLOB to reconfigure if a new asset directory is created.
		file(GLOB fake_sources LIST_DIRECTORIES true CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/assets/*")
		file(GLOB_RECURSE any_files "${CMAKE_SOURCE_DIR}/assets/${target_name}/*")
		if(any_files)
			find_program(python NAMES python3 python REQUIRED)
			set(data "${CMAKE_CURRENT_BINARY_DIR}/${target_name}.data")
			set(data_js "${data}.js")
			set(data_d "${data_js}.d")
			add_custom_command(
				OUTPUT "${data}" "${data_js}"
				COMMAND
					"${CMAKE_COMMAND}" -E env
					"EMSCRIPTEN_ROOT_PATH=${EMSCRIPTEN_ROOT_PATH}"
					"${python}"
					"${CMAKE_SOURCE_DIR}/tools/emscripten/file_packager_deps.py"
					"${data}"
					"--js-output=${data_js}"
					"--from-emcc"
					"--preload"
					"${CMAKE_SOURCE_DIR}/assets/${target_name}@"
				DEPENDS
					"${CMAKE_SOURCE_DIR}/tools/emscripten/file_packager_deps.py"
				DEPFILE "${data_d}"
				VERBATIM
			)
			target_link_options("${target_name}" PRIVATE --pre-js "${data_js}" -s FORCE_FILESYSTEM=1)
			target_sources("${target_name}" PRIVATE "${data}")

			install(
				FILES "${data}"
				DESTINATION "."
				COMPONENT "${target_name}/web"
			)
		endif()
	else()
		# Copy assets on install.
		install(TARGETS "${target_name}" COMPONENT "${target_name}/executables")
		install(
			DIRECTORY
			"${CMAKE_SOURCE_DIR}/assets/${target_name}"
			OPTIONAL
			DESTINATION "assets"
			COMPONENT "${target_name}/assets"
		)
	endif()
endfunction()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	function(gnu_compile_options)
		target_compile_options(${ARGN})
	endfunction()
	function(msvc_compile_options)
	endfunction()
elseif(MSVC)
	function(gnu_compile_options)
	endfunction()
	function(msvc_compile_options)
		target_compile_options(${ARGN})
	endfunction()
else()
	function(gnu_compile_options)
	endfunction()
	function(msvc_compile_options)
	endfunction()
endif()
