#include "engine/stream-nnet-decodable-score.h"
#include "utils.h"

namespace kaldi {

using namespace nnet1;

StreamNnetDecodableOptions::StreamNnetDecodableOptions():
  splice_left_ctx(5), 
  splice_right_ctx(5), 
  no_softmax(false),
  apply_log(false), 
  acoustic_scale(0.10)
  {}


void StreamNnetDecodableOptions::Register(OptionsItf *po) {
  feature_opts.Register(po);
  cmvn_opts.Register(po);
  pdf_prior_opts.Register(po);
  po->Register("feature-transform", &feature_transform, "Feature transform in front of main network (in nnet format)");
  po->Register("no-softmax", &no_softmax, "No softmax on MLP output (or remove it if found), the pre-softmax activations will be used as log-likelihoods, log-priors will be subtracted");
  po->Register("apply-log", &apply_log, "Transform MLP output to logscale");
  po->Register("nnet-model", &model_nnet_filename, "");
  po->Register("acoustic-scale", &acoustic_scale, "");
}
static int NetworkInit(const StreamNnetDecodableOptions config,Vector<BaseFloat> &priors_host);

StreamNnetDecodable::StreamNnetDecodable(StreamNnetDecodableOptions config,TransitionModel *trans_model): config_(config),
			pdf_prior_(config.pdf_prior_opts),trans_model_(trans_model) {
  feat_input_ = new StreamFeatureInput(config.feature_opts);
  cmvn_input_ = new StreamCmvnInput(config.cmvn_opts);

  nnet_.Read(config.model_nnet_filename);
  if (config.feature_transform != "") {
    nnet_transf_.Read(config.feature_transform);
    remainder_.Resize(config.splice_left_ctx + config.splice_right_ctx, config.feature_opts.Dim());
    remainder_size_ = 0;
  }

  if (config.no_softmax && nnet_.GetComponent(nnet_.NumComponents() - 1).GetType() == Component::kSoftmax) {
    KALDI_LOG << "Removing softmax from the nnet ";
    nnet_.RemoveComponent(nnet_.NumComponents()-1);
  }
  if (config.apply_log && config.no_softmax) {
    KALDI_ERR << "Nonsense option combination : --apply-log=true and --no-softmax=true";
  }
  if (config.apply_log && nnet_.GetComponent(nnet_.NumComponents() - 1).GetType() != Component::kSoftmax) {
    KALDI_ERR << "Used --apply-log=true, but nnet does not have <softmax> as last component!";
  }
  if (config.pdf_prior_opts.class_frame_counts != "" && (!config.no_softmax && !config.apply_log)) {
    KALDI_ERR << "Option --class-frame-counts has to be used together with "
          << "--no-softmax or --apply-log";
  }

  NetworkInit(config,priors_host);
}
static int NetworkInit(const StreamNnetDecodableOptions config,Vector<BaseFloat> &priors_host) 
{
    // Read the class-counts, compute priors
    // FIXME clear needed?
    if(config.pdf_prior_opts.class_frame_counts != "") 
	{
      Input in;
      in.OpenTextMode(config.pdf_prior_opts.class_frame_counts);
      priors_host.Read(in.Stream(), false);
      in.Close();
     
      //create inv. priors, or log inv priors 
      BaseFloat sum = priors_host.Sum();
      priors_host.Scale(1.0/sum);
      if (config.apply_log || config.no_softmax) {
        priors_host.ApplyLog();
        priors_host.Scale(-1.0);
      } else {
        priors_host.ApplyPow(-1.0);
      }

      //detect the inf, replace by something reasonable (maximal non-inf value)
      //a) replace inf by -inf
      for(int32 i=0; i<priors_host.Dim(); i++) {
        if(priors_host(i) == std::numeric_limits<BaseFloat>::infinity()) {
          priors_host(i) *= -1.0;
        }
      }
      //b) find max
      BaseFloat max = priors_host.Max();
      //c) replace -inf by max prior
      for(int32 i=0; i<priors_host.Dim(); i++) {
        if(priors_host(i) == -std::numeric_limits<BaseFloat>::infinity()) {
          priors_host(i) = max;
        }
      }
    }
}

StreamNnetDecodable::~StreamNnetDecodable() {
  delete feat_input_;
  delete cmvn_input_;
  trans_model_ = NULL;//be set by outer
}

int32 StreamNnetDecodable::SetData(const char *data, uint32_t len, int chunk_no) {
  // deal with 16bit wave only
  len /= sizeof(short);
  Vector<BaseFloat> wave(len, kUndefined);
  for (uint32_t i = 0; i < len; ++i)
    wave(i) = reinterpret_cast<const int16 *>(data)[i];
  Matrix<BaseFloat> features;
  feat_input_->Process(wave, features);
  
  if (!features.NumRows())
    return likelihood_.NumRows();
  
  // add by get feature
  MatrixIndexT last = fbank_.NumRows();
  fbank_.Resize(last + features.NumRows(), features.NumCols(), kCopyData);
  SubMatrix<BaseFloat> sub = fbank_.Range(last, features.NumRows(), 0, fbank_.NumCols());
 // features.CopyToMat(&sub);
  sub.CopyFromMat(features);

  cmvn_input_->Process(features);
  return Process(features, likelihood_, chunk_no);
}

void StreamNnetDecodable::Reset() {
  likelihood_.Resize(0, 0);
  fbank_.Resize(0, 0);
  remainder_size_ = 0;  
  feat_input_->Reset();
  cmvn_input_->Reset();
}

bool StreamNnetDecodable::IsLastFrame(int32 frame) const {
  KALDI_ERR << __func__ << "unimplemented";
  return false;
}

int32 StreamNnetDecodable::NumIndices() const {
  return trans_model_->NumTransitionIds();
}

int32 StreamNnetDecodable::Process(const Matrix<BaseFloat> &in, Matrix<BaseFloat> &out, int chunk_no) {
  CuMatrix<BaseFloat> feats, feats_transf, nnet_out;
  // paste remainder & input
  int feats_size = in.NumRows() + remainder_size_;
  int full_ctx = config_.splice_left_ctx + config_.splice_right_ctx;
  Matrix<BaseFloat> feats_cpu(feats_size, in.NumCols());
  {
    SubMatrix<BaseFloat> sub(feats_cpu, 0, full_ctx, 0, in.NumCols());
    sub.CopyFromMat(remainder_);
  }
  {
    SubMatrix<BaseFloat> sub(feats_cpu, remainder_size_, in.NumRows(), 0, in.NumCols());
    sub.CopyFromMat(in);
  }
  feats = feats_cpu;

  // save to remainder
  remainder_size_ = in.NumRows() + remainder_size_;
  if (remainder_size_ > full_ctx)
    remainder_size_ = full_ctx;
  {
    SubMatrix<BaseFloat> sub(feats_cpu, feats_size - remainder_size_, remainder_size_, 0, in.NumCols()); 
    remainder_.CopyFromMat(sub);
  }

  nnet_transf_.Feedforward(feats, &feats_transf);

  //cut off head & tail
  int head_frame = config_.splice_left_ctx, tail_frame = feats_size - config_.splice_right_ctx;
  if (abs(chunk_no) == 1)
    head_frame -= config_.splice_left_ctx;
  if (chunk_no < 0)
    tail_frame += config_.splice_right_ctx;
  CuSubMatrix<BaseFloat> sub_feats_transf(feats_transf, head_frame, tail_frame - head_frame, 0, feats_transf.NumCols());
  // TODO: feedforward should accept CuSubMatrix
  CuMatrix<BaseFloat> new_feats_transf(sub_feats_transf);
  nnet_.Feedforward(new_feats_transf, &nnet_out);
  // convert posteriors to log-posteriors
  if (config_.apply_log) {
    nnet_out.ApplyLog();
  }
  // subtract log-priors from log-posteriors to get quasi-likelihoods
  if(config_.pdf_prior_opts.class_frame_counts != "" && (config_.no_softmax || config_.apply_log)) {
    pdf_prior_.SubtractOnLogpost(&nnet_out);
  }

  // acoustic_scale
  nnet_out.Scale(config_.acoustic_scale);
  //download from GPU
  MatrixIndexT last = out.NumRows();
  out.Resize(last + nnet_out.NumRows(), nnet_out.NumCols(), kCopyData);
  SubMatrix<BaseFloat> sub = out.Range(last, nnet_out.NumRows(), 0, out.NumCols());
  nnet_out.CopyToMat(&sub);
  
  //check for NaN/inf
  /*
  for (int32 r = last; r < out.NumRows(); ++r) {
    for (int32 c = 0; c < out.NumCols(); ++c) {
      BaseFloat val = out(r, c);
      if(val != val) 
        KALDI_ERR << "NaN in NNet output";
      if(val == std::numeric_limits<BaseFloat>::infinity())
        KALDI_ERR << "inf in NNet coutput";
    }
  }
  */
  return out.NumRows();
}

BaseFloat StreamNnetDecodable::LogLikelihood(int32 frame, int32 index) {
  // return config_.acoustic_scale * likelihood_(frame, trans_id_to_pdf_[index]);
  // already scaled in nnet forward process
  return likelihood_(frame, trans_model_->TransitionIdToPdf(index));
  //
  //return likelihood_(frame, index)/config_.acoustic_scale;
}
//next getresult
bool StreamNnetDecodable::GetResult(const std::vector<int32> &phones,std::vector<nnet1::PhoneConfidence> &phone_confs,int *frame_array,Matrix<BaseFloat>**features) 
{
	(*features) = &fbank_;
	ComputeConfidence(*trans_model_, phones, likelihood_, priors_host, frame_array, &phone_confs);

	return true;
}
int StreamNnetDecodable::LoadStat(const Matrix<double> &in) {
  return cmvn_input_->Load(in);
}
int StreamNnetDecodable::SaveStat(Matrix<double> &out) {
  return cmvn_input_->Save(out);
}

} // namesapce kaldi
