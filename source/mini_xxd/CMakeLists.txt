add_executable(mini_xxd main.cpp)
# Always optimize, even in Debug mode, because otherwise we're just wasting time.
target_compile_options(mini_xxd PRIVATE -O3)

function(MiniXxd_add_command bin_fname cpp_fname symbol_name)
	add_custom_command(
		OUTPUT "${cpp_fname}"
		COMMAND mini_xxd "${bin_fname}" "${cpp_fname}" "${symbol_name}"
		MAIN_DEPENDENCY "${bin_fname}"
	)
endfunction()
