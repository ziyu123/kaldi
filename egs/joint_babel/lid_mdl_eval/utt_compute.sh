#!/bin/bash

. ./path.sh
. ./cmd.sh


dir=net_output
mkdir -p $dir


i=0
for mdl in models/*/1_lid* ; do
  for id in 1 2 3 4 ; do 

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

