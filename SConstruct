# Dr. Lunatic SCons configuration script

import os
import platform

LIBS = ['winmm', 'alleg44', 'ws2_32', 'logg', 'vorbisfile', 'vorbis', 'ogg', 'vorbisenc']

def getFileList(dir, ext):
	for dirpath, dirnames, filenames in os.walk(dir):
		for name in filenames:
			if 'old/' not in name and name.endswith(ext):
				yield os.path.join(dirpath, name)

def program(output, debug):
	# if we're on Windows, force Mingw use
	if platform.system() == 'Windows':
		env = Environment(ENV = os.environ, tools = ['mingw'])
	else:
		env = Environment(ENV = os.environ)

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
	for source in getFileList('source/', '.cpp'):
		object = 'build/' + output + '/' + source.replace('.cpp', '.o')
		objects.append(env.Object(target=object, source=source))

	# resources
	for source in getFileList('source/', '.rc'):
		object = 'build/' + output + '/' + source.replace('.rc', '.res')
		objects.append(env.Command(object, source, 'windres ' + source + ' -O coff -o ' + object))

	# finish
	outputExe = 'bin/' + output + '.exe'
	result = [env.Program(target=outputExe, source=objects)]
	result.append(env.Install('game/', outputExe))
	return Alias(output, result)

lunatic = program('lunatic', False)
lunatic_debug = program('lunatic_debug', True)
