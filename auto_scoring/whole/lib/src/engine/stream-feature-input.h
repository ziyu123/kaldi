#ifndef __STREAM_STREAM_FEATURE_INPUT_H__
#define __STREAM_STREAM_FEATURE_INPUT_H__
#include <string>
#include "feat/feature-mfcc.h"
#include "feat/feature-fbank.h"

namespace kaldi {

using std::string;

struct StreamFeatureOptions {
  string feat_type;
  MfccOptions mfcc_opts;
  FbankOptions fbank_opts;
  void Register(OptionsItf *po);
  int Dim();
};

class StreamFeatureInput {
public:
  StreamFeatureInput(const StreamFeatureOptions &opts);
  virtual void Process(const VectorBase<BaseFloat> &in, Matrix<BaseFloat> &out);
  virtual void Reset();
  virtual ~StreamFeatureInput();

private:
  StreamFeatureOptions opts_;
  const FrameExtractionOptions *frame_opts_;
  Mfcc mfcc_;
  Fbank fbank_;
  Vector<BaseFloat> remainder_;
};

}
#endif
