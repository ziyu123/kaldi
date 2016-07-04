#!/bin/bash


## utils/prepare_lang.sh ...


srcdir=exp/tri4b
(
echo '=================================aurora'
local/aurora4_format_data.sh
utils/mkgraph.sh data/lang_graph/grpah_aurora4_2      $srcdir $srcdir/graph_tgpr_5k
) &

thchs=/nfs/public/materials/data/thchs30-openslr
src=/work4/maqy/kaldis/kaldi_20160412/egs/thchs30/s5

(
echo '=================================thchs'
utils/format_lm.sh data/lang       $src/data/graph/word.3gram.lm.gz        $thchs/data_thchs30/lm_word/lexicon.txt    data/lang_graph/graph_thchs30_2
utils/mkgraph.sh  data/lang_graph/graph_thchs30_2      $srcdir $srcdir/graph_word
) &

(
echo '=================================thchs30'
utils/format_lm.sh data/lang_all_thchs30_phone $src/data/graph_phone/phone.3gram.lm.gz $thchs/data_thchs30/lm_phone/lexicon.txt   data/lang_graph/graph_thchs30_phone_2
utils/mkgraph.sh data/lang_graph/graph_thchs30_phone_2 $srcdir $srcdir/graph_phone
) &

wait;
exit 0;

