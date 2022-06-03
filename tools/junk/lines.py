#!/usr/bin/env python2
# Lunatic lines.py
# Tad Hardesty, aka SpaceManiac, 2011
# Counts lines of code
# usage: 'python tools/lines.py'

from glob import glob
from sys import argv
from os import path
import os, re

def getFileList(dir, ext):
	result = []
	for file in map(lambda(x): x.replace('\\', '/'), glob(dir + '/*')):
		if path.isdir(file):
			result += getFileList(file, ext)
		else:
			if file.find("old/") < 0 and file.endswith(ext):
				result += [file]
	return result

commentRe = re.compile(r'/\*.*?\*/', re.DOTALL)

def calculateStats(ext):
	files = getFileList('source', '.' + ext)

	lines = {
		'comment': 0,
		'blank': 0,
		'braces': 0,
		'code': 0
	}

	for path in files:
		f = file(path)
		data = f.read()
		f.close()

		match = commentRe.search(data)
		while match:
			lines['comment'] += data[match.start():match.end()].count('\n')
			data = data[:match.start()] + data[match.end()+1:]
			match = commentRe.search(data)

		data = data.split('\n')

		for line in data:
			if len(line.strip()) == 0:
				t = 'blank'
			elif len(line.replace('{', '').replace('}', '').strip()) == 0:
				t = 'braces'
			elif '//' in line and len(line[:line.find('//')].strip()) == 0:
				t = 'comment'
			else:
				t = 'code'
			lines[t] += 1

	print ext + ' files (' + str(len(files)) + '):'
	for key in lines:
		print '  ' + key + ': ' + str(lines[key])
	return lines

cpp = calculateStats('cpp')
h = calculateStats('h')

print "total:"
for key in cpp:
	print '  ' + key + ': ' + str(cpp[key] + h[key])
