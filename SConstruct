# Dr. Lunatic SCons configuration script

from glob import glob
from sys import argv
from os import path, environ
from subprocess import Popen, PIPE
import re, platform

LIBS = ['winmm', 'alleg44', 'ws2_32', 'logg', 'vorbisfile', 'vorbis', 'ogg', 'vorbisenc']

#git_describe = Popen(["git", "describe", "--always"], stdout=PIPE).communicate()[0][:-1]
#vfile = open('source/version.h', 'w')
#vfile.write('#define VERSION "dev"' % git_describe)
#vfile.close()

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
	# if we're on Windows, force Mingw use
	if platform.system() == 'Windows':
		env = Environment(ENV = environ, tools = ['mingw'])
	else:
		env = Environment(ENV = environ)

	# compiler
	env.Append(CCFLAGS = ['-Wall', '-std=c++0x'])
	env.Append(CPPPATH = ['include'])
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
	env.Append(LIBPATH = ['include'])
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
