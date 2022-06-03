# Usage: cmake -Dbin_fname=X -Dcpp_fname=Y -Dsymbol_name=Z -P embed.cmake

file(READ "${bin_fname}" data HEX)

set(output "
#include <stddef.h>
extern const unsigned char ${symbol_name}[] = {
")

set(len 0)
string(REGEX MATCHALL ".." separated_hex "${data}")
foreach(hex IN LISTS separated_hex)
	math(EXPR len "${len}+1")
	string(APPEND output "0x${hex},")
endforeach()
string(APPEND output "
};
extern const size_t ${symbol_name}_size = sizeof(${symbol_name});
")

file(WRITE "${cpp_fname}" "${output}")
