file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	sleepless
	ICON sleepless.ico
	SOURCES ${sources}
)

if(CMAKE_COMPILER_IS_GNUCC)
	target_compile_options(sleepless PRIVATE -Wall -Wextra -Wno-missing-field-initializers -Wno-unused-parameter -Wno-unused-but-set-variable)
endif()
