#!/usr/bin/python

import sys, collections, os

dir = sys.argv[1]
#dir = 'exp/spk_ali_from_asr'

counts = []  # silence in [0]
for n in range(0, 2):
        counts.append(0)

ali_ark = open(dir + '/ali.ark', 'w')
with open(dir + '/phone.ali', 'r') as lines:                 #language id
	for col in [line.strip().split(' ') for line in lines]:
		for n in range(1, len(col)):
			if int(col[n]) <= 20:
				col[n] = '0'
				counts[0] += 1
			else:
				col[n] = '1'                  #language id
				counts[1] += 1
		ali_ark.write(" ".join(col) + '\n')
ali_ark.close()

spk_counts = open(dir + '/spk_counts', 'w')
spk_counts.write('[')
for j in counts:
        spk_counts.write(' ' + str(j))
spk_counts.write(' ]')
spk_counts.close()

spk_num = open(dir + '/spk_num', 'w')
spk_num.write('2')
spk_num.close()


