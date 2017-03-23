#!/usr/bin/python

import sys, collections


phone_dict = collections.OrderedDict()
with open('merge_phone.txt', 'r') as lines:
	for phone in [line.strip().split() for line in lines]:
		phone_dict[phone[0]] = phone[1]

ass_list = []
with open('ass_phone.txt', 'r') as lines:
	for phone in [line.strip().split() for line in lines]:
		ass_list.append(phone[0])

geo_list = []
with open('geo_phone.txt', 'r') as lines:
        for phone in [line.strip().split() for line in lines]:
                geo_list.append(phone[0])

same_set = set(ass_list) & set(geo_list)
print len(phone_dict), len(ass_list), len(geo_list)


ass_phone_id = open('ass_phone_id', 'w')
geo_phone_id = open('geo_phone_id', 'w')
ass_phone_id.write('id ');
geo_phone_id.write('id ');
for i in phone_dict.keys():
	if i in ass_list:
		ass_phone_id.write(phone_dict[i] + ' ')
	if i in geo_list:
                geo_phone_id.write(phone_dict[i] + ' ')
ass_phone_id.close()
geo_phone_id.close()
		

#dir = sys.argv[1]
##dir = 'data_fbank/train_si284'
#
#len_dict = collections.OrderedDict()
#with open(dir + '/feats.len', 'r') as utt_lens:
#        for utt_len in [line.strip().split(' ') for line in utt_lens]:
#                len_dict[utt_len[0]] = utt_len[1]
#
#spk_dict = {}
#all_num = 0
#with open(dir + '/utt2spk_num', 'r') as spk_ids:
#        for spk_id in [line.strip().split(' ') for line in spk_ids]:
#                spk_dict[spk_id[0]] = spk_id[1]
#		if all_num < int(spk_id[1]):
#			all_num = int(spk_id[1])
#
#vad_dict = {}
#with open(dir + '/vad_from_mfcc', 'r') as vads:
#	for vad in [line.strip().split(' ') for line in vads]:
#		vad_dict[vad[0]] = vad[1:]
#
#
#counts = []  # silence in [0]
#for n in range(0, all_num + 1):
#	counts.append(0)
#
#spk_ali = open('exp/spk_ali/ali.ark', 'w')
#for i in len_dict.keys():
#        line = i
#        num = int(len_dict[i])
#	for j in range(0, num):	
#		if vad_dict[i][j] == '0':
#			counts[0] += 1
#			line += ' ' + '0'
#		else:
#	                counts[int(spk_dict[i])] += 1
#			line += ' ' +  spk_dict[i]
#        spk_ali.write(line + '\n')
#spk_ali.close()
#
#spk_counts = open('exp/spk_ali/spk_counts', 'w')
#spk_counts.write('[')
#for j in counts:
#        spk_counts.write(' ' + str(j))
#spk_counts.write(' ]')
#spk_counts.close()
#
#spk_num = open('exp/spk_ali/spk_num', 'w')
#spk_num.write(str(all_num + 1))
#spk_num.close()
#
