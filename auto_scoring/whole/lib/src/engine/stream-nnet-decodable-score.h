#ifndef __STREAM_STREAM_NNET_DECODABLE_SCORE_H__
#define __STREAM_STREAM_NNET_DECODABLE_SCORE_H__
#include "feat/feature-mfcc.h"
#include "feat/feature-fbank.h"
#include "hmm/transition-model.h"
#include "nnet/nnet-nnet.h"
#include "nnet/nnet-pdf-prior.h"
#include "engine/stream-decodable.h"
#include "engine/stream-feature-input.h"
#include "engine/stream-cmvn-input.h"

#include <string>
#include "utils.h"

namespace kaldi {

using std::string;
using nnet1::Nnet;
using nnet1::PdfPrior;
using nnet1::PdfPriorOptions;

struct StreamNnetDecodableOptions {
  StreamFeatureOptions feature_opts;
  StreamCmvnOptions cmvn_opts;
  PdfPriorOptions pdf_prior_opts;
  // nnet_transf 
  // FIXME should read from file but not specified here; we can't get internal states of Splice now
  int splice_left_ctx, splice_right_ctx;
  // nnet
  string feature_transform;
  bool no_softmax;
  bool apply_log;
  string model_nnet_filename;
  // decodable
  BaseFloat acoustic_scale;

  StreamNnetDecodableOptions();
  void Register(OptionsItf *po);
};

class StreamNnetDecodable: public StreamDecodableInterface {
public:
  StreamNnetDecodable(StreamNnetDecodableOptions config);
  StreamNnetDecodable(StreamNnetDecodableOptions config,TransitionModel*);

  virtual bool GetResult(const std::vector<int32> &phones,std::vector<nnet1::PhoneConfidence> &phone_confs,
  		int *frame_array,Matrix<BaseFloat>**features); 
  virtual int32 SetData(const char *data, uint32_t len, int chunk_no);
  virtual void Reset();
  virtual int LoadStat(const Matrix<double> &in);
  virtual int SaveStat(Matrix<double> &out);
  virtual inline BaseFloat LogLikelihood(int32 frame, int32 index);
  virtual bool IsLastFrame(int32 frame) const;
  virtual int32 NumIndices() const;
  
  virtual ~StreamNnetDecodable();
private:
  int32 Process(const Matrix<BaseFloat> &in, Matrix<BaseFloat> &out, int chunk_no);
  StreamNnetDecodableOptions config_;
  //feature
  StreamFeatureInput *feat_input_;
  StreamCmvnInput *cmvn_input_;
  PdfPrior pdf_prior_;
  Nnet nnet_, nnet_transf_;
  TransitionModel *trans_model_;
  Matrix<BaseFloat> remainder_;
  int32 remainder_size_;
  
  Matrix<BaseFloat> fbank_;
  Matrix<BaseFloat> likelihood_;
  Vector<BaseFloat> priors_host;
};

} //namespace kaldi

#endif
