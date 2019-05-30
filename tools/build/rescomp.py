#!/usr/bin/env python3
# rescomp.py - provide icon embedding on non-Windows platforms

# Dependencies
import sys
import os
from io import BytesIO

try:
	from PIL import Image
except ImportError:
	import subprocess
	subprocess.call([sys.executable, "-m", "pip", "install", "Pillow"])
	from PIL import Image

# Actual script
exe, in_fname, out_fname = sys.argv

icon_fname = None
with open(in_fname, "r") as f:
	for line in f:
		name, kind, fname = line.split()
		if kind.upper() == "ICON":
			dir, _ = os.path.split(in_fname)
			icon_fname = os.path.join(dir, fname.strip('"'))
			break

if not icon_fname:
	print(f"{in_fname}:1: error: file did not contain an ICON")
	exit(1)

im = Image.open(icon_fname)
data = BytesIO()
im.save(data, 'PNG')
buf = data.getbuffer()
data_array = ','.join(str(x) for x in buf)

with open(out_fname, "w") as f:
	print(f"#include <stddef.h>", file=f)
	print(f"size_t WINDOW_ICON_SZ = {len(buf)};", file=f)
	print(f"unsigned char WINDOW_ICON[] = {{{data_array}}};", file=f)
