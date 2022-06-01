# Launcher executable.
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(jspedit ${sources})
target_link_libraries(jspedit SDL2 SDL2_image SDL2_ttf tinyfiledialogs)

# Embed the font.
HamSandwich_embed_file(jspedit "${CMAKE_CURRENT_SOURCE_DIR}/vera.ttf" embed_vera)
HamSandwich_embed_file(jspedit "${CMAKE_CURRENT_SOURCE_DIR}/fontawesome.ttf" embed_fontawesome)
HamSandwich_executable_icon(jspedit "${CMAKE_CURRENT_SOURCE_DIR}/bomb.ico")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(jspedit PRIVATE -Wall -Wextra)
endif()

if(WIN32 AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set_target_properties(jspedit PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
