file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	supreme
	ICON lunatic.ico
	SOURCES ${sources}
)
target_link_libraries(supreme steam_api)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(supreme PRIVATE
		# Lots of warnings, but some exclusions
		-Wall -Wextra -Wpedantic
		-Wno-unused-parameter -Wno-unused-but-set-variable
		# STEAM_CALLBACK macro warns about this, but it seems fine in practice.
		-Wno-invalid-offsetof
	)
endif()
