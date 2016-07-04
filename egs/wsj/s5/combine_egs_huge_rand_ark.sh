#!/bin/bash

echo "$0 $@" # Print the command line for logging

. ./path.sh


src_1=/nfs/raid88/thintern/tangzy_joint_enhance_swbd/exp/nnet3/lstm_speech_a/egs  #$1
src_2=/nfs/raid88/thintern/tangzy_joint_enhance_fisher/exp/nnet3/lstm_spk_a_2-12/egs  #$2
dir=exp/nnet3/partial_egs_rand_ark  #$3

num_ark_1=`cat $src_1/info/num_archives` || exit 1;
num_ark_2=`cat $src_2/info/num_archives` || exit 1;

if false; then

# small scp
split -d -a 1 -l 646486 exp/nnet3/partial_egs/egs_1/egs.scp $dir/egs_1/scp_split9/egs.scp.

mv $dir/egs_1/scp_split9/egs.scp.0 $dir/egs_1/scp_split9/egs.scp.9

# small egs.list
nj=9
a=1
b=31
for i in $(seq $nj);do
  egs_list=
  for j in $(seq $a $b);do
    egs_list="$egs_list ark:$dir/egs_1/egs.$j.ark"
  done
  echo $egs_list > $dir/egs_1/scp_split9/egs.list.$i
  a=$[$a+31]
  b=$[$b+31]
done

nj=9
for i in $(seq $nj);do
  (
    list=`cat $dir/egs_1/scp_split9/egs.list.$i`
    nnet3-copy-egs scp:$dir/egs_1/scp_split9/egs.scp.$i $list
  ) &
done


realpath exp/nnet3/partial_egs_rand_ark/egs_1/egs.*.ark > $dir/egs_1.path
realpath exp/nnet3/partial_egs/egs_2/egs.*.ark >          $dir/egs_2.path

cat $dir/egs_1.path $dir/egs_2.path > $dir/egs.path.org


cat $dir/egs.path.org | \
  awk '{printf "%.0f %s \n", rand()*999999999, $0; }' | \
  sort -n | \
  cut -d" " -f2- \
  > $dir/egs.path

fi


split -d -a 1 -l 46 exp/nnet3/partial_egs_rand_ark_414/egs.path exp/nnet3/partial_egs_rand_ark_414/path_split9/egs.path.

mv exp/nnet3/partial_egs_rand_ark_414/path_split9/egs.path.0 exp/nnet3/partial_egs_rand_ark_414/path_split9/egs.path.9



nj=9
for i in $(seq $nj);do
  ( 
    j=$[1+46*$[$i-1]]
    echo $j
    for LINE in `cat exp/nnet3/partial_egs_rand_ark_414/path_split9/egs.path.$i`; do
       cp $LINE exp/nnet3/partial_egs_rand_ark_414/egs.${j}.ark
       j=$[$j+1]
    done
  ) &
done

wait;

exit 0;


