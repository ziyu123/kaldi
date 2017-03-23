#!/bin/bash

. path.sh
. cmd.sh

if false; then

if [[ ! -f data/lang2gram/G.fst || data/lang2gram/G.fst -ot data/srilm/lm2gram.gz ]]; then
  echo ---------------------------------------------------------------------
  echo "Creating G.fst in data\/lang2gram on " `date`
  echo ---------------------------------------------------------------------
  rm -rf data/lang2gram
  cp -rf data/lang data/lang2gram
  rm data/lang2gram/G.fst
  local/arpa2G.sh data/srilm/lm2gram.gz data/lang2gram data/lang2gram
fi

echo "------make graph and decode the test set------"

utils/mkgraph.sh data/lang2gram \
   exp/tri4b exp/tri4b/graph2gram || exit 1;

fi

for x in test; do
(
  steps/decode_fmllr.sh --nj 10 --cmd "$decode_cmd" --stage -2 \
     exp/tri4b/graph2gram data/$x \
     exp/tri4b/decode_2gram_$x || exit 1;
) &
done

wait
echo "------2 gram LM decoding done------"


exit 0

