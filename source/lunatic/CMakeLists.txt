file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	lunatic
	SOURCES ${sources}
)

target_compile_definitions(lunatic PRIVATE EXPANDO)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(lunatic PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()
