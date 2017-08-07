// featbin/compute-peak-from-mfcc.cc

// Copyright 2009-2012  Microsoft Corporation
//                      Johns Hopkins University (author: Daniel Povey)

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
#include "feat/feature-mfcc.h"
#include "feat/wave-reader.h"

int main(int argc, char *argv[]) {
  try {
    using namespace kaldi;
    const char *usage =
        "Create peak positions from MFCC feature.\n"
        "Usage:  compute-peak-from-mfcc [options...] <wav-path> <output-path>\n";

    // construct all the global objects
    ParseOptions po(usage);
    MfccOptions mfcc_opts;
    bool subtract_mean = false;
    BaseFloat vtln_warp = 1.0;
    std::string vtln_map_rspecifier;
    std::string utt2spk_rspecifier;
    int32 channel = -1;
    BaseFloat min_duration = 0.0;
    // Define defaults for gobal options
    std::string output_format = "kaldi";

    // Register the MFCC option struct
    mfcc_opts.Register(&po);

    // Register the options
    po.Register("output-format", &output_format, "Format of the output "
                "files [kaldi, htk]");
    po.Register("subtract-mean", &subtract_mean, "Subtract mean of each "
                "feature file [CMS]; not recommended to do it this way. ");
    po.Register("vtln-warp", &vtln_warp, "Vtln warp factor (only applicable "
                "if vtln-map not specified)");
    po.Register("vtln-map", &vtln_map_rspecifier, "Map from utterance or "
                "speaker-id to vtln warp factor (rspecifier)");
    po.Register("utt2spk", &utt2spk_rspecifier, "Utterance to speaker-id map "
                "rspecifier (if doing VTLN and you have warps per speaker)");
    po.Register("channel", &channel, "Channel to extract (-1 -> expect mono, "
                "0 -> left, 1 -> right)");
    po.Register("min-duration", &min_duration, "Minimum duration of segments "
                "to process (in seconds).");

    po.Read(argc, argv);

    if (po.NumArgs() != 2) {
      po.PrintUsage();
      exit(1);
    }

    std::string wav_path = po.GetArg(1);

    std::string out_path = po.GetArg(2);

    Mfcc mfcc(mfcc_opts);

    if (utt2spk_rspecifier != "")
      KALDI_ASSERT(vtln_map_rspecifier != "" && "the utt2spk option is only "
                   "needed if the vtln-map option is used.");
    RandomAccessBaseFloatReaderMapped vtln_map_reader(vtln_map_rspecifier,
                                                      utt2spk_rspecifier);

      Input ki(wav_path);
      WaveData wave_data ;
      wave_data.Read(ki.Stream());
      if (wave_data.Duration() < min_duration) {
        KALDI_WARN << "File is too short ("
                   << wave_data.Duration() << " sec): producing no output.";
        return 0;
      }
      int32 num_chan = wave_data.Data().NumRows(), this_chan = channel;
      {  // This block works out the channel (0=left, 1=right...)
        KALDI_ASSERT(num_chan > 0);  // should have been caught in
        // reading code if no channels.
        if (channel == -1) {
          this_chan = 0;
          if (num_chan != 1)
            KALDI_WARN << "Channel not specified but you have data with "
                       << num_chan  << " channels; defaulting to zero";
        } else {
          if (this_chan >= num_chan) {
            KALDI_WARN << "this_utt has "
                       << num_chan << " channels but you specified channel "
                       << channel << ", producing no output.";
            continue;
          }
        }
      }
      BaseFloat vtln_warp_local;  // Work out VTLN warp factor.
      if (vtln_map_rspecifier != "") {
        if (!vtln_map_reader.HasKey("this_utt")) {
          KALDI_WARN << "No vtln-map entry for utterance-id (or speaker-id) this_utt";
          continue;
        }
        vtln_warp_local = vtln_map_reader.Value("this_utt");
      } else {
        vtln_warp_local = vtln_warp;
      }
      if (mfcc_opts.frame_opts.samp_freq != wave_data.SampFreq())
        KALDI_ERR << "Sample frequency mismatch: you specified "
                  << mfcc_opts.frame_opts.samp_freq << " but data has "
                  << wave_data.SampFreq() << " (use --sample-frequency "
                  << "option).";

      SubVector<BaseFloat> waveform(wave_data.Data(), this_chan);
      Matrix<BaseFloat> features;
      char *peak_out = out_path.data();
      try {
        mfcc.ComputePeak(waveform, vtln_warp_local, &features, peak_out, NULL);
      } catch (...) {
        KALDI_WARN << "Failed to compute features for this utterance.";
        return 0;
      }
  

    KALDI_LOG << " Done the computation of extracting peak positions from mfcc.";
  } catch(const std::exception &e) {
    std::cerr << e.what();
    return -1;
  }
}

