#!/bin/python

import sys, os, shutil

dir = sys.argv[1]
dir_utf8 = sys.argv[2]
#shutil.rmtree(dir_utf8)
#os.makedirs(dir_utf8)

for dirpath, dirnames, filenames in os.walk(dir):
	for file in filenames:
		with open(os.path.join(dir, file), 'r') as src:
			with open(os.path.join(dir_utf8, file + '_tmp'), 'w') as dest:
				txt = src.read().decode('utf-16le')
				if txt[0] == U'\uFEFF':
					txt = txt[1:]
				dest.write(txt.encode('utf-8'))
