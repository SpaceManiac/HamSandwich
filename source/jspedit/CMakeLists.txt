# Launcher executable.
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(jspedit ${sources})
target_link_libraries(jspedit lunaticpal_lib SDL2 SDL2_image SDL2_ttf tinyfiledialogs)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(jspedit PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(WIN32 AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set_target_properties(jspedit PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Embed the font.
HamSandwich_embed_file(jspedit "${CMAKE_CURRENT_SOURCE_DIR}/vera.ttf" embed_vera)
HamSandwich_embed_file(jspedit "${CMAKE_CURRENT_SOURCE_DIR}/fontawesome.ttf" embed_fontawesome)
HamSandwich_executable_icon(jspedit "${CMAKE_CURRENT_SOURCE_DIR}/bomb.ico")

# Embed icon in the launcher.
add_library(jspedit_icon STATIC)
HamSandwich_embed_file(jspedit_icon "${CMAKE_CURRENT_SOURCE_DIR}/bomb.ico" embed_icon_jspedit)
get_property(launcher_icons GLOBAL PROPERTY HamSandwich_launcher_icons)
list(APPEND launcher_icons "jspedit")
set_property(GLOBAL PROPERTY HamSandwich_launcher_icons "${launcher_icons}")

# Install
install(TARGETS jspedit COMPONENT "jspedit/executables")
if(NOT WIN32)
	# Install lunaticpal_lib only if it's shared.
	install(TARGETS lunaticpal_lib COMPONENT "jspedit/executables")
endif()
