#include "engine/stream-faster-decoder.h"

namespace kaldi {

StreamFasterDecoder::StreamFasterDecoder(
    const fst::Fst<fst::StdArc> &fst,
    const FasterDecoderOptions &config): FasterDecoder(fst, config) { }

void StreamFasterDecoder::Reset() {
  // clean up from last time:
  ClearToks(toks_.Clear());
  StateId start_state = fst_.Start();
  KALDI_ASSERT(start_state != fst::kNoStateId);
  Arc dummy_arc(0, 0, Weight::One(), start_state);
  toks_.Insert(start_state, new Token(dummy_arc, NULL));
  ProcessNonemitting(std::numeric_limits<float>::max());
}

void StreamFasterDecoder::DecodeFrame(DecodableInterface *decodable, int32 frame) {
  // BaseFloat weight_cutoff = ProcessEmitting(decodable, frame);
  BaseFloat weight_cutoff = ProcessEmitting(decodable);
  ProcessNonemitting(weight_cutoff);
}

bool StreamFasterDecoder::GetBestPath(fst::MutableFst<LatticeArc> *fst_out) {
  return FasterDecoder::GetBestPath(fst_out);
}

} //namespace kaldi
