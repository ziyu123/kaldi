#!/bin/bash

. ./path.sh


sed -i 's/Append(Offset(input, -2), Offset(input, -1), input, Offset(input, 1), Offset(input, 2))/Append(Offset(ivector, -2), Offset(ivector, -1), ivector, Offset(ivector, 1), Offset(ivector, 2))/g' test.raw
sed -i 's/spk_L0_lda input=ivector/spk_L0_lda input=input/g' test.raw
sed -i '/output input/d' test.raw
sed -i 's/output_plus/output/g' test.raw

#for model in exp/nnet3/lstm_lang_raw/final.raw; do
for model in test.raw; do

#nnet3-compute --apply-exp=true  --use-gpu=no --extra-left-context=40 --frames-per-chunk=20 $model scp:data_fbank/test2_1k/feats.scp ark,t:eval_lid/lstm_lang_raw.ark
#nnet3-compute --apply-exp=true  --use-gpu=no --extra-left-context=40 --frames-per-chunk=20 $model scp:exp/nnet3/ivectors_test2_dim23_per1/ivector_online.scp.1k ark,t:eval_lid/iv_as_input.ark

#nnet3-compute --apply-exp=true --use-gpu=no --extra-left-context=40 --frames-per-chunk=20 --online-ivector-period=1  --online-ivectors='scp:exp/nnet3/ivectors_test2_1k_dim23_per1/ivector_online.scp' test.raw scp:data_fbank/test2_1k/feats.scp ark,t:|less

nnet3-compute --apply-exp=true --use-gpu=no --extra-left-context=40 --frames-per-chunk=20 --online-ivector-period=1  --online-ivectors='scp:data_fbank/test2_1k/feats.scp' test.raw scp:exp/nnet3/ivectors_test2_1k_dim23_per1/ivector_online.scp ark,t:eval_lid/lstm_lang_raw.ark

done

if [ -f eval_lid/output.ark ]; then
  rm eval_lid/output.ark
fi

ln -s lstm_lang_raw.ark eval_lid/output.ark
python eval_lid.py




#nnet3-compute --apply-exp=true --use-gpu=no --extra-left-context=40 --frames-per-chunk=20 --online-ivector-period=1  --online-ivectors='scp:exp/nnet3/ivectors_test2_dim23_per1/ivector_online.scp' test.raw scp:data_fbank/test2/feats.scp ark,t:|less


#nnet3-compute --apply-exp=true  --use-gpu=no --extra-left-context=40 --frames-per-chunk=20 test.raw scp:data_fbank/test2/feats.scp ark,t:|less

#nnet3-compute --apply-exp=true --use-gpu=no --extra-left-context=40 --frames-per-chunk=20 test.raw scp:exp/nnet3/ivectors_test2_dim23_per1/ivector_online.scp ark,t:|less

#nnet3-compute --apply-exp=true --use-gpu=no  test.raw scp:exp/nnet3/ivectors_test1_dim23_per1/ivector_online.scp ark,t:|less

