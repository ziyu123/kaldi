#!/bin/bash


#for x in exp/*/decode_*test*; do [ -d $x ] && grep Sum $x/score_*/test.ctm.sys | utils/best_wer.sh; done 2>/dev/null
for x in exp/nnet3/lstm_asr*/decode_*test; do [ -d $x ] && grep Sum $x/score_*/test.ctm.sys | utils/best_wer.sh; done 2>/dev/null






#for x in exp/nnet3/*/decode*/; do [ -d $x ] && grep WER $x/wer_* | utils/best_wer.sh; done
#for x in /nfs/raid88/zhangzhiyong/work/train_sino_1000h_16k/exp_nnet3/*/*2000*; do [ -d $x ] && grep WER $x/wer_* | utils/best_wer.sh; done
