file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(launcher ${SOURCES})
target_link_libraries(launcher SDL2 imgui)

if(NOT MSVC)
	target_link_libraries(launcher stdc++fs)
endif()
