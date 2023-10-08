# Flags that apply to our code only, not external code.
if(MSVC)
	add_compile_options(
		-D_CRT_SECURE_NO_WARNINGS
		-DNOMINMAX
	)
elseif(EMSCRIPTEN)
	# Emscripten essentials
	add_link_options(
		-sALLOW_MEMORY_GROWTH=1
		--use-preload-cache
		-sENVIRONMENT=web
		-sFORCE_FILESYSTEM=1
		-sEXTRA_EXPORTED_RUNTIME_METHODS=['ENV']
	)
	# Coroutine support
	add_compile_options(
		-DUSE_COROUTINES
		-fcoroutines-ts
		-Werror=unused-result
	)
elseif(ANDROID)
	add_compile_options(
		-fsigned-char
		-fexceptions
	)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	link_libraries(ws2_32 winmm)
endif()

# -----------------------------------------------------------------------------
# Libraries
include("ham_executable.cmake")
#add_subdirectory("mini_xxd")
add_subdirectory("vanilla_extract")
add_subdirectory("ham")

# For games only, use -no-pie so that the executables are double-clickable in Thunar.
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	add_link_options(
		-no-pie
	)
endif()

# -----------------------------------------------------------------------------
# Games
# Use EXCLUDE_FROM_ALL to exclude a game from the launcher and distribution.
# Comment out an entry to exclude it from the build entirely.
# The order here is the order that games will appear in the launcher.
add_subdirectory("spookycastle")  # 1998-12, re-release 2004-05
add_subdirectory("eddie")  # 1999-03, re-release 2004-06
add_subdirectory("amazin")  # 1999-07
add_subdirectory("mystic")  # 1999-08, re-release 2004-10
add_subdirectory("lunatic" EXCLUDE_FROM_ALL)  # 2000-05
add_subdirectory("loonyland")  # 2001-03
add_subdirectory("stockboy")  # 2002-06
add_subdirectory("supreme")  # 2003-08
add_subdirectory("loonyland2")  # 2007-02
add_subdirectory("sleepless")  # 2007-10

# -----------------------------------------------------------------------------
# Developer Tools
if(NOT EMSCRIPTEN AND NOT ANDROID)
	add_subdirectory("lunaticpal" EXCLUDE_FROM_ALL)
	add_subdirectory("jspedit")
	add_subdirectory("jfttool" EXCLUDE_FROM_ALL)
endif()

# -----------------------------------------------------------------------------
# Launcher
if(EMSCRIPTEN)
	# Assets for web distribution
	get_property(launcher_metadata GLOBAL PROPERTY HamSandwich_launcher_metadata)
	string(JSON num_projects LENGTH "${launcher_metadata}" "project_list")
	if (num_projects EQUAL 1)
		# Exactly one game: rename its .html to index.html.
		string(JSON target_name GET "${launcher_metadata}" "project_list" 0)
		install(CODE "file(RENAME \${CMAKE_INSTALL_PREFIX}/${target_name}.html \${CMAKE_INSTALL_PREFIX}/index.html)")
	else()
		# More than one game: create a root index.html listing all games.
		set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/assets/emscripten/index.html")
		file(READ "${CMAKE_SOURCE_DIR}/assets/emscripten/index.html" index_html)
		string(REPLACE "__HOMEPAGE_METADATA__" "${launcher_metadata}" index_html "${index_html}")
		file(GENERATE OUTPUT index.html CONTENT "${index_html}")

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
			DESTINATION "${CMAKE_INSTALL_PREFIX}/98"
			COMPONENT "${target_name}/web"
		)
		install(
			FILES
			"${CMAKE_SOURCE_DIR}/assets/emscripten/ham.css"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/shortcut.png"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/yerfdog.png"
			"${CMAKE_SOURCE_DIR}/assets/emscripten/github.png"
			"${CMAKE_CURRENT_BINARY_DIR}/index.html"
			DESTINATION "${CMAKE_INSTALL_PREFIX}"
		)
		install(
			FILES "${CMAKE_CURRENT_SOURCE_DIR}/supreme/lunatic.ico"
			RENAME "favicon.ico"
			DESTINATION "${CMAKE_INSTALL_PREFIX}"
		)
	endif()
elseif(ANDROID)
	# Assets for Android build
	# This folder corresponds to the one set in `android-project/build.gradle`. It erases the
	# distinction between ABIs and debug/release, but the assets are the same so it's fine.
	set(hamsandwich_assets "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/../../../../hamsandwich-assets")

	get_property(launcher_metadata GLOBAL PROPERTY HamSandwich_launcher_metadata)
	file(GENERATE OUTPUT "${hamsandwich_assets}/launcher.json" CONTENT "${launcher_metadata}")
else()
	# Launcher for desktop platforms
	add_subdirectory("launcher")
endif()
