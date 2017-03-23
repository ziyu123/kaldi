#!/bin/bash

. ./cmd.sh ## You'll want to change cmd.sh to something that will work on your system.
           ## This relates to the queue.
. ./path.sh

echo "------make mfcc feautres with energy------
the conf/mfcc.conf should set '--use-energy=true' while default is '--use-energy=false' "

mkdir -p data_energy/{train,test}

for x in train test; do
 cp data/train/{oovCounts,segments,skipped_utts.log,spk2utt,text,utt2spk,wav.scp} data_energy/train
 cp data/test/{oovCounts,segments,skipped_utts.log,spk2utt,text,utt2spk,wav.scp} data_energy/test
 steps/make_mfcc.sh  --nj 30 --cmd "$train_cmd" data_energy/$x || exit 1;

 compute-vad scp:data_energy/$x/feats.scp ark,t:data_energy/$x/feats.vad
 sed -i 's/ \[//g' data_energy/$x/feats.vad
 sed -i 's/ \]//g' data_energy/$x/feats.vad
 feat-to-len scp:data_energy/$x/feats.scp  ark,t:data_energy/$x/feats.len

done

echo "------mfcc feautres with energy, vad, length done------"

exit 0



