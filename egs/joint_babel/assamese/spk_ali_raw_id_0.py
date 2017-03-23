#!/usr/bin/python

import sys, collections, os

dir = sys.argv[1]
#dir = 'data_energy/train'

if not os.path.isdir(r'exp/spk_ali_raw'):
	os.makedirs(r'exp/spk_ali_raw') # store the alignment of speaker or language id

len_dict = collections.OrderedDict()
with open(dir + '/feats.len', 'r') as utt_lens:
        for utt_len in [line.strip().split(' ') for line in utt_lens]:
                len_dict[utt_len[0]] = utt_len[1]

#spk_dict = {}
#all_num = 0
#with open(dir + '/utt2spk_num', 'r') as spk_ids:
#        for spk_id in [line.strip().split(' ') for line in spk_ids]:
#                spk_dict[spk_id[0]] = spk_id[1]
#		if all_num < int(spk_id[1]):
#			all_num = int(spk_id[1])

#vad_dict = {}
#with open(dir + '/feats.vad', 'r') as vads:
#	for vad in [line.strip().split(' ') for line in vads]:
#		vad_dict[vad[0]] = vad[1:]


counts = []  # silence in [0]
for n in range(0, 1):
	counts.append(0)

spk_ali_id = open('exp/spk_ali_raw/ali.ark.id0', 'w') # language id, minus 1
for i in len_dict.keys():
        line_id = i
        num = int(len_dict[i])
	counts[0] += num
	line_id += ' 0'*num  # language id, minus 1
        spk_ali_id.write(line_id + '\n')
spk_ali_id.close()

spk_counts = open('exp/spk_ali_raw/spk_counts', 'w')
spk_counts.write('[')
for j in counts:
        spk_counts.write(' ' + str(j))
spk_counts.write(' ]')
spk_counts.close()

spk_num = open('exp/spk_ali_raw/spk_num', 'w')
spk_num.write('1')
spk_num.close()

