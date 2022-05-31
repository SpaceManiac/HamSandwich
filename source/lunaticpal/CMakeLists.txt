# Dr. L palette conversion tool
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
add_executable(lunaticpal ${sources})
target_link_libraries(lunaticpal SDL2 SDL2_image tinyfiledialogs)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(lunaticpal PRIVATE -Wall -Wextra -Wno-missing-field-initializers)
endif()

if(WIN32 AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set_target_properties(lunaticpal PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
