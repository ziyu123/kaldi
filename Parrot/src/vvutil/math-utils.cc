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
  KALDI_ASSERT(x.size()==y.size() && x.size()>0);
  int len = x.size();

  // mean
  double e_x, e_y;
  e_x = (std::accumulate(x.begin(), x.end(), 0.0)) / len;  // <numeric>
  e_y = (std::accumulate(y.begin(), y.end(), 0.0)) / len;

  // variance, covariance
  double v_x, v_y, cov;
  for(int i = 0; i < len; i++) {
    x[i] -= e_x;
    y[i] -= e_y;
  }
  v_x = (std::inner_product(x.begin(), x.end(), x.begin(), 0.0)) / len;  // <numeric>
  v_y = (std::inner_product(y.begin(), y.end(), y.begin(), 0.0)) / len;
  cov = (std::inner_product(x.begin(), x.end(), y.begin(), 0.0)) / len;

  // correlation coefficient
  if (v_x*v_y == 0.0)
    KALDI_ERR << "Correlation coefficient failed. All rezos.";
  return cov/sqrt(v_x*v_y);
}


double SimilarityOfTwoVectors(std::vector<double> x, std::vector<double> y) {
  KALDI_ASSERT(x.size()==y.size() && x.size()>0);
  int len = x.size();

  // element-wise x^2, y^2, xy
  // similarity is average of sum 2xy/(x^2+y^2)
  double x2;
  double y2;
  double xy;
  double similarity = 0.0;
  for(int i = 0; i < len; i++) {
    x2 = x[i]*x[i];
    y2 = y[i]*y[i];
    xy = x[i]*y[i];
    if ( x2+y2 != 0.0 )
      similarity += xy * 2 / (x2 + y2);
    else
      similarity += 1;  // when all equal 0
  }
  similarity /= len;
  return similarity;
}


void ConvertTwoVectorsToSameLength(std::vector<double> *x, std::vector<double> *y) {
  KALDI_ASSERT(x != NULL && y != NULL);
  if (x->size() == y->size())
    return;
  
  int32 len = (x->size() < y->size() ? x->size() : y->size());
  std::vector<double> *&vec = (x->size() < y->size() ? y : x);  // to be converted
  int32 len_m = vec->size();
  int32 filt = len_m - len + 1;  // pooling size
  int32 j = 0;
  for(std::vector<double>::iterator i = vec->begin(); i+filt-1 != vec->end(); i++) {
    (*vec)[j] = (std::accumulate(i, i+filt, 0.0)) / filt;  // average pooling, <numeric>
    j++;
  }

  // remove the last (len_n-len) elements of vec
  for(int32 k=0; k<len_m-len; k++)
    vec->pop_back();
}




}  // namespace kaldi
