// vvutil/kaldi-utils.cc

// Copyright 2009-2012  Microsoft Corporation
//                      Johns Hopkins University (author: Daniel Povey)
//                      Tsinghua University (Zhiyuan Tang)

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

#include "vvutil/kaldi-utils.h"


namespace kaldi {


/// Check file path is good.
bool GoodFile(std::string file) {
  std::ifstream is(file.c_str(), std::ifstream::in);
  return is.good();
}


// refer to featbin/compute-fbank-feats.cc
void FbankOfOneWav(const WaveData &wave_data,
                   Matrix<BaseFloat> *features,
                   std::string option_file /*""*/) {
  const char *usage = "Get Mel-filter bank (FBANK) feature for one wav.\n";
  
  // construct all the global objects
  ParseOptions po(usage);
  FbankOptions fbank_opts;
  bool subtract_mean = false;
  BaseFloat vtln_warp = 1.0;
  std::string vtln_map_rspecifier;
  std::string utt2spk_rspecifier;
  int32 channel = -1;
  BaseFloat min_duration = 0.0;
  // Define defaults for gobal options
  std::string output_format = "kaldi";
  
  // Register the option struct
  fbank_opts.Register(&po);
  // Register the options
  po.Register("output-format", &output_format, "Format of the output files [kaldi, htk]");
  po.Register("subtract-mean", &subtract_mean, "Subtract mean of each feature file [CMS]; not recommended to do it this way. ");
  po.Register("vtln-warp", &vtln_warp, "Vtln warp factor (only applicable if vtln-map not specified)");
  po.Register("vtln-map", &vtln_map_rspecifier, "Map from utterance or speaker-id to vtln warp factor (rspecifier)");
  po.Register("utt2spk", &utt2spk_rspecifier, "Utterance to speaker-id map (if doing VTLN and you have warps per speaker)");
  po.Register("channel", &channel, "Channel to extract (-1 -> expect mono, 0 -> left, 1 -> right)");
  po.Register("min-duration", &min_duration, "Minimum duration of segments to process (in seconds).");
  
  // OPTION PARSING ..........................................................
  //
  
  // parse options (+filling the registered variables)
  po.PrintUsage();
  if (GoodFile(option_file))
    po.ReadConfigFile(option_file);
  
  
  Fbank fbank(fbank_opts);
  
  if (utt2spk_rspecifier != "")
    KALDI_ASSERT(vtln_map_rspecifier != "" && "the utt2spk option is only "
                 "needed if the vtln-map option is used.");
  RandomAccessBaseFloatReaderMapped vtln_map_reader(vtln_map_rspecifier,
                                                    utt2spk_rspecifier);
  
  if (wave_data.Duration() < min_duration) {
    KALDI_WARN << "File is too short ("
               << wave_data.Duration() << " sec): producing no output.";
    exit(1);
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
        KALDI_WARN << "File has "
                   << num_chan << " channels but you specified channel "
                   << channel << ", producing no output.";
        exit(1);
      }
    }
  }
  
  SubVector<BaseFloat> waveform(wave_data.Data(), this_chan);
  try {
    fbank.ComputeFeatures(waveform, wave_data.SampFreq(), vtln_warp, features);
  } catch (...) {
    KALDI_WARN << "Failed to compute features.";
    exit(1);
  }
  if (subtract_mean) {
    Vector<BaseFloat> mean(features->NumCols());
    mean.AddRowSumMat(1.0, *features);
    mean.Scale(1.0 / features->NumRows());
    for (int32 i = 0; i < features->NumRows(); i++)
      features->Row(i).AddVec(-1.0, mean);
  }
}


// refer to bin/compile-train-graphs.cc
void TrainingGraphOfOneUtt(std::string tree_rxfilename,
                           std::string model_rxfilename,
                           std::string lex_rxfilename,
                           std::vector<int32> transcript,
                           VectorFst<StdArc> *decode_fst,
                           std::string option_file /* "" */) {
  typedef kaldi::int32 int32;

  const char *usage = "Get training graph for one utterance as "
                      "kaldi::Creates training graphs.\n";
  ParseOptions po(usage);

  TrainingGraphCompilerOptions gopts;
  int32 batch_size = 1;
  gopts.transition_scale = 0.0;  // Change the default to 0.0 since we will generally add the
  // transition probs in the alignment phase (since they change eacm time)
  gopts.self_loop_scale = 0.0;  // Ditto for self-loop probs.
  std::string disambig_rxfilename;
  gopts.Register(&po);

  po.Register("batch-size", &batch_size,
              "Number of FSTs to compile at a time (more -> faster but uses "
              "more memory.  E.g. 500");
  po.Register("read-disambig-syms", &disambig_rxfilename, "File containing "
              "list of disambiguation symbols in phone symbol table");
  
  po.PrintUsage();
  if (GoodFile(option_file))
    po.ReadConfigFile(option_file);


  ContextDependency ctx_dep;  // the tree.
  ReadKaldiObject(tree_rxfilename, &ctx_dep);

  TransitionModel trans_model;
  ReadKaldiObject(model_rxfilename, &trans_model);

  // need VectorFst because we will change it by adding subseq symbol.
  VectorFst<StdArc> *lex_fst = fst::ReadFstKaldi(lex_rxfilename);

  std::vector<int32> disambig_syms;
  if (disambig_rxfilename != "")
    if (!ReadIntegerVectorSimple(disambig_rxfilename, &disambig_syms))
      KALDI_ERR << "fstcomposecontext: Could not read disambiguation symbols from "
                << disambig_rxfilename;
  
  TrainingGraphCompiler gc(trans_model, ctx_dep, lex_fst, disambig_syms, gopts);

  lex_fst = NULL;  // we gave ownership to gc.

  // only one utterance
  if (!gc.CompileGraphFromText(transcript, decode_fst))
    decode_fst->DeleteStates();  // Just make it empty.
  if (decode_fst->Start() == fst::kNoStateId)
    KALDI_WARN << "Empty decoding graph for the utterance.";
}
                          

// refer to nnet3bin/nnet3-align-compiled.cc
double AverageLogLikelihoodPerFrame(std::string model_in_filename,
                                    VectorFst<StdArc> decode_fst,
                                    Matrix<BaseFloat> features,
                                    std::string option_file /* "" */) {
  using namespace kaldi::nnet3;
  typedef kaldi::int32 int32;

  const char *usage = "Get average loglikelihood per frame of one wav with referred text.\n";

  ParseOptions po(usage);
  AlignConfig align_config;
  align_config.beam = 10;
  align_config.retry_beam = 40;
  NnetSimpleComputationOptions decodable_opts;
  std::string use_gpu = "no";
  BaseFloat transition_scale = 1.0;
  BaseFloat self_loop_scale = 0.1;
  std::string per_frame_acwt_wspecifier;

  std::string ivector_rspecifier,
      online_ivector_rspecifier,
      utt2spk_rspecifier;
  int32 online_ivector_period = 0;
  align_config.Register(&po);
  decodable_opts.Register(&po);

  po.Register("use-gpu", &use_gpu,
              "yes|no|optional|wait, only has effect if compiled with CUDA");
  po.Register("transition-scale", &transition_scale,
              "Transition-probability scale [relative to acoustics]");
  po.Register("self-loop-scale", &self_loop_scale,
              "Scale of self-loop versus non-self-loop "
              "log probs [relative to acoustics]");
  po.Register("write-per-frame-acoustic-loglikes", &per_frame_acwt_wspecifier,
              "Wspecifier for table of vectors containing the acoustic log-likelihoods "
              "per frame for each utterance. E.g. ark:foo/per_frame_logprobs.1.ark");
  po.Register("ivectors", &ivector_rspecifier, "Rspecifier for "
              "iVectors as vectors (i.e. not estimated online); per utterance "
              "by default, or per speaker if you provide the --utt2spk option.");
  po.Register("online-ivectors", &online_ivector_rspecifier, "Rspecifier for "
              "iVectors estimated online, as matrices.  If you supply this,"
              " you must set the --online-ivector-period option.");
  po.Register("online-ivector-period", &online_ivector_period, "Number of frames "
              "between iVectors in matrices supplied to the --online-ivectors "
              "option");
  
  po.PrintUsage();
  if (GoodFile(option_file))
    po.ReadConfigFile(option_file);

#if HAVE_CUDA==1
  CuDevice::Instantiate().SelectGpuId(use_gpu);
#endif

  double tot_like = 0.0;
  kaldi::int64 frame_count = 0;


  {
    TransitionModel trans_model;
    AmNnetSimple am_nnet;
    {
      bool binary;
      Input ki(model_in_filename, &binary);
      trans_model.Read(ki.Stream(), binary);
      am_nnet.Read(ki.Stream(), binary);
    }
    SetBatchnormTestMode(true, &(am_nnet.GetNnet()));
    SetDropoutTestMode(true, &(am_nnet.GetNnet()));
    CollapseModel(CollapseModelConfig(), &(am_nnet.GetNnet()));
    // this compiler object allows caching of computations across
    // different utterances.
    CachingOptimizingCompiler compiler(am_nnet.GetNnet(),
                                       decodable_opts.optimize_config);

    RandomAccessBaseFloatMatrixReader online_ivector_reader(
        online_ivector_rspecifier);
    RandomAccessBaseFloatVectorReaderMapped ivector_reader(
        ivector_rspecifier, utt2spk_rspecifier);

    BaseFloatVectorWriter per_frame_acwt_writer(per_frame_acwt_wspecifier);

    // only one utterance
    if (features.NumRows() == 0) {
      KALDI_WARN << "Zero-length utterance.";
      exit(1);
    }
    
    const Matrix<BaseFloat> *online_ivectors = NULL;
    const Vector<BaseFloat> *ivector = NULL;
    
    {  // Add transition-probs to the FST.
      std::vector<int32> disambig_syms;  // empty.
      AddTransitionProbs(trans_model, disambig_syms,
                         transition_scale, self_loop_scale,
                         &decode_fst);
    }
    
    DecodableAmNnetSimple nnet_decodable(
        decodable_opts, trans_model, am_nnet,
        features, ivector, online_ivectors,
        online_ivector_period, &compiler);
    
    AlignUtteranceWrapper(align_config, "",
                          decodable_opts.acoustic_scale,
                          &decode_fst, &nnet_decodable,
                          NULL, NULL, NULL, NULL, NULL,
                          &tot_like, &frame_count, &per_frame_acwt_writer);

#if HAVE_CUDA==1
    CuDevice::Instantiate().PrintProfile();
#endif

    return tot_like/frame_count;
  }
}


// refer to AlignUtteranceWrapper() in src/decoder/decoder-wrappers.cc
// not write the alignment, but pass it in and out
void AlignUtteranceWrapperOnline(
    const AlignConfig &config,
    const std::string &utt,
    BaseFloat acoustic_scale,  // affects scores written to scores_writer, if
                               // present
    fst::VectorFst<fst::StdArc> *fst,  // non-const in case config.careful ==
                                       // true.
    DecodableInterface *decodable,  // not const but is really an input.
    std::vector<int32> *alignment,
    BaseFloatWriter *scores_writer,
    int32 *num_done,
    int32 *num_error,
    int32 *num_retried,
    double *tot_like,
    int64 *frame_count,
    BaseFloatVectorWriter *per_frame_acwt_writer) {
    
  if ((config.retry_beam != 0 && config.retry_beam <= config.beam) ||
      config.beam <= 0.0) {
    KALDI_ERR << "Beams do not make sense: beam " << config.beam
              << ", retry-beam " << config.retry_beam;
  }

  if (fst->Start() == fst::kNoStateId) {
    KALDI_WARN << "Empty decoding graph for " << utt;
    if (num_error != NULL) (*num_error)++;
    return;
  }


  fst::StdArc::Label special_symbol = 0;
  if (config.careful)
    ModifyGraphForCarefulAlignment(fst);

  FasterDecoderOptions decode_opts;
  decode_opts.beam = config.beam;

  FasterDecoder decoder(*fst, decode_opts);
  decoder.Decode(decodable);

  bool ans = decoder.ReachedFinal();  // consider only final states.

  if (!ans && config.retry_beam != 0.0) {
    if (num_retried != NULL) (*num_retried)++;
    KALDI_WARN << "Retrying utterance " << utt << " with beam "
               << config.retry_beam;
    decode_opts.beam = config.retry_beam;
    decoder.SetOptions(decode_opts);
    decoder.Decode(decodable);
    ans = decoder.ReachedFinal();
  }

  if (!ans) {  // Still did not reach final state.
    KALDI_WARN << "Did not successfully decode file " << utt << ", len = "
               << decodable->NumFramesReady();
    if (num_error != NULL) (*num_error)++;
    return;
  }

  fst::VectorFst<LatticeArc> decoded;  // linear FST.
  decoder.GetBestPath(&decoded);
  if (decoded.NumStates() == 0) {
    KALDI_WARN << "Error getting best path from decoder (likely a bug)";
    if (num_error != NULL) (*num_error)++;
    return;
  }

  std::vector<int32> words;
  LatticeWeight weight;

  GetLinearSymbolSequence(decoded, alignment, &words, &weight);
  BaseFloat like = -(weight.Value1()+weight.Value2()) / acoustic_scale;

  if (num_done != NULL) (*num_done)++;
  if (tot_like != NULL) (*tot_like) += like;
  if (frame_count != NULL) (*frame_count) += decodable->NumFramesReady();

  if (scores_writer != NULL && scores_writer->IsOpen())
    scores_writer->Write(utt, -(weight.Value1()+weight.Value2()));

  Vector<BaseFloat> per_frame_loglikes;
  if (per_frame_acwt_writer != NULL && per_frame_acwt_writer->IsOpen()) {
    GetPerFrameAcousticCosts(decoded, &per_frame_loglikes);
    per_frame_loglikes.Scale(-1 / acoustic_scale);
    per_frame_acwt_writer->Write(utt, per_frame_loglikes);
  }
}


// refer to nnet3bin/nnet3-align-compiled.cc
void AlignFeatWithNnet3(std::string model_in_filename,
                        VectorFst<StdArc> decode_fst,
                        Matrix<BaseFloat> features,
                        std::vector<int32> *alignment,
                        std::string option_file /* "" */) {
  using namespace kaldi::nnet3;
  typedef kaldi::int32 int32;

  const char *usage = "Align feature using nnet3 with referred text.\n";

  ParseOptions po(usage);
  AlignConfig align_config;
  align_config.beam = 10;
  align_config.retry_beam = 40;
  NnetSimpleComputationOptions decodable_opts;
  std::string use_gpu = "no";
  BaseFloat transition_scale = 1.0;
  BaseFloat self_loop_scale = 0.1;
  std::string per_frame_acwt_wspecifier;

  std::string ivector_rspecifier,
      online_ivector_rspecifier,
      utt2spk_rspecifier;
  int32 online_ivector_period = 0;
  align_config.Register(&po);
  decodable_opts.Register(&po);

  po.Register("use-gpu", &use_gpu,
              "yes|no|optional|wait, only has effect if compiled with CUDA");
  po.Register("transition-scale", &transition_scale,
              "Transition-probability scale [relative to acoustics]");
  po.Register("self-loop-scale", &self_loop_scale,
              "Scale of self-loop versus non-self-loop "
              "log probs [relative to acoustics]");
  po.Register("write-per-frame-acoustic-loglikes", &per_frame_acwt_wspecifier,
              "Wspecifier for table of vectors containing the acoustic log-likelihoods "
              "per frame for each utterance. E.g. ark:foo/per_frame_logprobs.1.ark");
  po.Register("ivectors", &ivector_rspecifier, "Rspecifier for "
              "iVectors as vectors (i.e. not estimated online); per utterance "
              "by default, or per speaker if you provide the --utt2spk option.");
  po.Register("online-ivectors", &online_ivector_rspecifier, "Rspecifier for "
              "iVectors estimated online, as matrices.  If you supply this,"
              " you must set the --online-ivector-period option.");
  po.Register("online-ivector-period", &online_ivector_period, "Number of frames "
              "between iVectors in matrices supplied to the --online-ivectors "
              "option");
  
  po.PrintUsage();
  if (GoodFile(option_file))
    po.ReadConfigFile(option_file);

#if HAVE_CUDA==1
  CuDevice::Instantiate().SelectGpuId(use_gpu);
#endif


  {
    TransitionModel trans_model;
    AmNnetSimple am_nnet;
    {
      bool binary;
      Input ki(model_in_filename, &binary);
      trans_model.Read(ki.Stream(), binary);
      am_nnet.Read(ki.Stream(), binary);
    }
    SetBatchnormTestMode(true, &(am_nnet.GetNnet()));
    SetDropoutTestMode(true, &(am_nnet.GetNnet()));
    CollapseModel(CollapseModelConfig(), &(am_nnet.GetNnet()));
    // this compiler object allows caching of computations across
    // different utterances.
    CachingOptimizingCompiler compiler(am_nnet.GetNnet(),
                                       decodable_opts.optimize_config);

    RandomAccessBaseFloatMatrixReader online_ivector_reader(
        online_ivector_rspecifier);
    RandomAccessBaseFloatVectorReaderMapped ivector_reader(
        ivector_rspecifier, utt2spk_rspecifier);

    BaseFloatVectorWriter per_frame_acwt_writer(per_frame_acwt_wspecifier);

    // only one utterance
    if (features.NumRows() == 0) {
      KALDI_WARN << "Zero-length utterance.";
      exit(1);
    }
    
    const Matrix<BaseFloat> *online_ivectors = NULL;
    const Vector<BaseFloat> *ivector = NULL;
    
    {  // Add transition-probs to the FST.
      std::vector<int32> disambig_syms;  // empty.
      AddTransitionProbs(trans_model, disambig_syms,
                         transition_scale, self_loop_scale,
                         &decode_fst);
    }
    
    DecodableAmNnetSimple nnet_decodable(
        decodable_opts, trans_model, am_nnet,
        features, ivector, online_ivectors,
        online_ivector_period, &compiler);
    
    AlignUtteranceWrapperOnline(align_config, "",
                          decodable_opts.acoustic_scale,
                          &decode_fst, &nnet_decodable,
                          alignment, NULL, NULL, NULL, NULL,
                          NULL, NULL, &per_frame_acwt_writer);

#if HAVE_CUDA==1
    CuDevice::Instantiate().PrintProfile();
#endif
  }
}


// refer to bin/ali-to-phone.cc
void GetPhonePronunciationLens(std::string model_filename,
                               std::vector<int32> alignment,
                               std::vector<double> *phone_lens,
                               std::string option_file /* "" */) {
    const char *usage = "Get phone pronunciation length from alignment.\n";
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
    
    po.PrintUsage();
    if (GoodFile(option_file))
      po.ReadConfigFile(option_file);
                

    KALDI_ASSERT(!(per_frame && write_lengths) && "Incompatible options.");

    TransitionModel trans_model;
    ReadKaldiObject(model_filename, &trans_model);

    std::vector<std::vector<int32> > split;
    SplitToPhones(trans_model, alignment, &split);

    {
      for (size_t i = 0; i < split.size(); i++) {
        KALDI_ASSERT(split[i].size() > 0);
        int32 num_repeats = split[i].size();
        //KALDI_ASSERT(num_repeats!=0);
        phone_lens->push_back(num_repeats);
      }
    }
}                                    


// refer to featbin/compute-and-process-kaldi-pitch-feats.cc
void GetKalidPitchOfOneWav(const WaveData &wave_data,
                           std::vector<double> *pitch,
                           std::string option_file /* "" */) {
    const char *usage = "Get raw log kaldi pitch (Hz) or normalized one (default) of one wav.\n";

    ParseOptions po(usage);
    PitchExtractionOptions pitch_opts;
    ProcessPitchOptions process_opts;
    // use one of normalized_log_pitch and raw_log_pitch to score the candidate wav
    process_opts.add_pov_feature = false;
    process_opts.add_normalized_log_pitch = true;
    process_opts.add_delta_pitch = false;
    process_opts.add_raw_log_pitch = false;
    if (process_opts.add_normalized_log_pitch && process_opts.add_raw_log_pitch)
      KALDI_ERR << "Only one of normalized_log_pitch and raw_log_pitch to be used.";

    int32 channel = -1; // Note: this isn't configurable because it's not a very
                        // good idea to control it this way: better to extract the
                        // on the command line (in the .scp file) using sox or
                        // similar.

    pitch_opts.Register(&po);
    process_opts.Register(&po);

    po.PrintUsage();
    if (GoodFile(option_file))
      po.ReadConfigFile(option_file);

    
    // only process one wav
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
          KALDI_WARN << "File with has "
                     << num_chan << " channels but you specified channel "
                     << channel << ", producing no output.";
          exit(1);
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
      KALDI_ERR << "Failed to compute pitch for the utterance.";
    }

    Vector<BaseFloat> pit_col(features.NumRows());
    pit_col.CopyColFromMat(features, 0);
    float* p = pit_col.Data();
    for(int i = 0; i < pit_col.Dim(); i++)
      pitch->push_back(p[i]);

}

                                    


}   // namespace kaldi
