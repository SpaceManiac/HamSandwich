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

# Libraries
add_subdirectory("vanilla_extract")
add_subdirectory("ham")

# For games only, use -no-pie so that the executables are double-clickable in Thunar.
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	add_link_options(
		-no-pie
	)
endif()

# Games
# Use EXCLUDE_FROM_ALL to exclude a game from the launcher and distribution.
# Comment out an entry to exclude it from the build entirely.
# The order here is the order that games will appear in the launcher.
add_subdirectory("lunatic" EXCLUDE_FROM_ALL)
add_subdirectory("supreme")
add_subdirectory("mystic")
add_subdirectory("loonyland")
add_subdirectory("loonyland2")
add_subdirectory("sleepless")

# Launcher
if(EMSCRIPTEN)
	# Assets for web distribution
	get_property(launcher_metadata GLOBAL PROPERTY HamSandwich_launcher_metadata)
	file(READ "${CMAKE_SOURCE_DIR}/assets/homepage/index.html" index_html)
	string(REPLACE "__HOMEPAGE_METADATA__" "${launcher_metadata}" index_html "${index_html}")
	file(GENERATE OUTPUT index.html CONTENT "${index_html}")

	install(FILES
		"${CMAKE_CURRENT_BINARY_DIR}/index.html"
		DESTINATION "${CMAKE_INSTALL_PREFIX}"
	)
	install(
		FILES "${CMAKE_CURRENT_SOURCE_DIR}/supreme/lunatic.ico"
		RENAME "favicon.ico"
		DESTINATION "${CMAKE_INSTALL_PREFIX}"
	)
elseif(ANDROID)
	# Assets for Android build
	# This folder corresponds to the one set in `android-project/build.gradle`. It erases the
	# distinction between ABIs and debug/release, but the assets are the same so it's fine.
	set(hamsandwich_assets "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/../../../../hamsandwich-assets")

	file(GENERATE OUTPUT "${hamsandwich_assets}/test.txt" CONTENT "Hello from CMake")
	message("put it at: ${hamsandwich_assets}/test.txt")
	#set_target_properties(ham PROPERTIES ANDROID_ASSETS_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/android-assets")
else()
	# Launcher for desktop platforms
	add_subdirectory("launcher")
endif()
