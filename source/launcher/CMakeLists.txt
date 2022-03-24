file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

get_property(launcher_metadata GLOBAL PROPERTY HamSandwich_launcher_metadata)
file(GENERATE OUTPUT launcher.json CONTENT "${launcher_metadata}")
HamSandwich_embed_file(SOURCES launcher.json embed_launcher_json)

add_executable(launcher ${SOURCES})
target_link_libraries(launcher SDL2 imgui libcurl nlohmann_json)

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
