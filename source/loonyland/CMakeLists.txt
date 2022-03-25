file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(loonyland ${sources})
target_link_libraries(loonyland ham)

HamSandwich_icon(loonyland loonyland.ico)
HamSandwich_metadata(loonyland loonyland.json)
HamSandwich_install(loonyland)
