#include "engine/stream-hclg-decoder.h"
#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "online/onlinebin-util.h"
#include "hmm/transition-model.h"
#include "engine/stream-faster-decoder.h"

namespace kaldi {

StreamHCLGDecoderOptions::StreamHCLGDecoderOptions():
  biglm(false) {}

void StreamHCLGDecoderOptions::Register(OptionsItf *po) {
  decoder_opts.Register(po, true);
  //po->Register("trans-model", &model_trans_filename, "Transition model");
  po->Register("decode-graph", &fst_filename, "HCLG fst");
  po->Register("biglm", &biglm, "Big LM support");
  po->Register("oldlm", &old_lm_fst_filename, "Old LM FSA");
  po->Register("newlm", &new_lm_fst_filename, "New LM FSA");
}

fst::Fst<fst::StdArc> *StreamHCLGDecoder::ReadNetwork(std::string filename) {
  // read decoding network FST
  Input ki(filename); // use ki.Stream() instead of is.
  if (!ki.Stream().good()) KALDI_ERR << "Could not open decoding-graph FST "
                                     << filename;

  fst::FstHeader hdr;
  if (!hdr.Read(ki.Stream(), "<unknown>")) {
  KALDI_ERR << "Reading FST: error reading FST header.";
  }
  if (hdr.ArcType() != fst::StdArc::Type()) {
  KALDI_ERR << "FST with arc type " << hdr.ArcType() << " not supported.\n";
  }
  fst::FstReadOptions ropts(filename, &hdr);
//  ropts.mode = fst::FstReadOptions::MAP;

  fst::Fst<fst::StdArc> *decode_fst = NULL;

  if (hdr.FstType() == "vector") {
  decode_fst = fst::VectorFst<fst::StdArc>::Read(ki.Stream(), ropts);
  } else if (hdr.FstType() == "const") {
  decode_fst = fst::ConstFst<fst::StdArc>::Read(ki.Stream(), ropts);
  } else {
  KALDI_ERR << "Reading FST: unsupported FST type: " << hdr.FstType();
  }
  if (decode_fst == NULL) { // fst code will warn.
  KALDI_ERR << "Error reading FST (after reading header).";
  return NULL;
  } else {
  return decode_fst;
  }
}

StreamHCLGDecoder::StreamHCLGDecoder(const StreamHCLGDecoderOptions &config, StreamDecodableInterface &decodable, fst::Fst<fst::StdArc> *decode_fst,fst::SymbolTable *word_syms): decodable_(decodable),word_syms_(word_syms), opts_(config) {

  if (decode_fst) {
    decode_fst_ = decode_fst;
    own_decode_fst_ = false;
  } else {
    own_decode_fst_ = true;
    decode_fst_ = ReadNetwork(config.fst_filename);
    // biglm
    compose_dfst_ = NULL;
    cache_dfst_ = NULL;
    old_lm_dfst_ = NULL;
    new_lm_dfst_ = NULL;
    old_lm_fst_ = NULL;
    new_lm_fst_ = NULL;
    if (config.biglm) {
      old_lm_fst_ = fst::ReadFstKaldi(config.old_lm_fst_filename);
      ApplyProbabilityScale(-1.0, old_lm_fst_);
      new_lm_fst_ = ReadNetwork(config.new_lm_fst_filename);
      old_lm_dfst_ = new fst::BackoffDeterministicOnDemandFst<StdArc>(*old_lm_fst_);
      new_lm_dfst_ = new fst::BackoffDeterministicOnDemandFst<StdArc>(*new_lm_fst_);
      compose_dfst_ = new fst::ComposeDeterministicOnDemandFst<StdArc>(old_lm_dfst_,new_lm_dfst_);
      cache_dfst_ = new fst::CacheDeterministicOnDemandFst<StdArc>(compose_dfst_); 
    }
  }
  if (config.biglm) {
    decoder_ = new StreamBiglmFasterDecoder(*decode_fst_, config.decoder_opts, cache_dfst_);
  } else {
    decoder_ = new StreamFasterDecoder(*decode_fst_, config.decoder_opts);
  }
}

bool StreamHCLGDecoder::ChangeFst(fst::Fst<fst::StdArc> *decode_fst)
{
	if (decode_fst) {
		if (own_decode_fst_)
			delete decode_fst_;
		if(decoder_){
			delete decoder_;
			decoder_ = NULL;
		}
	
		decode_fst_ = decode_fst;
		own_decode_fst_ = false;//mark fst comes from outer
		
		decoder_ = new StreamFasterDecoder(*decode_fst_, opts_.decoder_opts);
		
		return true;
	} else {
		return false;
	}
}

void StreamHCLGDecoder::Reset()
{
	word_syms_ = NULL;
	if (opts_.biglm) {
		delete cache_dfst_;
		delete compose_dfst_;
		delete new_lm_dfst_;
		delete old_lm_dfst_;
		delete old_lm_fst_;
		delete new_lm_fst_;
	}
	if (own_decode_fst_)
	{
		own_decode_fst_ = false;
		delete decode_fst_;
	}
	if(decoder_)
	{
		delete decoder_;
		decoder_ = NULL;
	}  
    decodable_.Reset();
    curr_frame_ = 0;
}
StreamHCLGDecoder::~StreamHCLGDecoder() {
	word_syms_ = NULL;
	if (opts_.biglm) {
		delete cache_dfst_;
		delete compose_dfst_;
		delete new_lm_dfst_;
		delete old_lm_dfst_;
		delete old_lm_fst_;
		delete new_lm_fst_;
	}
	if (own_decode_fst_)
	{
		delete decode_fst_;
	}
	if(decoder_){
		delete decoder_;
		decoder_ = NULL;
	}
}

int StreamHCLGDecoder::Decode(const char *data, uint32 len, int chunk_no) {
  if (abs(chunk_no) == 1) {
    decodable_.Reset();
    decoder_->Reset();
    curr_frame_ = 0;
  }
  int next = decodable_.SetData(data, len, chunk_no);
  for (int i = curr_frame_; i < next; ++i)
    decoder_->DecodeFrame(&decodable_, i);
  curr_frame_ = next;
  return curr_frame_;
}
//in from here
bool StreamHCLGDecoder::GetResult(std::vector<int32> &words,std::vector<nnet1::PhoneConfidence> &phone_confs
		,int *frame_array,Matrix<BaseFloat>**features) 
{
	fst::VectorFst<LatticeArc> decoded;
	bool res=0;
	res=decoder_->GetBestPath(&decoded);
	if(res==false)
		return false;

	std::vector<int32> alignment;
	res=GetLinearSymbolSequence(decoded, &alignment, &words, reinterpret_cast<LatticeWeight *>(NULL));
	if(res==false)
		return false;
	return decodable_.GetResult(alignment,phone_confs,frame_array,features); 
}

} // namespace kaldi
