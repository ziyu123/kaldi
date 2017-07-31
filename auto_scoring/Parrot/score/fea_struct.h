#define WORD_L  30//word leng
#define PHONE_L 20//phone length
#define FBANK_L 40

//using linked list statistics word and phone
typedef struct fea_m
{
	/*
	int n_fea;
	float *fea;
	*/
	float fea[FBANK_L];
}Fea;

#define F0_PHONE 3
#define HAR  20
typedef struct phone_m
{
	char phone_mark;
	char phone_L[PHONE_L];//音素名
	int start;
	int end;
	float conf;//confidence coefficient
	Fea fea_inf;
	float phone_f0[F0_PHONE];//每个音素被平均分为3部分，每部分一个基频
	float phone_peak_eng[HAR];//奇数是对数幅度，偶数是对应的频率
//	float phone_peak_f[4];
	struct phone_m *next;
}Phone_inf;

typedef struct word_m
{
	int phone_n;//phone number
	Phone_inf *phone_head;
	float word_f0;
	struct word_m *next;
}Word_inf;

typedef struct text_m
{
	int word_n;
	struct word_m *word_head;
}Text_inf;


/*save single word detail*/
typedef struct statistics
{
	char name[WORD_L];
	int serial;//start from 0,record how many word
	int phone_n;
	int frame_n;
	float conf;
	float fea_mean[FBANK_L];
}Sta_word;

/*save all word information*/
typedef struct word_sta
{
	int word_n;
	Sta_word *word;
}Sta_word_inf;


//
typedef struct dis_word
{
	int word_n;//word number
	int dim;//dimension
	float *dis;
}Dis_word;

