file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(sleepless ${sources})
target_link_libraries(sleepless ham)

HamSandwich_icon(sleepless lunatic.ico)
HamSandwich_metadata(sleepless sleepless.json)
HamSandwich_install(sleepless)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(sleepless PRIVATE -Wall -Wno-unused-but-set-variable)
endif()
