#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "pachira_score.h"
#include <assert.h>
#include<sys/time.h>
#include <unistd.h>
#define FILE_LONG 1024


// header of wav file
typedef struct
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
}WAV_HDR;

typedef struct
{
	char dId[4];				 // 'data' or 'fact'
	int dLen;
}CHUNK_HDR;

int cut_str(char *src,char op ,int n , char **const dest)
{
	int i=0;
	n=0;
	int start=0;
	for(i=0;src[i] != '\0';++i)
	{
		if(src[i] != op)
			continue;
		strncpy(dest[n],src+start,i-start);
		n++;
		start=i+1;
		dest[n][i-start+1]='\0';
	}
	strncpy(dest[n],src+start,i-start);
	dest[n][i-start-1]='\0';
	return n;
}




int comp_test(char *temp,char *file,char *match);
int main(int argc,char *argv[])
{
	if(argc<2)
	{
		printf("create_template file\n");
		return -1;
	}
	FILE *fp;
	if(NULL==(fp=fopen(argv[1],"r")))
	{
		printf("fopen argv[1] wrong!\n");
		return -1;
	}

	char test[FILE_LONG];
	char temp[3][FILE_LONG];
	char *aaa[3];
	aaa[0]=temp[0];
	aaa[1]=temp[1];
	aaa[2]=temp[2];
	//add sys_init function
//	printf("start init\n");
	if(0 !=sys_init())
	{
		fprintf(stderr,"init error!\n");
		return -1;
	}
//	printf("init system OK!\n");
//	printf("start match\n");
	while(1)
	{
		memset(test,0x00,FILE_LONG*sizeof(char));
		memset(temp,0x00,FILE_LONG*sizeof(char) * 3);
		if(fgets(test,FILE_LONG,fp)==NULL)
		{
			break;
		}
		if(test[0]=='#')
			continue;
	//	sscanf(test,"%s %s %s",temp,file,match);
		cut_str(test,'|',3,aaa);
	//	printf("%s %s\n",temp,file);
	//
#define VAD
#ifdef VAD
		char vad_wav[FILE_LONG]="/work7/tangzy/github/auto_scoring_20170726/auto_scoring/Cuckoo/bin/vad.ark";
	//	char *abc="Today I want to tell you three stories from my life";
		strcat(vad_wav,temp[0]);
		printf(vad_wav);

		strcat(vad_wav," temp.wav");
		printf(vad_wav);

		system(vad_wav);
		fprintf(stderr,"%s| %s |%s   ",temp[0],temp[1],temp[2]);
		printf(temp[0]);
		printf(temp[1]);
		printf(temp[2]);

		printf("vad\n");
		if(comp_test("temp.wav",temp[1],temp[2])!=0)
#else
		printf("no vad\n");
		if(comp_test(temp[0],temp[1],temp[2])!=0)
#endif
		{
			printf("test_wrong!\n");
			//continue;
		}
		char fea[1024];
		memset(fea,0x00,sizeof(char)*1024);
//		strncpy(fea,file,strlen(file)-3);
//		strcat(fea,"fea");
//		printf("%s\n",fea);
		char buffer[512];
		sprintf(buffer,"mv a.fea %s",temp[2]);
		int ret = system(buffer);
		if(ret!=0)
			perror("system error!\n");
		//if(WIFSIGNALED(ret) && (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT))
		//	printf("system error!\n");
		//sleep(3);
	}
	return 0;
}


int comp_test(char *file,char *text,char *temp)
{
//	score_res result;
	{
		int re = score_init(text);
		if(0 != re)
		{
			fprintf(stderr,"%d word error!\n",re);
			return -1;
		}
	}
		
	FILE *f = fopen(file, "rb");
	if (f == NULL) {
		fprintf(stderr, "open %s error %d\n",file,__LINE__);
		return -1;
	}

	char obuff[20];
	WAV_HDR *wav = (WAV_HDR*)malloc(sizeof(WAV_HDR));
	CHUNK_HDR *chk = (CHUNK_HDR *)malloc(sizeof(CHUNK_HDR));

	//read wave header
	if (fread((void *) wav, sizeof (WAV_HDR), 1, f) != 1){
		fprintf(stderr, "cant read wav\n");
		return -1;
	}
	int i = 0;
	for(i=0;i<4;i++) obuff[i] = wav->rID[i];
	obuff[4] = 0;
	if(strcmp(obuff,"RIFF")!=0){
		fseek(f,0,SEEK_END);
		int pcmlen = ftell(f);
		char *pcmdata = (char*)malloc(sizeof(char)*pcmlen);
		fseek(f,0,SEEK_SET);
		if (fread((void *)pcmdata,pcmlen,(size_t)1,f) != 1){
			fprintf(stderr, "cant read pcm data\n");
			exit(-1);
		}

		free(wav);
		free(chk);
		fclose(f);

		return pcmlen / 2 ;		
	}
	//fprintf(stderr, "%s bad RIFF format\n", obuff); exit(-1); }

	for(i=0;i<4;i++) obuff[i] = wav->wID[i];
	obuff[4] = 0;
	if(strcmp(obuff,"WAVE")!=0){ fprintf(stderr, "%s bad WAVE format\n", obuff); exit(-1); }

	for(i=0;i<3;i++) obuff[i] = wav->fId[i];
	obuff[3] = 0;
	if(strcmp(obuff,"fmt")!=0){ fprintf(stderr, "%s bad fmt format\n", obuff); exit(-1); }
	if(wav->wFormatTag!=1){ fprintf(stderr, "bad wav wFormatTag\n"); exit(-1); }
	if(wav->nBitsPerSample != 16){
		fprintf(stderr, "bad wav nBitsPerSample\n");
		return -1;
	}


	if(wav->pcm_header_len == 18){
		fseek(f,2,SEEK_CUR);
	}

	// read chunks until a 'data' chunk is found
	int sflag = 1;
	while(sflag!=0){
		// check attempts
		if(sflag>10){
			fprintf(stderr, "too many chunks\n");
			return -1;
		}
		// read chunk header
		int ret = 0;
		if (  (ret = fread((void *)chk,sizeof(CHUNK_HDR),(size_t)1,f)) != 1){
			fprintf(stderr, "cant read chunk-%u.Readen:%d,Flags:%d\n",(unsigned int)sizeof(CHUNK_HDR),ret,sflag);
			return -1;
		}

		// check chunk type
		for(i=0;i<4;i++)
			obuff[i] = chk->dId[i];
		obuff[4] = 0;
		if(strcmp(obuff,"data")==0) break;

		// skip over chunk
		sflag++;
		fseek(f,chk->dLen,SEEK_CUR);

	}


	// find length of remaining data
	int wbuff_len = chk->dLen;
	char * wbuff = (char*)malloc(wbuff_len);
	int now_read_len=0,read_len=0;
	
	if(wbuff==NULL){ fprintf(stderr, "cant alloc\n"); exit(-1); }
	//read signal data
	int i_h=0;
	int len_r=4000;
	while (now_read_len+len_r<wbuff_len)
	{
		memset(wbuff,0x00,wbuff_len);
		read_len = fread((void *)wbuff,sizeof(char),len_r,f);
		now_read_len += read_len;
		//printf("%d: %d \n",i_h++,read_len);
		if(0 != send_wav(wbuff,read_len,0))
		{
			free(wbuff);
			free(wav);
			free(chk);
			fclose(f);
			return 0;
		}
	}
	memset(wbuff,0x00,wbuff_len);
	read_len = fread((void *)wbuff,sizeof(char),wbuff_len-now_read_len,f);
	struct timeval start,end;
	float timeuse;
		gettimeofday(&start,NULL);
	if(0!=send_wav(wbuff,read_len,1))
	{
		free(wbuff);
		free(wav);
		free(chk);
		fclose(f);
		return 0;
	}

	// set returned data
	free(wbuff);
	free(wav);
	free(chk);
	fclose(f);
	
	//assert(!score_get(&result));
	int score=0;
	int *w_score=NULL;
	int n;
	score=score_get(&w_score,&n);
	if(score < 0)
	{
		fprintf(stderr,"score_get error %d!\n",score);
		return -1;
	}
	//int i=0;
	int j=0,k=0;
	char word_arr[n][128];
	memset(word_arr,0x00,sizeof(char)*n*128);
	int start_w = 0;
	printf("%s\n",text);
	i=0;
	while(text[i])
	{
		if(text[i] == ' ' && start_w == 1)
		{
			start_w = 0;
			++j;
			k=0;
		}
		if(text[i] != ' ')
		{
			word_arr[j][k]=text[i];
			k++;
			start_w = 1;
		}
		++i;
	}
	for(i=0;i<n;++i)
	{
	//	printf("%d ",w_score[i]);
		fprintf(stdout,"%10s %2d\n",word_arr[i],w_score[i]);
	}
	printf("\n");
	free(w_score);
	

	gettimeofday(&end,NULL);
	timeuse=1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;
	timeuse/=1000000;
	printf("time=%6.3f\nglobal score %d\n",timeuse,score);
	return 0;
}
