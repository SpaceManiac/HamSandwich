file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(supreme ${sources})
target_link_libraries(supreme ham)

HamSandwich_icon(supreme lunatic.ico)
HamSandwich_metadata(supreme supreme.json)
HamSandwich_install(supreme)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(supreme PRIVATE -Wall -Wno-unused-but-set-variable)
endif()
