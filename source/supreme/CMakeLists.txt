file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

HamSandwich_metadata(supreme SOURCES supreme.json)

add_executable(supreme ${SOURCES})
target_link_libraries(supreme ham)

target_compile_options(supreme PRIVATE -Wall -Wno-unused-but-set-variable)
