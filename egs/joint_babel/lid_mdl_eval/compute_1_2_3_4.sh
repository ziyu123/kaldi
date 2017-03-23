#!/bin/bash

. ./path.sh
. ./cmd.sh


nj=8
dir=net_output
mkdir -p $dir

for id in 1 2 3 4; do
  data=data_fbank/test${id}_2k
  sdata=$data/split$nj
  split_data.sh $data $nj
done

for mdl in models/ass_geo_ben_tur_1_2_3_4/* ; do
  for id in 1 2 3 4 ; do 

(
    m_base=`basename $mdl`
    echo ">>>>>>>>>>>${m_base}.${id}"
    data=data_fbank/test${id}_2k
    sdata=$data/split$nj
    $cpu_cmd JOB=1:$nj $dir/log/lid_forward.${m_base}.${id}.JOB.log \
    nnet3-compute --use-gpu=no --extra-left-context=40 --frames-per-chunk=20 $mdl scp:$sdata/JOB/feats.scp ark,t:$dir/output.${m_base}.${id}.JOB.ark || exit 1

    if [ -f $dir/output.${m_base}.${id}.ark ]; then
      rm $dir/output.${m_base}.${id}.ark
    fi

    for job in `seq $nj`; do
      cat $dir/output.${m_base}.${id}.${job}.ark  >> $dir/output.${m_base}.${id}.ark
      rm $dir/output.${m_base}.${id}.${job}.ark
    done
    echo "<<<<<<<<<<<<<<<<<<<<<<<<<<${m_base}.${id}"

) &

  done
done

wait; 
echo "*************All done**************"

exit 0;

