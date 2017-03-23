#!/bin/bash

. ./path.sh
. ./cmd.sh


seg=20

dir=net_output
mkdir -p $dir


i=0
for mdl in models/*/* ; do
  for id in 1_seg${seg}  2_seg${seg}  3_seg${seg}  4_seg${seg}  ; do 

(
    m_base=`basename $mdl`
    echo ">>>>>>>>>>>${m_base}.${id}"

    if [ -f $dir/output.${m_base}.${id}.ark ]; then
      if [ ! -f $dir/output.${m_base}.${id}.ark.utt ]; then
        matrix-sum-rows ark:$dir/output.${m_base}.${id}.ark ark,t:$dir/output.${m_base}.${id}.ark.utt
      fi
    fi

    echo "<<<<<<<<<<<<<<<<<<<<<<<<<<${m_base}.${id}" 

    i=$[i+1]
    if [ $i -eq 6 ]; then
      wait;
      i=0
    fi

) &

  done
done

wait; 
echo "*************All done**************"

exit 0;

