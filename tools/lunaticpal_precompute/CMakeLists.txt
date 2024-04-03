# -----------------------------------------------------------------------------
# A precomputer which generates a compressed 16MiB RGB -> 8-bit lookup table.
add_executable(lunaticpal_precompute ../../source/lunaticpal/precompute.main.cpp)
target_link_libraries(lunaticpal_precompute z)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(lunaticpal_precompute PRIVATE -Wall -Wextra -Wpedantic -Wno-missing-field-initializers)
	# Always optimize, even in Debug mode, because otherwise we're just wasting time.
	target_compile_options(lunaticpal_precompute PRIVATE -O3)
endif()

export_tool(lunaticpal_precompute)
export_script("command.cmake")
