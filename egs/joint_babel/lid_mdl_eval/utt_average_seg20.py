#!/usr/bin/python

import sys, os, os.path

#dir = 'data_fbank/train_si284'

len_dict = {}
with open('./feats.len.seg20', 'r') as utt_lens:
	for utt_len in [line.strip().split() for line in utt_lens]:
                len_dict[utt_len[0]] = utt_len[1]


dir = 'net_output'
for parent,dirnames,filenames in os.walk(dir):
	for file in filenames:
		if file.endswith('utt') and 'seg20' in file:
			print file
			new_file = file + '_average'
			w_f = open(dir+'/'+new_file,'w')
			with open(dir+'/'+file, 'r') as lines:
				for col in [line.strip().split() for line in lines]:
					w_f.write(col[0] + ' [ ')
					utt_id = col[0]
					for i in range(2, len(col)-1):
						w_f.write(str(float(col[i])/float(len_dict[utt_id])) + ' ')
					w_f.write(']\n')
			w_f.close()
