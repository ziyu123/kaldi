#include "engine/stream-biglm-faster-decoder.h"

namespace kaldi {

StreamBiglmFasterDecoder::StreamBiglmFasterDecoder(
    const fst::Fst<fst::StdArc> &fst,
    const FasterDecoderOptions &config, 
    fst::DeterministicOnDemandFst<fst::StdArc> *lm_diff_fst):
    BiglmFasterDecoderMod(fst, config, lm_diff_fst) { }

void StreamBiglmFasterDecoder::Reset() {
  // clean up from last time:
  ClearToks(toks_.Clear());
  StateId start_state = fst_.Start();
  KALDI_ASSERT(start_state != fst::kNoStateId);
  Arc dummy_arc(0, 0, Weight::One(), start_state);
  toks_.Insert(start_state, new Token(dummy_arc, NULL));
  ProcessNonemitting(std::numeric_limits<float>::max());
}

void StreamBiglmFasterDecoder::DecodeFrame(DecodableInterface *decodable, int32 frame) {
  BaseFloat weight_cutoff = ProcessEmitting(decodable, frame);
  ProcessNonemitting(weight_cutoff);
}

bool StreamBiglmFasterDecoder::GetBestPath(fst::MutableFst<LatticeArc> *fst_out) {
  return BiglmFasterDecoderMod::GetBestPath(fst_out);
}

} //namespace kaldi
