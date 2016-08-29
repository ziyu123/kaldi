#!/bin/bash

srcdir=exp/tri4b


th_lm=data/local/thchs.3gram.lm.gz
utils/format_lm.sh data/lang $th_lm data/dict/lexicon.txt data/lang_graph/th
utils/mkgraph.sh  data/lang_graph/th $srcdir $srcdir/graph_th


for sub_dir in oc oc_th merge; do
  lm=data/local/lm_$sub_dir/o3g.kn.gz
(
echo '================================= $sub_dir'
utils/format_lm.sh data/lang $lm data/dict/lexicon.txt data/lang_graph/$sub_dir
utils/mkgraph.sh  data/lang_graph/$sub_dir $srcdir $srcdir/graph_$sub_dir
) &

done

wait;
exit 0;

