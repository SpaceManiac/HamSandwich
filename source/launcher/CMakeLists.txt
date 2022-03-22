file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(launcher ${SOURCES})
target_link_libraries(launcher SDL2 imgui libcurl nlohmann_json)

if(NOT MSVC)
	target_link_libraries(launcher stdc++fs)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(launcher PRIVATE -Wall -Wextra)
endif()
