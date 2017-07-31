#!/bin/sh

if [ -z $4 ]
then
	echo ./demo.sh wav1 wav2 fea1 fea2
	exit -1;
fi

work_path=`pwd`/../

wav1=$1
wav2=$2

fea1=$3
fea2=$4

peak1=$wav1.peak
peak2=$wav2.peak

f01=$wav1.f0
f02=$wav2.f0

cd $work_path/peak
$work_path/peak/compute-peak $wav1 $peak1
$work_path/peak/compute-peak $wav2 $peak2


cd $work_path/pitch
$work_path/pitch/shell_f0.sh $wav1 $f01
$work_path/pitch/shell_f0.sh $wav2 $f02

cd $work_path/score

$work_path/score/compute-mlp-feats $fea1 $fea2 $f01 $f02 $peak1 $peak2
