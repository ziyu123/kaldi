#ifndef __STREAM_STREAM_CMVN_INPUT_H__
#define __STREAM_STREAM_CMVN_INPUT_H__

#include "itf/options-itf.h"
#include "matrix/kaldi-matrix.h"

namespace kaldi {

struct StreamCmvnOptions {
  StreamCmvnOptions();
  int initial_frame_count;
  bool norm_vars;
  void Register(OptionsItf *po);
};

class StreamCmvnInput {
public:
  StreamCmvnInput(struct StreamCmvnOptions &opts);
  void Reset();
  int Process(Matrix<BaseFloat> &feats);
  int Load(const Matrix<double> &in);
  int Save(Matrix<double> &out);
  ~StreamCmvnInput();
private:
  StreamCmvnOptions opts_;
  Matrix<double> current_stats_, saved_stats_;
  int32 current_frame_count_;
};

} // namespace kaldi
#endif
