#!/usr/bin/env python

import sys, collections

p_list_1 = []
with open('assamese/data/lang/phones.txt', 'r') as lines:
	for col in [line.strip().split() for line in lines]:
                p_list_1.append(col[0])

p_list_2 = []
with open('georgian/data/lang/phones.txt', 'r') as lines:
        for col in [line.strip().split() for line in lines]:
                p_list_2.append(col[0])

same = set(p_list_1) & set(p_list_2)

print same, len(same)



#sheet1_id = []
#with open('sheet1.id.s', 'r') as lines:
#	for line in lines:
#		sheet1_id.append(line.strip())
#
#sheet2_id = []
#with open('sheet2.id.s', 'r') as lines:
#        for line in lines:
#                sheet2_id.append(line.strip())
#
#sheet3_id = []
#with open('sheet3.id.s', 'r') as lines:
#        for line in lines:
#                sheet3_id.append(line.strip())
#
#sheet4_id = []
#with open('sheet4.id.s', 'r') as lines:
#        for line in lines:
#                sheet4_id.append(line.strip())
#
#
#sheet_id = set(sheet1_id) & set(sheet2_id) & set(sheet3_id) & set(sheet4_id)
#
#print sheet_id, len(sheet_id)
#sheet1_r = open('sheet1.result.txt', 'w')
#sheet2_r = open('sheet2.result.txt', 'w')
#sheet3_r = open('sheet3.result.txt', 'w')
#sheet4_r = open('sheet4.result.txt', 'w')
#
#with open('sheet1.all.s', 'r') as lines1:
#	for col in [line.strip().split(' ') for line in lines1]:
#		if col[0] in sheet_id:
#			sheet1_r.write( " ".join(col) + '\n')
#
#with open('sheet2.all.s', 'r') as lines1:
#        for col in [line.strip().split(' ') for line in lines1]:
#                if col[0] in sheet_id:
#                        sheet2_r.write( " ".join(col) + '\n')
#
#
#with open('sheet3.all.s', 'r') as lines1:
#        for col in [line.strip().split(' ') for line in lines1]:
#                if col[0] in sheet_id:
#                        sheet3_r.write( " ".join(col) + '\n')
#
#
#with open('sheet4.all.s', 'r') as lines1:
#        for col in [line.strip().split(' ') for line in lines1]:
#                if col[0] in sheet_id:
#                        sheet4_r.write( " ".join(col) + '\n')
#


#id_pre = 'no_id'
#id_num = 0
#stock_num = 0
#stock_top_num = 0
#sum_all = 0.0
#sum_top = 0.0    # top 10
#
#with open(file, 'r') as content:
#	for col in [line.strip().split(' ') for line in content]:
#		if id_pre != col[0]:
#			if id_pre != 'no_id':
#				print id_pre, stock_num, stock_top_num, sum_all, sum_top
#			id_pre = col[0]
#			id_num += 1
#			stock_num = 0
#			stock_top_num = 0
#			sum_all = 0.0
#			sum_top = 0.0
#		stock_num += 1
#		sum_all += float(col[-1])
#		if int(col[1]) >= 1 and int(col[1]) <= 10:
#			sum_top += float(col[-1])
#			stock_top_num += 1
#
#print id_pre, stock_num, stock_top_num, sum_all, sum_top
#print id_num
			


#		id_list[col[0]] = col[1:]
#		num += 1
		#if num < 10:
		#	break

#for x in id_list.items():
#	print x




##dir = ''
#dir = sys.argv[1]
#
#utt2spk_dict = collections.OrderedDict()
#with open(dir + '/utt2spk', 'r') as utt2spks:
#	for utt2spk in [line.strip().split(' ') for line in utt2spks]:
#		utt2spk_dict[utt2spk[0]] = utt2spk[1]
#
#utt2spk_num = open(dir + '/utt2spk_num', 'w')
#id = 0
#count = 0
#max = 0
#min = 10000
#spk_pre = 'nospeaker'
#for i in utt2spk_dict.keys():
#		line = i
#		if spk_pre != utt2spk_dict[i][:3]:
#			id += 1
#			spk_pre = utt2spk_dict[i][:3]
#			if max < count:
#				max = count
#			if min > count and count > 0:
#				min = count
#			count = 0
#		count += 1
#		utt2spk_num.write(line + ' ' + str(id) + '\n')
#utt2spk_num.close()
#
#if max < count:
#	max = count
#if min > count and count > 0:
#	min = count
#print('max utts spoken by one person:')
#print(max)
#print('min utts spoken by one person:')
#print(min)
