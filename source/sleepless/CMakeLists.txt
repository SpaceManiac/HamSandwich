file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	sleepless
	ICON sleepless.ico
	SOURCES ${sources}
)

if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$" AND NOT MSVC)
	target_compile_options(sleepless PRIVATE
		-Wall -Wextra -Wpedantic
		-Wno-missing-field-initializers
		-Wno-unused-parameter
		-Wno-unused-but-set-variable
	)
endif()
