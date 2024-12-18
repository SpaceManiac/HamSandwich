file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
add_library(aegea STATIC ${sources})
target_include_directories(aegea PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT MSVC)
	target_compile_options(aegea PRIVATE -fPIC -Wall -Wextra -Wpedantic)
endif()

if(EMSCRIPTEN)
	target_compile_definitions(aegea PRIVATE AEGEA_WEBSOCKET_EMSCRIPTEN)
	target_link_libraries(aegea websocket.js)
else()
	target_compile_definitions(aegea PRIVATE AEGEA_WEBSOCKET_CURL)
	target_link_libraries(aegea PRIVATE libcurl ham) # ham for owned_curl
endif()
