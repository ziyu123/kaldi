#!/bin/bash

. ./path.sh
. ./cmd.sh


nj=8
dir=net_output
new_dir=lid_eval2_utt
mkdir -p $new_dir


i=0
for mdl in models/ben_tur_3_4/* ; do
  for id in 3 4; do 
    m_base=`basename $mdl`
    echo ">>>>>>>>>>>${m_base}.${id}"

    if [  -f $dir/output.${m_base}.${id}.ark.utt_average ]; then
      sed 's/\[//g' $dir/output.${m_base}.${id}.ark.utt_average > $new_dir/temp
      sed -i 's/\]//g' $new_dir/temp
      awk -v lang=lang$id '{print lang, $2, $3}' $new_dir/temp >> $new_dir/output.${m_base}.ark.utt_average
      rm $new_dir/temp
    fi
    echo "<<<<<<<<<<<<<<<<<<<<<<<<<<${m_base}.${id}"
  done

  sed -i '1s/^/      lang3    lang4\n/' $new_dir/output.${m_base}.ark.utt_average
done

wait; 
echo "*************All done**************"

exit 0;

