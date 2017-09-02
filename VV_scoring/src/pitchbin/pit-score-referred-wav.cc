// pitchbin/pit-score-referred-wav.cc, from
// featbin/compute-and-process-kaldi-pitch-feats.cc

// Copyright 2013        Pegah Ghahremani
//           2013-2014   Johns Hopkins University (author: Daniel Povey)
//           2017        Tsinghua University (author: Zhiyuan Tang)
//
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
#include "util/common-utils.h"
#include "feat/pitch-functions.h"
#include "feat/wave-reader.h"
#include "vvutil/common-utils.h"

/// Score candidate wav with referred wav based on kaldi pitch.
int main(int argc, char *argv[]) {
  try {
    using namespace kaldi;
    const char *usage =
        "Usage like compute-and-process-kaldi-pitch-feats:\n"
        "Apply Kaldi pitch extractor and pitch post-processor, starting from wav input.\n"
        "Equivalent to compute-kaldi-pitch-feats | process-kaldi-pitch-feats, except\n"
        "that it is able to simulate online pitch extraction; see options like\n"
        "--frames-per-chunk, --simulate-first-pass-online, --recompute-frame.\n"
        "\n"
        "Usage: compute-and-process-kaldi-pitch-feats [options...] <wav-rspecifier> <feats-wspecifier>\n"
        "e.g.\n"
        "compute-and-process-kaldi-pitch-feats --simulate-first-pass-online=true \\\n"
        "  --frames-per-chunk=10 --sample-frequency=8000 scp:wav.scp ark:- \n"
        "See also: compute-kaldi-pitch-feats, process-kaldi-pitch-feats\n";

    ParseOptions po(usage);
    PitchExtractionOptions pitch_opts;
    ProcessPitchOptions process_opts;
    // use one of normalized_log_pitch and raw_log_pitch to score the candidate wav
    process_opts.add_pov_feature = false;
    process_opts.add_normalized_log_pitch = true;
    process_opts.add_delta_pitch = false;
    process_opts.add_raw_log_pitch = false;
    if (process_opts.add_normalized_log_pitch && process_opts.add_raw_log_pitch) {
      KALDI_LOG << "Only one of normalized_log_pitch and raw_log_pitch to be used.";
      return 1;
    }

    int32 channel = -1; // Note: this isn't configurable because it's not a very
                        // good idea to control it this way: better to extract the
                        // on the command line (in the .scp file) using sox or
                        // similar.

    pitch_opts.Register(&po);
    process_opts.Register(&po);

    po.Read(argc, argv);

    if (po.NumArgs() != 1) {
      po.PrintUsage();
      exit(1);
    }

    std::string wav_rspecifier = po.GetArg(1);

    SequentialTableReader<WaveHolder> wav_reader(wav_rspecifier);

    int32 num_done = 0, num_err = 0;
    std::vector<double> pitch_ref;  // pitch feat for referrence
    std::vector<double> pitch;
    
    // only process 2 wavs (referred and candidate)
    for (int32 i = 0; !wav_reader.Done() && i <= 1; wav_reader.Next(), i++) {
      std::string utt = wav_reader.Key();
      const WaveData &wave_data = wav_reader.Value();

      int32 num_chan = wave_data.Data().NumRows(), this_chan = channel;
      {
        KALDI_ASSERT(num_chan > 0);
        // reading code if no channels.
        if (channel == -1) {
          this_chan = 0;
          if (num_chan != 1)
            KALDI_WARN << "Channel not specified but you have data with "
                       << num_chan  << " channels; defaulting to zero";
        } else {
          if (this_chan >= num_chan) {
            KALDI_WARN << "File with id " << utt << " has "
                       << num_chan << " channels but you specified channel "
                       << channel << ", producing no output.";
            continue;
          }
        }
      }

      if (pitch_opts.samp_freq != wave_data.SampFreq())
        KALDI_ERR << "Sample frequency mismatch: you specified "
                  << pitch_opts.samp_freq << " but data has "
                  << wave_data.SampFreq() << " (use --sample-frequency option)";


      SubVector<BaseFloat> waveform(wave_data.Data(), this_chan);
      Matrix<BaseFloat> features;
      try {
        ComputeAndProcessKaldiPitch(pitch_opts, process_opts,
                                    waveform, &features);
      } catch (...) {
        KALDI_WARN << "Failed to compute pitch for utterance "
                   << utt;
        num_err++;
        continue;
      }

      Vector<BaseFloat> pit_col(features.NumRows());
      pit_col.CopyColFromMat(features, 0);
      float* p = pit_col.Data();
      if (i == 0)
        for(int j = 0; j < pit_col.Dim(); j++)
          pitch_ref.push_back(p[j]);
      else
        for(int k = 0; k < pit_col.Dim(); k++)
          pitch.push_back(p[k]);

      num_done++;
    }

    if (num_done < 2) {
        KALDI_LOG << num_done << " wav(s) processed, not enough.";
        return 1;
    }

    // correlation coefficient
    double corr = CorrelationOfTwoVectors(pitch_ref, pitch);  // between -1 and 1
    double score_c = (corr - (-1)) / 2 * 100;  // between 0 and 100
    KALDI_LOG << "The score based on correlation coefficient of "
              << "pitch feats is "
              << score_c;

    // similarity
    double similar = SimilarityOfTwoVectors(pitch_ref, pitch);  // between 0 and 1
    double score_s = similar * 100;  // between 0 and 100
    KALDI_LOG << "The score based on similarity of "
              << "pitch feats is "
              << score_s;
    
    return 0;
  } catch(const std::exception &e) {
    std::cerr << e.what();
    return -1;
  }
}


