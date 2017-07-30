#include "pachira_score.h"
#include "mlp.h"
#include "ps.h"
#include <iomanip>
#ifdef  __ANDROID_API__
#include<jni.h>
#include <android/log.h>
#endif
#ifdef IOS

#include "CoreFoundation/CoreFoundation.h"
#include <string.h>

#endif

#ifdef WP8				// ---wangyajun 2014/8/24
#include <random>		//for random() 
std::random_device rd;
std::mt19937 random(rd());
#include <cstring>		 //for strtok_s() 
#endif

//#define MLP_DATA_TEST  //为了产生mlp的数据
//#define TEST_WORD_SORE //为了产生word打分的相关数据
#define MLP_FEA_TEST   //为了得到特征
//#define AUDIO_TEST

#define S_SCORE_RATE 0.3
#define G_SCORE_RATE 0.7

/*get the sigelon*/
#ifdef __ANDROID_API__
pachira_score& pachira_score::get_single(const char* conf_file="/mnt/sdcard/Pince/conf/pachira.conf")
#elif IOS
pachira_score& pachira_score::get_single(const char* conf_file="./pachira.conf")
#elif __unix__
pachira_score& pachira_score::get_single(const char* conf_file="./conf/pachira.conf")
#elif WP8
pachira_score& pachira_score::get_single(const char* conf_file = "pachira/conf/pachira.conf")
#else
pachira_score& pachira_score::get_single(const char* conf_file)
#endif
{
#ifdef IOS
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	char path[1024];
	if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, 1024))
	{
		// error!
	}
	CFRelease(resourcesURL);
	chdir(path);
#endif
#ifdef DEBUG
#ifdef WP8
	mylog << info_str << "info get_single" << endl;
#endif
#endif
	static pachira_score match(conf_file);
#ifdef DEBUG
#ifdef WP8
	mylog << info_str << "leave get_single" << endl;
#endif
#endif
	return match;
}
/*init the pachira_score*/
const int batch_size_init = (32 * 160 * 2);
pachira_score::pachira_score(const char* conf_file) :score_state(Parchira_OK), word_syms(NULL), gc(NULL),
batch_size(batch_size_init), decoder(NULL), wav_data(NULL), buf_start(0), buf_end(0)
{
	if (!set_conf(conf_file))
	{
		return;
	}
	if (Parchira_OK != score_state)
	{
		PACHIRA_DEBUG << "load the conf setting: " << conf_file << " fail";
	}
}

bool pachira_score::set_conf(const char* conf_file)
{
	int argc = 0;
	char **argv = NULL;
	if (!load_conf(argc, &argv, conf_file))
	{
		return false;
	}
	if (!set_conf(argc, (const char**)argv))
	{
		return false;
	}
	while (NULL != argv && argc >= 0)
	{
		if (NULL != argv[argc]) delete[] argv[argc];
		--argc;
	}
	delete[] argv;
	argv = NULL;
	return true;
}

bool pachira_score::load_conf(int &set_nu, char*** set_conf, const char *conf_file)
{
	PACHIRA_DEBUG << "load the conf setting: " << conf_file << "\n";

	if (set_nu != 0 && NULL != (*set_conf))
	{
		while (set_nu)
		{
			if (NULL != (*set_conf)[--set_nu]) delete[](*set_conf)[set_nu];
		}
	}
	(*set_conf) = NULL;
	set_nu = 0;

	std::fstream fs(conf_file, std::fstream::in);
	do{
		if (fs.fail())
		{
			PACHIRA_DEBUG << "fail to open the conf setting: " << conf_file << "\n";
			score_state = Parchira_CONF_LOAD_FAIL;
			break;
		}

		const int SET_LEN = 100;
		char str[SET_LEN];
		while (!fs.eof())
		{
			fs.getline(str, SET_LEN);

			if (str[0] != '#' && str[0] != '\0')
			{
				set_nu++;
			}
		}

		char **setting = new char *[set_nu + 1];//set_nu+1是为了接收最后一列不会出错
		while (set_nu >= 0)
		{
			setting[set_nu--] = new char[SET_LEN];
		}
		
		set_nu = 0;
		fs.clear();
		fs.seekg(0, std::fstream::beg);
		while (!fs.eof())
		{
			fs.getline(setting[set_nu], SET_LEN);

			if (setting[set_nu][0] != '#' && setting[set_nu][0] != '\0')
			{
				set_nu++;
			}
		}
		(*set_conf) = setting;
		fs.close();
		return true;
	} while (0);

	fs.close();
	return false;
}

bool pachira_score::set_conf(int argc, const char **argv)
{
	ParseOptions po("");
#ifdef  IOS
	std::string model_trans_filename("./final.mdl"),
		tree_in_filename("./tree"),
		lex_in_filename("./L_disambig.fst"),
		phone_filename("./phones.txt"),
		word_syms_filename("./words.txt");
	std::string disambig_rxfilename("./disambig.int");
#elif __ANDROID_API__
	std::string model_trans_filename("/mnt/sdcard/Pince/model/final.mdl"),
		tree_in_filename("/mnt/sdcard/Pince/model/tree"),
		lex_in_filename("/mnt/sdcard/Pince/model/L_disambig.fst"),
		phone_filename("/mnt/sdcard/Pince/model/phones.txt"),
		word_syms_filename("/mnt/sdcard/Pince/model/words.txt");
	std::string disambig_rxfilename("/mnt/sdcard/Pince/model/disambig.int");
#elif WP8
	std::string model_trans_filename("pachira/model/final.mdl"),
		tree_in_filename("pachira/model/tree"),
		lex_in_filename("pachira/model/L_disambig.fst"),
		phone_filename("pachira/model/phones.txt"),
		word_syms_filename("pachira/model/words.txt");
	std::string disambig_rxfilename("pachira/model/disambig.int");
#else
	std::string model_trans_filename("./model/final.mdl"),
		tree_in_filename("./model/tree"),
		lex_in_filename("./model/L_disambig.fst"),
		phone_filename("./model/phones.txt"),
		word_syms_filename("./model/words.txt");
	std::string disambig_rxfilename("./model/disambig.int");
#endif
	TrainingGraphCompilerOptions gopts;
	gopts.Register(&po);
	decoder_opts.Register(&po);
	//add hubo
	//	conf_opts.Register(&po);
	StreamNnetDecodableOptions decodable_opts;
	decodable_opts.Register(&po);
	po.Register("cmvn-mean", &cmvn_mean_filename, "cmvn mean binary file");
	po.Read(argc, argv);
	//load transition model
	{
		bool binary;
		Input ki(model_trans_filename, &binary);
		trans_model.Read(ki.Stream(), binary);
	}
	ReadKaldiObject(tree_in_filename, &ctx_dep);
	VectorFst<StdArc> *lex_fst = fst::ReadFstKaldi(lex_in_filename);

	if (phone_filename != "")
	{
		phone_syms = fst::SymbolTable::ReadText(phone_filename);
		if (!phone_syms)
		{
			PACHIRA_DEBUG << "Could not read symbol table from file " << phone_filename << "\n";
			score_state = Parchira_CONF_LOAD_ERROR;
			return false;
		}
	}
	if (word_syms_filename != "")
	{
		word_syms = fst::SymbolTable::ReadText(word_syms_filename);
		if (!word_syms)
		{
			PACHIRA_DEBUG << "Could not read symbol table from file " << word_syms_filename << "\n";
			score_state = Parchira_CONF_LOAD_ERROR;
			return false;
		}
	}

	std::vector<int> disambig_syms;
	if (disambig_rxfilename != "")
	{
		if (!ReadIntegerVectorSimple(disambig_rxfilename, &disambig_syms))
		{
			PACHIRA_DEBUG << "fstcomposecontext: Could not read disambiguation symbols from " << disambig_rxfilename << "\n";
			score_state = Parchira_CONF_LOAD_ERROR;
			return false;
		}
	}

	gc = new TrainingGraphCompiler(trans_model, ctx_dep, lex_fst, disambig_syms, gopts);
	lex_fst = NULL;  // we gave ownership to gc.

	decodable = new StreamNnetDecodable(decodable_opts, &trans_model);

	ifstream cmvn(cmvn_mean_filename.c_str(), ios::in | ios::binary);
	if (!cmvn.fail())
	{
		Matrix<double> cmvn_mean;
		cmvn_mean.Read(cmvn, true);
		cmvn.close();
		decodable->LoadStat(cmvn_mean);
	}

	return true;
}
void pachira_score::clean_stream_state()
{
//	stand_tem_in = "";
//	com_tem_out = "";
//	math_out = "";
	stream_id = 0;

	if (wav_data)
	{
		free(wav_data);
		wav_data = NULL;
	}
	word_ids.erase(word_ids.begin(),word_ids.end());
	buf_len = 0;
	buf_start = 0;
	buf_end = 0;
}

/**the functions to prepare data*/
typedef std::vector<int> idx_vec;
void pachira_score::convert_text(char *text)
{
	bool num_not_start = true;
	while ('\0' != *text)
	{
		if (isalpha(*text)){
			*text = toupper(*text);
		}
		else if (isdigit(*text)){
			if (num_not_start && '0' == *text){
				*text = ' ';
			}
			else{
				num_not_start = true;
			}
		}
		else if ('\'' == (*text)){
			*text = toupper(*text);
		}
		else{
			*text = ' ';
		}

		++text;
	}
}

/*
 * 将文本转换为word 的id序列，保存在类的word_ids成员中
 * 返回正确，或者没有找到的第一个word
 * */
int pachira_score::get_trans_text(const char *text)
{
	int flag = Parchira_OK;

	char *text_tmp = new char[strlen(text) + 1];
	strncpy(text_tmp, text, strlen(text) + 1);
	convert_text(text_tmp);


	char* word = strtok(text_tmp, " ");
	int not_find = 0;//返回第一个没有找到的word
	if( !word )
		return 1;
	while (word)
	{
		int word_id = word_syms->Find(word);
			++not_find;
		if (-1 == word_id){
			return (-1 * not_find);
		}
		else{
			word_ids.push_back(word_id);
		}

		word = strtok(NULL, " ");
	}

	//printf(there are %d words not find,not_find);

	if (NULL != text_tmp) delete[] text_tmp;
	return flag;
}

//以下是完成将一句话的解码信息赋值给结构体的相关操作
//hubo
void fill_phone(Phone_Info &phone,std::string phone_name , int start , int end ,BaseFloat conf)
{
	phone.phone_name = phone_name;
	phone.start = start;
	phone.end = end;
	phone.ph_conf = conf;
}
void add_phone_word(Word_Info &word, Phone_Info &phone)
{
	if(word.phone_link.size() == 0)
	{
		word.start = phone.start;
	}
	word.end = phone.end;
	++word.n_phone;
	word.wd_conf += phone.ph_conf;
	word.phone_link.push_back(phone);
}

void add_name_word(Word_Info &word, std::string word_name)
{
	word.word_name = word_name;
}

void add_word_sent(Sent_Info &sent,Word_Info &word)
{
	++sent.n_word;
	word.wd_conf /= word.n_phone;
	sent.word_link.push_back(word);
	word.start = 0;
	word.end = 0;
	word.n_phone = 0;
	word.wd_conf = 0;
	word.phone_link.clear();
}

//将对齐后结果赋值给Sent_Info对象
//返回音素的总帧数
int pachira_score::make_align_sent(Sent_Info &sentence , std::vector<PhoneConfidence> &phone_confs ,std::vector<kaldi::int32> &words)
{
	//add word link
	Phone_Info phone;
	Word_Info word;
	int phone_frame_nu = 0;
	int flags=0;//1表示单词中，0表示这个单词结束
	for (std::vector<PhoneConfidence>::const_iterator i = phone_confs.begin(); i != phone_confs.end(); ++i)
	{
		if (phone_syms)
		{
			//according to phone obtain phone name.
			std::string phone_name = phone_syms->Find(i->phone_no);

			//if not silence
			if (phone_name.compare("SIL"))
			{
				//获得phone信息
				fill_phone(phone,phone_name,i->phone_start,i->phone_end,i->conf);
				phone_frame_nu += (i->phone_end - i->phone_start);
				switch(phone_name.at(phone_name.length() - 1))
				{
					case 'B':
						if(flags == 0)
						{
							add_phone_word(word,phone);
							flags = 1;
						}
						else
						{
							PACHIRA_DEBUG << "decode error!\n" ;
						}
						break;
					case 'I':
						if(flags == 1)
						{
							add_phone_word(word,phone);
						}
						else
						{
							PACHIRA_DEBUG << "decode error!\n" ;
						}
						break;
					case 'E':
						if(flags == 1)
						{
							add_phone_word(word,phone);
							add_name_word(word , word_syms->Find(words[sentence.n_word]));
							add_word_sent(sentence , word);
							flags = 0;
						}
						else
						{
							PACHIRA_DEBUG << "decode error!\n" ;
						}
						break;
					case 'S':
						if( flags == 0)
						{
							add_phone_word(word,phone);
							add_name_word(word , word_syms->Find(words[sentence.n_word]));
							add_word_sent(sentence , word);
						}
						else
						{
							PACHIRA_DEBUG << "decode error!\n" ;
						}
						break;
					default:
						PACHIRA_DEBUG << "decoder error!\n";
						break;
				}//switch
			}//if
		}//if
	}//for
	//end word link
	if (words.size() != sentence.n_word || word_ids.size() != words.size())
	{
		PACHIRA_DEBUG << __FILE__ << " " << __LINE__ << " " << "identification error!" ;
		return -1;
	}
	return phone_frame_nu;
}

//针对整句打分
//frame_array:帧统计数据
//n:数据长度
//all_frame: 语音所有帧数
//phone_frame_nu:语音有效帧数
//返回整句话得分
int sent_score(int *frame_array,int n,int all_frame,int phone_frame_nu,int *g_mlp, int *s_mlp)
{
	// accord feature score mlp
	// conf>1e-1 1e-2 1e-3 1e-4 percent and effective frame
	if(n != EXP_LEN)
	{
		return -1;
	}
	ostringstream feature_out;
	feature_out << frame_array[0] * 1.0 / phone_frame_nu << " " << frame_array[1] * 1.0 / phone_frame_nu << " "
		<< frame_array[2] * 1.0 / phone_frame_nu << " " << frame_array[3] * 1.0 / phone_frame_nu << " "
		<< frame_array[4] * 1.0 / phone_frame_nu << " " << frame_array[5] * 1.0 / phone_frame_nu << " "
		<< frame_array[6] * 1.0 / phone_frame_nu << " " << frame_array[7] * 1.0 / phone_frame_nu << " "
		<< phone_frame_nu*1.0 / all_frame;
	/*now mlp feature*/
	/*
#ifdef MLP_DATA_TEST
	FILE *fp_old=NULL;
	fp_old=fopen("now_mlp_fea","a");
	if(fp_old==NULL)
	return -1;
	fprintf(fp_old,"%s   %s   %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f\n",
	stand_tem_in.c_str(),
	template_file,
	frame_array[0]*1.0/phone_frame_nu,
	frame_array[1]*1.0/phone_frame_nu,
	frame_array[2]*1.0/phone_frame_nu,
	frame_array[3]*1.0/phone_frame_nu,
	frame_array[4]*1.0/phone_frame_nu,
	frame_array[5]*1.0/phone_frame_nu,
	frame_array[6]*1.0/phone_frame_nu,
	frame_array[7]*1.0/phone_frame_nu,
	phone_frame_nu*1.0/all_frame);
	fclose(fp_old);
#endif
	*/
	/*mlp feature*/
	/*
	FILE *fp_mlp=NULL;
	fp_mlp=fopen("new_mlp_fea","a");
	if(fp_mlp==NULL)
	return -1;
	fprintf(fp_mlp,"%8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f\n",
	frame_array[0]*1.0/all_frame,
	(frame_array[1]-frame_array[2])*1.0/all_frame,
	(frame_array[2]-frame_array[3])*1.0/all_frame,
	(frame_array[3]-frame_array[4])*1.0/all_frame,
	(frame_array[4]-frame_array[5])*1.0/all_frame,
	(frame_array[5]-frame_array[6])*1.0/all_frame,
	(frame_array[6]-frame_array[7])*1.0/all_frame,
	(frame_array[7]-frame_array[8])*1.0/all_frame,
	(frame_array[8]-frame_array[9])*1.0/all_frame,
	(frame_array[9]-frame_array[10])*1.0/all_frame);
	fclose(fp_mlp);
	*/
#ifdef IOS
	char *global_mlp = (char*)"./global.mlp";
	char *standard_mlp = (char *) "./standard.mlp";
#elif __ANDROID_API__
	char *global_mlp = "/mnt/sdcard/Pince/model/global.mlp";
	char *standard_mlp = "/mnt/sdcard/Pince/model/standard.mlp";
#elif __unix__
	char *global_mlp = "model/global.mlp";
	char *standard_mlp = "model/standard.mlp";
#elif WP8
	char *global_mlp = "pachira/model/global.mlp";
	char *standard_mlp = "pachira/model/standard.mlp";
#endif
	//首先判断是不是这句话
	*g_mlp = mlp_score((char*)feature_out.str().c_str(),global_mlp,1.0);
	//标准打分是多少
	*s_mlp = mlp_score((char *)feature_out.str().c_str(),standard_mlp,5.0);
	*g_mlp = (*g_mlp > 0 ? *g_mlp : 0);
	*s_mlp = (*s_mlp > 0 ? *s_mlp : 0);
	return 0;
}
float only_word_score(float conf)
{
	float score=0.0;
	if(conf < 0.02)
		score = 2500 * conf;
	else if(conf < 0.03)
		score = 1000.0 * conf + 30;
	else if(conf < 0.1)
		score = 3000.0/7.0 * conf + 330.0 / 7;
	else if(conf < 0.3)
		score = 25.0 * conf + 87.5 ;
	else
		score = 50.0 / 7.0 * conf + 650.0 / 7.0;
	if(score > 100)
		score = 100;
	if(score < 0)
		score = 0;
	return score;
}
//为每一个单词打分，根据单词的置信度
//sentence:保存单词信息的对象
//score:保存单词打分的结果，由外部申请空间
//n:单词的个数
//g_score:>=50表示文本与音频对应，<50表示音频与文本不对应
//s_score:标准英语打分
//返回单词个数
int word_score(Sent_Info sentence,int *score,int n,int *g_score, int *s_score)
{
	if(sentence.n_word == 0 || sentence.n_word != n)
	{
		PACHIRA_DEBUG << "save result error!\n";
		return -1;
	}
	int i=0;
	int average_word=0;
	for(i=0 ; i < sentence.n_word ; ++i)
	{
		score[i]=(int) only_word_score(sentence.word_link[i].wd_conf);
		average_word += score[i];
#ifdef TEST_WORD_SORE
		printf("%d ",score[i]);
		//printf("%7.5e \n",sentence.word_link[i].wd_conf);
#endif
	}
#ifdef TEST_WORD_SORE
	printf("\n");
	fprintf(stderr,"average=%d g_score=%d s_score=%d\n",
			average_word,*g_score,*s_score);
#endif
	average_word /= sentence.n_word;
	//如果单词个数为1个的时候打分用下面
	if(sentence.n_word == 1)
	{//取单词打分，g_score，s_score中最大的2个打分
		int first_max=0,second_max=0;
		if( *g_score > *s_score )
		{
			if(*s_score > average_word)
			{
				first_max = *g_score;
				second_max = *s_score;
			}
			else if(*g_score > average_word)
			{
				first_max = *g_score;
				second_max = average_word;
			}
			else
			{
				first_max = average_word;
				second_max = *g_score;
			}
		}
		else
		{
			if(*g_score > average_word)
			{
				first_max = *s_score;
				second_max = *g_score;
			}
			else if(*s_score > average_word)
			{
				first_max = *s_score;
				second_max = average_word;
			}
			else
			{
				first_max = average_word;
				second_max = *s_score;
			}
		}
		*g_score = (first_max * 0.8 + second_max * 0.2);
		*s_score = *g_score;
		score[0] = *g_score;
		return sentence.n_word;
	}
	//如果单词个数不为1个，修正单词打分和整句话打分
	if(*g_score >= 50 && average_word > 20)// 认为文本与音频对应，这样提高单词打分
	{
		if(average_word < (*s_score+*g_score)/2)//认为单词打分过低，进行提升
		{
			for(i=0 ; i < sentence.n_word ; ++i)
			{
				//为了保证与整体打分相一致
				score[i] = 0.2 * score[i] + (*s_score * S_SCORE_RATE + *g_score * G_SCORE_RATE) * 0.8;
			}
		}
	}
	else if(average_word <= 20 && *g_score >=50)//认为文本与音频不对应，这样降低整句话打分
	{
		for(i=0 ; i < sentence.n_word ; ++i)
		{
			score[i] = 0.5 * score[i] + (*s_score * S_SCORE_RATE + *g_score * G_SCORE_RATE) * 0.5;
		}
		*g_score = (average_word + *g_score)/2;
		*s_score = (average_word + *s_score)/2;
	}
	//printf("\n");
	return i;
}

/*
 * 输出对齐模板
 * */

#ifdef WP8
bool pachira_score::out_put_temp(const char *template_file, std::vector<int32> &words,
	std::vector<PhoneConfidence> &phone_confs, int *frame_array, const Matrix<BaseFloat>*features,int *score)
#else
bool pachira_score::out_put_temp(const char *template_file, std::vector<kaldi::int32> &words, 
	std::vector<PhoneConfidence> &phone_confs,int *frame_array,const Matrix<BaseFloat>*features,int *score )
#endif
{
	if (NULL == template_file)
	{
		return false;
	};
	std::fstream result_out(template_file, std::fstream::out);
	if (result_out.fail())
	{
		return false;
	};
	result_out << "# words:\t";
	for (std::vector<int>::iterator m = words.begin(); m != words.end(); m++)
	{
		result_out << *m << "\t";
	}
	result_out << "\n";

	result_out << "# correspond word:\t";
	for (std::vector<int>::iterator m = words.begin(); m != words.end(); m++)
	{
		result_out << word_syms->Find(*m) << "\t";
	}
	result_out << "\n";

	int word_n = 0;
	//to colllect the feature for mlp_score
	int all_frame = features->NumRows();
	int sil_ports = 0, B15 = 0, B25 = 0, phone_frame_nu = 0;
	for (std::vector<PhoneConfidence>::const_iterator i = phone_confs.begin(); i != phone_confs.end(); ++i)
	{
		if (phone_syms){
			//according to phone obtain phone name.
			std::string phone_name = phone_syms->Find(i->phone_no);

			//output << fbank_vec;
			//if not silence
			if (phone_name.compare("SIL")){
				result_out << phone_name.at(phone_name.length() - 1) << "\t" << phone_name << "\t"
					<< i->phone_start << "\t" << i->phone_end << "\t" << i->conf;

				//for record word number
				if (phone_name.at(phone_name.length() - 1) == 'B')
					++word_n;
				if (phone_name.at(phone_name.length() - 1) == 'S')
					++word_n;

				//output << fbank_vec;
				BaseFloat fbank_vec[feature_vec_len] = { 0 };
				//deal per row, do well in I/O
				int phone_idx = i->phone_start;
				//calculate fbanks cumulative sum
				//here should bu < not <=
				while (phone_idx < i->phone_end)
				{
					int feat_idx = 0;

					while (feat_idx < feature_vec_len)
					{
						fbank_vec[feat_idx] += (*features)(phone_idx, feat_idx);
						++feat_idx;
					}
					++phone_idx;
				}

				int count = i->phone_end - i->phone_start;
				int feat_idx = 0;
				while (feat_idx < feature_vec_len)
				{
					fbank_vec[feat_idx] /= count;
					result_out << "\t" << fbank_vec[feat_idx];
					++feat_idx;
				}
				result_out << "\n";

				if (count > 15) B15 += count;
				if (count > 25) B25 += count;
				phone_frame_nu += count;
			}
			else if (phone_confs.begin() == i || (phone_confs.end() - 1) == i){
				sil_ports += i->phone_end - i->phone_start + 1;
			}
		}
	}
	if (words.size() != word_n)
	{
		//fprintf(stderr, "%s %d identifcation error!\n", __FILE__, __LINE__);
		PACHIRA_DEBUG << __FILE__ << " " << __LINE__ << " " << "identification error!" << endl;
		return false;
	}

	// accord feature score mlp
	// conf>1e-1 1e-2 1e-3 1e-4 percent and effective frame
	ostringstream feature_out;
	feature_out << frame_array[0] * 1.0 / phone_frame_nu << " " << frame_array[1] * 1.0 / phone_frame_nu << " "
		<< frame_array[2] * 1.0 / phone_frame_nu << " " << frame_array[3] * 1.0 / phone_frame_nu << " "
		<< frame_array[4] * 1.0 / phone_frame_nu << " " << frame_array[5] * 1.0 / phone_frame_nu << " "
		<< frame_array[6] * 1.0 / phone_frame_nu << " " << frame_array[7] * 1.0 / phone_frame_nu << " "
		<< phone_frame_nu*1.0 / all_frame;
	/*now mlp feature*/
//	/*
#ifdef MLP_DATA_TEST
	FILE *fp_old=NULL;
	fp_old=fopen("now_mlp_fea","a");
	if(fp_old==NULL)
	return -1;
	fprintf(fp_old,"%s   %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f\n",
	template_file,
	frame_array[0]*1.0/phone_frame_nu,
	frame_array[1]*1.0/phone_frame_nu,
	frame_array[2]*1.0/phone_frame_nu,
	frame_array[3]*1.0/phone_frame_nu,
	frame_array[4]*1.0/phone_frame_nu,
	frame_array[5]*1.0/phone_frame_nu,
	frame_array[6]*1.0/phone_frame_nu,
	frame_array[7]*1.0/phone_frame_nu,
	phone_frame_nu*1.0/all_frame);
	fclose(fp_old);
#endif
//	*/
	/*mlp feature*/
	/*
	FILE *fp_mlp=NULL;
	fp_mlp=fopen("new_mlp_fea","a");
	if(fp_mlp==NULL)
	return -1;
	fprintf(fp_mlp,"%8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f\n",
	frame_array[0]*1.0/all_frame,
	(frame_array[1]-frame_array[2])*1.0/all_frame,
	(frame_array[2]-frame_array[3])*1.0/all_frame,
	(frame_array[3]-frame_array[4])*1.0/all_frame,
	(frame_array[4]-frame_array[5])*1.0/all_frame,
	(frame_array[5]-frame_array[6])*1.0/all_frame,
	(frame_array[6]-frame_array[7])*1.0/all_frame,
	(frame_array[7]-frame_array[8])*1.0/all_frame,
	(frame_array[8]-frame_array[9])*1.0/all_frame,
	(frame_array[9]-frame_array[10])*1.0/all_frame);
	fclose(fp_mlp);
	*/
#ifdef __MLP_FEATURE__
	std::cout << audio_file<< "\nfeature:" <<  feature_out.str() <<"\n";
#endif

#ifdef IOS
	char *class_mlp = (char*)"./class.mlp";
#elif __ANDROID_API__
	char *class_mlp = "/mnt/sdcard/Pince/model/class.mlp";
#elif __unix__
	char *class_mlp = "model/global.mlp";
#elif WP8
	char *class_mlp = "pachira/model/class.mlp";
#endif

	int h_mlp = mlp_score((char*)feature_out.str().c_str(),class_mlp,1.0);
	h_mlp = (h_mlp > 0 ? h_mlp : 0);
	*score = h_mlp;
	result_out << "G\tg_score\t" << h_mlp;
	return true;
}



struct WAV_HDR
{
	char rID[4];				// 'RIFF'
	int rLen;
	char wID[4];				// 'WAVE'
	char fId[4];				// 'fmt '
	int pcm_header_len;		// varies...
	int16_t wFormatTag;
	int16_t nChannels;		// 1,2 for stereo data is (l,r) pairs
	int nSamplesPerSec;        // 采样率
	int nAvgBytesPerSec;
	int16_t nBlockAlign;
	int16_t nBitsPerSample;    // 码率
};
struct CHUNK_HDR
{
	char dId[4];				 // 'data' or 'fact'
	int dLen;
};

int  pachira_score::get_wave_data(const char *audio_file, char **data, int &size)
{
	int flag = Parchira_OK;

	FILE *f = fopen(audio_file, "rb");
	WAV_HDR *wav = NULL;
	CHUNK_HDR *chk = NULL;
	do{
		if (f == NULL)
		{
			PACHIRA_DEBUG << "open " << audio_file << "%s error\n";
			flag = Parchira_READ_WAV_FAIL;
		}

		char obuff[20];
		wav = (WAV_HDR*)malloc(sizeof(WAV_HDR));
		chk = (CHUNK_HDR *)malloc(sizeof(CHUNK_HDR));

		//read wave header
		if (fread((void *)wav, sizeof(WAV_HDR), 1, f) != 1)
		{
			PACHIRA_DEBUG << "cant read wav\n";
			flag = Parchira_READ_WAV_FAIL;
			break;
		}

		int i = 0;
		for (i = 0; i < 4; i++)
		{
			obuff[i] = wav->rID[i];
		}
		obuff[4] = 0;
		if (strcmp(obuff, "RIFF") != 0){
			fseek(f, 0, SEEK_END);
			int pcmlen = ftell(f);
			char *pcmdata = (char*)malloc(sizeof(char)*pcmlen);
			fseek(f, 0, SEEK_SET);
			if (fread((void *)pcmdata, pcmlen, (size_t)1, f) != 1)
			{
				PACHIRA_DEBUG << "cant read pcm data\n";
				flag = Parchira_READ_WAV_FAIL;
				break;
			}
			*data = pcmdata;

			size = pcmlen;
			break;
		}

		for (i = 0; i < 4; i++)
		{
			obuff[i] = wav->wID[i];
		}
		obuff[4] = 0;
		if (strcmp(obuff, "WAVE") != 0)
		{
			PACHIRA_DEBUG << "%s bad WAVE format\n";
			flag = Parchira_READ_WAV_FAIL;
			break;
		}

		for (i = 0; i < 3; i++)
		{
			obuff[i] = wav->fId[i];
		}
		obuff[3] = 0;
		if (strcmp(obuff, "fmt") != 0 || wav->wFormatTag != 1 || wav->nBitsPerSample != 16)
		{
			PACHIRA_DEBUG << "%s bad WAVE format\n";
			flag = Parchira_READ_WAV_FAIL;
			break;
		}


		if (wav->pcm_header_len == 18)
		{
			fseek(f, 2, SEEK_CUR);
		}

		// read chunks until a 'data' chunk is found
		int sflag = 1;
		while (sflag != 0)
		{
			// check attempts
			if (sflag > 10)
			{
				PACHIRA_DEBUG << "too many chunks\n";
				flag = Parchira_READ_WAV_FAIL;
				break;
			}

			if (1 != fread((void *)chk, sizeof(CHUNK_HDR), (size_t)1, f))
			{
				PACHIRA_DEBUG << "cant read chunk\n";
			}

			// check chunk type
			for (i = 0; i < 4; i++)
			{
				obuff[i] = chk->dId[i];
			}
			obuff[4] = 0;
			if (strcmp(obuff, "data") == 0)
			{
				break;
			}

			// skip over chunk
			sflag++;
			fseek(f, chk->dLen, SEEK_CUR);

		}
		if (Parchira_OK != flag)
		{
			break;
		}


		// find length of remaining data
		int wbuff_len = chk->dLen;
		char * wbuff = (char*)malloc(wbuff_len);
		if (wbuff == NULL)
		{
			PACHIRA_DEBUG << "malloc error\n";
			flag = Parchira_READ_WAV_FAIL;
			break;
		}

		size = chk->dLen;
		//read signal data
		if (fread((void *)wbuff, sizeof(char), wbuff_len, f) != wbuff_len)
		{
			PACHIRA_DEBUG << "read error\n";
			flag = Parchira_READ_WAV_FAIL;
			break;
		}

		// set returned data
		*data = wbuff;
	} while (0);


	if (wav) free(wav);
	if (chk)free(chk);
	if (f) fclose(f);
	return flag;
}

/*-------------------------where for stream------------------------------------*/
/**the functions to deal with stream state*/

/*
 * 用text初始化解码路径保存在pachira_score类变量中
 * */
int pachira_score::set_steam_conf(const char *text)
{
	int flag = score_state;
	clean_stream_state();//avoid the error in last call

	do{
		if (Parchira_OK != flag && Parchira_STREAM_CONF_ERROR != flag)
		{
			PACHIRA_DEBUG << "the conf is error, fail to init.\n";
			break;
		}
		flag = Parchira_OK;

		if (NULL == text || Parchira_OK != (flag = get_trans_text( text)))
		{
			PACHIRA_DEBUG << "text can't be NULL\n";
			return flag;
		//	break;
		}

		if (NULL == gc || !gc->CompileGraphFromText(word_ids, &decode_fst))
		{
			PACHIRA_DEBUG << "Problem creating decoding graph for utterance\n";
			score_state = Parchira_STREAM_CONF_ERROR;
			break;
		}

		if (NULL == decoder){
			decoder = new StreamHCLGDecoder(decoder_opts, *decodable, &decode_fst, word_syms);
		}
		else{
			if (!decoder->ChangeFst(&decode_fst))
			{
				score_state = Parchira_STREAM_CONF_ERROR;
				break;
			}
		}

		wav_data = (char*)malloc(batch_size_init * 3 * sizeof(char));
		if (NULL == wav_data)
		{
			score_state = Parchira_STREAM_CONF_ERROR;
			break;
		}
		buf_len = batch_size_init * 3;
	} while (0);
	return -1 * score_state;
}



int pachira_score::send_wav(const char* wav_buf, int wav_len, int end)
{
	int flag = score_state;
	do{
		if (Parchira_OK != score_state)
		{
			break;
		}

		if (stream_id < 0)
		{
			flag = Parchira_STREAM_END;
			break;
		}

		int wav_size = 0;


		if ((buf_end + wav_len) >= buf_len)
		{
			char *temp = NULL;
			if (batch_size_init > wav_len)
			{
				//modify
				temp = (char*)realloc(wav_data, (buf_len + batch_size_init)*sizeof(char));
				buf_len += batch_size_init;
			}
			else
			{
				temp = (char*)realloc(wav_data, (buf_len + wav_len)*sizeof(char));
				buf_len += wav_len;
			}
			if (NULL == temp)
			{
				flag = Parchira_RUN_ERROR;
				break;
			}
			wav_data = temp;
		}
		memcpy(wav_data + buf_end, wav_buf, wav_len);
		buf_end += wav_len;
#ifdef AUDIO_TEST
		if(end == 1)
		{
			FILE *fp_11 = NULL;
#ifdef __ANDROID_API__
			fp_11=fopen("/mnt/sdcard/Pince/temp.pcm","w");
#else
			fp_11=fopen("./temp.pcm","w");
#endif
			fwrite(wav_data,sizeof(char),buf_end,fp_11);
			fclose(fp_11);
		}
#endif
		switch (end)
		{
		case 0:
			if (buf_start == 0 && (buf_end - buf_start) < (12 * 80 * 2))
			{
				return 0;
			}
			if (buf_end - buf_start >= batch_size)
			{
				decoder->Decode(wav_data + buf_start, buf_end - buf_start, ++stream_id);
				buf_start = buf_end;
			}
			break;
		case 1:
			if (buf_start == 0 && (buf_end - buf_start) < (12 * 80 * 2))
			{
				//fprintf(stderr, "first can't introduction < 12 frame=12*80*2*sizeof(char)\n");
				PACHIRA_DEBUG << "first can't introduction < 12 frame=12*80*2*sizeof(char)" << endl;
				return -1;
			}
			decoder->Decode(wav_data + buf_start, buf_end - buf_start, (++stream_id) *= -1);
			//if(wav_len=0)
			//output the result(template)
			/*
			{
				std::vector<kaldi::int32> words;
				std::vector<PhoneConfidence> phone_confs;
				int frame_array[EXP_LEN] = { 0 };
				Matrix<BaseFloat> *fbank;
				if (!decoder->GetResult(words, phone_confs, frame_array, &fbank))
				{
					//fprintf(stdout, "get result error!\n");
					PACHIRA_DEBUG << "get result error!" << endl;
					flag = Parchira_RUN_ERROR;
				}

				if (true != out_put_temp("abc.fea", words, phone_confs, frame_array, fbank))
				{
					//fprintf(stderr, "out_put_temp wrong!\n");
					PACHIRA_DEBUG << "out_out_temp wrong" << endl;
					decoder->Reset();
					return -1;
				}
				decoder->Reset();
			}
			*/
			break;
		default:
			flag = Parchira_STREAM_FLAG_ERROR;
		}//switch
	} while (0);
	return -1 * flag;
}


#ifdef IOS
bool pachira_score::get_ios_home_dir(char *home)
{
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	char path[1024];
	if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, 1024))
	{
		return false;
	}
	CFRelease(resourcesURL);


	size_t len = strlen((char*)path);
	if (len == 0) return false;

	size_t location = len - 1;
	for (; location >= 0; --location)
	{
		if ('/' == path[location]) break;
	}

	bzero(home, strlen(home));
	memcpy(home, path, location+1);
	home[location + 1] = '\0';
	strncat(home, "Documents", sizeof("Documents"));

	return true;

}
#endif

//destruct
pachira_score::~pachira_score()
{
	//free memory
	if (phone_syms) delete phone_syms;
	if (word_syms) delete word_syms;
	if (gc) delete gc;
	if (decoder) delete decoder;
	if (decodable) delete decodable;
	//if (wav_data) delete[] wav_data;
	if (wav_data) free(wav_data);
}

/*-----------------stream--------------------------*/

int sys_init()
{
	if ( 0 != pachira_score::get_single().get_state())
	{
		fprintf(stderr,"system init error!\n");
		return -1;
	}
	return 0;
}
int score_init(char *text)
{
	time_t rawtime;
	time(&rawtime);
	struct tm *timeinfo = localtime(&rawtime);
	//printf("%d month:%d day:%d\n",timeinfo->tm_year,timeinfo->tm_mon+1,timeinfo->tm_mday);
	if (timeinfo->tm_year > 114 || 12 < (timeinfo->tm_mon + 1))
	{
	//	if(20<=timeinfo->tm_mday)
		return -5;
	}
	int word_judge=0;
	//如果文本中有词典中没有的单词，就返回单词下标从-1开始，一次-2 ，-3 ... 
	//返回1的话text没有单词
	//正确返回0
	word_judge = pachira_score::get_single().set_steam_conf(text);
	if (text == NULL ||0 != word_judge)
	{
		return word_judge;
	}
	return 0;
}

int send_wav(char* wav_data, int wav_len, int flag)
{
	if (NULL == wav_data)
	{
		return -1;
	}
	if (0 > pachira_score::get_single().send_wav(wav_data, wav_len, flag))
	{
		return -1;
	}
	return 0;
}
int pachira_score::score_get(int **w_score,int *n)
{
	int flag ;
	int score = 0;//用于保存最后整句话的标准打分结果
	int g_score=0;//保存全局打分，为了判断文本与音频是否相同
	int s_score=0;//保存标准打分，为了给音频打出标准分数
	{
#ifdef WP8
		std::vector<int32> words;
#else
		std::vector<kaldi::int32> words;
#endif
		std::vector<PhoneConfidence> phone_confs;
		int frame_array[EXP_LEN] = { 0 };
		Matrix<BaseFloat> *fbank;
		if (!decoder->GetResult(words, phone_confs, frame_array, &fbank))
		{
			//fprintf(stdout, "get result error!\n");
			PACHIRA_DEBUG << "get result error!" << endl;
			flag = Parchira_RUN_ERROR;
		}
		Sent_Info sentence;
		//将解码的对齐结果保存到sentence对象中
		int phone_frame_nu=0;
		if((phone_frame_nu = make_align_sent(sentence , phone_confs ,words)) < 0)
		{
			decoder->Reset();
			return -1;
		}
		if(sentence.n_word == 0)
		{
			PACHIRA_DEBUG << "after decoder word number is 0 !\n" ;
			decoder->Reset();
			return -1;
		}
		//先对整句话打分，然后更具整句话打分对单词打分。
		int all_frame = fbank->NumRows();
		if(sent_score(frame_array,EXP_LEN,all_frame,phone_frame_nu,&g_score, &s_score) < 0 )
		{
			PACHIRA_DEBUG << "sent_score error!\n" ;
			return -1;
		}
		//给每个单词打分
		(*w_score) = (int *)malloc(sizeof(int) * sentence.n_word);
		if(w_score == NULL)
		{
			decoder->Reset();
			return -1;
		}
		//对单词打分
		*n = sentence.n_word;
		if(sentence.n_word != word_score(sentence,*w_score,sentence.n_word,&g_score , &s_score))
		{
			PACHIRA_DEBUG << "word_score error!\n" ;
			decoder->Reset();
			return -1;
		}
		//整句话打分
		score = g_score * G_SCORE_RATE + s_score * S_SCORE_RATE;
		if( *n == 1)
		{
			score = score > (*w_score)[0] ? score : (*w_score)[0];
			(*w_score)[0] = score ;
		}
		//free(w_score);
#ifdef MLP_FEA_TEST
		int score_g=0;
		if (true != out_put_temp("a.fea", words, phone_confs, frame_array, fbank,&score_g ))
		{
			//fprintf(stderr, "out_put_temp wrong!\n");
			PACHIRA_DEBUG << "out temp wrong" << endl;
			decoder->Reset();
			return -1;
		}
#endif
		decoder->Reset();
	}
	return score;
}
int score_get(int **w_score, int *n)
{
	int score=0;
	if((score = pachira_score::get_single().score_get(w_score,n)) < 0)
	{
		PACHIRA_DEBUG << "score get error!" << endl;
		return -1;
	}
	return score;
}

