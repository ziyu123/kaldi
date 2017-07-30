// feat/feature-mfcc.cc

// Copyright 2009-2011  Karel Vesely;  Petr Motlicek
//                2016  Johns Hopkins University (author: Daniel Povey)

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


#include "feat/feature-mfcc.h"


namespace kaldi {


void MfccComputer::Compute(BaseFloat signal_log_energy,
                           BaseFloat vtln_warp,
                           VectorBase<BaseFloat> *signal_frame,
                           VectorBase<BaseFloat> *feature) {
  KALDI_ASSERT(signal_frame->Dim() == opts_.frame_opts.PaddedWindowSize() &&
               feature->Dim() == this->Dim());

  const MelBanks &mel_banks = *(GetMelBanks(vtln_warp));

  if (opts_.use_energy && !opts_.raw_energy)
    signal_log_energy = Log(std::max(VecVec(*signal_frame, *signal_frame),
                                     std::numeric_limits<BaseFloat>::min()));

  if (srfft_ != NULL)  // Compute FFT using the split-radix algorithm.
    srfft_->Compute(signal_frame->Data(), true);
  else  // An alternative algorithm that works for non-powers-of-two.
    RealFft(signal_frame, true);

  // Convert the FFT into a power spectrum.
  ComputePowerSpectrum(signal_frame);
  SubVector<BaseFloat> power_spectrum(*signal_frame, 0,
                                      signal_frame->Dim() / 2 + 1);

  mel_banks.Compute(power_spectrum, &mel_energies_);

  // avoid log of zero (which should be prevented anyway by dithering).
  mel_energies_.ApplyFloor(std::numeric_limits<BaseFloat>::epsilon());
  mel_energies_.ApplyLog();  // take the log.

  feature->SetZero();  // in case there were NaNs.
  // feature = dct_matrix_ * mel_energies [which now have log]
  feature->AddMatVec(1.0, dct_matrix_, kNoTrans, mel_energies_, 0.0);

  if (opts_.cepstral_lifter != 0.0)
    feature->MulElements(lifter_coeffs_);

  if (opts_.use_energy) {
    if (opts_.energy_floor > 0.0 && signal_log_energy < log_energy_floor_)
      signal_log_energy = log_energy_floor_;
    (*feature)(0) = signal_log_energy;
  }

  if (opts_.htk_compat) {
    BaseFloat energy = (*feature)(0);
    for (int32 i = 0; i < opts_.num_ceps - 1; i++)
      (*feature)(i) = (*feature)(i+1);
    if (!opts_.use_energy)
      energy *= M_SQRT2;  // scale on C0 (actually removing a scale
    // we previously added that's part of one common definition of
    // the cosine transform.)
    (*feature)(opts_.num_ceps - 1)  = energy;
  }
}

void MfccComputer::Compute(BaseFloat signal_log_energy,
                           BaseFloat vtln_warp,
                           VectorBase<BaseFloat> *signal_frame,
                           VectorBase<BaseFloat> *feature
                           char *peak_out) {
  KALDI_ASSERT(signal_frame->Dim() == opts_.frame_opts.PaddedWindowSize() &&
               feature->Dim() == this->Dim());

  const MelBanks &mel_banks = *(GetMelBanks(vtln_warp));

  if (opts_.use_energy && !opts_.raw_energy)
    signal_log_energy = Log(std::max(VecVec(*signal_frame, *signal_frame),
                                     std::numeric_limits<BaseFloat>::min()));

  if (srfft_ != NULL)  // Compute FFT using the split-radix algorithm.
    srfft_->Compute(signal_frame->Data(), true);
  else  // An alternative algorithm that works for non-powers-of-two.
    RealFft(signal_frame, true);

  // Convert the FFT into a power spectrum.
  ComputePowerSpectrum(signal_frame);
  SubVector<BaseFloat> power_spectrum(*signal_frame, 0,
                                      signal_frame->Dim() / 2 + 1);

  mel_banks.Compute(power_spectrum, &mel_energies_);


#define GZF
#ifdef GZF
  //处理求取共振峰
      {
          int i=0;
          Vector<BaseFloat> window_temp = *signal_frame;
          float har[window_temp.Dim()/2 + 1];
          float av_5[window_temp.Dim()/2 + 1];
  
          float peak_10[20];
  
          memset(peak_10,0x00,sizeof(peak_10));
          memset(har,0x00,sizeof(har));
          memset(av_5,0x00,sizeof(av_5));
          for(i=0;i<window_temp.Dim()/2 + 1;i++)
          {
              har[i]=power_spectrum.Data()[i];
          }
          int j=0,k=0;
          for(i=0;i+5<window_temp.Dim()/2 + 1;i++)
          {
              for(j=i;j<i+5;j++)
              {
                  av_5[i] += har[j];
              }
              av_5[i] /=5;
              //printf("%f ",log(sqrtf(av_5[i])));
  //          printf("%f ",log(sqrtf(har[i])));
          }
  //      printf("\n");
      /*
#define START_F 0
#define P_N 4
          for(i=START_F;i+P_N*2<window.Dim()/2 ;++i)
          {
              if(av_5[i] > av_5[i-1] && av_5[i] > av_5[i+1])
              {
                  for(j=2;j<P_N;j++)
                  {
                      if((i >= j && av_5[i] < av_5[i-j]) || av_5[i] < av_5[i+j])
                      {
                          break;
                      }
                  }
                  if(j == P_N)
                  {
                      peak_4[k]=av_5[i];
                      peak_4[k+1]=i;
                      k += 2;
                      i += P_N;
                  }
                  if(k>=8)
                      break;
              }
          }
          */
  
          for(i=1;i+1<window_temp.Dim()/2;++i)
          {
              if(har[i]>har[i-1] && har[i]>har[i+1])
              {
                  for(j=2;j<3;++j)
                  {
                      //only need to i local minimum
                      if((i>=j&& har[i]<har[i-j]) ||(i+j<window_temp.Dim()/2 && har[i]<har[i+j]))
                          break;
                  }
                  if(j==3)
                  {
                      peak_10[k]=har[i];
                      peak_10[k+1]=i;
                      k += 2;
                      i += 2;
                  }
              }
              if(k>=20)
                  break;
          }
          for(k=0;k<10;++k)
          {
              peak_10[2*k]=log(sqrtf(peak_10[2*k]));
              peak_10[2*k+1]=(peak_10[2*k+1])*8000/256;
          }
          FILE *fp=fopen(_filename,"a");
          if(fp==NULL)
              return ;
          fprintf(fp,"%-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f\n",
                  peak_10[0],peak_10[1],peak_10[2],peak_10[3],peak_10[4],peak_10[5],peak_10[6],peak_10[7],peak_10[8],peak_10[9],
                  peak_10[10],peak_10[11],peak_10[12],peak_10[13],peak_10[14],peak_10[15],peak_10[16],peak_10[17],peak_10[18],peak_10[19]);
          fclose(fp);
      }
  //求取共振峰结束
#endif



  // avoid log of zero (which should be prevented anyway by dithering).
  mel_energies_.ApplyFloor(std::numeric_limits<BaseFloat>::epsilon());
  mel_energies_.ApplyLog();  // take the log.

  feature->SetZero();  // in case there were NaNs.
  // feature = dct_matrix_ * mel_energies [which now have log]
  feature->AddMatVec(1.0, dct_matrix_, kNoTrans, mel_energies_, 0.0);

  if (opts_.cepstral_lifter != 0.0)
    feature->MulElements(lifter_coeffs_);

  if (opts_.use_energy) {
    if (opts_.energy_floor > 0.0 && signal_log_energy < log_energy_floor_)
      signal_log_energy = log_energy_floor_;
    (*feature)(0) = signal_log_energy;
  }

  if (opts_.htk_compat) {
    BaseFloat energy = (*feature)(0);
    for (int32 i = 0; i < opts_.num_ceps - 1; i++)
      (*feature)(i) = (*feature)(i+1);
    if (!opts_.use_energy)
      energy *= M_SQRT2;  // scale on C0 (actually removing a scale
    // we previously added that's part of one common definition of
    // the cosine transform.)
    (*feature)(opts_.num_ceps - 1)  = energy;
  }
}

MfccComputer::MfccComputer(const MfccOptions &opts):
    opts_(opts), srfft_(NULL),
    mel_energies_(opts.mel_opts.num_bins) {
  int32 num_bins = opts.mel_opts.num_bins;
  Matrix<BaseFloat> dct_matrix(num_bins, num_bins);
  ComputeDctMatrix(&dct_matrix);
  // Note that we include zeroth dct in either case.  If using the
  // energy we replace this with the energy.  This means a different
  // ordering of features than HTK.
  SubMatrix<BaseFloat> dct_rows(dct_matrix, 0, opts.num_ceps, 0, num_bins);
  dct_matrix_.Resize(opts.num_ceps, num_bins);
  dct_matrix_.CopyFromMat(dct_rows);  // subset of rows.
  if (opts.cepstral_lifter != 0.0) {
    lifter_coeffs_.Resize(opts.num_ceps);
    ComputeLifterCoeffs(opts.cepstral_lifter, &lifter_coeffs_);
  }
  if (opts.energy_floor > 0.0)
    log_energy_floor_ = Log(opts.energy_floor);

  int32 padded_window_size = opts.frame_opts.PaddedWindowSize();
  if ((padded_window_size & (padded_window_size-1)) == 0)  // Is a power of two...
    srfft_ = new SplitRadixRealFft<BaseFloat>(padded_window_size);

  // We'll definitely need the filterbanks info for VTLN warping factor 1.0.
  // [note: this call caches it.]
  GetMelBanks(1.0);
}

MfccComputer::MfccComputer(const MfccComputer &other):
    opts_(other.opts_), lifter_coeffs_(other.lifter_coeffs_),
    dct_matrix_(other.dct_matrix_),
    log_energy_floor_(other.log_energy_floor_),
    mel_banks_(other.mel_banks_),
    srfft_(NULL),
    mel_energies_(other.mel_energies_.Dim(), kUndefined) {
  for (std::map<BaseFloat, MelBanks*>::iterator iter = mel_banks_.begin();
       iter != mel_banks_.end(); ++iter)
    iter->second = new MelBanks(*(iter->second));
  if (other.srfft_ != NULL)
    srfft_ = new SplitRadixRealFft<BaseFloat>(*(other.srfft_));
}



MfccComputer::~MfccComputer() {
  for (std::map<BaseFloat, MelBanks*>::iterator iter = mel_banks_.begin();
      iter != mel_banks_.end();
      ++iter)
    delete iter->second;
  delete srfft_;
}

const MelBanks *MfccComputer::GetMelBanks(BaseFloat vtln_warp) {
  MelBanks *this_mel_banks = NULL;
  std::map<BaseFloat, MelBanks*>::iterator iter = mel_banks_.find(vtln_warp);
  if (iter == mel_banks_.end()) {
    this_mel_banks = new MelBanks(opts_.mel_opts,
                                  opts_.frame_opts,
                                  vtln_warp);
    mel_banks_[vtln_warp] = this_mel_banks;
  } else {
    this_mel_banks = iter->second;
  }
  return this_mel_banks;
}



}  // namespace kaldi
