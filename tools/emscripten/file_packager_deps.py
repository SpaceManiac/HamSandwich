#!/usr/bin/env python3
# file_packager_deps.py - calls EMSDK file_packager.py, and creates .d depfile

# Import the original file_packager
import sys, os
sys.path.insert(1, os.environ['EMSCRIPTEN_ROOT_PATH'])
from tools import file_packager

# Find the --js-output arg
js_output = None
packfile = sys.argv[1]
for arg in sys.argv[2:]:
	if arg.startswith('--js-output') and '=' in arg:
		_, js_output = arg.split('=', 1)
if not js_output:
	print('need --js-output')
	exit(1)

# Override the add() function so we know what dirs were walked
walked = [
	__file__,
	file_packager.__file__,
]
def add(mode, rootpathsrc, rootpathdst):
	"""Expand directories into individual files

	rootpathsrc: The path name of the root directory on the local FS we are adding to emscripten virtual FS.
	rootpathdst: The name we want to make the source path available on the emscripten virtual FS.
	"""
	walked.append(rootpathsrc)
	for dirpath, dirnames, filenames in os.walk(rootpathsrc):
		new_dirnames = []
		for name in dirnames:
			fullname = os.path.join(dirpath, name)
			if not file_packager.should_ignore(fullname):
				walked.append(fullname)
				new_dirnames.append(name)
			elif file_packager.DEBUG:
				print('Skipping directory "%s" from inclusion in the emscripten virtual file system.' % fullname, file=sys.stderr)
		for name in filenames:
			fullname = os.path.join(dirpath, name)
			if not file_packager.should_ignore(fullname):
				walked.append(fullname)
				# Convert source filename relative to root directory of target FS.
				dstpath = os.path.join(rootpathdst,
				                       os.path.relpath(fullname, rootpathsrc))
				file_packager.new_data_files.append({
					'srcpath': fullname,
					'dstpath': dstpath,
					'mode': mode,
					'explicit_dst_path': True
				})
			elif file_packager.DEBUG:
				print('Skipping file "%s" from inclusion in the emscripten virtual file system.' % fullname, file=sys.stderr)
		del dirnames[:]
		dirnames.extend(new_dirnames)
file_packager.add = add

# List walked files and directories in output
ret = file_packager.main()
with open(f'{js_output}.d', 'w', encoding='utf-8') as f:
	joined = " \\\n".join(walked)
	f.write(f'{packfile} \\\n{js_output} \\\n: \\\n{joined} \\\n')
exit(ret)
