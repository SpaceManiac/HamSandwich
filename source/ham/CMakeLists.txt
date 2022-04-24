file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
if(WIN32 OR EMSCRIPTEN)
	# TODO: Must explicitly declare exported symbols on Windows before it can
	# be a shared library.
	set(ham_library_type STATIC)
else()
	set(ham_library_type SHARED)
endif()
add_library(ham ${ham_library_type} ${sources})
target_link_libraries(ham minizip vanilla_extract SDL2 SDL2_image SDL2_mixer)
if(ANDROID)
	target_link_libraries(ham log)
endif()
target_include_directories(ham INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

if(ham_library_type STREQUAL "SHARED")
	install(TARGETS ham LIBRARY RUNTIME COMPONENT generic/executables)
endif()
