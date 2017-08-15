#ifndef __STREAM_STREAM_HCLG_DECODER_H__
#define __STREAM_STREAM_HCLG_DECODER_H__

#include <cstring>
#include <string>
#include "fstext/fstext-lib.h"
#include "engine/stream-decodable.h"
#include "engine/stream-faster-decoder.h"
#include "engine/stream-biglm-faster-decoder.h"

#include "utils.h"

namespace kaldi {

using std::string;
using fst::StdArc;

struct StreamHCLGDecoderOptions {
  StreamHCLGDecoderOptions();
  FasterDecoderOptions decoder_opts;
  // input
  //string model_trans_filename;
  string fst_filename;
  // biglm
  bool biglm;
  string old_lm_fst_filename;
  string new_lm_fst_filename;
  void Register(OptionsItf *po);
};

class StreamHCLGDecoder {
public:
  StreamHCLGDecoder(const StreamHCLGDecoderOptions &config, StreamDecodableInterface &decodable, fst::Fst<fst::StdArc> *decode_fst = NULL,fst::SymbolTable *word_syms=NULL);
  bool ChangeFst(fst::Fst<fst::StdArc> *decode_fst);
  int Decode(const char *data, uint32 len, int chunk_no);
  bool GetResult(std::vector<int32> &word,std::vector<nnet1::PhoneConfidence> &phone_confs,int *,Matrix<BaseFloat>**); 

  void Reset();
  ~StreamHCLGDecoder();


private:
  fst::Fst<fst::StdArc> *ReadNetwork(std::string filename);
  fst::SymbolTable *word_syms_;
  fst::Fst<StdArc> *decode_fst_;
  bool own_decode_fst_;
  // biglm
  fst::ComposeDeterministicOnDemandFst<StdArc> *compose_dfst_;
  fst::CacheDeterministicOnDemandFst<StdArc> *cache_dfst_;
  fst::BackoffDeterministicOnDemandFst<StdArc> *old_lm_dfst_;
  fst::BackoffDeterministicOnDemandFst<StdArc> *new_lm_dfst_;
  fst::VectorFst<StdArc> *old_lm_fst_;
  fst::Fst<StdArc> *new_lm_fst_;
  StreamDecoder *decoder_;
  StreamDecodableInterface &decodable_;//just a refrence
  StreamHCLGDecoderOptions opts_;
  int32 curr_frame_;
};
}

#endif
