file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	loonyland
	SOURCES ${sources}
)
if (MSVC AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_link_options(loonyland PRIVATE /ZI /INCREMENTAL)
	target_compile_options(loonyland PRIVATE /ZI)
endif()
target_link_libraries(loonyland steam_api aegea jtjson)
