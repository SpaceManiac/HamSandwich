# HamPlatformer SCons configuration script

from glob import glob
from sys import argv
from os import path, name
import re

windows = (name == 'nt')

if windows:
	LIBS = ['winmm', 'alleg44', 'ws2_32', 'logg', 'vorbisfile', 'vorbis', 'ogg', 'vorbisenc']
else:
	LIBS = ['alleg', 'logg', 'vorbisfile', 'vorbis', 'ogg', 'vorbisenc', 'rt']

# don't compile certain files
blacklist = [
	'source/monsterlist.cpp',
	'source/monsterai1.cpp',
	'source/monsterai2.cpp',
	'source/monsterai3.cpp',
	'source/monsterai4.cpp',
	'source/textitems.cpp',
	'source/textrooms.cpp',
	'source/options.cpp',
	'source/spcldialog.cpp',
]

def getFileList(dir, ext='.cpp'):
	result = []
	for file in map(lambda(x): x.replace('\\', '/'), glob(dir + '/*')):
		if path.isdir(file):
			result += getFileList(file, ext)
		else:
			if file.endswith(ext) and not file in blacklist:
				result.append(file)
	return result

def program(output, debug, **kwargs):
	env = Environment()

	# compiler
	env.Append(CCFLAGS = ['-Wall', '-Wno-write-strings', '-std=gnu++0x'])
	env.Append(CPPPATH = ['include'])
	env.Append(CPPDEFINES = ['ALLEGRO_MINGW32' if windows else 'ALLEGRO_UNIX'])
	env.Append(**kwargs)
	if debug:
		env.Append(CPPDEFINES = ['_DEBUG', 'EXPANDO', 'LOG'])
		env.Append(CCFLAGS = ['-g'])
	else:
		env.Append(CPPDEFINES = ['NDEBUG', 'EXPANDO'])
		env.Append(CCFLAGS = ['-O2', '-s'])
		env.Append(LINKFLAGS = ['-O2', '-s'])
		if windows:
			env.Append(LINKFLAGS = ['-mwindows'])

	# linker
	env.Append(LINKFLAGS = ['-static-libgcc', '-std=c++0x'])
	if windows:
	    env.Append(LINKFLAGS = ['-static-libstdc++'])
	    env.Append(LIBPATH = ['include'])
	env.Append(LIBS = LIBS)

	# output files
	objects = []
	sourceFiles = getFileList('source/')
	for source in sourceFiles:
		object = 'build/' + output + '/' + source.replace('.cpp', '.o')
		objects.append(env.Object(target=object, source=source))

	# resources
	if windows:
		resFiles = getFileList('source/', '.rc')
		for source in resFiles:
			object = 'build/' + output + '/' + source.replace('.res', '.rc')
			objects.append(env.Command(object, source, 'windres ' + source + ' -O coff -o ' + object))

	# finish
	outputExe = 'bin/' + output + ('.exe' if windows else '')
	result = [env.Program(target=outputExe, source=objects)]
	result.append(env.Install('game/', outputExe))
	return Alias(output, result)

lunatic = program('lunatic', False)
lunatic_debug = program('lunatic_debug', True)
scanninglunatic = program('scanninglunatic', True, CPPDEFINES = ['LEVELLIST'])
