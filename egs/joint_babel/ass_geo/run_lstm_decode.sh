#!/bin/bash

# this is a basic lstm script
# LSTM script runs for more epochs than the TDNN script
# and each epoch takes twice the time

# At this script level we don't support not running on GPU, as it would be painfully slow.
# If you want to run without GPU you'd have to call lstm/train.sh with --gpu false

stage=9 #0
train_stage=-10 #-10
affix=

# LSTM options
splice_indexes="-2,-1,0,1,2"
lstm_delay=" -1 "
label_delay=5
num_lstm_layers=1
cell_dim=1024
hidden_dim=1024
recurrent_projection_dim=256
non_recurrent_projection_dim=256
chunk_width=20
chunk_left_context=40
chunk_right_context=0


# training options
num_epochs=10
initial_effective_lrate=0.0006
final_effective_lrate=0.00006
num_jobs_initial=2
num_jobs_final=12
momentum=0.5
num_chunk_per_minibatch=100
samples_per_iter=20000
remove_egs=false

#decode options
extra_left_context=
extra_right_context=
frames_per_chunk=

#End configuration section

echo "$0 $@" # Print the command line for logging

. ./cmd.sh
. ./path.sh
. ./utils/parse_options.sh


if ! cuda-compiled; then
  cat <<EOF && exit 1
This script is intended to be used with GPUs but you have not compiled Kaldi with CUDA
If you want to use GPUs (and have them), go to src/, and configure and make on a machine
where "nvcc" is installed.
EOF
fi

common_egs_dir=
dir=exp/nnet3/lstm_joint_g_raw_shared_4layers_lr0.00006

if [ $stage -le 9 ]; then
  if [ -z $extra_left_context ]; then
    extra_left_context=$chunk_left_context
  fi
  if [ -z $extra_right_context ]; then
    extra_right_context=$chunk_right_context
  fi
  if [ -z $frames_per_chunk ]; then
    frames_per_chunk=$chunk_width
  fi

   graph_dir=exp/tri4b/graphMono1
   # use already-built graphs
   num_jobs=15
   for x in test1; do
   (
     steps/nnet3/decode.sh --nj $num_jobs --cmd "$decode_cmd" --stage -3 \
         --extra-left-context $extra_left_context \
         --extra-right-context $extra_right_context \
         --frames-per-chunk "$frames_per_chunk" \
        $graph_dir data_fbank/$x $dir/decode_Mono1_$x || exit 1;
   ) &
   done
fi

wait;
exit 0;

