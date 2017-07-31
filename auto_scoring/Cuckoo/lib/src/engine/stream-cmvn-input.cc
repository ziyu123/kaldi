#include "engine/stream-cmvn-input.h"
#include "transform/cmvn.h"
namespace kaldi {

StreamCmvnOptions::StreamCmvnOptions(): 
  initial_frame_count(50), norm_vars(false) { }

void StreamCmvnOptions::Register(OptionsItf *po) {
  po->Register("norm-vars", &norm_vars, "Normalize variances");
  po->Register("initial-frame-count", &initial_frame_count, "");
}

StreamCmvnInput::StreamCmvnInput(struct StreamCmvnOptions &opts): opts_(opts) {

}

void StreamCmvnInput::Reset() {
  //current_frame_count_ = opts_.initial_frame_count;
  if (saved_stats_.NumRows()) {
    current_stats_.Resize(saved_stats_.NumRows(), saved_stats_.NumCols());
    current_stats_.CopyFromMat(saved_stats_);
  }
}

int StreamCmvnInput::Process(Matrix<BaseFloat> &feats) {
  Matrix<double> stats;
  InitCmvnStats(feats.NumCols(), &stats);
  AccCmvnStats(feats, NULL, &stats);
  //int32 frames = feats.NumRows();
  //double x = (double)frames / (frames + current_frame_count_);
  //stats.Scale(x);
  //stats.AddMat(1. - x, current_stats_);
  if (current_stats_.NumRows()) {
    stats.AddMat(1., current_stats_);
  }
  ApplyCmvn(stats, opts_.norm_vars, &feats);
  //current_frame_count_ += frames;
  if (current_stats_.NumRows()) {
    current_stats_.CopyFromMat(stats);
  }
  return 0;
}

int StreamCmvnInput::Load(const Matrix<double> &in) {
  saved_stats_.Resize(in.NumRows(), in.NumCols());
  saved_stats_.CopyFromMat(in);
  int32 dim = saved_stats_.NumCols() - 1;
  double count = saved_stats_(0, dim);
  double scale = (double)opts_.initial_frame_count / count;
  // TODO: not considering about variance
  saved_stats_.Scale(scale);
  return 0;
}

int StreamCmvnInput::Save(Matrix<double> &out) {
  KALDI_ERR << __func__ << " unimplemented";
  return -1;
}

StreamCmvnInput::~StreamCmvnInput() {

}

} //namespace kaldi
