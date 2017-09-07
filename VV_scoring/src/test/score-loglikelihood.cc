// test/kaldi-test.cc

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

#include "vvscore/simple-score.h"


int main(int argc, char *argv[]) {
  using namespace kaldi;
  std :: string wav_rspecifier(argv[1]), 
         tree_rxfilename(argv[2]), 
         model_rxfilename(argv[3]), 
         lex_rxfilename(argv[4]), 
         transcript_rspecifier(argv[5]),
         fbank_option_file(argv[6]),
         graph_option_file(argv[7]),
         likelihood_option_file(argv[8]);
  double score = ScoreOnAverageLogLikelihood(wav_rspecifier, tree_rxfilename, 
                              model_rxfilename, lex_rxfilename, 
                              transcript_rspecifier,
                              fbank_option_file,
                              graph_option_file,
                              likelihood_option_file);
  std::cout << "ScoreOnAverageLogLikelihood is: " << score << "\n";
  return 0;
}

