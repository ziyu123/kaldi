#!/bin/bash

for dir in IARPA_BABEL_BP_101  IARPA_BABEL_BP_105  IARPA_BABEL_OP1_102  IARPA_BABEL_OP3_404 IARPA_BABEL_BP_104  IARPA_BABEL_BP_106  IARPA_BABEL_OP1_103; do
  for dir1 in conversational scripted; do
    for dir2 in training sub-train dev eval; do
	ls -l $dir/$dir1/$dir2/transcription/ | awk '{print $9}' | awk -F . '{print $1}' > $dir/$dir1/$dir2/data.list
	echo $dir/$dir1/$dir2/data.list
	cat $dir/$dir1/$dir2/data.list | wc -l
    done
  done
done

