#!/bin/bash

. ./cmd.sh ## You'll want to change cmd.sh to something that will work on your system.
           ## This relates to the queue.
. ./path.sh

H=`pwd`  #exp home
n=12      #parallel jobs

#corpus and trans directory
#thchs=/nfs/public/materials/data/thchs30-openslr
ce80=/work4/tzy/OC16-CE80
#you can obtain the database by uncommting the following lines
#[ -d $thchs ] || mkdir -p $thchs  || exit 1
#echo "downloading THCHS30 at $thchs ..."
#local/download_and_untar.sh $thchs  http://www.openslr.org/resources/18 data_thchs30  || exit 1
#local/download_and_untar.sh $thchs  http://www.openslr.org/resources/18 resource      || exit 1
#local/download_and_untar.sh $thchs  http://www.openslr.org/resources/18 test-noise    || exit 1



#(
#local/oc16_ce80_data_prep_test.sh $H $ce80 || exit 1;
#
##produce MFCC/Fbank features
#cp -R data/test data/fbank || exit 1;
#for x in test; do
#   #make  fbank
#(   steps/make_fbank.sh --nj $n --cmd "$train_cmd" data/fbank/$x data/fbank/$x/log data/fbank/$x/fbank || exit 1;
#   #compute cmvn
#   steps/compute_cmvn_stats.sh data/fbank/$x data/fbank/$x/log data/fbank/$x/cmvn || exit 1;
#) &
#done
#)
#exit 0


#data preparation 
#generate text, wav.scp, utt2pk, spk2utt
if false; then
local/oc16_ce80_data_prep.sh $H $ce80 || exit 1;

#produce MFCC/Fbank features 
rm -rf data/mfcc && mkdir -p data/mfcc &&  cp -R data/{train,dev} data/mfcc || exit 1;
rm -rf data/fbank && mkdir -p data/fbank &&  cp -R data/{train,dev} data/fbank || exit 1;
for x in train dev; do
   #make  mfcc 
(   steps/make_mfcc.sh --nj $n --cmd "$train_cmd" data/mfcc/$x data/mfcc/$x/log data/mfcc/$x/mfcc || exit 1;
   #compute cmvn
   steps/compute_cmvn_stats.sh data/mfcc/$x data/mfcc/$x/log data/mfcc/$x/cmvn || exit 1;
) &
   #make  fbank
(   steps/make_fbank.sh --nj $n --cmd "$train_cmd" data/fbank/$x data/fbank/$x/log data/fbank/$x/fbank || exit 1;
   #compute cmvn
   steps/compute_cmvn_stats.sh data/fbank/$x data/fbank/$x/log data/fbank/$x/cmvn || exit 1;
) &
done


#prepare language stuff
#build a large lexicon that invovles words in both the training and decoding. 
(
  echo "make word graph ..."
  echo "data/dict already prepared" 
  cd $H; mkdir -p data/{lang,graph} || exit 1;
  utils/prepare_lang.sh --position_dependent_phones false data/dict "<SPOKEN_NOISE>" data/local/lang data/lang || exit 1;

  #thchs30_lm=/nfs/public/materials/data/thchs30-openslr/data_thchs30/lm_word/word.3gram.lm

  thchs30_lm=
  local/oc16_train_lms_pure.sh data/local/text_oc data/dict/lexicon.txt data/local/lm_oc $thchs30_lm
  local/oc16_train_lms_pure.sh data/local/text_oc_th data/dict/lexicon.txt data/local/lm_oc_th $thchs30_lm
  thchs30_lm=data/local/thchs.3gram.lm.gz
  local/oc16_train_lms.sh data/local/text_oc data/dict/lexicon.txt data/local/lm_merge $thchs30_lm

#  gzip -c $thchs/data_thchs30/lm_word/word.3gram.lm > data/graph/word.3gram.lm.gz || exit 1;
#  utils/format_lm.sh data/lang data/graph/word.3gram.lm.gz $thchs/data_thchs30/lm_word/lexicon.txt data/graph/lang || exit 1;
)

fi

#monophone
steps/train_mono.sh --boost-silence 1.25 --nj $n --cmd "$train_cmd" data/mfcc/train data/lang exp/mono || exit 1; 
#test monophone model
#local/thchs-30_decode.sh --mono true --nj $n "steps/decode.sh" exp/mono data/mfcc &

#monophone_ali
steps/align_si.sh --boost-silence 1.25 --nj $n --cmd "$train_cmd" data/mfcc/train data/lang exp/mono exp/mono_ali || exit 1;

#triphone
steps/train_deltas.sh --boost-silence 1.25 --cmd "$train_cmd" 2000 10000 data/mfcc/train data/lang exp/mono_ali exp/tri1 || exit 1;
#test tri1 model
#local/thchs-30_decode.sh --nj $n "steps/decode.sh" exp/tri1 data/mfcc &

#triphone_ali
steps/align_si.sh --nj $n --cmd "$train_cmd" data/mfcc/train data/lang exp/tri1 exp/tri1_ali || exit 1;

#lda_mllt
steps/train_lda_mllt.sh --cmd "$train_cmd" --splice-opts "--left-context=3 --right-context=3" 2500 15000 data/mfcc/train data/lang exp/tri1_ali exp/tri2b || exit 1;
#test tri2b model
#local/thchs-30_decode.sh --nj $n "steps/decode.sh" exp/tri2b data/mfcc &


#lda_mllt_ali
steps/align_si.sh  --nj $n --cmd "$train_cmd" --use-graphs true data/mfcc/train data/lang exp/tri2b exp/tri2b_ali || exit 1;

#sat
steps/train_sat.sh --cmd "$train_cmd" 2500 15000 data/mfcc/train data/lang exp/tri2b_ali exp/tri3b || exit 1;
#test tri3b model
#local/thchs-30_decode.sh --nj $n "steps/decode_fmllr.sh" exp/tri3b data/mfcc &

#sat_ali
steps/align_fmllr.sh --nj $n --cmd "$train_cmd" data/mfcc/train data/lang exp/tri3b exp/tri3b_ali || exit 1;

#quick
steps/train_quick.sh --cmd "$train_cmd" 4200 40000 data/mfcc/train data/lang exp/tri3b_ali exp/tri4b || exit 1;
#test tri4b model
#local/thchs-30_decode.sh --nj $n "steps/decode_fmllr.sh" exp/tri4b data/mfcc &

#quick_ali
steps/align_fmllr.sh --nj $n --cmd "$train_cmd" data/mfcc/train data/lang exp/tri4b exp/tri4b_ali || exit 1;

#quick_ali_cv
#steps/align_fmllr.sh --nj $n --cmd "$train_cmd" data/mfcc/dev data/lang exp/tri4b exp/tri4b_ali_cv || exit 1;


exit 0
#train dnn model
#local/nnet/run_dnn.sh --stage 0 --nj $n  exp/tri4b exp/tri4b_ali exp/tri4b_ali_cv || exit 1;  

#train dae model
#python2.6 or above is required for noisy data generation.
#To speed up the process, pyximport for python is recommeded.
#local/dae/run_dae.sh --stage 0  $thchs || exit 1;
