#!/bin/sh

if [ -z $1 ] || [ -z $2 ]
then
	echo ./shell_f0.sh wav_name output_f0_file
	exit -1;
fi

wav=$1
f0_file=$wav.k
f0=$2
echo 1 $wav > scp_file

./compute-kaldi-pitch-feats --sample-frequency=8000 scp:scp_file  ark,t:$f0_file 2>> log.testlog

if [ -f $f0_file ]
then
	sed '1d' $f0_file| sed 's/\]//g' |awk '{print $2}' > $f0
else
	echo $f0_file no save
fi

rm scp_file $f0_file

