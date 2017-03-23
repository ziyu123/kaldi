#!/usr/bin/env python

import sys, os


num_all = 0.0
num_1 = 0.0
lang = sys.argv[1]
file_path = 'eval_lid/output.ark' + lang

lid = int(lang) - 1  #### lang id, 0~1, 1~2, ...
with open(file_path, 'r') as lines:
	for col in [line.strip().split() for line in lines]:
		if col[-1] != '[' and col[-1] != ']':
			num_all += 1
			li = []
			for index in range(0, len(col)):
				li.append(float(col[index]))
			if li.index(max(li)) == lid:
				num_1 += 1
		if col[-1] == ']':
                        num_all += 1
			li = []
                        for index in range(0, len(col)-1):
                                li.append(float(col[index]))
                        if li.index(max(li)) == lid:
                                num_1 += 1

print('---language ' + lang + ', id ' + str(lid) + '---')
print('frame correct: ' + str(num_1/num_all))
print('EER: ' + str(1-num_1/num_all))
print('----------------------')

