#!/bin/bash

dir_1=$1
dir_2=$2


echo "------merge two language models------"
lm_1=$dir_1/data/srilm/lm2gram.gz
lm_2=$dir_2/data/srilm/lm2gram.gz
mkdir -p data/srilm
ngram -order 2 -lm $lm_1 -mix-lm $lm_2 -lambda 0.5 -write-lm data/srilm/lm2gram.gz # if max order is 3(3-gram)


echo "------done------"

exit 0

