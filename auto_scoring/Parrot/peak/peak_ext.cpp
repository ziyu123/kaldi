#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "feat/feature-mfcc.h"
#include "feat/wave-reader.h"

using namespace kaldi;
using namespace std ;

int get_mfcc(char *file ,char *file1, Matrix<BaseFloat> &features);


//提取共振峰代码 
//file: wav
//file1: output gongzhenfeng file
int get_mfcc(char *file ,char *file1, Matrix<BaseFloat> &features_r)
{
	typedef kaldi::int32 int32;
	//读取文件
	std::string wav_rspecifier = file;
	Matrix<BaseFloat> features;
	WaveData wave_data ;
	bool binary_in;
	BaseFloat min_duration = 0.1;
	Input ki(wav_rspecifier,&binary_in);
	wave_data.Read(ki.Stream());
	MfccOptions mfcc_opts;
	//bool subtract_mean = false;
	BaseFloat vtln_warp = 1.0;
	//int32 channel = -1;
	//BaseFloat min_duration = 0.0;

	if(wave_data.Duration()<min_duration)
	{
		fprintf(stderr,"wav is too short!\n");
		return -1;
	}
	mfcc_opts.frame_opts.samp_freq = 8000;
	mfcc_opts.frame_opts.frame_length_ms=25;
	mfcc_opts.mel_opts.low_freq = 20 ;
	mfcc_opts.mel_opts.high_freq = 3700 ;
	mfcc_opts.mel_opts.num_bins = 30 ;
	mfcc_opts.num_ceps = 20;
	Mfcc mfcc(mfcc_opts);
	
	BaseFloat vtln_warp_local;
	int32 this_chan = 0;
	vtln_warp_local = vtln_warp;
	//Matrix<BaseFloat> features;
	if (mfcc_opts.frame_opts.samp_freq != wave_data.SampFreq())	
	{
	//	fptintf(stderr,"Sample frequency mismatch!\n");
		return -1;
	}
	SubVector<BaseFloat> waveform(wave_data.Data(), this_chan);
	try 
	{
		mfcc.ComputePeak(waveform, vtln_warp_local, &features, file1, NULL);
		// TO DO
		// compute peak/formant
	} catch (...) 
	{
	//	fprintf(stderr, "Failed to compute features for utterance\n");
		return -1;
	}
	/*
	if (subtract_mean) 
	{
		Vector<BaseFloat> mean(features.NumCols());
		mean.AddRowSumMat(1.0, features);
		mean.Scale(1.0 / features.NumRows());
		for (int32 i = 0; i < features.NumRows(); i++)
			features.Row(i).AddVec(-1.0, mean);
	}

	//apply-cmvn-sliding
//	feature->features->Resize(features.NumRows(),feature->col);

	Matrix<BaseFloat> cmvn_feat(features.NumRows(),
			features.NumCols(), kUndefined);
	SlidingWindowCmnOptions opts;
	opts.center = true ;
	opts.normalize_variance = false;
	opts.cmn_window = 300 ;

	 SlidingWindowCmn(opts,features, &cmvn_feat);

	 //apply_cmvn_sliding end

	 //add-delta
	 DeltaFeaturesOptions delta_opts;
	 ComputeDeltas(delta_opts,cmvn_feat,&features_r);
	*/
	return 0;
}

int main(int argc,char *argv[])
{
	typedef kaldi::int32 int32;
	typedef kaldi::int64 int64;
	
	Matrix<BaseFloat> features;
	if(argc != 3)
	{
		printf("./peak_ext wav_name outputpeak_name\n");
		return -1;
	}
	get_mfcc(argv[1],argv[2],features);//wav

	return 0;
}
