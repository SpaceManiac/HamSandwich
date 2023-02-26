# zlib target
set(zlib "${CMAKE_CURRENT_SOURCE_DIR}/zlib")

if(EMSCRIPTEN)
	add_library(z INTERFACE)
	target_compile_options(z INTERFACE -sUSE_ZLIB=1)
	target_link_options(z INTERFACE -sUSE_ZLIB=1)
elseif(MSVC)
	add_library(z INTERFACE)
	add_subdirectory("zlib" EXCLUDE_FROM_ALL)
	target_link_libraries(z INTERFACE zlibstatic)
	target_include_directories(z INTERFACE "${zlib}" "${CMAKE_CURRENT_BINARY_DIR}/zlib")
endif()

add_library(minizip STATIC
	"${zlib}/contrib/minizip/unzip.c"
	"${zlib}/contrib/minizip/ioapi.c")
target_include_directories(minizip PUBLIC "${zlib}/contrib/minizip")
target_link_libraries(minizip PUBLIC z)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(minizip PRIVATE -fPIC)
endif()
if(ANDROID)
	target_compile_definitions(minizip PRIVATE IOAPI_NO_64)
endif()
