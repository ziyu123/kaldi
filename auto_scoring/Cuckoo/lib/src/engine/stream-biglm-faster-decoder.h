#ifndef __STREAM_STREAM_BIGLM_FASTER_DECODER_H__
#define __STREAM_STREAM_BIGLM_FASTER_DECODER_H__

#include "decoder/biglm-faster-decoder-mod.h"
#include "engine/stream-faster-decoder.h"

namespace kaldi {

class StreamBiglmFasterDecoder: public StreamDecoder, private BiglmFasterDecoderMod {
public:
  StreamBiglmFasterDecoder(const fst::Fst<fst::StdArc> &fst,
                           const FasterDecoderOptions &config, 
                           fst::DeterministicOnDemandFst<fst::StdArc> *lm_diff_fst);
  virtual ~StreamBiglmFasterDecoder() {}
  virtual void Reset();
  virtual void DecodeFrame(DecodableInterface *decodable, int32 frame);
  virtual bool GetBestPath(fst::MutableFst<LatticeArc> *fst_out);
  KALDI_DISALLOW_COPY_AND_ASSIGN(StreamBiglmFasterDecoder);
};

} //namespace kaldi

#endif
