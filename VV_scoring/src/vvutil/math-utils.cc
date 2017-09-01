// vvutil/math-utils.cc

// Copyright 2017  Tsinghua University (author: Zhiyuan Tang)

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

#include "vvutil/math-utils.h"
#include <numeric>


namespace kaldi {


double CorrelationOfTwoVectors(std::vector<double> x, std::vector<double> y) {
  KALDI_ASSERT(x.size()==y.size());
  int len = x.size();

  // mean
  double e_x, e_y;
  e_x = (std::accumulate(x.begin(), x.end(), 0.0)) / len;
  e_y = (std::accumulate(y.begin(), y.end(), 0.0)) / len;

  // variance, covariance
  double v_x, v_y, cov;
  for(int i = 0; i < len; i++) {
    x[i] -= e_x;
    y[i] -= e_y;
  }
  v_x = (std::inner_product(x.begin(), x.end(), x.begin(), 0.0)) / len;
  v_y = (std::inner_product(y.begin(), y.end(), y.begin(), 0.0)) / len;
  cov = (std::inner_product(x.begin(), x.end(), y.begin(), 0.0)) / len;

  // correlation coefficient
  double corr = cov/sqrt(v_x*v_y);
  return corr;
}


}  // namespace kaldi
