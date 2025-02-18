file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	supreme
	ICON lunatic.ico
	SOURCES ${sources}
)
target_sources(supreme PRIVATE supreme.manifest)
target_link_libraries(supreme steam_api minizip)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT MSVC)
	target_compile_options(supreme PRIVATE
		# Lots of warnings, but some exclusions
		-Wall -Wextra -Wpedantic
		-Wno-unused-parameter -Wno-unused-but-set-variable -Wno-unknown-warning-option
	)
endif()
