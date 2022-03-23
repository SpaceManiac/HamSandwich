file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

HamSandwich_icon(SOURCES loonyland.rc)
HamSandwich_metadata(loonyland SOURCES loonyland.json)

HamSandwich_add_executable(loonyland ${SOURCES})
target_link_libraries(loonyland ham)

HamSandwich_install(loonyland)
