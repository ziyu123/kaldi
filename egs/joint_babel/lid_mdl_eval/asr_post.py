#!/usr/bin/env python

import sys, os


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

# net_output/output.0_asr.1.ark  net_output/output.0_asr.2.ark net_output/output.0_multi-asr.1.ark net_output/output.0_multi-asr.2.ark
file_path = 'asr_output/output.0_multi-asr.1.ark'
name = file_path + '.score'
w_file = open(name, 'w')

f = open(file_path, 'r')
for line in f.readlines():
	col = line.strip().split()
	if col[-1] == '[':
		utt_id = col[0]
		w_file.write(col[0]+' '+col[1]+'\n')
        score_ass = 0.0
        score_geo = 0.0
	if col[-1] != '[' and col[-1] != ']':
		for index in range(0, len(col)):
			if str(index) in ass_list:
				score_ass += float(col[index])
                        if str(index) in geo_list:
                                score_geo += float(col[index])
		w_file.write(str(score_ass)+' '+str(score_geo)+'\n')
	if col[-1] == ']':
                for index in range(0, len(col)-1):
                        if str(index) in ass_list:
                                score_ass += float(col[index])
                        if str(index) in geo_list:
                                score_geo += float(col[index])
		w_file.write(str(score_ass)+' '+str(score_geo)+' ]'+'\n')

w_file.close()
print file_path
print 'done'

