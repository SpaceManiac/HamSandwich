#!/usr/bin/env python3
# rescomp.py - provide resource embedding on non-Windows platforms

import sys
import os
from io import BytesIO

exe, in_fname, cpp_fname, h_fname = sys.argv

src = open(in_fname, "r")
cpp = open(cpp_fname, "w")
h = open(h_fname, "w")

print(f"#include <stddef.h>", file=h)
print(f"#define EmbeddedRW(NAME) SDL_RWFromMem(RES_##NAME, RES_##NAME##_SZ)", file=h)
print(f"#include <stddef.h>", file=cpp)

for line in src:
	name, kind, fname = line.split()
	dir, _ = os.path.split(in_fname)
	with open(os.path.join(dir, fname.strip('"')), 'rb') as f:
		data = f.read()
	print(f"extern size_t RES_{name}_SZ;", file=h)
	print(f"extern unsigned char RES_{name}[];", file=h)
	print(f"size_t RES_{name}_SZ = {len(data)};", file=cpp)
	print(f"unsigned char RES_{name}[] = {{{','.join(str(x) for x in data)}}};", file=cpp)
