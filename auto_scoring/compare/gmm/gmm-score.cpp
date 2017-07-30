#include "base/kaldi-common.h"
#include "feat/feature-mfcc.h"
#include "feat/wave-reader.h"
#include "feat/feature-functions.h"
#include "matrix/kaldi-matrix.h"

#include "gmm/am-diag-gmm.h"
#include "gmm/diag-gmm.h"
#include "gmm/full-gmm.h"
#include "gmm/mle-full-gmm.h"

#include "gmm/mle-diag-gmm.h"
#include "util/common-utils.h"

//gmm
#include "gmm/model-common.h"
//#include "hmm/transition-model.h"

//#include "vector_dtw.h"
#include <vector>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
//#include "fea_struct.h"
//#include "link_list.h"
/*
 * process data in line
 * */

namespace kaldi {

// We initialize the GMM parameters by setting the variance to the global
// variance of the features, and the means to distinct randomly chosen frames.
void InitGmmFromRandomFrames(const Matrix<BaseFloat> &feats, DiagGmm *gmm) {
  int num_gauss = gmm->NumGauss(), num_frames = feats.NumRows(),
      dim = feats.NumCols();
  if(num_frames <= 10 * num_gauss )
  {
	  //exit(-1);
  }
	  fprintf(stdout,"num_frames=%2d, num_gauss=%d",num_frames,num_gauss);
  Vector<double> mean(dim), var(dim);
  for (int i = 0; i < num_frames; i++) {
    mean.AddVec(1.0 / num_frames, feats.Row(i));
    var.AddVec2(1.0 / num_frames, feats.Row(i));
  }
  var.AddVec2(-1.0, mean);
  if (var.Max() <= 0.0)
    KALDI_ERR << "Features do not have positive variance " << var;
  
  DiagGmmNormal gmm_normal(*gmm);

  std::set<int> used_frames;
  for (int g = 0; g < num_gauss; g++) {
    int random_frame = RandInt(0, num_frames - 1);
    while (used_frames.count(random_frame != 0))
      random_frame = RandInt(0, num_frames - 1);
    used_frames.insert(random_frame);
    gmm_normal.weights_(g) = 1.0 / num_gauss;
    gmm_normal.means_.Row(g).CopyFromVec(feats.Row(random_frame));
    gmm_normal.vars_.Row(g).CopyFromVec(var);
  }
  gmm->CopyFromNormal(gmm_normal);
  gmm->ComputeGconsts();
}

void TrainOneIter(const Matrix<BaseFloat> &feats,
                  const MleDiagGmmOptions &gmm_opts,
                  int iter,
                  int num_threads,
                  DiagGmm *gmm) {
  AccumDiagGmm gmm_acc(*gmm, kGmmAll);

  Vector<BaseFloat> frame_weights(feats.NumRows(), kUndefined);
  frame_weights.Set(1.0);

  double tot_like;
  tot_like = gmm_acc.AccumulateFromDiagMultiThreaded(*gmm, feats, frame_weights,
                                                     num_threads);

  KALDI_LOG << "Likelihood per frame on iteration " << iter
            << " was " << (tot_like / feats.NumRows()) << " over "
            << feats.NumRows() << " frames.";

  BaseFloat objf_change, count;
  MleDiagGmmUpdate(gmm_opts, gmm_acc, kGmmAll, gmm, &objf_change, &count);

  KALDI_LOG << "Objective-function change on iteration " << iter << " was "
            << (objf_change / count) << " over " << count << " frames.";
}

} // namespace kaldi

using namespace std;
using namespace kaldi;

int get_mfcc(char *file , Matrix<BaseFloat> &features_r)
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
	bool subtract_mean = false;
	BaseFloat vtln_warp = 1.0;
	std::string vtln_map_rspecifier;
	std::string utt2spk_rspecifier;
	//int channel = -1;
	//BaseFloat min_duration = 0.0;

	if(wave_data.Duration()<min_duration)
	{
		fprintf(stderr,"wav is too short!\n");
		return -1;
	}
	mfcc_opts.frame_opts.samp_freq = 8000;
	mfcc_opts.frame_opts.frame_length_ms=20;
	mfcc_opts.mel_opts.low_freq = 20 ;
	mfcc_opts.mel_opts.high_freq = 3700 ;
	mfcc_opts.mel_opts.num_bins = 30 ;
	mfcc_opts.num_ceps = 13;
	Mfcc mfcc(mfcc_opts);

	RandomAccessBaseFloatReaderMapped vtln_map_reader(vtln_map_rspecifier,utt2spk_rspecifier);
	
	BaseFloat vtln_warp_local;
	int this_chan = 0;
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
		mfcc.Compute(waveform, vtln_warp_local, &features, NULL);
	} catch (...) 
	{
	//	fprintf(stderr, "Failed to compute features for utterance\n");
		return -1;
	}
	if (subtract_mean) 
	{
		Vector<BaseFloat> mean(features.NumCols());
		mean.AddRowSumMat(1.0, features);
		mean.Scale(1.0 / features.NumRows());
		for (int i = 0; i < features.NumRows(); i++)
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


	return 0;
}

FullGmm fgmm;
DiagGmm gmm;

//GMM start
using namespace std ;
int gmm_gselect(DiagGmm &gmm,Matrix<BaseFloat> &features,vector<vector<kaldi::int32> > &gselect,int start,int end)
{
	using std::vector;
	//typedef kaldi::int int;
	int num_gselect = 30;
    Matrix<BaseFloat> feats;
    int num_frames = end-start;
	int dim=0;
	int num_read=0;
      const Matrix<BaseFloat>  &this_feats = features;
      for (int t = start; t < end; t++) 
	  {
		  num_read++;
        if (dim == 0) 
		{
          dim = this_feats.NumCols();
          feats.Resize(num_frames, dim);
        } 
		else if (this_feats.NumCols() != dim)
	   	{
//          KALDI_ERR << "Features have inconsistent dims "
//                            << this_feats.NumCols() << " vs. " << dim
//                    << " (current utt is) " << feature_reader.Key();
        }
        if (num_read <= num_frames)
	   	{
          feats.Row(num_read - 1).CopyFromVec(this_feats.Row(t));
        } 
		else
	   	{
          BaseFloat keep_prob = num_frames / static_cast<BaseFloat>(num_read);
          if (WithProb(keep_prob)) 
		  { // With probability "keep_prob"
            feats.Row(RandInt(0, num_frames - 1)).CopyFromVec(this_feats.Row(t));
	   	  }
        }
      }

	KALDI_ASSERT(num_gselect > 0);
	int num_gauss = gmm.NumGauss();
	KALDI_ASSERT(num_gauss);
	if (num_gselect > num_gauss) 
	{
		KALDI_WARN << "You asked for " << num_gselect << " Gaussians but GMM "
			                 << "only has " << num_gauss << ", returning this many. "
							                  << "Note: this means the Gaussian selection is pointless.";
		num_gselect = num_gauss;
	}

	double tot_like = 0.0;
	kaldi::int64 tot_t = 0;
	
	//以下的2各变量都是""（空），可以不要
	int num_done = 0;//, num_err = 0;
	{
		int tot_t_this_file = 0; double tot_like_this_file = 0;
		//std::string utt=1;
		const Matrix<BaseFloat> &mat=feats;
		tot_t_this_file += mat.NumRows();

		tot_like_this_file = gmm.GaussianSelection(mat, num_gselect, &gselect);

		tot_t += tot_t_this_file;
		tot_like += tot_like_this_file;
		num_done++;
	}
	return 0;
}

int gmm_global_init_from_feats(Matrix<BaseFloat> &features, DiagGmm &gmm,int start,int end)
{

    MleDiagGmmOptions gmm_opts;

	gmm_opts.min_gaussian_weight = 0.0001;
	gmm_opts.min_gaussian_occupancy = 10;
	gmm_opts.min_variance = 0.001;
	gmm_opts.remove_low_count_gaussians = true;
//    bool binary = true;

    int num_gauss = 2;
    int num_gauss_init = 0;
    int num_iters = 20;
    int num_frames = (end-start > 30 ? 30 : end - start);
    int srand_seed = 0;
    int num_threads = 1;
		

    srand(srand_seed);	
    
    Matrix<BaseFloat> feats;

    if(num_frames <= 0)
	{
		return -1;
	}

    int num_read = 0, dim = 0;
    
	KALDI_LOG << "Reading features (will keep " << num_frames << " frames.)";
    
      const Matrix<BaseFloat>  &this_feats = features;
      for (int t = start; t < end; t++) 
	  {
        num_read++;
        if (dim == 0) 
		{
          dim = this_feats.NumCols();
          feats.Resize(num_frames, dim);
        } 
		else if (this_feats.NumCols() != dim)
	   	{
//          KALDI_ERR << "Features have inconsistent dims "
//                            << this_feats.NumCols() << " vs. " << dim
//                    << " (current utt is) " << feature_reader.Key();
        }
        if (num_read <= num_frames)
	   	{
          feats.Row(num_read - 1).CopyFromVec(this_feats.Row(t));
        } 
		else
	   	{
          BaseFloat keep_prob = num_frames / static_cast<BaseFloat>(num_read);
          if (WithProb(keep_prob)) 
		  { // With probability "keep_prob"
            feats.Row(RandInt(0, num_frames - 1)).CopyFromVec(this_feats.Row(t));
	   	  }
        }
      }

    if (num_read < num_frames) {
      KALDI_WARN << "Number of frames read " << num_read << " was less than "
                 << "target number " << num_frames << ", using all we read.";
      feats.Resize(num_read, dim, kCopyData);
    } else {
      BaseFloat percent = num_frames * 100.0 / num_read;
      KALDI_LOG << "Kept " << num_frames << " out of " << num_read
                << " input frames = " << percent << "%.";
    }

    if (num_gauss_init <= 0 || num_gauss_init > num_gauss)
      num_gauss_init = num_gauss;
    
  //  DiagGmm gmm(num_gauss_init, dim);
    
    KALDI_LOG << "Initializing GMM means from random frames to "
              << num_gauss_init << " Gaussians.";
    InitGmmFromRandomFrames(feats, &gmm);

    // we'll increase the #Gaussians by splitting,
    // till halfway through training.
    int cur_num_gauss = num_gauss_init,
        gauss_inc = (num_gauss - num_gauss_init) / (num_iters / 2);
        
    for (int iter = 0; iter < num_iters; iter++) {
      TrainOneIter(feats, gmm_opts, iter, num_threads, &gmm);

      int next_num_gauss = std::min(num_gauss, cur_num_gauss + gauss_inc);
      if (next_num_gauss > gmm.NumGauss()) {
        KALDI_LOG << "Splitting to " << next_num_gauss << " Gaussians.";
        gmm.Split(next_num_gauss, 0.1);
        cur_num_gauss = next_num_gauss;
      }
    }

 //   WriteKaldiObject(gmm, model_wxfilename, binary);
   // KALDI_LOG << "Wrote model to " << model_wxfilename;
    return 0;
}


int gmm_global_acc_stats(DiagGmm &gmm, AccumDiagGmm &gmm_accs,Matrix<BaseFloat> features,
		const std::vector<std::vector<int> > &gselects,int start,int end)
{
// 	using namespace kaldi;

    double tot_like = 0.0, tot_weight = 0.0;

    Matrix<BaseFloat> feats;
      const Matrix<BaseFloat>  &this_feats = features;
	  int num_frames=end-start;
	  int dim=0,num_read=0;
      for (int t = start; t < end; t++) 
	  {
        num_read++;
        if (dim == 0) 
		{
          dim = this_feats.NumCols();
          feats.Resize(num_frames, dim);
        } 
		else if (this_feats.NumCols() != dim)
	   	{
			fprintf(stderr,"%s %d erroe!\n",__FILE__,__LINE__);
			return -1;
        }
          feats.Row(num_read - 1).CopyFromVec(this_feats.Row(t));
      }
      
	  const Matrix<BaseFloat> &mat = feats;
      int file_frames = mat.NumRows();
      BaseFloat file_like = 0.0,
          file_weight = 0.0; // total of weights of frames (will each be 1 unless
      // --weights option supplied.
      Vector<BaseFloat> weights;
      
	  {
        const std::vector<std::vector<int> > &gselect = gselects;
        if (gselect.size() != static_cast<size_t>(file_frames)) 
		{
		  fprintf(stderr,"gselect.size %d != file_frames %d error!\n",(int)gselect.size(),file_frames);
		  return -1;
		}
        
        for (int i = 0; i < file_frames; i++) 
		{
          BaseFloat weight = (weights.Dim() != 0) ? weights(i) : 1.0;
          if (weight == 0.0) continue;
          file_weight += weight;
          SubVector<BaseFloat> data(mat, i);
          const std::vector<int> &this_gselect = gselect[i];
          int gselect_size = this_gselect.size();
          KALDI_ASSERT(gselect_size > 0);
          Vector<BaseFloat> loglikes;
          gmm.LogLikelihoodsPreselect(data, this_gselect, &loglikes);
          file_like += weight * loglikes.ApplySoftMax();
          loglikes.Scale(weight);
          for (int j = 0; j < loglikes.Dim(); j++)
            gmm_accs.AccumulateForComponent(data, this_gselect[j], loglikes(j));
        }
      } 
/*	  else 
	  { // no gselect..
        int i=0;
		  for ( i = 0; i < file_frames; i++) 
		{
          BaseFloat weight = (weights.Dim() != 0) ? weights(i) : 1.0;
          if (weight == 0.0) continue;
          file_weight += weight;
          file_like += weight *
              gmm_accs.AccumulateFromDiag(gmm, mat.Row(i), weight);
        }
      }
*/
      tot_like += file_like;
      tot_weight += file_weight;

    return  0 ;
}
// gmmbin/gmm-global-est.cc

int gmm_global_est(DiagGmm &gmm ,AccumDiagGmm &gmm_accs,int flags ) {
    typedef kaldi::int32 int32;

    MleDiagGmmOptions gmm_opts;

	gmm_opts.min_gaussian_weight =0.0001;
	gmm_opts.min_gaussian_occupancy = 10;
	gmm_opts.min_variance=0.001;
	if(flags == 1)
	{
		gmm_opts.remove_low_count_gaussians = true;
	}
	else
	{
		gmm_opts.remove_low_count_gaussians = false;
	}
//    bool binary_write = true;
    int mixup = 0;
    BaseFloat perturb_factor = 0.01;
    std::string update_flags_str = "mvw"; 

    {  // Update GMMs.
      BaseFloat objf_impr, count;
      MleDiagGmmUpdate(gmm_opts, gmm_accs,
                       StringToGmmFlags(update_flags_str),
                       &gmm, &objf_impr, &count);
      KALDI_LOG << "Overall objective function improvement is "
                << (objf_impr/count) << " per frame over "
                <<  (count) <<  " frames.";
    }

    if (mixup != 0)
      gmm.Split(mixup, perturb_factor);
	return 0;
  //  WriteKaldiObject(gmm, model_out_filename, binary_write);

   // KALDI_LOG << "Written model to " << model_out_filename;
}


//GMM end
#define MAP
int main(int argc,char *argv[])
{
	if(argc != 4)
	{
		printf("./ubm ubm_model wav1 wav2\n");
		return -1;
	}
	//read ubm model
	bool binary_read;
	/*
	Input ki_male(argv[1],&binary_read);
	fgmm.Read(ki_male.Stream(),binary_read);
	gmm.CopyFromFullGmm(fgmm);
	*/
	Input ki_male(argv[1],&binary_read);
	gmm.Read(ki_male.Stream(),binary_read);
	//read ubm end
	
	//get audio mfcc
	Matrix<float> features1 ;
	Matrix<float> features2 ;
	get_mfcc(argv[2],features1);
	get_mfcc(argv[3],features2);
	//end get mfcc
#ifdef MAP
		//对这个句子进行MAP计算
		DiagGmm temp_gmm1;
		DiagGmm temp_gmm2;
		temp_gmm1.CopyFromDiagGmm(gmm);
		temp_gmm2.CopyFromDiagGmm(gmm);
		MapDiagGmmOptions config;
		MleDiagGmmOptions gmm_opts;
		config.mean_tau = 0;

	gmm_opts.min_gaussian_weight =0.0001;
	gmm_opts.min_gaussian_occupancy = 10;
	gmm_opts.min_variance=0.001;
	int flags=0;
	if(flags == 1)
	{
		gmm_opts.remove_low_count_gaussians = true;
	}
	else
	{
		gmm_opts.remove_low_count_gaussians = false;
	}
		//config1.mean_tau = 15.0;
		float spk_change1 = 0.0, spk_frames1 = 0.0 , last_change1 = 0.0,change1=0;
		float spk_change2 = 0.0, spk_frames2 = 0.0 , last_change2 = 0.0,change2=0;
		AccumDiagGmm acc1(gmm, kGmmMeans);//save size
		AccumDiagGmm acc2(gmm, kGmmMeans);//save size
	
		//AccumDiagGmm acc1(gmm, kGmmAll);//save size
		//AccumDiagGmm acc2(gmm, kGmmAll);//save size
		//adapt model
		int map_iter=0;
		DiagGmm temp_gmm_1;
		DiagGmm temp_gmm_2;
			temp_gmm_1.CopyFromDiagGmm(gmm);
			temp_gmm_2.CopyFromDiagGmm(gmm);
		int i=0;
		float score1_gen=0.0 ,score1_self=0.0,score1_other=0.0;
		float score2_gen=0.0 ,score2_self=0.0,score2_other=0.0;
		while(1)
		{
			score1_gen=0.0 ,score1_self=0.0,score1_other=0.0;
			score2_gen=0.0 ,score2_self=0.0,score2_other=0.0;
			for(i=0;i<features1.NumRows();++i)
			{
			   //double	like=
			   acc1.AccumulateFromDiag(temp_gmm1, features1.Row(i), 1.0);
	//		     std::cout<<like<<"!!!"<<std::endl;
			}
			for(i=0;i<features2.NumRows();++i)
			{
				acc2.AccumulateFromDiag(temp_gmm2, features2.Row(i), 1.0);
			}
			GmmFlagsType flags = StringToGmmFlags("m");

			MapDiagGmmUpdate(config, acc1, flags, &temp_gmm_1, &spk_change1, &spk_frames1);

			MapDiagGmmUpdate(config, acc2, flags, &temp_gmm_2, &spk_change2, &spk_frames2);

		//	MleDiagGmmUpdate(gmm_opts, acc1, flags, &temp_gmm_1, &spk_change1, &spk_frames1);
		//	MleDiagGmmUpdate(gmm_opts, acc2, flags, &temp_gmm_2, &spk_change2, &spk_frames2);
			change1 = spk_change1 - last_change1;
			change2 = spk_change2 - last_change2;
			
			last_change1 = spk_change1;
			last_change2 = spk_change2;
			if ( map_iter >10 || (fabs(change1) < 20.0 && fabs(change2) < 20.0)) 
			{
				break;
			}
			map_iter++;
			for(i=0;i<features1.NumRows();++i)
			{
				score1_gen += gmm.LogLikelihood(features1.Row(i));
				score1_self += temp_gmm_1.LogLikelihood(features1.Row(i));
				score1_other += temp_gmm_2.LogLikelihood(features1.Row(i));
				//printf("%f \n",temp_gmm_1.LogLikelihood(features1.Row(i)));
			}
			for(i=0;i<features2.NumRows();++i)
			{
				score2_gen += gmm.LogLikelihood(features2.Row(i));
				score2_self += temp_gmm_2.LogLikelihood(features2.Row(i));
				score2_other += temp_gmm_1.LogLikelihood(features2.Row(i));
			}
			/*
			printf("%d: %f %f\n",map_iter,change1,change2);
			printf("map_iter %d %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f\n",
					map_iter,score1_gen/features1.NumRows(),score1_self/features1.NumRows(),score1_other/features1.NumRows(),
					score2_gen/features2.NumRows(),score2_self/features2.NumRows(),score2_other/features2.NumRows());
			temp_gmm1.CopyFromDiagGmm(temp_gmm_1);
			temp_gmm2.CopyFromDiagGmm(temp_gmm_2);
			*/
		}
		bool binary_write = false;
		
		WriteKaldiObject(gmm, "old.dubm", binary_write);
		WriteKaldiObject(temp_gmm_1, "new.dubm", binary_write);
		//score
		for(i=0;i<features1.NumRows();++i)
		{
			score1_gen += gmm.LogLikelihood(features1.Row(i));
			score1_self += temp_gmm_1.LogLikelihood(features1.Row(i));
			score1_other += temp_gmm_2.LogLikelihood(features1.Row(i));
			//printf("%f \n",temp_gmm_1.LogLikelihood(features1.Row(i)));
		}
		for(i=0;i<features2.NumRows();++i)
		{
			score2_gen += gmm.LogLikelihood(features2.Row(i));
			score2_self += temp_gmm_2.LogLikelihood(features2.Row(i));
			score2_other += temp_gmm_1.LogLikelihood(features2.Row(i));
		}

		fprintf(stdout,"**********\nscore=%5.3f\n**********\n",
				100/(1.0+expf( score1_gen/features1.NumRows() - score1_other/features1.NumRows() )));
		//fprintf(stderr,"map_iter %d %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f\n",
		//		map_iter,score1_gen/features1.NumRows(),score1_self/features1.NumRows(),score1_other/features1.NumRows(),
		//		score2_gen/features2.NumRows(),score2_self/features2.NumRows(),score2_other/features2.NumRows());
#endif
		//score end
		//MAP end
/*
	for(i=0;i<features2.NumRows();++i)
	{
		for(j=0;j<features2.NumCols();++j)
		{
			float b=features2(i,j);
			printf("%5.4f ",b);
		}
		printf("\n");
		fflush(stdout);
		for(j=0;j<features2.NumCols();++j)
		{
			printf("%5.4f ",fea_2[i*features2.NumCols()+j]);
		}
		printf("\n");
		fflush(stdout);
	}
*/
	//score
	//
	/*
#ifdef DTW
		float dis=0;
		dis=VDTWDistanceFun(fea_1+w_start_1*39 , fea_2+w_start_2*39 , 39 , w_end_1-w_start_1 , w_end_2-w_start_2 ,5,0);
		printf("dis=%f\n",dis);
#endif
//构建gmm然后用于打分

#ifdef GMM
		DiagGmm gmm_gmm(2, 39);
		if(0 != gmm_global_init_from_feats(features1,gmm_gmm,w_start_1,w_end_1))
		{
			fprintf(stderr,"%s %d error!\n",__FILE__,__LINE__);
			return -1;
		}

		vector<vector<kaldi::int32> > gselect_gmm;
		if(0 != gmm_gselect(gmm_gmm , features1,gselect_gmm,w_start_1,w_end_1))
		{
			fprintf(stderr,"gmm_select error!\n");
			return -1;
		}
		int iter=0;
		std::string update_flags_str = "mvw";
		for(iter=0;iter < 20;++iter)
		{
			AccumDiagGmm gmm_gmm_accs;
			gmm_gmm_accs.Resize(gmm_gmm, StringToGmmFlags(update_flags_str));
			if( 0 != gmm_global_acc_stats(gmm_gmm,gmm_gmm_accs,features1,gselect_gmm,w_start_1,w_end_1))
			{
				fprintf(stderr,"gmm-global-acc-stats error!\n");
				return -1;
			}
			if(iter == 19)
			{
				if(0 != gmm_global_est(gmm_gmm,gmm_gmm_accs,1))
				{
					fprintf(stderr,"%s %d error!\n",__FILE__,__LINE__);
					return -1;
				}
			}
			else
			{
				if( 0 != gmm_global_est(gmm_gmm,gmm_gmm_accs,0))
				{
					fprintf(stderr,"%s %d error!\n",__FILE__,__LINE__);
					return -1;
				}
			}
		}//for(iter=0;iter < 20;++iter)
		float score1_gmm=0;
		float score2_gmm=0;
		for(i=w_start_1;i<w_end_1;++i)
		{
			score1_gmm += gmm_gmm.LogLikelihood(features1.Row(i));
			//printf("%f \n",temp_gmm_1.LogLikelihood(features1.Row(i)));
		}
		for(i=w_start_2;i<w_end_2;++i)
		{
			score2_gmm += gmm_gmm.LogLikelihood(features2.Row(i));
		}
		printf(" socre1=%7.5f score2=%7.5f\n",score1_gmm/(w_end_1-w_start_1),score2_gmm/(w_end_2-w_start_2));
#endif
*/


	return 0;
}

