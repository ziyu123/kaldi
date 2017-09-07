// vvscore/simple-score.h

// Copyright  2017  Tsinghua University (author: Zhiyuan Tang)

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

#ifndef KALDI_VVSCORE_SIMPLE_SCORE_H_
#define KALDI_VVSCORE_SIMPLE_SCORE_H_

#include "vvutil/common-utils.h"


namespace kaldi {


/// Score based on average log likelihood per frame for a candidate 
/// wav with referred text (referred wav is optional).
///
/// @param [in] avg_like_ref    A good average log likelihood can 
///        reset the default one (5.6) which is computed 
///        with wavs in ASR training set, for referrence 
///        if no referred wav provided.
double ScoreOnAverageLogLikelihood(std::string wav_rspecifier,
                                   std::string tree_rxfilename,
                                   std::string model_rxfilename,
                                   std::string lex_rxfilename,
                                   std::string transcript_rspecifier,
                                   std::string fbank_option_file = "",
                                   std::string graph_option_file = "",
                                   std::string likelihood_option_file = "",
                                   double avg_like_ref = 5.6);


/// Correlation coeffient score based on the kaldi pitch vectors of two wavs.
double CorrelationScoreOnKaldiPitch(std::string wav_rspecifier,
                                    std::string option_file = ""); 


/// Similarity score based on the kaldi pitch vectors of two wavs.
double SimilarScoreOnKaldiPitch(std::string wav_rspecifier,
                                std::string option_file = ""); 



}  // namespace kaldi
#endif  // KALDI_VVSCORE_SIMPLE_SCORE_H_