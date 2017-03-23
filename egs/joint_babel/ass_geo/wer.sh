#!/bin/bash

for i in 1 2; do
  #for x in exp/*/decode_*test${i}*; do [ -d $x ] && grep Sum $x/score_*/test*.ctm.sys | utils/best_wer.sh; done 2>/dev/null
  for x in exp/nnet3/*/decode_*test${i}; do [ -d $x ] && grep Sum $x/score_*/test*.ctm.sys | utils/best_wer.sh; done 2>/dev/null
  echo '------------------'
done 





#for x in exp/nnet3/*/decode*/; do [ -d $x ] && grep WER $x/wer_* | utils/best_wer.sh; done
