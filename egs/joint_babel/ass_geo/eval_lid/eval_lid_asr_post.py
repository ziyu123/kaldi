#!/usr/bin/env python

import sys, os


num_all = 0.0
num_1 = 0.0
num_2 = 0.0
lang = sys.argv[1]
file_path = 'eval_lid/output.ark' + lang

ass_list = []
with open('asr_posterior/ass_pdf_id', 'r') as lines:
        for phone in [line.strip().split() for line in lines]:
		for index in range(0, len(phone)):
	                ass_list.append(phone[index])

geo_list = []
with open('asr_posterior/geo_pdf_id', 'r') as lines:
        for phone in [line.strip().split() for line in lines]:
                for index in range(0, len(phone)):
	                geo_list.append(phone[index])


lid = int(lang) - 1  #### lang id, 0~1, 1~2, ...
with open(file_path, 'r') as lines:
	for col in [line.strip().split() for line in lines]:
                score_ass = 0.0
                score_geo = 0.0
		if col[-1] != '[' and col[-1] != ']':
                	num_all += 1
			for index in range(0, len(col)):
				if str(index) in ass_list:
					score_ass += float(col[index])
                                if str(index) in geo_list:
                                        score_geo += float(col[index])
                        if score_ass > score_geo:
                                num_1 += 1
                        if score_ass < score_geo:
                                num_2 += 1

		if col[-1] == ']':
                	num_all += 1
                        for index in range(0, len(col)-1):
                                if index in ass_list:
                                        score_ass += float(col[index])
                                if index in geo_list:
                                        score_geo += float(col[index])
			if score_ass > score_geo:
        	                num_1 += 1
			if score_ass < score_geo:
                		num_2 += 1

print('---language ' + lang + ', id ' + str(lid) + '---')
if lang == '1':
	print('frame correct: ' + str(num_1/num_all))
	print('EER: ' + str(1-num_1/num_all))
if lang == '2':
	print('frame correct: ' + str(num_2/num_all))
	print('EER: ' + str(1-num_2/num_all))
print('----------------------')

