file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

HamSandwich_icon(SOURCES lunatic.rc)
HamSandwich_metadata(lunatic SOURCES lunatic.json)

add_executable(lunatic ${SOURCES})
target_link_libraries(lunatic ham)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(lunatic PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()

HamSandwich_install(lunatic)
