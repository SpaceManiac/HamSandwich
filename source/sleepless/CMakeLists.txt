file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

HamSandwich_icon(SOURCES lunatic.rc)
HamSandwich_metadata(sleepless SOURCES sleepless.json)

HamSandwich_add_executable(sleepless ${SOURCES})
target_link_libraries(sleepless ham)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(sleepless PRIVATE -Wall -Wno-unused-but-set-variable)
endif()

HamSandwich_install(sleepless)
