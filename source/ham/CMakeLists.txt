file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
if(EMSCRIPTEN)
	set(ham_library_type STATIC)
else()
	set(ham_library_type SHARED)
endif()
add_library(ham ${ham_library_type} ${sources})
target_link_libraries(ham minizip vanilla_extract SDL3 SDL3_image SDL3_mixer)
if(ANDROID)
	target_link_libraries(ham log)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT EMSCRIPTEN AND NOT MSVC)
	target_link_libraries(ham m stdc++)
endif()
target_include_directories(ham INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

if(ham_library_type STREQUAL "SHARED")
	if(WIN32)
		target_compile_definitions(ham PRIVATE "HAM_API=__declspec(dllexport)")
		target_compile_definitions(ham INTERFACE "HAM_API=__declspec(dllimport)")
	endif()

	install(
		TARGETS ham
		ARCHIVE EXCLUDE_FROM_ALL
		LIBRARY COMPONENT generic/executables
		RUNTIME COMPONENT generic/executables
	)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$" AND NOT MSVC)
	target_compile_options(ham PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(EMSCRIPTEN)
	target_compile_options(ham PRIVATE -Wno-dollar-in-identifier-extension -Wno-gnu-zero-variadic-macro-arguments)
endif()
