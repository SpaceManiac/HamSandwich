file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	loonyland
	SOURCES ${sources}
)
target_link_libraries(loonyland steam_api)
