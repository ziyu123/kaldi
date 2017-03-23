#!/bin/bash

. path.sh

echo "$0: copying data alignments to phones"

mkdir -p exp/spk_ali_from_asr
dir=exp/spk_ali_from_asr
alidir=exp/tri4b_ali
if [ -f $dir/phone.ali ]; then
  rm $dir/phone.ali
fi

nj=`cat $alidir/num_jobs`

for id in $(seq $nj); do
 ali-to-phones --per-frame=true $alidir/final.mdl ark:"gunzip -c $alidir/ali.$id.gz |" ark,t:$dir/phone.ali.temp;
 cat $dir/phone.ali.temp >> $dir/phone.ali || exit 1;
done

sort $dir/phone.ali > $dir/phone.ali.temp
mv $dir/phone.ali.temp $dir/phone.ali
