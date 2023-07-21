# Launcher executable.
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(launcher ${sources})
target_link_libraries(launcher ham SDL2 SDL2_image imgui libcurl nlohmann_json)

# Embed the font.
HamSandwich_embed_file(launcher "${CMAKE_SOURCE_DIR}/assets/launcher/verdana.jft" embed_verdana)

# Embed the metadata json.
get_property(launcher_metadata GLOBAL PROPERTY HamSandwich_launcher_metadata)
file(GENERATE OUTPUT launcher.json CONTENT "${launcher_metadata}")
HamSandwich_embed_file(launcher launcher.json embed_launcher_json)

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
target_link_libraries(launcher ${icon_libs})

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
target_sources(launcher PRIVATE embed_game_icons.cpp)

# Embed a Git version number.
find_program(git git)
if(NOT "${git}" STREQUAL "git-NOTFOUND")
	execute_process(COMMAND "${git}" rev-parse HEAD OUTPUT_VARIABLE git_version)
	string(STRIP "${git_version}" git_version)
	target_compile_definitions(launcher PRIVATE GIT_VERSION="${git_version}")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_libraries(launcher stdc++fs)
	target_compile_options(launcher PRIVATE -Wall -Wextra -Wpedantic -Wno-missing-field-initializers)
endif()

if(WIN32 AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set_target_properties(launcher PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

install(TARGETS launcher COMPONENT launcher/executables)
if(NOT APPLE)
	install(TARGETS libcurl ARCHIVE EXCLUDE_FROM_ALL COMPONENT launcher/executables)
endif()
