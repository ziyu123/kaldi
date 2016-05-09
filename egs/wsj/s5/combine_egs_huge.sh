#!/bin/bash

echo "$0 $@" # Print the command line for logging

. ./path.sh

if [ $# != 3 ]; then
  echo "Usage: $0 <src1> <src2> <dir>";
#  exit 1; 
fi

src_1=/nfs/raid88/thintern/tangzy_joint_enhance_swbd/exp/nnet3/lstm_speech_a/egs  #$1
src_2=/nfs/raid88/thintern/tangzy_joint_enhance_fisher/exp/nnet3/lstm_spk_a_2-12/egs  #$2
dir=exp/nnet3/partial_egs  #$3

num_ark_1=`cat $src_1/info/num_archives` || exit 1;
num_ark_2=`cat $src_2/info/num_archives` || exit 1;



if false; then

# egs.*.ark of src_1, or scp, may use 'for' as src_2
echo "scp for egs_1"
nnet3-copy-egs ark:$src_1/egs.all.ark ark,scp:$dir/egs_1/egs.ark,$dir/egs_1/egs.scp || exit 1;

# egs.*.ark of src_2, or scp
echo "scp for egs_2"
rm $dir/egs_2/egs.scp;
touch $dir/egs_2/egs.scp;

for n in $(seq $num_ark_2); do
  nnet3-copy-egs ark:$src_2/egs.$n.ark ark,scp:$dir/egs_2/egs.$n.ark,$dir/egs_2/egs.$n.scp || exit 1;
  cat $dir/egs_2/egs.$n.scp >> $dir/egs_2/egs.scp || exit 1;
done

# scp for all egs
echo "scp for all egs"
cat $dir/egs_1/egs.scp $dir/egs_2/egs.scp > $dir/egs.scp.org;
cat $dir/egs.scp.org | \
  awk '{printf "%.0f %s \n", rand()*9999999999, $0; }' | \
  sort -n | \
  cut -d" " -f2- \
  > $dir/egs.scp

#echo "split egs.scp to small scps:"
mkdir $dir/scp_split
split -d -a 1 -l 2317526 $dir/egs.scp $dir/scp_split10/egs.scp.
mv $dir/scp_split10/egs.scp.0 $dir/scp_split10/egs.scp.10

echo "get egs_list from small scp:"
nj=10
a=1
b=116
for i in $(seq $nj);do
  egs_list=
  for j in $(seq $a $b);do
    egs_list="$egs_list ark:$dir/egs.$j.ark"
  done
  echo $egs_list > $dir/scp_split10/egs.list.$i
  a=$[$a+116]
  b=$[$b+116]
done

echo "get egs from small list and scp:"
for i in $(seq $nj);do
 ( 
  list=`cat $dir/scp_split10/egs.list.$i`
  nnet3-copy-egs scp:$dir/scp_split10/egs.scp.$i $list || exit 1;
 ) &
done

fi
#for file in `ls $dir/scp_split`; do
#  nnet3-copy-egs scp:$dir/scp_split/$file ark:$dir/egs.$n.ark || exit 1;
#  n=$[$n+1]
#done

#other files
echo "other files:"

cp $src_2/cmvn_opts $dir || exit 1;

nnet3-shuffle-egs --srand=-3 "ark:cat $src_1/combine.egs.output_speech $src_2/combine.egs |" ark:$dir/combine.egs || exit 1;

#cp -rf $src_2/info $dir || exit 1;

nnet3-shuffle-egs --srand=-3 "ark:cat $src_1/train_diagnostic.egs.output_speech $src_2/train_diagnostic.egs |" ark:$dir/train_diagnostic.egs || exit 1;

cat $src_1/train_subset_uttlist $src_2/train_subset_uttlist > $dir/train_subset_uttlist || exit 1;


nnet3-shuffle-egs --srand=-3 "ark:cat $src_1/valid_diagnostic.egs.output_speech $src_2/valid_diagnostic.egs |" ark:$dir/valid_diagnostic.egs || exit 1;

cat $src_1/valid_uttlist $src_2/valid_uttlist > $dir/valid_uttlist || exit 1;

wait;

exit 0;

