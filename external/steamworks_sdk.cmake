# The steamworks_sdk subproject exports one thing we care about, the steam_api library.
set(steamworks_sdk "${CMAKE_CURRENT_SOURCE_DIR}/steamworks_sdk")
add_library(steam_api INTERFACE)

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${steamworks_sdk}/Readme.txt")

if(ANDROID OR EMSCRIPTEN)
	# Nothing.
elseif(NOT EXISTS "${steamworks_sdk}")
	# No warning, just a message, because all the Itch builds are like this.
	message("To build with Steamworks SDK, extract it to: ${steamworks_sdk}")
elseif(WIN32)
	target_include_directories(steam_api SYSTEM INTERFACE "${steamworks_sdk}/public")
	target_compile_definitions(steam_api INTERFACE HAS_STEAM_API)

	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		target_link_libraries(steam_api INTERFACE "${steamworks_sdk}/redistributable_bin/steam_api.lib")
		install(FILES "${steamworks_sdk}/redistributable_bin/steam_api.dll" TYPE BIN COMPONENT generic/executables)
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		target_link_libraries(steam_api INTERFACE "${steamworks_sdk}/redistributable_bin/win64/steam_api64.lib")
		install(FILES "${steamworks_sdk}/redistributable_bin/win64/steam_api64.dll" TYPE BIN COMPONENT generic/executables)
	endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_include_directories(steam_api SYSTEM INTERFACE "${steamworks_sdk}/public")
	target_compile_definitions(steam_api INTERFACE HAS_STEAM_API)

	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		target_link_libraries(steam_api INTERFACE "${steamworks_sdk}/redistributable_bin/linux32/libsteam_api.so")
		install(FILES "${steamworks_sdk}/redistributable_bin/linux32/libsteam_api.so" TYPE BIN COMPONENT generic/executables)
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		target_link_libraries(steam_api INTERFACE "${steamworks_sdk}/redistributable_bin/linux64/libsteam_api.so")
		install(FILES "${steamworks_sdk}/redistributable_bin/linux64/libsteam_api.so" TYPE BIN COMPONENT generic/executables)
	endif()
else()
	message(WARNING "Steamworks SDK not available for current platform: ${CMAKE_SYSTEM_NAME}")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	# STEAM_CALLBACK macro warns about this, but it seems fine in practice.
	# Either way it's not for us to fix.
	target_compile_options(steam_api INTERFACE -Wno-invalid-offsetof)
endif()
