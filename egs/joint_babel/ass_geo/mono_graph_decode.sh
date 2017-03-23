#!/bin/bash

. path.sh
. cmd.sh

dir1=$1  #/work4/tzy/joint_babel/assamese 
dir2=$2  

ln -s $dir1/data/srilm/lm.gz data/srilm/lmMono1.gz
ln -s $dir2/data/srilm/lm.gz data/srilm/lmMono2.gz

for x in Mono1 Mono2; do


  if [[ ! -f data/lang${x}/G.fst || data/lang${x}/G.fst -ot data/srilm/lm${x}.gz ]]; then
    echo ---------------------------------------------------------------------
    echo "Creating G.fst in data\/lang${x} on " `date`
    echo ---------------------------------------------------------------------
    rm -rf data/lang${x}
    cp -rf data/lang data/lang${x}
    rm data/lang${x}/G.fst
    local/arpa2G.sh data/srilm/lm${x}.gz data/lang${x} data/lang${x}
  fi
  
  echo "------make graph and decode the test set------"
  
  utils/mkgraph.sh data/lang${x} \
     exp/tri4b exp/tri4b/graph${x} || exit 1;
  
  
#  for x in test1 test2; do
#  (
#    steps/decode_fmllr.sh --nj 10 --cmd "$decode_cmd" --stage -12 \
#       exp/tri4b/graph${x} data/$x \
#       exp/tri4b/decode_${x}_$x || exit 1;
#  ) &
#  done


done


wait
echo "------graph done------"


exit 0

