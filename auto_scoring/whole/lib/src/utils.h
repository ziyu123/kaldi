#ifndef __KALDI_SCORE_UTILS_H__
#define __KALDI_SCORE_UTILS_H__

#include "nnet/nnet-nnet.h"
#include "feat/wave-reader.h"
#include "feat/feature-mfcc.h"
#include "feat/feature-fbank.h"
#include "transform/cmvn.h"
#include "hmm/transition-model.h"
#include "base/kaldi-error.h"
#include <vector>
namespace kaldi {

namespace nnet1 {

struct PhoneConfidence {
    int32 phone_no;
    BaseFloat conf;
    int32 phone_start;
    int32 phone_end;
    PhoneConfidence(int32 phone_no, BaseFloat conf,int32 count): phone_no(phone_no), conf(conf) {}
    PhoneConfidence(int32 phone_no, BaseFloat conf,int32 phone_start, int32 phone_end): phone_no(phone_no),
		conf(conf),phone_start(phone_start),phone_end(phone_end) {}
};
const int EXP_LEN = 8;
BaseFloat ComputeConfidence(const TransitionModel &trans_model, 
                            const std::vector<int32> &alignment,
                            const Matrix<BaseFloat> &loglikes,
                            const Vector<BaseFloat> &priors, 
							int *frame_array,
                            std::vector<PhoneConfidence> *conf_outputs);
} //namespace
} //namespace
#endif

