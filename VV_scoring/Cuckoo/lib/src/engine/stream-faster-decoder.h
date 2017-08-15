#ifndef __STREAM_STREAM_FASTER_DECODER_H__
#define __STREAM_STREAM_FASTER_DECODER_H__

#include "decoder/faster-decoder.h"

namespace kaldi {

class StreamDecoder {
public:
  virtual void Reset() = 0;
  virtual void DecodeFrame(DecodableInterface *decodable, int32 frame) = 0;
  virtual bool GetBestPath(fst::MutableFst<LatticeArc> *fst_out) = 0;
  virtual ~StreamDecoder() { };
};

class StreamFasterDecoder: public StreamDecoder, private FasterDecoder {
public:
  StreamFasterDecoder(const fst::Fst<fst::StdArc> &fst,
                      const FasterDecoderOptions &config);
  virtual ~StreamFasterDecoder() { }
  virtual void Reset();
  virtual void DecodeFrame(DecodableInterface *decodable, int32 frame);
  virtual bool GetBestPath(fst::MutableFst<LatticeArc> *fst_out);
  KALDI_DISALLOW_COPY_AND_ASSIGN(StreamFasterDecoder);
};

} // namespace kaldi

#endif

