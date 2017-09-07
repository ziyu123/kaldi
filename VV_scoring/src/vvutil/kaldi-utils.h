// vvutil/kaldi-utils.h

// Copyright 2009-2012  Microsoft Corporation
//                      Johns Hopkins University (author: Daniel Povey)
//           2017       Tsinghua University (author: Zhiyuan Tang)

// See ../../COPYING for clarification regarding multiple authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.

#ifndef KALDI_VVUTIL_KALDI_UTILS_H_
#define KALDI_VVUTIL_KALDI_UTILS_H_

#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "feat/feature-fbank.h"
#include "feat/wave-reader.h"
#include "tree/context-dep.h"
#include "hmm/transition-model.h"
#include "fstext/fstext-lib.h"
#include "decoder/training-graph-compiler.h"
#include "gmm/am-diag-gmm.h"
#include "hmm/hmm-utils.h"
#include "decoder/decoder-wrappers.h"
#include "nnet3/nnet-am-decodable-simple.h"
#include "nnet3/nnet-utils.h"
#include "lat/kaldi-lattice.h"
#include "feat/pitch-functions.h"

namespace kaldi {


/// Check file path is good.
bool GoodFile(std::string file) {
  std::ifstream is(file.c_str(), std::ifstream::in);
  return is.good();
}


/// Get Mel-filter bank (FBANK) feature of one wav.
void FbankOfOneWav(const WaveData &wave_data,
                   Matrix<BaseFloat> *features,
                   std::string option_file = "");


/// Get training graph of one utterance.
using fst::SymbolTable;
using fst::VectorFst;
using fst::StdArc;
void TrainingGraphOfOneUtt(std::string tree_rxfilename,
                           std::string model_rxfilename,
                           std::string lex_rxfilename,
                           std::vector<int32> transcript,
                           VectorFst<StdArc> *decode_fst,
                           std::string option_file = "");


/// Get average loglikelihood per frame of one wav with referred text.
double AverageLogLikelihoodPerFrame(std::string model_in_filename,
                                    VectorFst<StdArc> decode_fst,
                                    Matrix<BaseFloat> features,
                                    std::string options_file = "");


/// Align feature using nnet3 with referred text.
void AlignFeatWithNnet3();


/// Get phone pronunciation length from alignment.
void GetPhonePronunciationLens();


/// Get raw log kaldi pitch (Hz) or normalized one (default) of one wav.
void GetKalidPitchOfOneWav(const WaveData &wave_data,
                           std::vector<double> *pitch,
                           std::string option_file = "");


}  // namespace kaldi
#endif  // KALDI_VVUTIL_KALDI_UTILS_H_
