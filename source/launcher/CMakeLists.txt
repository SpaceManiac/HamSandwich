file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Embed the metadata json.
get_property(launcher_metadata GLOBAL PROPERTY HamSandwich_launcher_metadata)
file(GENERATE OUTPUT launcher.json CONTENT "${launcher_metadata}")
HamSandwich_embed_file(SOURCES launcher.json embed_launcher_json)

# Embed each game's icon in the launcher.
get_property(launcher_icons GLOBAL PROPERTY HamSandwich_launcher_icons)
set(cpp_contents_1 "")
set(cpp_contents_2 "")
foreach(game ${launcher_icons})
	list(APPEND icon_libs "${game}_icon")
	string(APPEND cpp_contents_1 "
		extern const unsigned char embed_icon_${game}[];
		extern const size_t embed_icon_${game}_size;
	")
	string(APPEND cpp_contents_2 "
		{ \"${game}\", embed_icon_${game}, embed_icon_${game}_size },
	")
endforeach()

file(GENERATE OUTPUT embed_game_icons.cpp CONTENT "
	#include <stdlib.h>
	struct Icon
	{
		const char* name;
		const unsigned char* data;
		const size_t size;
	};
	${cpp_contents_1}
	extern const Icon embed_icons[] =
	{
		${cpp_contents_2}
		{ nullptr }
	};
")
list(APPEND SOURCES embed_game_icons.cpp)

# Build the executable proper.
add_executable(launcher ${SOURCES})
target_link_libraries(launcher SDL2 SDL2_image imgui libcurl nlohmann_json)
target_link_libraries(launcher ${icon_libs})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_libraries(launcher stdc++fs)
	target_compile_options(launcher PRIVATE -Wall -Wextra -Wno-missing-field-initializers)
endif()

if(WIN32 AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set_target_properties(launcher PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

install(TARGETS launcher COMPONENT Executables)
if(NOT APPLE)
	install(TARGETS libcurl COMPONENT Executables)
endif()
