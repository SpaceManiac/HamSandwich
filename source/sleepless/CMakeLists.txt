file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

HamSandwich_metadata(sleepless SOURCES sleepless.json)

add_executable(sleepless ${SOURCES})
target_link_libraries(sleepless ham)

target_compile_options(sleepless PRIVATE -Wall -Wno-unused-but-set-variable)
