file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(lunatic ${sources})
target_link_libraries(lunatic ham)

HamSandwich_icon(lunatic lunatic.ico)
HamSandwich_metadata(lunatic lunatic.json)
HamSandwich_install(lunatic)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(lunatic PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()
