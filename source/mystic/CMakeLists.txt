file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(mystic ${sources})
target_link_libraries(mystic ham)

HamSandwich_icon(mystic mystic.ico)
HamSandwich_metadata(mystic mystic.json)
HamSandwich_install(mystic)
