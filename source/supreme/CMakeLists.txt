file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	supreme
	ICON lunatic.ico
	SOURCES ${sources}
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(supreme PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-but-set-variable)
endif()
