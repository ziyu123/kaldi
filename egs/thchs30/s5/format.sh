#!/bin/bash

rm -rf SCRIPT_utf8
mkdir SCRIPT_utf8

for i in SCRIPT/*; do
  i_new=SCRIPT_utf8/`basename $i`
  #iconv -f UTF-16LE -t UTF-8 $i > ${i_new}_tmp # this will contain BOM in the front
  python utf16le_to_utf8.py SCRIPT SCRIPT_utf8

  awk 'NR%2' ${i_new}_tmp > ${i_new}_tmp2
  sed -i 's/，/ /g' ${i_new}_tmp2
  sed -i 's/。/ /g' ${i_new}_tmp2
  sed -i 's/？/ /g' ${i_new}_tmp2
  sed -i 's/！/ /g' ${i_new}_tmp2
  sed -i 's/、/ /g' ${i_new}_tmp2
  sed -i 's/；/ /g' ${i_new}_tmp2
  sed -i 's/：/ /g' ${i_new}_tmp2
  
  awk '{print $1}' ${i_new}_tmp2 > ${i_new}_tmp2_1
  awk '{sub(/^[^[:space:]]*[[:space:]]+/,"");print}' ${i_new}_tmp2 > ${i_new}_tmp2_2
  #sed -i 's/[A-Z]/\l&/g' ${i_new}_tmp2_2
  sed -i 's/[a-z]/\u&/g' ${i_new}_tmp2_2

  vocab=../ch_vocab.txt #utf-8
  cat ${i_new}_tmp2_2 | java -jar ../LM_tools_v2c/MultilingualSegmenter.jar $vocab > ${i_new}_tmp2_3 
  paste -d " " ${i_new}_tmp2_1 ${i_new}_tmp2_3 > $i_new
done

rm SCRIPT_utf8/*tmp*
exit 0
