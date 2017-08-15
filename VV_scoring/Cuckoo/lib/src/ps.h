#ifndef __PS_H__
#define __PS_H__

#include "base/kaldi-common.h"
#include "matrix/matrix-common.h"
#include "util/common-utils.h"
#include "gmm/am-diag-gmm.h"
#include "hmm/transition-model.h"
#include "fstext/fstext-lib.h"
#include "decoder/biglm-faster-decoder.h"
#include "decoder/training-graph-compiler.h"
//#include "util/timer.h"
#include "base/timer.h"

#include "nnet/nnet-nnet.h"
#include "decoder/decodable-matrix.h"
#include <string>

#include <iostream>
#include <fstream>
#include "engine/stream-nnet-decodable-score.h"
#include "engine/stream-hclg-decoder.h"
#include "fst/flags.h"

#include "utils.h"

#ifdef WP8
#include "LogStream.h"
#endif


#ifdef __unix__
	#define PACHIRA_DEBUG log_out
	std::fstream log_out("./log.testlog",std::fstream::out | std::fstream::app );
#elif IOS
	#define PACHIRA_DEBUG std::cerr 
#elif __ANDROID_API__
	#define PACHIRA_DEBUG log_out
	std::fstream log_out("/mnt/sdcard/MoFang/log.testlog",std::fstream::out | std::fstream::app );
#elif WP8
	#define PACHIRA_DEBUG log_out
	my_io::stream<My_Sink> log_out(0);
#endif

#ifdef __SCORE_INFO__
const int g_score_threshold=0;
#else
const int g_score_threshold=20;
#endif
const double threshold = 1e-4;

const int feature_vec_len = 40;

using namespace kaldi;
using namespace nnet1;
using namespace fst;

/*读入模板*/
struct Phone_Info
{
	std::string phone_name;
	int start;
	int end;
	//to support the word_vec for template information
	BaseFloat ph_conf; //confidience

	Phone_Info():start(0),end(0),ph_conf(0){}; //constructor function for initialize
};
struct Word_Info
{
	std::string word_name;
	int start;
	int end;
	int n_phone;
	BaseFloat wd_conf;
	std::vector<Phone_Info> phone_link;

	Word_Info():start(0),end(0),wd_conf(0),n_phone(0){};
};
struct Sent_Info
{
	int n_word;
	std::vector<Word_Info> word_link;

	Sent_Info():n_word(0){};
};

void fill_phone(Phone_Info &phone,std::string phone_name , int start , int end ,BaseFloat conf);

void add_phone_word(Word_Info &word, Phone_Info &phone);

void add_name_word(Word_Info &word, std::string word_name);

void add_word_sent(Sent_Info &sent,Word_Info &word);

//sentence:整句话的对齐信息
//score:单词打分
//n:单词个数
//g_score:整体打分，用于判断文本语音频是否对应
//s_score:标准打分，用于对应于的标准程度给出度量
//本函数主要目的在于给单词打分，并根据g_score，s_score和单词打分高低，相互进行调整分数，为了处理不合理情况。
int word_score(Sent_Info sentence,int *score,int n,int &g_score , int &s_score);

//针对整句打分
//frame_array:帧统计数据
//n:指针数据长度
//all_frame: 语音所有帧数
//phone_frame_nu:语音有效帧数
//g_score:用于判断文本是否和语音对应
//s_score:用于给语音标准打分
//返回0正确，-1错误
int sent_score(int *frame_array,int n,int all_frame,int phone_frame_nu,int *g_score , int *s_score);

typedef enum
{
	SEN_END,
	SEN_BAD,
	SEN_POOR,
	SEN_OK,
	SEN_WORD_END,
	FILE_FAIL
}SEN_INFO;

typedef enum
{
	Parchira_OK=0,
	Parchira_CONF_LOAD_FAIL=10,
	Parchira_CONF_LOAD_ERROR,
	Parchira_CONF_SET_ERROR,
	Parchira_OPEN_WAV_FAIL,
	Parchira_READ_WAV_FAIL,
	Parchira_GET_WORD_ERROR,
	Parchira_TEXT_NO_WORD,
	Parchira_OUTPUT_ERROR,
	Parchira_RUN_ERROR,
	Parchira_WAV_POOR,
	Parchira_STREAM_CONF_ERROR,
	Parchira_STREAM_FLAG_ERROR,
	Parchira_STREAM_END,
	Parchira_STREAM_NOT_END,
	Parchira_GET_WAV_FEATURE_ERROR
}SCORE_STATE,RET_TYPE;

class  pachira_score
{
private:
	//the gloable setting
	StreamNnetDecodable *decodable;
	StreamHCLGDecoder *decoder;
    StreamHCLGDecoderOptions decoder_opts;
	TransitionModel trans_model;//to get gc
	ContextDependency ctx_dep;

	VectorFst<StdArc> decode_fst;
	SCORE_STATE score_state;
	
    fst::SymbolTable *word_syms;
    fst::SymbolTable *phone_syms;
	TrainingGraphCompiler *gc;
	
	
	//for stream score
	Matrix< BaseFloat > wav_store;//for pitch
    int batch_size;// = 32 * 160 * 2;
	char* wav_data;
	int buf_len;
	int buf_start;
	int buf_end;
	
	std::string cmvn_mean_filename;
	int stream_id;
//	std::string stand_tem_in; // standard template file name
//	std::string com_tem_out;  // compare template file name
//	std::string math_out;     // match file name
	std::vector<int> word_ids;
	
	
	/*to init the state machine*/
	bool load_conf(int &set_nu,char*** set_conf,const char *conf_file);
	bool set_conf(const char *conf_file);
	bool set_conf(int argc,const char **argv);
	void clean_stream_state();
	
	
	/**the functions to prepare data*/
	inline void convert_text(char *text);
	int get_trans_text(const char *text);
	int get_trans_cri(std::vector<int>&,const char *cri_tem);
	int get_wave_data(const char *audio_file,char **data,int &size);
#ifdef WP8
	bool out_put_temp(const char *, std::vector<int32> &,
		std::vector<PhoneConfidence> &,int *,const Matrix<BaseFloat>*,int *score);
#else
	bool out_put_temp(const char *, std::vector<kaldi::int32> &,
			std::vector<PhoneConfidence> &,int *,const Matrix<BaseFloat>*,int *score);
#endif
	
public:
	/*get the sigelon to works as state machine */
	static pachira_score& get_single(const char* conf_file);
	
	inline int set_steam_conf(const char *text);
	inline int send_wav(const char* wav_buf, int buf_len, int is_end);
	int score_get(int **w_score,int *n);
	inline int get_state()
	{
		return score_state;
	}
	int make_align_sent(Sent_Info &sentence , std::vector<PhoneConfidence> &phone_confs ,std::vector<kaldi::int32> &words);
#ifdef IOS
    //used to get the ios home diretory, such as /user_sanbox/Documents
    inline bool get_ios_home_dir(char *home);
#endif
	
private:
	pachira_score(const char* conf_file);
	pachira_score(const pachira_score& other){}
	pachira_score operator=(const pachira_score& other){}
	~pachira_score();
};

#endif
