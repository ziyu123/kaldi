#!/bin/bash

# Copyright 2013  Arnab Ghoshal
#                 Johns Hopkins University (author: Daniel Povey)
#           2014  Guoguo Chen

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
# WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
# MERCHANTABLITY OR NON-INFRINGEMENT.
# See the Apache 2 License for the specific language governing permissions and
# limitations under the License.


# To be run from one directory above this script.

# Begin configuration section.
weblm=
# end configuration sections

help_message="Usage: $0 [options] <train-txt> <dict> <out-dir> [fisher-dirs]
Train language models for Switchboard-1, and optionally for Fisher and \n
web-data from University of Washington.\n
options: 
  --help          # print this message and exit
  --weblm DIR     # directory for web-data from University of Washington
";

. utils/parse_options.sh

#if [ $# != 4 ]; then
#  printf "$help_message\n";
#  exit 1;
#fi

text=data/train/text     # data/local/train/text
text_dev=data/test/text     # data/local/test/text
lexicon=data/local/lexicon.txt  # data/local/dict/lexicon.txt
dir=data/srilm      # data/local/lm


for f in "$text" "$lexicon"; do
  [ ! -f $x ] && echo "$0: No such file $f" && exit 1;
done

set -o errexit
mkdir -p $dir
export LC_ALL=C 

#heldout_sent=2000
#cut -d' ' -f2- $text | gzip -c > $dir/train.all.gz
awk '{sub(/^[^[:space:]]*[[:space:]]+/,"");print}' $text | gzip -c > $dir/train2gram.all.gz
#cut -d' ' -f2- $text | head -n $heldout_sent > $dir/heldout
awk '{sub(/^[^[:space:]]*[[:space:]]+/,"");print}' $text_dev > $dir/heldout2gram

awk '{print $1}' $lexicon > $dir/wordlist2gram


# 1gram language model
ngram-count -text $dir/train2gram.all.gz -order 1 -limit-vocab -vocab $dir/wordlist2gram \
  -unk -map-unk "<unk>" -kndiscount -interpolate -lm $dir/lm1gram.gz

echo "PPL for 1gram LM:"
ngram -unk -lm $dir/lm1gram.gz -ppl $dir/heldout2gram
ngram -unk -lm $dir/lm1gram.gz -ppl $dir/heldout2gram -debug 2 >& $dir/1gram.ppl2

# 2gram language model
ngram-count -text $dir/train2gram.all.gz -order 2 -limit-vocab -vocab $dir/wordlist2gram \
  -unk -map-unk "<unk>" -kndiscount -interpolate -lm $dir/lm2gram.gz


echo "PPL for 2gram LM:"
ngram -unk -lm $dir/lm2gram.gz -ppl $dir/heldout2gram
ngram -unk -lm $dir/lm2gram.gz -ppl $dir/heldout2gram -debug 2 >& $dir/2gram.ppl2

echo "-------done------"

exit 0

