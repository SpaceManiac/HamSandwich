include("zlib.cmake")
include("SDL2.cmake")
include("imgui.cmake")
add_subdirectory("lzma1900" EXCLUDE_FROM_ALL)
add_subdirectory("nsis_exehead" EXCLUDE_FROM_ALL)

if(NOT EMSCRIPTEN AND NOT ANDROID)
	add_subdirectory("nlohmann_json")

	set(HTTP_ONLY TRUE)
	add_subdirectory("curl" EXCLUDE_FROM_ALL)
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/curl/COPYING" TYPE BIN COMPONENT Executables RENAME "LICENSE.libcurl.txt")
endif()
