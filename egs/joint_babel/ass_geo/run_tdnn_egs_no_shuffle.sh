#!/bin/bash

# this is the standard "tdnn" system, built in nnet3; it's what we use to
# call multi-splice.

. cmd.sh


# At this script level we don't support not running on GPU, as it would be painfully slow.
# If you want to run without GPU you'd have to call train_tdnn.sh with --gpu false,
# --num-threads 16 and --minibatch-size 128.

stage=0
train_stage=-10 #-10



common_egs_dir=exp/nnet3/egs_merge_dnn
dir=exp/nnet3/dnn_semi_share4


. cmd.sh
. ./path.sh
. ./utils/parse_options.sh


if ! cuda-compiled; then
  cat <<EOF && exit 1
This script is intended to be used with GPUs but you have not compiled Kaldi with CUDA
If you want to use GPUs (and have them), go to src/, and configure and make on a machine
where "nvcc" is installed.
EOF
fi

if [ $stage -le 8 ]; then
  steps/nnet3/train_tdnn_raw_from_egs_no_shuffle.sh --stage $train_stage \
    --num-epochs 8 --num-jobs-initial 2 --num-jobs-final 14 \
    --splice-indexes "-5,-4,-3,-2,-1,0,1,2,3,4,5  0  0  0  0  0" \
    --feat-type raw \
    --cmvn-opts "--norm-means=false --norm-vars=false" \
    --initial-effective-lrate 0.005 --final-effective-lrate 0.0005 \
    --cmd "$cpu_cmd" \
    --gpu-cmd "$gpu_cmd" \
    --relu-dim 1024 \
    --egs-dir "$common_egs_dir" \
    $dir  || exit 1;
fi



exit 0;



if [ $stage -le 9 ]; then
  # this does offline decoding that should give the same results as the real
  # online decoding.
  graph_dir=exp/tri4b/graph
  # use already-built graphs.
  steps/nnet3/decode.sh --nj 15 --cmd "$decode_cmd" \
     $graph_dir data_fbank/test $dir/decode_test || exit 1;
fi


exit 0;

