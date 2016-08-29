#!/bin/bash
# Copyright 2016  Tsinghua University (Author: Dong Wang, Xuewei Zhang).  Apache 2.0.
#           2016  LeSpeech (Author: Xingyu Na)

#This script pepares the data directory for thchs30 recipe. 
#It reads the corpus and get wav.scp and transcriptions.

dir=$1
corpus_dir=$2


cd $dir

echo "creating data/{train,dev}"
mkdir -p data/{train,dev}

#create wav.scp, utt2spk.scp, spk2utt.scp, text
(
for x in train dev; do
  echo "cleaning data/$x"
  cd $dir/data/$x
  rm -rf wav.scp utt2spk spk2utt text
  echo "preparing scps and text in data/$x"
  for spk_dir in $corpus_dir/$x/WAVE/*; do
    spk_id=`basename $spk_dir`
    spk_id=${spk_id#*Speaker}
    for wav in $spk_dir/*; do
      wav_id=`basename $wav .wav`
      utt_id=${spk_id}_${wav_id}
      echo $utt_id $wav >> wav.scp
      echo $utt_id $spk_id >> utt2spk
    done
  done
  sort wav.scp -o wav.scp
  sort utt2spk -o utt2spk

  for y in $corpus_dir/$x/SCRIPT_utf8/*; do
    spk_id=`basename $y .txt`
    spk_id=${spk_id#*Speaker}
    sed "s/^/${spk_id}_/g" $y >> $dir/data/$x/text
  done
  sort $dir/data/$x/text -o $dir/data/$x/text
done

) || exit 1

utils/utt2spk_to_spk2utt.pl data/train/utt2spk > data/train/spk2utt
utils/utt2spk_to_spk2utt.pl data/dev/utt2spk > data/dev/spk2utt

exit 0

