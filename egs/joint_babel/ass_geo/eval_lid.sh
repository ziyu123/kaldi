#!/bin/bash

nj=8
for lang in `seq 2`; do

  ./eval_lid/eval_lid_compute.sh $lang $nj &
  ./eval_lid/eval_lid_compute_utt.sh $lang $nj &

done

wait;

exit 0;

