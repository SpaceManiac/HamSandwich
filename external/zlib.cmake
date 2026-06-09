# zlib target
set(zlib "${CMAKE_CURRENT_LIST_DIR}/zlib")

set(ZLIB_BUILD_SHARED OFF)
set(ZLIB_BUILD_TESTING OFF)
add_subdirectory("${zlib}" "${CMAKE_CURRENT_BINARY_DIR}/zlib" EXCLUDE_FROM_ALL)
add_library(z ALIAS zlibstatic)

add_library(minizip STATIC
	"${zlib}/contrib/minizip/zip.c"
	"${zlib}/contrib/minizip/unzip.c"
	"${zlib}/contrib/minizip/ioapi.c")
target_compile_definitions(minizip PUBLIC NOCRYPT)
target_include_directories(minizip PUBLIC "${zlib}/contrib/minizip")
target_link_libraries(minizip PUBLIC z)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	if(NOT MSVC)
		target_compile_options(minizip PRIVATE -fPIC)
		target_compile_options(zlibstatic PRIVATE -fPIC)
	endif()
	target_compile_options(minizip PRIVATE -Wno-unused-value)
endif()
if(ANDROID)
	target_compile_definitions(minizip PRIVATE IOAPI_NO_64)
endif()
