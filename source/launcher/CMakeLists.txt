file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

get_property(launcher_metadata GLOBAL PROPERTY HamSandwich_launcher_metadata)
file(GENERATE OUTPUT launcher.json CONTENT "${launcher_metadata}")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/launcher.json" TYPE BIN COMPONENT Executables)

add_executable(launcher ${SOURCES})
target_link_libraries(launcher SDL2 imgui libcurl nlohmann_json)

if(NOT MSVC)
	target_link_libraries(launcher stdc++fs)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(launcher PRIVATE -Wall -Wextra -Wno-missing-field-initializers)
endif()

if(WIN32)
	set_target_properties(launcher PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

install(TARGETS libcurl launcher COMPONENT Executables)
