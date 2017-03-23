#!/bin/bash

oovSymbol="<unk>"

if [ $# != 2 ]; then
  echo "Usage: $0 <dir1> <dir2>"
  exit 1;
fi

dir_1=$1 #/work4/tzy/joint_babel/assamese
dir_2=$2 #/work4/tzy/joint_babel/bengali

echo "------combine data/local------"
mkdir -p data/local
for x in extra_questions.txt lexicon.txt nonsilence_phones.txt silence_phones.txt optional_silence.txt; do
  cat $dir_1/data/local/$x $dir_2/data/local/$x > data/local/${x}.temp
  sort -u data/local/${x}.temp > data/local/$x

  if [ $x == 'lexicon.txt' ]; then
    cp data/local/$x data/local/${x}.temp
    cat data/local/${x}.temp | \
      perl -e 'while(<>){@A = split; if(! $seen{$A[0]}) {$seen{$A[0]} = 1; print $_;}}' \
      > data/local/$x || exit 1;
  fi

  rm data/local/${x}.temp
done

echo "------merge two language models------"
lm_1=$dir_1/data/srilm/lm.gz
lm_2=$dir_2/data/srilm/lm.gz
mkdir -p data/srilm
ngram -order 3 -lm $lm_1 -mix-lm $lm_2 -lambda 0.5 -write-lm data/srilm/lm.gz # if max order is 3(3-gram)



mkdir -p data/lang
if [[ ! -f data/lang/L.fst || data/lang/L.fst -ot data/local/lexicon.txt ]]; then
  echo ---------------------------------------------------------------------
  echo "Creating L.fst etc in data/lang on" `date`
  echo ---------------------------------------------------------------------
  utils/prepare_lang.sh \
    --share-silence-phones true \
    data/local $oovSymbol data/local/tmp.lang data/lang
fi

if [[ ! -f data/lang/G.fst || data/lang/G.fst -ot data/srilm/lm.gz ]]; then
  echo ---------------------------------------------------------------------
  echo "Creating G.fst on " `date`
  echo ---------------------------------------------------------------------
  local/arpa2G.sh data/srilm/lm.gz data/lang data/lang
fi

echo "------preparing lang done------"

exit 0

