#!/usr/bin/env python

import sys, os, random


dura = 160 # 40 80 160


for id in range(1,5):
	len_path = 'test'+str(id)+'_seg'+str(dura)+'_2k/feats.len'
	seg_path = 'test'+str(id)+'_seg'+str(dura)+'_2k/segments'
	seg = open(seg_path, 'w')
	with open(len_path, 'r') as lines:
		for col in [line.strip().split() for line in lines]:
			if int(col[1])-1-dura >= 0:
				start = random.randint(0, int(col[1])-1-dura)
				end = start + dura
				seg.write(col[0]+' '+col[0]+' '+str(start)+' '+str(end)+'\n')
			else:
				seg.write(col[0]+' '+col[0]+' '+str(0)+' '+str(int(col[1])-1)+'\n')
		
seg.close()
print('Done segment file.')
