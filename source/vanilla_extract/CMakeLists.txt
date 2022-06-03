file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
add_library(vanilla_extract STATIC ${sources})
target_link_libraries(vanilla_extract SDL2 lzma1900 nsis_exehead minizip)
target_include_directories(vanilla_extract INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(vanilla_extract PRIVATE -fPIC)
endif()
