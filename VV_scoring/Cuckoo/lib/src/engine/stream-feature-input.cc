#include "engine/stream-feature-input.h"

namespace kaldi {

void StreamFeatureOptions::Register(OptionsItf *po) {
  po->Register("feat-type", &feat_type, "feature type. (Available types: MFCC, Fbank)");
  //FIXME another generic options parser should be implemented
  ParseOptions po_mfcc("mfcc", (ParseOptions *)po);
  ParseOptions po_fbank("fbank", (ParseOptions *)po);
  mfcc_opts.Register(&po_mfcc);
  fbank_opts.Register(&po_fbank);
}

int StreamFeatureOptions::Dim() {
  if (feat_type == "mfcc")
    return mfcc_opts.num_ceps;
  else if (feat_type == "fbank")
    return fbank_opts.mel_opts.num_bins;
  else
    return -1;
}

StreamFeatureInput::StreamFeatureInput(const StreamFeatureOptions &opts):
  opts_(opts), mfcc_(opts.mfcc_opts), fbank_(opts.fbank_opts) {
  if (opts.feat_type == "mfcc")
    frame_opts_ = &opts_.mfcc_opts.frame_opts;
  else if (opts.feat_type == "fbank")
    frame_opts_ = &opts_.fbank_opts.frame_opts;
}

void StreamFeatureInput::Reset() {
  remainder_.Resize(0);
}

void StreamFeatureInput::Process(const VectorBase<BaseFloat> &in, Matrix<BaseFloat> &out) {
  Vector<BaseFloat> wave;
  if (remainder_.Dim()) {
    wave.Resize(remainder_.Dim() + in.Dim());
    SubVector<BaseFloat> head(wave, 0, remainder_.Dim());
    head.CopyFromVec(remainder_);
    SubVector<BaseFloat> tail(wave, remainder_.Dim(), in.Dim());
    tail.CopyFromVec(in);
  } else {
    wave.Resize(in.Dim());
    wave.CopyFromVec(in);
  }

  if (!NumFrames(wave.Dim(), *frame_opts_)) {
    remainder_.Resize(wave.Dim());
    remainder_.CopyFromVec(wave);
    out.Resize(0, 0);
    return;
  }

  if (opts_.feat_type == "mfcc") {
    mfcc_.Compute(wave, 1.0, &out, &remainder_);

  }
  else if (opts_.feat_type == "fbank") {
    fbank_.Compute(wave, 1.0, &out, &remainder_);
  }
}

StreamFeatureInput::~StreamFeatureInput() { 

}


}
