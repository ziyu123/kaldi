#!/bin/bash

srcdir=exp/tri4b
#(
#echo '=================================aurora'
#local/aurora4_format_data.sh
#utils/mkgraph.sh data/lang_graph/grpah_aurora4_2      $srcdir $srcdir/graph_tgpr_5k
#) &


# ngram -lm LM1 -renorm -write-lm LM1norm
# ngram -order N  -lm LM1  -mix-lm LM2 -lambda W -write-lm MIXLM

thchs=/nfs/public/materials/data/thchs30-openslr
(
echo '=================================thchs_word'
utils/format_lm.sh data/lang                    lm_merge/all.word.0.5.lm.gz  $thchs/data_thchs30/lm_word/lexicon.txt    data/lang_graph/graph_word_all
utils/mkgraph.sh  data/lang_graph/graph_word_all      $srcdir $srcdir/graph_word_all
) &

(
echo '=================================thchs_phone'
utils/format_lm.sh data/lang_all_thchs30_phone  lm_merge/all.phone.0.5.lm.gz $thchs/data_thchs30/lm_phone/lexicon.txt   data/lang_graph/graph_phone_all
utils/mkgraph.sh data/lang_graph/graph_phone_all $srcdir $srcdir/graph_phone_all
) &

wait;
exit 0;

