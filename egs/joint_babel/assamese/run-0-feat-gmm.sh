#!/bin/bash

. ./cmd.sh ## You'll want to change cmd.sh to something that will work on your system.
           ## This relates to the queue.


echo "------make mfcc/fbank feautres------"

if false; then

# Now make MFCC features.
for x in train test; do 
 steps/make_mfcc.sh  --nj 30 --cmd "$train_cmd" data/$x || exit 1;
 steps/compute_cmvn_stats.sh data/$x || exit 1;
done


# make fbank features
mkdir -p data_fbank
for x in train test; do
 cp -r data/$x data_fbank/$x
 steps/make_fbank.sh --nj 30 --cmd "$train_cmd" data_fbank/$x || exit 1;
 steps/compute_cmvn_stats.sh data_fbank/$x || exit 1;
done


echo "------train gmm------"

steps/train_mono.sh --boost-silence 1.25 --nj 30 --cmd "$train_cmd" \
  data/train data/lang exp/mono || exit 1;

steps/align_si.sh --boost-silence 1.25 --nj 30 --cmd "$train_cmd" \
  data/train data/lang exp/mono exp/mono_ali || exit 1;

steps/train_deltas.sh --boost-silence 1.25 --cmd "$train_cmd" 2000 10000 \
  data/train data/lang exp/mono_ali exp/tri1 || exit 1;

steps/align_si.sh --nj 30 --cmd "$train_cmd" \
  data/train data/lang exp/tri1 exp/tri1_ali || exit 1;

steps/train_lda_mllt.sh --cmd "$train_cmd" \
  --splice-opts "--left-context=3 --right-context=3" 2500 15000 \
  data/train data/lang exp/tri1_ali exp/tri2b || exit 1;

steps/align_si.sh  --nj 30 --cmd "$train_cmd" --use-graphs true \
  data/train data/lang exp/tri2b exp/tri2b_ali  || exit 1;

steps/train_sat.sh --cmd "$train_cmd" 2500 15000 \
  data/train data/lang exp/tri2b_ali exp/tri3b || exit 1;

steps/align_fmllr.sh --nj 30 --cmd "$train_cmd" \
  data/train data/lang exp/tri3b exp/tri3b_ali || exit 1;

steps/train_quick.sh --cmd "$train_cmd" 4200 40000 \
  data/train data/lang exp/tri3b_ali exp/tri4b || exit 1;

steps/align_fmllr.sh --nj 30 --cmd "$train_cmd" \
  data/train data/lang exp/tri4b exp/tri4b_ali || exit 1;



echo "------make graph and decode the test set------"

utils/mkgraph.sh data/lang \
   exp/tri4b exp/tri4b/graph || exit 1;

fi

steps/decode_fmllr.sh --nj 10 --cmd "$decode_cmd" --stage 2 \
   exp/tri4b/graph data/test \
   exp/tri4b/decode_test || exit 1;


echo "------gmm and decoding done------"
exit 0






