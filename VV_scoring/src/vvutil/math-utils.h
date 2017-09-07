// vvutil/math-utils.h

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

#ifndef KALDI_VVUTIL_MATH_UTILS_H_
#define KALDI_VVUTIL_MATH_UTILS_H_

#include "base/kaldi-common.h"


namespace kaldi {


/// Compute the correlation coefficient of two vectors.
double CorrelationOfTwoVectors(std::vector<double> x, std::vector<double> y);

/// Compute the similarity of two vectors in a simple way by 
/// comparing element-wise sum of 2xy and sum of x^2+y^2.
double SimilarityOfTwoVectors(std::vector<double> x, std::vector<double> y);


/// Convert two vectors to same length.
void ConvertTwoVectorsToSameLength(std::vector<double> *x, std::vector<double> *y);


}  // namespace kaldi
#endif  // KALDI_VVUTIL_MATH_UTILS_H_
