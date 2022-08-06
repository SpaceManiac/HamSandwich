# Quick-and-dirty JFT tool.
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
add_executable(jfttool ${sources})
target_link_libraries(jfttool lunaticpal_lib SDL2 SDL2_image)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(jfttool PRIVATE -Wall -Wextra)
endif()
