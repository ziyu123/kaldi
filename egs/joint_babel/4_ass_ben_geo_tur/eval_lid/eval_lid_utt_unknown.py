#!/usr/bin/env python

import sys


num_all = 0.0
num_1 = 0.0
lang = sys.argv[1]
lid = sys.argv[2]
file_path = 'eval_lid/output_utt.ark' + lang

with open(file_path, 'r') as lines:
	for col in [line.strip().split() for line in lines]:
		num_all += 1
		li = []
                for index in range(2, len(col)-1):
	                li.append(float(col[index]))
		if li.index(max(li)) == lid:
			num_1 += 1

print('---language ' + lang + ', id ' + str(lid) + '---')
print('utt level: ' + str(num_1/num_all))
print('EER : ' + str(1-num_1/num_all))
print('----------------------')

