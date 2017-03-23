#!/bin/bash

# this is the standard "tdnn" system, built in nnet3; it's what we use to
# call multi-splice.


# At this script level we don't support not running on GPU, as it would be painfully slow.
# If you want to run without GPU you'd have to call train_tdnn.sh with --gpu false,
# --num-threads 16 and --minibatch-size 128.

stage=0
train_stage=-10 #-10
dir=exp/nnet3/dnn_lang_raw_rec-1000

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

#    --pnorm-input-dim 2000 \
#    --pnorm-output-dim 250 \
#    --relu-dim 1000 \
#    --splice-indexes "-4,-3,-2,-1,0,1,2,3,4 0 0 0" \

if [ $stage -le 8 ]; then
  steps/nnet3/train_tdnn_spk.sh --stage $train_stage \
    --num-epochs 8 --num-jobs-initial 2 --num-jobs-final 12 \
    --splice-indexes "-4,-3,-2,-1,0,1,2,3,4  0 0 0" \
    --feat-type raw \
    --cmvn-opts "--norm-means=false --norm-vars=false" \
    --initial-effective-lrate 0.005 --final-effective-lrate 0.0005 \
    --cmd "$cpu_cmd" \
    --gpu-cmd "$gpu_cmd" \
    --relu-dim 1000 \
    --use-presoftmax-prior-scale "true" \
    data_fbank/train exp/spk_ali $dir  || exit 1;
fi

exit 0;

