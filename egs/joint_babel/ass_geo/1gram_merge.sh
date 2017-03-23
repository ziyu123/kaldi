#!/bin/bash

dir_1=$1
dir_2=$2


echo "------merge two language models------"
lm_1=$dir_1/data/srilm/lm1gram.gz
lm_2=$dir_2/data/srilm/lm1gram.gz
mkdir -p data/srilm
ngram -order 1 -lm $lm_1 -mix-lm $lm_2 -lambda 0.5 -write-lm data/srilm/lm1gram.gz # if max order is 3(3-gram)


echo "------done------"

exit 0

