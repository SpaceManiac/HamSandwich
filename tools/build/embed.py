#!/bin/sh
"exec" """$(dirname "$0")/../bootstrap/python""" "$0" "$@"
# -----------------------------------------------------------------------------
# embed.py - embed a file as binary content within a .cpp file

import sys

_, bin_fname, cpp_fname, symbol_name = sys.argv

with open(bin_fname, 'rb') as f:
	data = f.read()

data_str = ','.join(str(x) for x in data)

with open(cpp_fname, "w") as f:
	print(f"#include <stddef.h>", file=f)
	print(f"extern const size_t {symbol_name}_size = {len(data)};", file=f)
	print(f"extern const unsigned char {symbol_name}[] = {{{data_str}}};", file=f)
