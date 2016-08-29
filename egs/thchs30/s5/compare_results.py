#!/usr/bin/env python

# write hyp and ref text in one file, comparing line by line.

from __future__ import print_function
import sys

# hyp, ref, output
file1=sys.argv[1]
file2=sys.argv[2]
file3=sys.argv[3]

f1=open(file1, 'r')
f2=open(file2, 'r')
f3=open(file3, 'w')

arr1=[]
for line1 in f1.readlines():
  arr1.append(line1)

arr2=[]
for line2 in f2.readlines():
  arr2.append(line2)

n=0.0
m=0.0
if len(arr1) != len(arr2):
  print("Error: lines not equal.")
else:
  for i in range(len(arr1)):
    f3.write('<hyp> '+arr1[i])
    f3.write('<ref> '+arr2[i])
    m = m+1
    if len(arr1[i]) < len(arr2[i]):
      n = n +1
    f3.write('\n')
  print("Done.")
  print(n/m)

f1.close() 
f2.close() 
f3.close() 


