#!/bin/bash

. ./path.sh
. ./cmd.sh


lang_id=$1 #1,2,3,4
nj=$2
dir=eval_lid
mkdir -p $dir

model=test.raw
data=data_fbank/test${lang_id}_1k
sdata=$data/split$nj
split_data.sh $data $nj

$cpu_cmd JOB=1:$nj $dir/log/lid_forward.${lang_id}.JOB.log \
  nnet3-compute --apply-exp=true  --use-gpu=no --extra-left-context=40 --frames-per-chunk=20 $model scp:$sdata/JOB/feats.scp ark,t:$dir/output.JOB.ark${lang_id} || exit 1

if [ -f $dir/output.ark${lang_id} ]; then
  rm $dir/output.ark${lang_id}
fi

for i in `seq $nj`; do
  cat $dir/output.${i}.ark${lang_id}  >> $dir/output.ark${lang_id}
  rm $dir/output.${i}.ark${lang_id}
done


python eval_lid/eval_lid.py $lang_id

exit 0;
