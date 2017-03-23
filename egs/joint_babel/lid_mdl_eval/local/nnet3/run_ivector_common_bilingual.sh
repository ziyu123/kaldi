#!/bin/bash

# this script is called from scripts like run_ms.sh; it does the common stages
# of the build, such as feature extraction.
# This is actually the same as local/online/run_nnet2_common.sh, except
# for the directory names.

. cmd.sh

stage=1

ivector_dim=20
ivector_period=1

. cmd.sh
. ./path.sh
. ./utils/parse_options.sh


if [ $stage -le 1 ]; then
  for datadir in train test1 test2; do
    utils/copy_data_dir.sh data/$datadir data/${datadir}_hires
    steps/make_mfcc.sh --nj 30 --mfcc-config conf/mfcc_hires.conf \
      --cmd "$train_cmd" data/${datadir}_hires || exit 1;
    steps/compute_cmvn_stats.sh data/${datadir}_hires || exit 1;
  done
  #utils/subset_data_dir.sh --first data/train_si284_hires 7138 data/train_si84_hires || exit 1
  utils/subset_data_dir.sh data/train_hires 10000 data/train_10k_hires || exit 1
  utils/subset_data_dir.sh --utt-list data/train_10k_hires/feats.scp data/train data/train_10k || exit 1
fi

if [ $stage -le 2 ]; then
  # We need to build a small system just because we need the LDA+MLLT transform
  # to train the diag-UBM on top of.  We align the 10k data for this purpose.

  steps/align_fmllr.sh --nj 30 --cmd "$train_cmd" \
    data/train_10k data/lang exp/tri4b exp/nnet3/tri4b_ali_10k
fi

if [ $stage -le 3 ]; then
  # Train a small system just for its LDA+MLLT transform.  We use --num-iters 13
  # because after we get the transform (12th iter is the last), any further
  # training is pointless.
  steps/train_lda_mllt.sh --cmd "$train_cmd" --num-iters 13 \
    --realign-iters "" \
    --splice-opts "--left-context=2 --right-context=2" \
    5000 10000 data/train_10k_hires data/lang \
     exp/nnet3/tri4b_ali_10k exp/nnet3/tri5b_for_ivector
fi

if [ $stage -le 4 ]; then
  mkdir -p exp/nnet3

  steps/online/nnet2/train_diag_ubm.sh --cmd "$train_cmd" --nj 30 \
     --num-frames 400000 data/train_10k_hires 256 exp/nnet3/tri5b_for_ivector exp/nnet3/diag_ubm
fi

if [ $stage -le 5 ]; then
  # even though $nj is just 10, each job uses multiple processes and threads.
  steps/online/nnet2/train_ivector_extractor.sh --cmd "$train_cmd" --nj 10 --ivector-dim $ivector_dim \
    data/train_hires exp/nnet3/diag_ubm exp/nnet3/extractor || exit 1;
fi

if [ $stage -le 6 ]; then
  # We extract iVectors on all the train data, which will be what we
  # train the system on.

  # having a larger number of speakers is helpful for generalization, and to
  # handle per-utterance decoding well (iVector starts at zero).
  steps/online/nnet2/copy_data_dir.sh --utts-per-spk-max 2 data/train_hires \
    data/train_hires_max2

  steps/online/nnet2/extract_ivectors_online.sh --cmd "$train_cmd" --nj 30 --ivector-period $ivector_period \
    data/train_hires_max2 exp/nnet3/extractor exp/nnet3/ivectors_train_dim${ivector_dim}_per$ivector_period || exit 1;
fi

if [ $stage -le 7 ]; then
  rm exp/nnet3/.error 2>/dev/null
  for data in test1 test2; do
    steps/online/nnet2/extract_ivectors_online.sh --cmd "$train_cmd" --nj 8 --ivector-period $ivector_period \
      data/${data}_hires exp/nnet3/extractor exp/nnet3/ivectors_${data}_dim${ivector_dim}_per$ivector_period || touch exp/nnet3/.error &
  done
  wait
  [ -f exp/nnet3/.error ] && echo "$0: error extracting iVectors." && exit 1;
fi

exit 0;
