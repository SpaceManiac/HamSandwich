file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	loonyland2
	SOURCES ${sources}
)
target_compile_definitions(loonyland2 PRIVATE DIRECTORS)
