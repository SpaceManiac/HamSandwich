file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

HamSandwich_icon(SOURCES mystic.rc)
HamSandwich_metadata(mystic SOURCES mystic.json)

add_executable(mystic ${SOURCES})
target_link_libraries(mystic ham)

HamSandwich_install(mystic)
