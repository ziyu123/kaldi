#!/usr/bin/python

import sys, os, os.path

#dir = 'data_fbank/train_si284'

len_dict = {}
with open('./feats.len', 'r') as utt_lens:
	for utt_len in [line.strip().split() for line in utt_lens]:
                len_dict[utt_len[0]] = utt_len[1]

max = 0
min = 1000
for i in len_dict.keys():
	if int(len_dict[i]) > max:
		max = int(len_dict[i])
	if int(len_dict[i]) < min:
		min = int(len_dict[i])

print max, min
