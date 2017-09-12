// test/math-test.cc

// Copyright 2017     Tsinghua University (author: Zhiyuan Tang)

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


#include "vvutil/common-vvutils.h"


int main() {
  using namespace kaldi;

  std::vector<double> v1{1,2,3,5,6};
  std::vector<double> v2{1,2,3,6,5};
  std::vector<double> v3{11,2,3,5,3};


  std::cout << "corr coeff is " << CorrelationOfTwoVectors(v1, v2) << "\n";
  std::cout << "corr coeff is " << CorrelationOfTwoVectors(v1, v3) << "\n";

  std::cout << "similar is " << SimilarityOfTwoVectors(v1, v2) << "\n";
  std::cout << "similar is " << SimilarityOfTwoVectors(v1, v3) << "\n";
 
  return 0; 
}


