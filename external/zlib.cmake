# zlib target
if(EMSCRIPTEN)
	add_library(z INTERFACE)
	target_compile_options(z INTERFACE -sUSE_ZLIB=1)
	target_link_options(z INTERFACE -sUSE_ZLIB=1)
elseif(MSVC)
	add_library(z INTERFACE)
	add_subdirectory("zlib" EXCLUDE_FROM_ALL)
	target_link_libraries(z INTERFACE zlibstatic)
	target_include_directories(z INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/zlib" "${CMAKE_CURRENT_BINARY_DIR}/zlib")
endif()

add_library(minizip STATIC
	"${CMAKE_CURRENT_SOURCE_DIR}/zlib/contrib/minizip/unzip.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/zlib/contrib/minizip/ioapi.c")
target_include_directories(minizip PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/zlib/contrib/minizip")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(minizip PRIVATE -fPIC)
endif()
if(ANDROID)
	target_compile_definitions(minizip PRIVATE IOAPI_NO_64)
endif()
