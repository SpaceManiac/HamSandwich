file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

HamSandwich_icon(SOURCES loonyland2.rc)
HamSandwich_metadata(loonyland2 SOURCES loonyland2.json)

add_executable(loonyland2 ${SOURCES})
target_link_libraries(loonyland2 ham)
target_compile_definitions(loonyland2 PRIVATE DIRECTORS)
