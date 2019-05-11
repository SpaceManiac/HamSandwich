# Lunatic lines.py
# Tad Hardesty, aka SpaceManiac, 2011
# Counts lines of code
# usage: 'python tools/lines.py'

import os
import re

def getFileList(dir, ext):
	for dirpath, dirnames, filenames in os.walk(dir):
		for name in filenames:
			if 'old/' not in name and name.endswith(ext):
				yield os.path.join(dirpath, name)

commentRe = re.compile(r'/\*.*?\*/', re.DOTALL)

def calculateStats(ext):
	lines = {
		'comment': 0,
		'blank': 0,
		'braces': 0,
		'code': 0
	}

	size = 0
	for path in getFileList('source', '.' + ext):
		size += 1
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
			elif len(line.replace('{', '').replace('}', '')) == 0:
				t = 'braces'
			elif len(line[:line.find('//')].strip()) == 0:
				t = 'comment'
			else:
				t = 'code'
			lines[t] += 1

	print ext + ' files (' + str(size) + '):'
	for key in lines:
		print '  ' + key + ': ' + str(lines[key])

calculateStats('cpp')
calculateStats('h')
