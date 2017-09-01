// phonelenbin/pl-score-referred-text-wav.cc, from
// bin/ali-to-phones.cc

// Copyright 2009-2011  Microsoft Corporation
//           2015       IMSL, PKU-HKUST (author: Wei Shi)

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


#include "base/kaldi-common.h"
#include "hmm/transition-model.h"
#include "hmm/hmm-utils.h"
#include "util/common-utils.h"
#include "fst/fstlib.h"
#include "vvutil/common-utils.h"

/// Score candidate wav with referred text and wav 
/// based on correlation coefficient of phones' pronunciation lengths.
int main(int argc, char *argv[]) {
  using namespace kaldi;
  typedef kaldi::int32 int32;
  try {
    const char *usage =
        "Usage like ali-to-phones:\n"
        "Convert model-level alignments to phone-sequences (in integer, "
        "not text, form)\n"
        "Usage:  ali-to-phones  [options] <model> <alignments-rspecifier> "
        "<phone-transcript-wspecifier|ctm-wxfilename>\n"
        "e.g.: \n"
        " ali-to-phones 1.mdl ark:1.ali ark:-\n"
        "or:\n"
        " ali-to-phones --ctm-output 1.mdl ark:1.ali 1.ctm\n"
        "See also: show-alignments lattice-align-phones\n";
    ParseOptions po(usage);
    bool per_frame = false;
    bool write_lengths = true;
    bool ctm_output = false;
    BaseFloat frame_shift = 0.01;
    po.Register("ctm-output", &ctm_output,
                "If true, output the alignments in ctm format "
                "(the confidences will be set to 1)");
    po.Register("frame-shift", &frame_shift,
                "frame shift used to control the times of the ctm output");
    po.Register("per-frame", &per_frame,
                "If true, write out the frame-level phone alignment "
                "(else phone sequence)");
    po.Register("write-lengths", &write_lengths,
                "If true, write the #frames for each phone (different format)");


    po.Read(argc, argv);

    KALDI_ASSERT(!(per_frame && write_lengths) && "Incompatible options.");

    if (po.NumArgs() != 3) {
      po.PrintUsage();
      exit(1);
    }

    std::string model_filename = po.GetArg(1),
        alignments_rspecifier = po.GetArg(2);

    TransitionModel trans_model;
    ReadKaldiObject(model_filename, &trans_model);

    SequentialInt32VectorReader reader(alignments_rspecifier);
    std::string empty;
    Int32VectorWriter phones_writer(ctm_output ? empty :
                                    (write_lengths ? empty : po.GetArg(3)));
    Int32PairVectorWriter pair_writer(ctm_output ? empty :
                                      (write_lengths ? po.GetArg(3) : empty));

    std::string ctm_wxfilename(ctm_output ? po.GetArg(3) : empty);
    Output ctm_writer(ctm_wxfilename, false);
    if (ctm_output) {
      ctm_writer.Stream() << std::fixed;
      ctm_writer.Stream().precision(frame_shift >= 0.01 ? 2 : 3);
    }

    int32 n_done = 0;
    std::vector<double> phone_len_ref;  // #frames for each phone for referred wav, in phone's order
    std::vector<double> phone_len;  // #frames for each phone for candidate wav

    // only 2 wavs (referred and candidate) to be processed
    for (int n = 0; !reader.Done() && n <= 1; reader.Next(), n++) {
      std::string key = reader.Key();
      const std::vector<int32> &alignment = reader.Value();

      std::vector<std::vector<int32> > split;
      SplitToPhones(trans_model, alignment, &split);

      if (ctm_output) {
        BaseFloat phone_start = 0.0;
        for (size_t i = 0; i < split.size(); i++) {
          KALDI_ASSERT(!split[i].empty());
          int32 phone = trans_model.TransitionIdToPhone(split[i][0]);
          int32 num_repeats = split[i].size();
          ctm_writer.Stream() << key << " 1 " << phone_start << " "
                      << (frame_shift * num_repeats) << " " << phone << std::endl;
          phone_start += frame_shift * num_repeats;
        }
      } else if (!write_lengths) {
        std::vector<int32> phones;
        for (size_t i = 0; i < split.size(); i++) {
          KALDI_ASSERT(!split[i].empty());
          int32 phone = trans_model.TransitionIdToPhone(split[i][0]);
          int32 num_repeats = split[i].size();
          //KALDI_ASSERT(num_repeats!=0);
          if (per_frame)
            for(int32 j = 0; j < num_repeats; j++)
              phones.push_back(phone);
          else
            phones.push_back(phone);
        }
        phones_writer.Write(key, phones);
      } else {
        std::vector<std::pair<int32, int32> > pairs;
        for (size_t i = 0; i < split.size(); i++) {
          KALDI_ASSERT(split[i].size() > 0);
          int32 phone = trans_model.TransitionIdToPhone(split[i][0]);
          int32 num_repeats = split[i].size();
          //KALDI_ASSERT(num_repeats!=0);
          pairs.push_back(std::make_pair(phone, num_repeats));

          if (n == 0)
            phone_len_ref.push_back(num_repeats);  // referred, the 1st in feats
          else
            phone_len.push_back(num_repeats);  // candidate, the 2nd in feats
        }
        pair_writer.Write(key, pairs);
      }
      n_done++;
    }

    if (n_done < 2) {
        KALDI_LOG << n_done << " wav(s) processed, not enough.";
        return 1;
    }
    if (phone_len_ref.size() == phone_len.size()) {
        double corr = CorrelationOfTwoVectors(phone_len_ref, phone_len);  // between -1 and 1
        double score = (corr - (-1)) / 2 * 100;  // between 0 and 100
        KALDI_LOG << "The score based on correlation coefficient of "
                  << "phones' pronunciation length is "
                  << score;
    }
    else {
        KALDI_LOG << "The 2 wavs have different numbers of phones.";
        return 1;
    }
    return 0;
        
  } catch(const std::exception &e) {
    std::cerr << e.what();
    return -1;
  }
}



