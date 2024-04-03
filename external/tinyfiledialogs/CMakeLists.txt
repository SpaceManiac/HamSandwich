file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
add_library(tinyfiledialogs STATIC ${sources})
target_include_directories(tinyfiledialogs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
