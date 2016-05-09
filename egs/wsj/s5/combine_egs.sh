#!/bin/bash

echo "$0 $@" # Print the command line for logging

. ./path.sh

if [ $# != 3 ]; then
  echo "Usage: $0 <src1> <src2> <dir>";
#  exit 1; 
fi

src_1=/nfs/raid88/thintern/tangzy_joint_enhance_swbd/exp/nnet3/lstm_speech_a/egs  #$1
src_2=/nfs/raid88/thintern/tangzy_joint_enhance_fisher/exp/nnet3/lstm_spk_a_2-12/egs  #$2
dir=/nfs/raid88/thintern/tangzy_joint_enhance_swbd_fisher/exp/nnet3/partial_egs  #$3

num_ark_1=`cat $src_1/info/num_archives` || exit 1;
num_ark_2=`cat $src_2/info/num_archives` || exit 1;

egs_list=

# egs.*.ark of src_1, 
# if not egs.all.ark, using following:
# for n in $(seq $num_ark_1); do
#   egs_list="$egs_list $src_1/egs.$n.ark"
# done
egs_list="$egs_list $src_1/egs.all.ark"

# egs.*.ark of src_2
for n in $(seq $num_ark_2); do
  egs_list="$egs_list $src_2/egs.$n.ark"
done

new_egs_list=
for n in $(seq $[$num_ark_1+$num_ark_2]); do
  new_egs_list="$new_egs_list ark:$dir/egs.$n.ark"
done

echo "combine egs:"

nnet3-shuffle-egs --srand=-3 "ark:cat $egs_list |" ark:- | nnet3-copy-egs ark:- $new_egs_list || exit 1;


echo "other files:"

cp $src_2/cmvn_opts $dir || exit 1;

nnet3-shuffle-egs --srand=-3 "ark:cat $src_1/combine.egs $src_2/combine.egs |" ark:$dir/combine.egs || exit 1;

cp -rf $src_2/info $dir || exit 1;

nnet3-shuffle-egs --srand=-3 "ark:cat $src_1/train_diagnostic.egs $src_2/train_diagnostic.egs |" ark:$dir/train_diagnostic.egs || exit 1;

cat $src_1/train_subset_uttlist $src_2/train_subset_uttlist > $dir/train_subset_uttlist || exit 1;


nnet3-shuffle-egs --srand=-3 "ark:cat $src_1/valid_diagnostic.egs $src_2/valid_diagnostic.egs |" ark:$dir/valid_diagnostic.egs || exit 1;

cat $src_1/valid_uttlist $src_2/valid_uttlist > $dir/valid_uttlist || exit 1;


exit 0;

