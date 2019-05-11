# Lunatic fixasm.py
# Tad Hardesty, aka SpaceManiac, 2011
# Surrounds assembly in the given file with _( and ), taking comments into account
# usage: 'python tools/fixasm.py filename'

from sys import argv

filename = argv[1]

def fixLine(size, line):
	line = line.replace('\t', '    ').rstrip()
	comment = ''
	i = line.find('//')
	if i >= 0:
		comment = '  ' + line[i:]
		line = line[:i].rstrip()
	spaces = size - len(line)
	return '_(  ' + line + (' ' * spaces) + '  )' + comment + '\n'

with open(filename) as f:
    lines = f.readlines()

size = 0
for line in lines:
	line = line.replace('\t', '    ').rstrip()
	if '//' in line:
        line = line[:line.index('//')].rstrip()
	if len(line) > size:
        size = len(line)

with open(filename, 'w') as f:
	for line in lines:
		f.write(fixLine(size, line))
