#!/bin/bash

. ./path.sh

data_dir=data
lang_dir=data/lang
nnet3_dir=model

score-phone-lens scp:$data_dir/wav.scp \
  $nnet3_dir/tree \
  $nnet3_dir/final.mdl \
  $lang_dir/L.fst \
  "ark:utils/sym2int.pl --map-oov 64 -f 2- $lang_dir/words.txt $data_dir/text|" \
  fbank.conf graph.conf align.conf phone.conf

