#!/bin/bash

nj=4
for lang in `seq 7`; do
  ./eval_lid/eval_lid_compute.sh $lang $nj &
  ./eval_lid/eval_lid_compute_utt.sh $lang $nj &
done

wait;

for lang in 5 6 7; do
  ./eval_lid/eval_lid_unknown.py $lang 5 &
  ./eval_lid/eval_lid_utt_unknown.py $lang 5 &
done

wait;

exit 0;

