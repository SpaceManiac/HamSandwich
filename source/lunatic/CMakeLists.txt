file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

HamSandwich_metadata(lunatic SOURCES lunatic.json)

add_executable(lunatic ${SOURCES})
target_link_libraries(lunatic ham)

target_compile_options(lunatic PRIVATE -Wall -Wextra -Wno-unused-parameter)
