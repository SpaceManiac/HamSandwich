file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	stockboy
	ICON stockboy.ico
	SOURCES ${sources}
)
