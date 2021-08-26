#!/usr/bin/env python3
# android_icons.py - convert .ico files to Android mipmap groups

import sys
import os
from io import BytesIO
from PIL.IcoImagePlugin import IcoFile

_, infile, outpath, outfname = sys.argv

android_sizes = [
	('ldpi', 36),
	('mdpi', 48),
	('hdpi', 72),
	('xhdpi', 96),
	('xxhdpi', 144),
	('xxxhdpi', 192),
]

ico = IcoFile(open(infile, 'rb'))
sizes = set(ico.sizes())

# Give each DPI the next biggest unused size
size_mappings = {}
last = float('Inf')
for name, dim in reversed(android_sizes):
	L = sorted(sz for sz in sizes if dim <= sz[0] < last)
	if L:
		size_mappings[name] = L[0]
		sizes.remove(L[0])
	last = dim

# If nothing got assigned, just set mdpi to the highest size there is
if not size_mappings:
	size_mappings['mdpi'] = max(sizes)

# Save each size as a .png to the output path
for name, sz in size_mappings.items():
	folder = os.path.join(outpath, f'mipmap-{name}')
	os.makedirs(folder, exist_ok=True)
	path = os.path.join(folder, f'{outfname}.png')

	# Don't show message or save if the file is already what we would save
	try:
		with open(path, 'rb') as f:
			previous = f.read()
	except:
		pass
	else:
		by = BytesIO()
		ico.getimage(sz).save(by, 'png')
		if by.getvalue() == previous:
			continue

	print(f'Generated {os.path.relpath(path)}...')
	ico.getimage(sz).save(path)
