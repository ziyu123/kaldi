#!/bin/bash

. ./path.sh

data_dir=data
lang_dir=data/lang
nnet3_dir=model

compute-fbank-feats --config=$nnet3_dir/conf/fbank.conf scp:$data_dir/wav.scp ark:$data_dir/feats.ark

compile-train-graphs --read-disambig-syms=$lang_dir/phones/disambig.int \
  $nnet3_dir/tree \
  $nnet3_dir/final.mdl \
  $lang_dir/L.fst \
  "ark:utils/sym2int.pl --map-oov 64 -f 2- $lang_dir/words.txt $data_dir/text|" ark:- | \
  ll-compare-with-referred-text-wav --transition-scale=1.0 --acoustic-scale=0.1 --self-loop-scale=0.1 --frames-per-chunk=50 --extra-left-context=0 --extra-right-context=0 --extra-left-context-initial=-1 --extra-right-context-final=-1 --use-gpu=no --beam=10 --retry-beam=40 $nnet3_dir/final.mdl ark:- ark:$data_dir/feats.ark ark:tmp.ark

rm $data_dir/feats.ark tmp.ark
