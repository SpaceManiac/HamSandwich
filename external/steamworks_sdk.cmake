# The steamworks_sdk subproject exports one thing we care about, the steam_api library.
set(STEAMWORKS_SDK "${CMAKE_CURRENT_SOURCE_DIR}/steamworks_sdk")
add_library(steam_api INTERFACE)

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${STEAMWORKS_SDK}/Readme.txt")

if(ANDROID OR EMSCRIPTEN)
	# Nothing.
elseif(NOT EXISTS "${STEAMWORKS_SDK}")
	# No warning, just a message, because all the Itch builds are like this.
	message("To build with Steamworks SDK, extract it to: ${STEAMWORKS_SDK}")
elseif(WIN32)
	target_include_directories(steam_api INTERFACE "${STEAMWORKS_SDK}/public")
	target_compile_definitions(steam_api INTERFACE HAS_STEAM_API)

	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		target_link_libraries(steam_api INTERFACE "${STEAMWORKS_SDK}/redistributable_bin/steam_api.lib")
		install(FILES "${STEAMWORKS_SDK}/redistributable_bin/steam_api.dll" TYPE BIN COMPONENT Executables)
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		target_link_libraries(steam_api INTERFACE "${STEAMWORKS_SDK}/redistributable_bin/win64/steam_api64.lib")
		install(FILES "${STEAMWORKS_SDK}/redistributable_bin/win64/steam_api64.dll" TYPE BIN COMPONENT Executables)
	endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_include_directories(steam_api INTERFACE "${STEAMWORKS_SDK}/public")
	target_compile_definitions(steam_api INTERFACE HAS_STEAM_API)

	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		target_link_libraries(steam_api INTERFACE "${STEAMWORKS_SDK}/redistributable_bin/linux32/libsteam_api.so")
		install(FILES "${STEAMWORKS_SDK}/redistributable_bin/linux32/libsteam_api.so" TYPE BIN COMPONENT Executables)
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		target_link_libraries(steam_api INTERFACE "${STEAMWORKS_SDK}/redistributable_bin/linux64/libsteam_api.so")
		install(FILES "${STEAMWORKS_SDK}/redistributable_bin/linux64/libsteam_api.so" TYPE BIN COMPONENT Executables)
	endif()
else()
	message(WARNING "Steamworks SDK not available for current platform: ${CMAKE_SYSTEM_NAME}")
endif()
