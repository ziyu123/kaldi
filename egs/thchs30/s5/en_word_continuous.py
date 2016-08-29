#!/usr/bin/env python

# There are blanks in some English words, like ' w o r d ', 
# transformed to ' word ' by this script.

from __future__ import print_function
import sys, re

file=sys.argv[1]
with open(file, 'r') as f:
  for line in f:
    line += ' '
    temp1 = re.sub('([^ a-zA-Z])([a-zA-Z])',  lambda r: r.group(1)+' '+r.group(2), line)
    temp2 = re.sub('([a-zA-Z])([^ a-zA-Z])',  lambda r: r.group(1)+' '+r.group(2), temp1)
    result = re.sub('(( [a-zA-Z])+( ))',  lambda r: ' '+r.group(0).replace(' ','')+' ', temp2)
    print(result.strip())
