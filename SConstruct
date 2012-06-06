# HamPlatformer SCons configuration script

from glob import glob
from sys import argv
from os import path, environ
from subprocess import Popen, PIPE
import re

LIBS = ['winmm', 'dsound', 'alleg44', 'ws2_32', 'logg', 'vorbisfile', 'vorbis', 'ogg', 'vorbisenc']

if 'DIRECTX' in environ:
	DIRECTX = environ['DIRECTX']
else:
	DIRECTX = 'D:/libraries/directx-sdk'

git_describe = Popen(["git", "describe"], stdout=PIPE).communicate()[0][:-1]
vfile = open('source/version.h', 'w')
vfile.write('#define VERSION "%s"' % git_describe)
vfile.close()

def getFileList(dir, ext='.cpp'):
	result = []
	for file in map(lambda(x): x.replace('\\', '/'), glob(dir + '/*')):
		if path.isdir(file):
			result += getFileList(file, ext)
		else:
			if file.endswith(ext):
				result.append(file)
	return result

def program(output, debug):
	env = Environment()

	# compiler
	env.Append(CCFLAGS = ['-Wall', '-std=c++0x'])
	env.Append(CPPPATH = ['include', path.join(DIRECTX, 'Include')])
	env.Append(CPPDEFINES = ['ALLEGRO_MINGW32', 'EXPANDO'])
	if debug:
		env.Append(CPPDEFINES = ['_DEBUG', 'LOG'])
		env.Append(CCFLAGS = ['-g'])
	else:
		env.Append(CPPDEFINES = ['NDEBUG'])
		env.Append(CCFLAGS = ['-O2', '-s', '-mwindows'])
		env.Append(LINKFLAGS = ['-O2', '-s'])

	# linker
	env.Append(LINKFLAGS = ['-static-libgcc', '-static-libstdc++', '-std=c++0x'])
	env.Append(LIBPATH = ['include', path.join(DIRECTX, 'Lib', 'x86')])
	env.Append(LIBS = LIBS)

	# output files
	objects = []
	sourceFiles = getFileList('source/')
	for source in sourceFiles:
		object = 'build/' + output + '/' + source.replace('.cpp', '.o')
		objects.append(env.Object(target=object, source=source))

	# resources
	resFiles = getFileList('source/', '.rc')
	for source in resFiles:
		object = 'build/' + output + '/' + source.replace('.rc', '.res')
		objects.append(env.Command(object, source, 'windres ' + source + ' -O coff -o ' + object))

	# finish
	outputExe = 'bin/' + output + '.exe'
	result = [env.Program(target=outputExe, source=objects)]
	result.append(env.Install('game/', outputExe))
	return Alias(output, result)

lunatic = program('lunatic', False)
lunatic_debug = program('lunatic_debug', True)
