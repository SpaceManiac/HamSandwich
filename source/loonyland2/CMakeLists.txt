file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(loonyland2 ${sources})
target_link_libraries(loonyland2 ham)
target_compile_definitions(loonyland2 PRIVATE DIRECTORS)

HamSandwich_icon(loonyland2 loonyland2.ico)
HamSandwich_metadata(loonyland2 loonyland2.json)
HamSandwich_install(loonyland2)
