file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	supreme
	ICON lunatic.ico
	APPDATA_NAME OperationScare
	SOURCES ${sources}
)

if(CMAKE_COMPILER_IS_GNUCC)
	target_compile_options(supreme PRIVATE -Wall -Wno-unused-but-set-variable)
endif()
