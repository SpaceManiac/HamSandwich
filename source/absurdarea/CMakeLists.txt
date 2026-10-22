file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	absurdarea
	SOURCES ${sources}
)
target_compile_definitions(absurdarea PRIVATE DIRECTORS)

install(FILES "LICENSE.txt" RENAME "LICENSE.AbsurdArea.txt" TYPE BIN COMPONENT generic/executables)
