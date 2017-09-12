// vvscore/simple-score.cc

// Copyright  2017  Tsinghua University (Zhiyuan Tang)

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


namespace kaldi {


double ScoreOnAverageLogLikelihood(std::string wav_rspecifier,
                                   std::string tree_rxfilename,
                                   std::string model_rxfilename,
                                   std::string lex_rxfilename,
                                   std::string transcript_rspecifier,
                                   std::string fbank_option_file /* "" */,
                                   std::string graph_option_file /* "" */,
                                   std::string likelihood_option_file /* "" */,
                                   double avg_like_ref /* 5.6 */) {
  SequentialTableReader<WaveHolder> reader(wav_rspecifier);
  RandomAccessInt32VectorReader transcript_reader(transcript_rspecifier);

  double avg_like = 0.0;  // for candidate wavv
  // at most 2 wavs (optional referred wav and the candidate one)
  // and follow the order of wav_rspecifier
  for (int32 i = 0; !reader.Done() && i <=1; reader.Next(), i++) {
    std::string utt = reader.Key();
    const WaveData &wave_data = reader.Value();

    std::vector<int32> transcript = transcript_reader.Value(utt);

    // get fbank feature
    Matrix<BaseFloat> features;
    FbankOfOneWav(wave_data, &features, fbank_option_file);

    // get training graph
    VectorFst <StdArc> decode_fst;
    TrainingGraphOfOneUtt(tree_rxfilename, model_rxfilename, lex_rxfilename, 
                          transcript, &decode_fst, graph_option_file);

    // get average log likelihood per frame
    if (i == 0)
      avg_like = AverageLogLikelihoodPerFrame(model_rxfilename, decode_fst, 
                                              features, likelihood_option_file);
    else {  // i == 1, with referred wav
      avg_like_ref = avg_like;
      avg_like = AverageLogLikelihoodPerFrame(model_rxfilename, decode_fst, 
                                            features, likelihood_option_file);
      break;
    }
  }
  
  KALDI_LOG << "avg_like_ref: " << avg_like_ref << "\n";
  KALDI_LOG << "avg_like: " << avg_like << "\n";
  double score = avg_like / avg_like_ref * 100;
  return (score > 0 ? (score < 100 ? score : 100) : 0);
}


double CorrelationScoreOnKaldiPitch(std::string wav_rspecifier,
                                    std::string option_file /* "" */) {
    SequentialTableReader<WaveHolder> reader(wav_rspecifier);
    // 2 wavs (referred wav and the candidate one)
    // follow the order of wav_rspecifier
    std::vector<double> pitch_ref;
    std::vector<double> pitch;
    int32 num = 0;
    for (int32 i = 0; !reader.Done() && i <=1; reader.Next(), i++) {
      const WaveData &wave_data = reader.Value();
      if(i == 0)  // referred wav
        GetKalidPitchOfOneWav(wave_data, &pitch_ref, option_file);
      else
        GetKalidPitchOfOneWav(wave_data, &pitch, option_file);
      num++;
    }

    if (num != 2)
        KALDI_ERR << "Two wavs need to be provided.\n";

    ConvertTwoVectorsToSameLength(&pitch_ref, &pitch);
    double corr = CorrelationOfTwoVectors(pitch_ref, pitch);  // between -1 and 1
    return (corr - (-1)) / 2 * 100;  // between 0 and 100
}
   
    
double SimilarScoreOnKaldiPitch(std::string wav_rspecifier,
                                std::string option_file /* "" */) {
    SequentialTableReader<WaveHolder> reader(wav_rspecifier);
    // 2 wavs (referred wav and the candidate one)
    // follow the order of wav_rspecifier
    std::vector<double> pitch_ref;
    std::vector<double> pitch;
    int32 num = 0;
    for (int32 i = 0; !reader.Done() && i <=1; reader.Next(), i++) {
      const WaveData &wave_data = reader.Value();
      if(i == 0)  // referred wav
        GetKalidPitchOfOneWav(wave_data, &pitch_ref, option_file);
      else
        GetKalidPitchOfOneWav(wave_data, &pitch, option_file);
      num++;
    }

    if (num != 2)
        KALDI_ERR << "Two wavs need to be provided.\n";

    ConvertTwoVectorsToSameLength(&pitch_ref, &pitch);
    double similar = SimilarityOfTwoVectors(pitch_ref, pitch);  // between 0 and 1
    return similar * 100;  // between 0 and 100
}     


double CorrelationScoreOnPhoneLens(std::string wav_rspecifier,
                                   std::string tree_rxfilename,
                                   std::string model_rxfilename,
                                   std::string lex_rxfilename,
                                   std::string transcript_rspecifier,
                                   std::string fbank_option_file /* "" */,
                                   std::string graph_option_file /* "" */,
                                   std::string align_option_file /* "" */,
                                   std::string phone_option_file /* "" */) {
  SequentialTableReader<WaveHolder> reader(wav_rspecifier);
  RandomAccessInt32VectorReader transcript_reader(transcript_rspecifier);

  // 2 wavs (referred wav and candidate one)
  // follow the order of wav_rspecifier
  std::vector<double> phone_lens_ref;
  std::vector<double> phone_lens;
  for (int32 i = 0; !reader.Done() && i <=1; reader.Next(), i++) {
    std::string utt = reader.Key();
    const WaveData &wave_data = reader.Value();

    std::vector<int32> transcript = transcript_reader.Value(utt);

    // get fbank feature
    Matrix<BaseFloat> features;
    FbankOfOneWav(wave_data, &features, fbank_option_file);

    // get training graph
    VectorFst <StdArc> decode_fst;
    TrainingGraphOfOneUtt(tree_rxfilename, model_rxfilename, lex_rxfilename, 
                          transcript, &decode_fst, graph_option_file);

    // get alignment
    std::vector<int32> alignment;
    AlignFeatWithNnet3(model_rxfilename, decode_fst, features, 
                       &alignment, align_option_file);
    
    // get phone pronunciation lengths
    if (i == 0)
      GetPhonePronunciationLens(model_rxfilename, alignment, 
                                &phone_lens_ref, phone_option_file);
    else
      GetPhonePronunciationLens(model_rxfilename, alignment, 
                                &phone_lens, phone_option_file);
  }
  
  ConvertTwoVectorsToSameLength(&phone_lens_ref, &phone_lens);  // todo, same text, not same len
  KALDI_ASSERT(phone_lens_ref.size() == phone_lens.size());
  double corr = CorrelationOfTwoVectors(phone_lens_ref, phone_lens);  // between -1 and 1
    return (corr - (-1)) / 2 * 100;  // between 0 and 100

}



double SimilarScoreOnPhoneLens(std::string wav_rspecifier,
                                   std::string tree_rxfilename,
                                   std::string model_rxfilename,
                                   std::string lex_rxfilename,
                                   std::string transcript_rspecifier,
                                   std::string fbank_option_file /* "" */,
                                   std::string graph_option_file /* "" */,
                                   std::string align_option_file /* "" */,
                                   std::string phone_option_file /* "" */) {
  SequentialTableReader<WaveHolder> reader(wav_rspecifier);
  RandomAccessInt32VectorReader transcript_reader(transcript_rspecifier);

  // 2 wavs (referred wav and candidate one)
  // follow the order of wav_rspecifier
  std::vector<double> phone_lens_ref;
  std::vector<double> phone_lens;
  for (int32 i = 0; !reader.Done() && i <=1; reader.Next(), i++) {
    std::string utt = reader.Key();
    const WaveData &wave_data = reader.Value();

    std::vector<int32> transcript = transcript_reader.Value(utt);

    // get fbank feature
    Matrix<BaseFloat> features;
    FbankOfOneWav(wave_data, &features, fbank_option_file);

    // get training graph
    VectorFst <StdArc> decode_fst;
    TrainingGraphOfOneUtt(tree_rxfilename, model_rxfilename, lex_rxfilename, 
                          transcript, &decode_fst, graph_option_file);

    // get alignment
    std::vector<int32> alignment;
    AlignFeatWithNnet3(model_rxfilename, decode_fst, features, 
                       &alignment, align_option_file);
    
    // get phone pronunciation lengths
    if (i == 0)
      GetPhonePronunciationLens(model_rxfilename, alignment, 
                                &phone_lens_ref, phone_option_file);
    else
      GetPhonePronunciationLens(model_rxfilename, alignment, 
                                &phone_lens, phone_option_file);
  }
  
  ConvertTwoVectorsToSameLength(&phone_lens_ref, &phone_lens);  // todo, same text, not same len
  KALDI_ASSERT(phone_lens_ref.size() == phone_lens.size());
  double similar = SimilarityOfTwoVectors(phone_lens_ref, phone_lens);  // between 0 and 1
  return similar * 100;  // between 0 and 100

}   



}   // namespace kaldi
