#!/usr/bin/env python3
# embed-metadata.py - replace tokens with the contents of specific files

import sys

data = sys.stdin.read()
for i in range(1, len(sys.argv), 2):
	data = data.replace(sys.argv[i], open(sys.argv[i + 1]).read())
sys.stdout.write(data)
sys.stdout.flush()
