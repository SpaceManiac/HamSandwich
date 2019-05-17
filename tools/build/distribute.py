#!/usr/bin/env python3
# distribute.py - zip an exe and its non-Windows DLLs together
# Accepts a project name or the path to a .exe

import os
import sys
import subprocess
import zipfile

if len(sys.argv) != 2:
	print("Usage:", sys.argv[0], "<exe>")
	exit(1)

exe = sys.argv[1]
# If a project name is passed in, build and take the release build
if exe in ('lunatic', 'supreme', 'sleepless', 'loonyland', 'loonyland2', 'mystic'):
	subprocess.call(["make", "config=release", exe])
	exe = f"build/gcc-release/{exe}/{exe}.exe"

if not os.path.exists(exe):
	print("Not found:", exe)
	exit(1)

outfile = f"{exe[:-4]}.zip"
print(f"==== Creating {outfile} ====")
with zipfile.ZipFile(outfile, 'w', zipfile.ZIP_DEFLATED) as outzip:
	print(f"EXE: {exe}")
	with open(exe, 'rb') as f:
		with outzip.open(os.path.basename(exe), 'w') as z:
			z.write(f.read())

	ntldd = subprocess.run(["ntldd", exe], stdout=subprocess.PIPE)
	for line in ntldd.stdout.decode('utf-8').split('\n'):
		line = line.strip()
		if not line:
			continue
		fname = line.split(' => ')[1].split(' (')[0]
		if not fname.startswith(os.environ['WINDIR']):
			print(f"DLL: {fname}")
			with open(fname, 'rb') as f:
				with outzip.open(os.path.basename(fname), 'w') as z:
					z.write(f.read())
