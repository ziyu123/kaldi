#!/bin/bash

. path.sh
. cmd.sh


if [[ ! -f data/lang1gram/G.fst || data/lang1gram/G.fst -ot data/srilm/lm1gram.gz ]]; then
  echo ---------------------------------------------------------------------
  echo "Creating G.fst in data\/lang1gram on " `date`
  echo ---------------------------------------------------------------------
  rm -rf data/lang1gram
  cp -rf data/lang data/lang1gram
  rm data/lang1gram/G.fst
  local/arpa2G.sh data/srilm/lm1gram.gz data/lang1gram data/lang1gram
fi

echo "------make graph and decode the test set------"

utils/mkgraph.sh data/lang1gram \
   exp/tri4b exp/tri4b/graph1gram || exit 1;


exit 0;


for x in test1 test2; do
(
  steps/decode_fmllr.sh --nj 10 --cmd "$decode_cmd" --stage -2 \
     exp/tri4b/graph1gram data/$x \
     exp/tri4b/decode_1gram_$x || exit 1;
) &
done

wait
echo "------2 gram LM decoding done------"


exit 0

