#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
//#include "fea_struct.h"
#include "link_list.h"
/*
 * process data in line
 * */
static char *fea_1;
static char *fea_2;

#define FY_LANG 127
const static char fuyin[FY_LANG][7]={	
	"SIL","SIL_B","SIL_E","SIL_I","SIL_S","SPN","SPN_B","SPN_E","SPN_I","SPN_S",
	"NSN","NSN_B","NSN_E","NSN_I","NSN_S","S_B","S_E","S_I","S_S","T_B","T_E",
	"T_I","T_S","N_B","N_E","N_I","N_S","K_B","K_E","K_I","K_S","Y_B","Y_E","Y_I","Y_S",
	"Z_B","Z_E","Z_I","Z_S","SH_B","SH_E","SH_I","SH_S","W_B","W_E","W_I","W_S",
	"NG_B","NG_E","NG_I","NG_S","B_B","B_E","B_I","B_S","CH_B","CH_E","CH_I","CH_S",
	"JH_B","JH_E","JH_I","JH_S","D_B","D_E","D_I","D_S","ZH_B","ZH_E","ZH_I","ZH_S",
	"G_B","G_E","G_I","G_S","UH_B","UH_E","UH_I","UH_S","UH0_B","UH0_E","UH0_I","UH0_S",
	"UH1_B","UH1_E","UH1_I","UH1_S","UH2_B","UH2_E","UH2_I","UH2_S","F_B","F_E","F_I","F_S",
	"V_B","V_E","V_I","V_S","M_B","M_E","M_I","M_S","DH_B","DH_E","DH_I","DH_S",
	"L_B","L_E","L_I","L_S","P_B","P_E","P_I","P_S","HH_B","HH_E","HH_I","HH_S",
	"R_B","R_E","R_I","R_S","TH_B","TH_E","TH_I","TH_S",};

int process_line(char *str,Phone_inf *phone)
{
	int length=0;
	int i=0;
	//save current state whether word
	//0 represent no word 1 is word
	int state=0;
	// word number
	int n=0;
	//record word offset
	int offset=0;
	length=strlen(str);
	char buff[100];
	while(1)
	{
		if(str[i] != ' ' && str[i] != '\t' && str[i] != '\0')
		{
			state=1;
			++offset;
			++i;
			continue;
		}
		if(str[i] == '\0')
		{
			if(state == 1 && n>4)
			{
				memset(buff,0x00,100*sizeof(char));
				strncpy(buff,&str[i-offset],offset);
				++n;
				state=0;
				offset=0;
				float f=atof(buff);
				phone->fea_inf.fea[n-5-1]=f;
//				printf("%f ",f);
//	printf("%d\n",n);
			}
			break;
		}
		if((str[i] == ' ' || str[i] == '\t') && state == 1)
		{
			memset(buff,0x00,100*sizeof(char));
			strncpy(buff,&str[i-offset],offset);
			++n;
			state=0;
			offset=0;
			if(n>4)
			{
				float f=atof(buff);
				if(n==5)
					phone->conf=f;
				else
					phone->fea_inf.fea[n-5-1]=f;
			//	printf("%f ",f);
			}
			else if(n==3)
			{
				int a=(int)atoi(buff);
				phone->start=a;
			//	printf("%d ",a);
			}
			else if(n==4)
			{
				int a=(int)atoi(buff);
				phone->end=a;
			}
			else if(n==1)
			{
				phone->phone_mark=buff[0];
			//	printf("%s ",buff);
			}
			else if(n==2)
			{
				strcpy(phone->phone_L,buff);
			}
			++i;
			continue;
		}
		++i;
	}//while(1)
	return 0;
}


/*
 * process align text
 * */
int process_text(char *file,Text_inf *text_p)
{
	if(NULL == file)
	{
		fprintf(stderr,"invalid parameter %s or word_n!\n",file);
		return -1;
	}
	FILE *fp;
	fp=fopen(file,"r");
	if(NULL == fp)
	{
		fprintf(stderr,"fopen(%s) wrong!\n",file);
		return -2;
	}
	char ph[1024];
	/* must be statistic word number
	 * */
	int begin_state=0;
//	int end_state=0;
	int n=0;
	int *word_n=&n;

	Word_inf *word_tem=NULL;
	Phone_inf *phone_tem=NULL;

	while(1)
	{
		memset(ph,0x00,1024*sizeof(char));
		if(fgets(ph,1024,fp)==NULL)
		{
			break;
		}
	//	printf("%s",ph);
		if(ph[0]=='#')
		{
			continue;
		}
		switch(ph[0])
		{
			case 'B':
				if(0 == begin_state)
				{
					begin_state=1;
					//add word and add phone
					word_tem=malloc_word();
					if(word_tem==NULL)
					{
						return -1;
					}
					phone_tem=malloc_phone();
					if(phone_tem==NULL)
					{
						return -1;
					}
					process_line(ph,phone_tem);
					if(add_text_word(text_p,word_tem)!=0)
					{
						return -1;
					}
					if(add_word_phone(word_tem,phone_tem)!=0)
					{
						return -1;
					}
		//			printf_phone(phone_tem);
					break;
				}
				else
				{
					fprintf(stderr,"text wrong!\n");
					fclose(fp);
					return -3;
				}
			case 'I':
				if(1 == begin_state)
				{
					//add phone
					phone_tem=malloc_phone();
					process_line(ph,phone_tem);
					//printf_phone(phone_tem);
					if(add_word_phone(word_tem,phone_tem)!=0)
					{
						return -1;
					}
					break;
				}
				else
				{
					fprintf(stderr,"text wrong!\n");
					fclose(fp);
					return -3;
				}
			case 'E':
				if(1 == begin_state)
				{
					++(*word_n);
					begin_state=0;
					phone_tem=malloc_phone();
					process_line(ph,phone_tem);
		//			printf_phone(phone_tem);
					//add phone
					if(add_word_phone(word_tem,phone_tem)!=0)
					{
						return -1;
					}
					break;
				}
				else
				{
					fprintf(stderr,"text wrong!\n");
					fclose(fp);
					return -3;
				}
			case 'S':
				if(begin_state!=0)
				{
					fprintf(stderr,"text wrong!\n");
					fclose(fp);
					return -3;
				}
				++(*word_n);
				//add word and add phone
				word_tem=malloc_word();
				if(word_tem==NULL)
				{
					return -1;
				}
				phone_tem=malloc_phone();
				if(phone_tem==NULL)
				{
					return -1;
				}
				process_line(ph,phone_tem);
				if(add_text_word(text_p,word_tem)!=0)
				{
					return -1;
				}
				if(add_word_phone(word_tem,phone_tem)!=0)
				{
					return -1;
				}
		//		printf_phone(phone_tem);
				break;
			case 'G':
				if(begin_state!=0)
				{
					fprintf(stderr,"text wrong!\n");
					fclose(fp);
					return -3;
				}
				break;
			default:
				fclose(fp);
				return -3;
				break;
		}//switch
	}//while(1)
	fclose(fp);
	/*
	tarverse_text(text_p);
	if(destory_text(text_p)!=0)
	{
		fprintf(stderr,"destory wrong!\n");
		return -1;
	}
	*/
	return 0;
}

/* 
 * n:     how many groups
 * */
int dis_w(Sta_word_inf *word_1,Sta_word_inf *word_2,Dis_word *res,int n)
{
	if(FBANK_L%n !=0)
	{
		fprintf(stderr,"n can't be devided exactly by FBANK_L %d",FBANK_L);
		return -1;
	}
	if(word_1->word_n != word_2->word_n)
	{
		fprintf(stderr,"different word number\n");
		return -2;
	}
	res->word_n=word_1->word_n;
	res->dim=FBANK_L/n;
	res->dis=(float *)malloc(sizeof(float) * res->word_n * res->dim);
	if(res->dis==NULL)
	{
		fprintf(stderr,"malloc dis wrong!\n");
		return -3;
	}
	memset(res->dis,0x00,sizeof(float) * res->word_n * res->dim);

	/* statistics mean and variance
	 * */
	float mean=0;
	float variance=0;
	float correl=0,mean_1=0,mean_2=0;
	float vari_1=0,vari_2=0;

	FILE *fp=NULL;
	fp=fopen("dis_fea","a");
#define SQUARE(x) (x)*(x)
	int i=0,j=0,k=0;
	for(i=0;i < res->word_n;i++)
	{
		mean=0,variance=0;
//		printf("%3d: ",i);

		correl=0,mean_1=0,mean_2=0;
		vari_1=0,vari_2=0;

		for(j=0;j < res->dim;j++)
		{
			for(k=0;k < n;k++)
			{
				//res->dis[i*res->dim + j]+=SQUARE(word_1->word[i].fea_mean[j*n+k]*word_1->word[i].conf - word_2->word[i].fea_mean[j*n+k]*word_2->word[i].conf);
				res->dis[i*res->dim + j]+=SQUARE(word_1->word[i].fea_mean[j*n+k] - word_2->word[i].fea_mean[j*n+k]);
				//res->dis[i*res->dim + j]+=SQUARE(word_1->word[i].conf * word_2->word[i].conf);
				mean_1+=word_1->word[i].fea_mean[j*n+k];
				mean_2+=word_2->word[i].fea_mean[j*n+k];
			}
			res->dis[i*res->dim + j]=sqrtf(res->dis[i*res->dim + j]/n);
			mean+=res->dis[i*res->dim + j];
		//	printf("%8.5f ",res->dis[i*res->dim + j]);
		//	fflush(stdout);
		}
		//calculate correlation
		mean_1/=(res->dim*n);
		mean_2/=(res->dim*n);
		for(j=0;j < res->dim;j++)
		{
			for(k=0;k<n;k++)
			{
				correl+=(word_1->word[i].fea_mean[j*n+k]-mean_1)*(word_2->word[i].fea_mean[j*n+k]-mean_2);
				vari_1+=SQUARE(word_1->word[i].fea_mean[j*n+k]-mean_1);
				vari_2+=SQUARE(word_2->word[i].fea_mean[j*n+k]-mean_2);
			}
		}
		correl/=sqrtf(vari_1*vari_2);
//		printf("correlate:%8.5f ",correl);
		//end correlation
		mean/=res->dim;
//		printf("%8.5f\n",mean);
	}
	//test
//	printf("mean:");

	//整句计算
	for(j=0;j < res->dim;j++)
	{
		mean=0;
		for(i=0;i < res->word_n;i++)
		{
			mean+=res->dis[i*res->dim+j];
		}
		fprintf(fp,"%8.5f ",mean/res->word_n);
	}
	fprintf(fp,"\n");
	//test end
#undef SQUARE
	fclose(fp);
	return 0;
}
void free_dis_word(Dis_word *res)
{
	if(res->dis==NULL)
		return ;
	free(res->dis);
}


/*calculate word confidence*/
int cal_word_conf(Sta_word_inf *word_1,Sta_word_inf *word_2)
{
	int i=0;
	if(word_1->word_n != word_2->word_n)
	{
		fprintf(stderr,"word number different!\n");
		return -1;
	}
	float dif=0,dif_1=0,dif_2=0,dif_3=0,dif_4=0,dif_5=0;
	float total=0,total_1=0,total_2=0,total_3=0,total_4=0,total_5=0,total_6=0;
	for(i=0;i<word_1->word_n;i++)
	{
		dif=0,dif_1=0,dif_2=0,dif_3=0,dif_4=0;
		dif=word_1->word[i].conf-word_2->word[i].conf;
		dif_1=word_1->word[i].conf*word_2->word[i].conf;
		dif_2=dif*dif;
		if(dif<0)
			dif=-1*dif;
		dif_5=abs(word_1->word[i].frame_n - word_2->word[i].frame_n);
		dif_3=dif_5/dif_1;
		dif_4=word_1->word[i].conf/word_2->word[i].conf;
		total+=dif;
		total_1+=dif_1;
		total_2+=dif_2;
		total_3+=dif_3;
		total_4+=dif_4;
		total_5+=dif_5;
		total_6+=logf(word_1->word[i].conf)+logf(word_2->word[i].conf);
	}
	total/=word_1->word_n;
	total_1/=word_1->word_n;
	total_2=sqrtf(total_2/word_1->word_n);
	total_3/=word_1->word_n;
	total_4/=word_1->word_n;
	total_5/=word_1->word_n;
	total_6/=word_1->word_n;
	float var_abs=0,per_abs=0;
	for(i=0;i<word_1->word_n;i++)
	{
		var_abs+=(abs(word_1->word[i].frame_n - word_2->word[i].frame_n)-total_5)*(abs(word_1->word[i].frame_n - word_2->word[i].frame_n)-total_5);
		per_abs+=abs(word_1->word[i].frame_n - word_2->word[i].frame_n)/word_1->word[i].frame_n;
	}
	var_abs=sqrtf(var_abs/word_1->word_n);
	per_abs/=word_1->word_n;

	FILE *fp=NULL;
	fp=fopen("conf_file","a");
	//保留4个，帧差均值，帧差方差，概率乘和，概率相乘，
	fprintf(fp,"%s %s   %8.5f %8.5f %8.5f %8.5f \n",fea_1,fea_2,total_5,var_abs,total_1,total_6);
	//           1    2      3      4      5      6    7       8      9
	//fprintf(fp,"%8.5f %8.5f  %8.5f  %8.5f  %8.5f  %8.5f  %8.5f  %8.5f\n",
	//	total,total_1,total_2,total_4,total_5,var_abs,per_abs,total_6);
	fclose(fp);
	return 0;
}
//n:对fbanks特征分组，每组n个，计算特征距离
int dis_P(Text_inf *A,Text_inf *B,int n)
{
	if(A->word_n != B->word_n)
		return -1;
	Word_inf *w_A = A->word_head;
	Word_inf *w_B = B->word_head;
	Phone_inf *p_A=NULL, *p_B=NULL;

	float dis[FBANK_L/n];
	int i,j;
	int total_phone_n=0; //统计音素总数
	int same_symbol_n=0; //统计音素符号相同个数
	while(w_A != NULL && w_B != NULL)
	{
		p_A = w_A->phone_head;
		p_B = w_B->phone_head;
		total_phone_n += w_A->phone_n;
		while(p_A != NULL && p_B != NULL)
		{
			memset(dis,0x00,sizeof(float)*FBANK_L/n);
			//音素个数不同
			if(w_A->phone_n != w_B->phone_n)
			{
			//	printf("%c %8s --------------------------\n",p_A->phone_mark,p_A->phone_L);
				p_A=p_A->next;
				p_B=p_B->next;
				continue;
			}
			//音素不同
			if(strcmp(p_A->phone_L,p_B->phone_L) != 0)
			{
			//	printf("%c %8s --------------------------\n",p_A->phone_mark,p_A->phone_L);
			}
			else
			{
//				printf("%c %8s: ",p_A->phone_mark,p_A->phone_L);
				//计算
				for(i=0;i<FBANK_L;i+=n)
				{
					for(j=0;j<n;j++)
					{
						dis[i/n]+=(p_A->fea_inf.fea[i+j] - p_B->fea_inf.fea[i+j]) * (p_A->fea_inf.fea[i+j] - p_B->fea_inf.fea[i+j]);
					}
					dis[i/n]=sqrt(dis[i/n]/4);
//					printf("%8.5f  ",dis[i/n]);
				}
				//add test 前后f0差值
				float a=0,b=0;
				if(p_A->next == NULL)
				{
					if(w_A->next != NULL)
					{
						a=w_A->next->phone_head->phone_f0 - p_A->phone_f0;
						b=w_B->next->phone_head->phone_f0 - p_B->phone_f0;
						if((a == 0 && b ==0) || a*b > 0)
						{
							++same_symbol_n;
						}
					}
				}
				else
				{
					a=p_A->next->phone_f0 - p_A->phone_f0;
					b=p_B->next->phone_f0 - p_B->phone_f0;
					if((a == 0 && b ==0) || a*b > 0)
					{
						++same_symbol_n;
					}
				}
//				printf(" f0 %8.5f ",(p_A->phone_f0-w_A->word_f0) - (p_B->phone_f0 - w_B->word_f0));
//				printf("\n");
			}
			p_A=p_A->next;
			p_B=p_B->next;
		}
		w_A = w_A->next;
		w_B = w_B->next;
	}
	if(total_phone_n < 2)
	{
		fprintf(stderr,"phone total number < 2\n");
	}
	else
	{
		//printf("%5.3f\n",(float)same_symbol_n/(total_phone_n-1)*100);
	}
	return 0;
}
//处理f0
//file 基频文件
//text 保存对齐信息文件
//函数目的：将f0的结果添加到文件信息结构体内
//将非辅音的f0保存到变量内
/*查找str是否是辅音，是返回0，否则返回-1*/
int search_fy(char *str)
{
	int i=0;
	for(i=0;i<FY_LANG;++i)
	{
		if(strcmp(fuyin[i],str) == 0)
			return 0;
	}
	return -1;
}
#define F0_LANG 15
/*
 * file :基频文件
 * file1:共振峰文件
 * text :对齐结构体
 * f0_p :保存统计的音素基频信息，内部申请空间
 * l :保存基频个数，即音素个数
 * har_p:保存共振峰信息，计算音色
 * h_l:保存共振峰长度，元音的个数
 * phone_L:保存音素信息，为了计算音素节奏
 * pl:音素个数
 * word_L:保存单词信息，为了计算单词节奏
 * wl:单词个数
 * har_L:保存音素共振峰信息
 * */
int pro_f0(char *file ,char *file1,Text_inf *text,float **f0_p,int *l,
		float **har_p , int *h_1 ,int **phone_L,int *pl,int **word_L,int *wl)
{
	FILE *fp_h=fopen(file1,"r");
	if(fp_h == NULL)
		return -1;
	FILE *fp=fopen(file,"r");
	if(fp==NULL)
		return -1;
	int n=0;
	int n_h=0;
	char f[256];
	while(fgets(f,sizeof(f),fp_h)!=NULL)
	{
		n_h++;
	}
	fseek(fp_h,0,SEEK_SET);
	float *har = (float *)malloc(sizeof(float)*n_h*HAR);
	if(har==NULL)
		return -1;
	int i=0;
	while(fgets(f,sizeof(f),fp_h)!=NULL)
	{
		sscanf(f,"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
				&har[i*HAR+0],&har[i*HAR+1],&har[i*HAR+2],&har[i*HAR+3],&har[i*HAR+4],&har[i*HAR+5],&har[i*HAR+6],&har[i*HAR+7],&har[i*HAR+8],&har[i*HAR+9],
				&har[i*HAR+10],&har[i*HAR+11],&har[i*HAR+12],&har[i*HAR+13],&har[i*HAR+14],&har[i*HAR+15],&har[i*HAR+16],&har[i*HAR+17],&har[i*HAR+18],&har[i*HAR+19]);
		++i;
	}
	
	while(fgets(f,16,fp)!=NULL)
	{
		n++;
	}
	fseek(fp,0,SEEK_SET);
	int lang_h=F0_LANG;
	int lang_h1=F0_LANG;
	(*har_p)=(float *)malloc(sizeof(float) * lang_h1 * HAR);
	if(*har_p == NULL)
		return -1;
	
	int lang=F0_LANG;
	(*f0_p)= (float *)malloc(sizeof(float)*lang);
	if(*f0_p == NULL)
		return -1;
	float *f0=(float *)malloc(sizeof(float)*n);
	if(f0==NULL)
		return -1;

	//add phone_L
	int mm=F0_LANG;
	(*phone_L) = (int *)malloc(sizeof(int)*mm);
	if(*phone_L == NULL)
		return -1;

	//add word_L
	int ww=F0_LANG;
	(*word_L) = (int *)malloc(sizeof(int)*ww);
	if(*word_L == NULL)
		return -1;
	
	int j=0;//统计辅音数量>F0_LANG要重新分配内存
	memset(f,0x00,sizeof(char)*16);
	i=0;
	while(fgets(f,16,fp)!=NULL)
	{
		f0[i]=atof(f);
		++i;
		memset(f,0x00,sizeof(char)*16);
	}
	Word_inf *word=text->word_head;
	Phone_inf *phone=NULL;
	int k=0,m=0,w=0;
	int h=0;//记录共振峰多少，音素个数
	int num_phone=0;
	int mean_num=0;
	float mean_f0=0;
	while(word!=NULL)
	{
		int total_n=0;
		phone=word->phone_head;
		word->word_f0=0;

		while(phone!=NULL)
		{
			num_phone = phone->end - phone->start;
			num_phone /= 3;

			//add phone_L
			if(m >= mm)
			{
				mm+=5;
				(*phone_L) =(int *) realloc(*phone_L,sizeof(int) * mm);
				if((*phone_L)==NULL)
					return -1;
			}
			(*phone_L)[m]=phone->end - phone->start;
			++m;

			//将一个音素分为前中后3个保存基频
			for(k=0;k<3;++k)
			{
				phone->phone_f0[k]=0;
				for(i=phone->start+k*num_phone;i < (k==2 ? phone->end:phone->start+(k+1)*num_phone) ; ++i)
				{
					phone->phone_f0[k]+=f0[i];
				}
				word->word_f0 += phone->phone_f0[k];
				phone->phone_f0[k] /= (k==2 ? (phone->end - (phone->start+2*num_phone)):num_phone);
			}
			/*
			for(i=phone->start;i<phone->end;i++)
			{
				phone->phone_f0+=f0[i];
			}
			word->word_f0 += phone->phone_f0;
			phone->phone_f0/=(phone->end - phone->start);
			*/
			//记录音素共振峰均值
			int hh=0;
			for(i=phone->start;i<phone->end;i++)
			{
				for(hh=0;hh<HAR;hh++)
				{
					phone->phone_peak_eng[hh]+=har[i*HAR+hh];
				}
			}
			for(hh=0;hh<HAR;hh++)
			{
				phone->phone_peak_eng[hh] /=(phone->end - phone->start);
			}
			total_n += (phone->end - phone->start);
			//如果是原音则记录基频
			if(search_fy(phone->phone_L)!=0)
			{
				if(h>=lang_h1)
				{
					lang_h1 += 5;
					(*har_p)=(float *)realloc(*har_p,sizeof(float)*lang_h1*HAR);
				}
				for(hh=0;hh<HAR;hh++)
				{
					(*har_p)[h*HAR+hh] = phone->phone_peak_eng[hh];
				}
				++h;
				//共振峰记录完成
				if(j >= lang)
				{
					lang=lang+15;
					(*f0_p) =(float *) realloc(*f0_p,sizeof(float) * lang);
					if((*f0_p)==NULL)
						return -1;
				}
				for(k=0;k<3;++k,++j)
				{
					(*f0_p)[j]=phone->phone_f0[k];
					mean_f0 += phone->phone_f0[k];
				}
				mean_num +=3;
			}
			phone=phone->next;
		}
		if(w >= ww)
		{
			ww+=5;
			(*word_L) =(int *) realloc(*word_L,sizeof(int) * ww);
			if((*word_L)==NULL)
				return -1;
		}
		(*word_L)[w]=total_n;
		++w;
		word->word_f0/=total_n;
		word=word->next;
	}
	if(mean_num != 0)
	{
		mean_f0 /=mean_num;
	}
	//fprintf(stdout,"f0 mean = %f ",mean_f0);
	*l=j;
	*pl=m;
	*wl=w;
	*h_1=h;
	free(f0);
	free(har);
	fclose(fp);
	fclose(fp_h);
	return 0;
}

int printf_array(float *a,int n)
{
	int i=0;
	if(a==NULL)
		return -1;
	for(i=0;i<n;++i)
	{
		printf("%7.3f ",a[i]);
	}
	printf("\n");
	return i;
}
#define ABS_F0(f0) ((f0>0)?f0:(-1*f0))
//统计计算基频走势
int trend_f0(float *a1,int L1,float *a2,int L2)
{
	float *delta1 = (float *)malloc(sizeof(float) * (L1-1));
	if(delta1 == NULL)
		return -1;
	float *delta2 = (float *)malloc(sizeof(float) * (L2-1));
	if(delta2 == NULL)
		return -1;
	int l=0;
	(L1 < L2) ? (l=L1) : (l=L2);
	int *n_delta = (int *)malloc(sizeof(int) * (l-1));
	if(n_delta == NULL)
		return -1;
	memset(delta1,0x00,sizeof(float) * (L1-1));
	memset(delta2,0x00,sizeof(float) * (L2-1));
	memset(n_delta,0x00,sizeof(float) * (l-1));
	int i=0;
	int n_delta_1=0;//基频走势
	int n_delta_2=0;
	int n_delta_3=0;
	float mean_f0_1=0,mean_f0_2=0;
	for(i=0;i<L1;++i)
		mean_f0_1 += a1[i];//基频均值a
	mean_f0_1 /=L1;
	for(i=0;i<L2;++i)
		mean_f0_2 += a2[i];
	mean_f0_2 /=L2;

	float ratio_delta1=0;//计算基频delta占平均基频比例
	float ratio_delta2=0;
	for(i=0; i<(l-1) ;++i)
	{
		delta1[i] = a1[i+1]-a1[i];
		delta2[i] = a2[i+1]-a2[i];
		ratio_delta1 += ABS_F0(delta1[i])/mean_f0_1;
		ratio_delta2 += ABS_F0(delta2[i])/mean_f0_2;

		if(delta1[i]*delta2[i] >= 0)
		{
			++n_delta_1;
			n_delta[i]=1;
		}
		else
		{
			n_delta[i]=0;
		}
	}
	printf("delta1/mean1=%7.3f delta2/mean2=%7.3f dif=%5.3f ",ratio_delta1/(l-1),ratio_delta2/(l-1),ratio_delta1/(l-1)-ratio_delta2/(l-1));
	for(i=0;i<(l-2);++i)
	{
		if(n_delta[i]*n_delta[i+1] > 0)
		{	
			++n_delta_2;
			if((i < (l-3)) && (n_delta[i+1]*n_delta[i+2] > 0))
			{
				++n_delta_3;
			}
		}
	}
	if(l > 2)
	{
		printf("delta1=%7.3f ",100.0*n_delta_1/(l-1));
		if(l>3)
			printf(" delta2=%7.3f ",100.0*n_delta_2/(l-2));
		if(l>4)
			printf(" delta3=%7.3f ",100.0*n_delta_3/(l-3));

	}
	printf("\n");
	free(delta1);
	free(delta2);
	free(n_delta);
	return 0;
}
#define SQUARE(a)  (a)*(a)
/*
 * 计算两个数组的相关系数，数组个数要一样，为n
 * */
float corrl_f0(float *a1,float *a2,int n)
{
	int i=0;
	float mean1=0,mean2=0;
	float var1=0,var2=0;
	for(i=0;i<n;i++)
	{
		mean1 += a1[i];
		mean2 += a2[i];
	}
	mean1 /=n;
	mean2 /=n;
	for(i=0;i<n;++i)
	{
		var1 += SQUARE(a1[i]-mean1);
		var2 += SQUARE(a2[i]-mean2);
	}
	//var1 = sqrtf(var1);
	//var2 = sqrtf(var2);
	float cor=0;
	for(i=0;i<n;++i)
	{
		cor += (a1[i]-mean1)*(a2[i]-mean2);
	}
	cor /=sqrtf(var1 * var2);
	return cor;
}
#undef SQUARE

//rhythm 计算
void liner_fit(float *x,float *y,int n,float *a,float *b)
{
	int i=0;
	float sumx=0,sumy=0,sumxy=0,sumx2=0;
	for(i=0;i<n;++i)
	{
		sumx+=x[i];
		sumy+=y[i];
		sumxy+=x[i]*y[i];
		sumx2+=x[i]*x[i];
	}
	*a = (n*sumxy-sumx*sumy)/(n*sumx2-sumx*sumx);
	*b = (sumy - *a * sumx)/n;

}
//#define EXPRI
//#define TEST
#ifdef EXPRI
int pro_text(char *file,float *f0)
{
	FILE *fp = fopen(file,"r");
	if(fp == NULL)
		return -1;
	int n=0;
	char str[1024];
	while(fgets(str,1024,fp)!=NULL)
		n++;
	float f0_t[n];
	fseek(fp,0,SEEK_SET);
	memset(str,0x00,1024);
	int i=0;
	while(fgets(str,1024,fp)!=NULL)
	{
		f0_t[i]=atof(str);
		memset(str,0x00,1024);
		i++;
	}
	int j=0;
	if( n%9 > 9/2)
		j=n/9+1;
	else
		j=n/9;
	int k=0;
	for(i=0;i<9;i++)
	{
		f0[i]=0;
		for(k=0;k<j && i*j+k < n;k++)
		{
			f0[i]+=f0_t[i*j+k];
		}
		f0[i] /=k;
	}
	return 0;
}



int main(int argc,char *argv[])
{
	float *f1=(float *) malloc(sizeof(float)*9);
	if(f1==NULL)
		return -1;
	float *f2=(float *) malloc(sizeof(float)*9);
	if(f2==NULL)
		return -1;
	if(pro_text(argv[1],f1) != 0)
		return -1;
	if(pro_text(argv[2],f2) != 0)
		return -1;

	float cor=0;
	cor=corrl_f0(f1,f2,9);
	printf(" cor= %4.3f ",cor);
	if(trend_f0(f1,9,f2,9) != 0)
		return -1;
	free(f1);
	free(f2);
	return 0;
}
#else
#define SOURCE(x) (x)*(x)
void tone_cal(float *peak1,float *peak2,int dim,int row,float *dis_tone,float *dis_pitch)
{
	int i=0,j=0;
	float dis1=0.0,dis2=0.0;
	for(i = 0 ; i < row ; ++i)
	{//xiu gai zhe li ,wei lei xiu gai wei du
		for(j=0;j<(dim-0)/2;++j)
		{
			dis1 += SOURCE(peak1[i*dim+2*j]-peak2[i*dim+2*j]);
			dis2 += SOURCE(peak1[i*dim+2*j+1]-peak2[i*dim+2*j+1]);
		}
		//打印每个元音的共振峰信息
	//	printf("%7.3f %7.3f \n",sqrtf(dis1),sqrtf(dis2));
		*dis_tone += sqrtf(dis1);
		*dis_pitch += sqrtf(dis2);
		dis1=0.0,dis2=0.0;
	}
	*dis_tone /= row;
	*dis_pitch /= row;
	//打印整句话元音的共振峰信息均值
	//printf("%7.3f %7.3f ",*dis_tone,*dis_pitch);
	//harmonic
	float har_score =100 - 100 / (1 + exp((-1*(*dis_pitch - 450))/100));
	printf("harmonic_score = %5.2f\n",har_score);

}
#undef SOURCE
#define TONE
#define PITCH
#define RHYTHM_PHONE
#define RHYTHM_WORD
int main(int argc,char *argv[])
{
	if(argc!=7)
	{
		fprintf(stderr,"command temp1 temp2 f01 f02 peak1 peak2\n");
		return -1;
	}
	fea_1=argv[1];
	fea_2=argv[2];


	//用于保存非辅音基频，保存为连续的
	float *f0_p1=NULL;
	float *f0_p2=NULL;
	int l1=0,l2=0;
	int *phone_L1=NULL;
	int *phone_L2=NULL;
	int pl1=0,pl2=0;

	//保存共振峰
	float *har_p1=NULL;
	float *har_p2=NULL;
	int h1=0,h2=0;

	int *word_L1=NULL;
	int *word_L2=NULL;
	int wl1=0,wl2=0;

	Text_inf text;
	init_text(&text);
	Text_inf *text_p=NULL;
	text_p=&text;
	if(0 != process_text(argv[1],text_p))
	{
		fprintf(stderr,"process_text wrong!\n");
		return -1;
	}
	pro_f0(argv[3],argv[5],text_p,&f0_p1,&l1,&har_p1,&h1,&phone_L1,&pl1,&word_L1,&wl1);
#ifdef TEST
	if(printf_array(f0_p1,l1)<0)
	{
		printf("printf_array error!\n");
		return -1;
	}
#endif
	/*
	Sta_word_inf sta_word;
	if(0 != statistics_word(text_p,&sta_word))
	{
		fprintf(stderr,"statistics_word wrong!\n");
		return -1;
	}
	*/
//	tarverse_sta_word(&sta_word);

	Text_inf text_1;
	init_text(&text_1);
	text_p=&text_1;
	if(0 != process_text(argv[2],text_p))
	{
		fprintf(stderr,"process_text wrong-1!\n");
		return -1;
	}

	pro_f0(argv[4],argv[6],text_p,&f0_p2,&l2, &har_p2, &h2, &phone_L2,&pl2,&word_L2,&wl2);
#ifdef TEST
	if(printf_array(f0_p2,l2)<0)
	{
		printf("printf_array error!\n");
		return -1;
	}
#endif
//ji suan gong zhen feng 
#ifdef TONE
	float tone_dis=0.0,pitch_dis=0.0;
	int t=0;
	//if(h1<=h2)
	tone_cal(har_p1,har_p2,2,(h1<h2?h1:h2),&tone_dis,&pitch_dis);
/*	if(h1>h2)
	{
		for(t=h2;t<h1;++t)
			printf("null null\n");
	}
*/	//fprintf(stderr,"%s %d %d\n",argv[1],h1,h2);
#endif

	if(0 != dis_P(&text,&text_1,4))
		printf("dis_P error!\n");
#ifdef PITCH
	float cor=0;
	if(l1 == l2)
	{
		cor = corrl_f0(f0_p1,f0_p2,(l1<l2?l1:l2));
	}
	else
	{
		fprintf(stderr,"f0 is different: ");
		cor = corrl_f0(f0_p1,f0_p2,(l1<l2?l1:l2));
	}
	//printf("correlation = %5.3f ",cor);
	float pitch_score =  100 / (1 + exp((-1*(cor - 0.3))/0.15));
	printf("pitch_score = %5.2f\n",pitch_score);
#endif

	
#ifdef RHYTHM_PHONE
	float a=0,b=0;
	int l=0;
	float ph_1[pl1],ph_2[pl2];
	float cor_r=0;
	for(l=0;l<pl1;++l)
	{
		ph_1[l]=(float)phone_L1[l];
	}
	for(l=0;l<pl2;++l)
	{
		ph_2[l]=(float)phone_L2[l];
	}
	if(pl1 == pl2)
	{
		liner_fit((float *)ph_1,(float *)ph_2,pl1,&a,&b);
		cor_r = corrl_f0(ph_1,ph_2,(pl1<pl2?pl1:pl2));
	}
	else
	{
		fprintf(stderr,"phone num is different: ");
		liner_fit((float *)ph_1,(float *)ph_2,(pl1<pl2?pl1:pl2) ,&a,&b);
		cor_r = corrl_f0(ph_1,ph_2,(pl1<pl2?pl1:pl2));
	}
	//printf(" RHYTHM_PHONE cor_r=%f a=%f , b=%f ",cor_r,a,b);
	float rhythm_phone_score =  100 / (1 + exp((-1*(cor_r - 0.4))/0.15));
	printf("rhythm_phone_score = %5.2f\n",rhythm_phone_score);
#endif

#ifdef RHYTHM_WORD
	float a1=0,b1=0;
	int h_w=0;
	float wo_1[wl1],wo_2[wl2];
	float cor_rw=0;
	for(h_w=0;h_w<wl1;++h_w)
	{
		wo_1[h_w]=(float)word_L1[h_w];
	}
	for(h_w=0;h_w<wl2;++h_w)
	{
		wo_2[h_w]=(float)word_L2[h_w];
	}
	if(wl1 == wl2)
	{
		liner_fit((float *)wo_1,(float *)wo_2,wl1,&a1,&b1);
		cor_rw = corrl_f0(wo_1,wo_2,(pl1<pl2?wl1:wl2));
	}
	else
	{
		fprintf(stderr,"phone num is different: ");
		liner_fit((float *)wo_1,(float *)wo_2,(wl1<wl2?wl1:wl2) ,&a1,&b1);
		cor_rw = corrl_f0(wo_1,wo_2,(pl1<pl2?wl1:wl2));
	}
	//printf(" a1=%f , b1=%f ",a1,b1);
	float rhythm_word_score =  100 / (1 + exp((-1*(cor_rw - 0.6))/0.1));
	printf("rhythm_word_score = %5.2f\n",rhythm_word_score);
#endif
	//printf("\n");
	//now 实现对音素的特征距离计算
/*
	if(trend_f0(f0_p1,l1,f0_p2,l2) != 0)
	{
		fprintf(stderr,"trend_f0 error!\n");
		return -1;
	}
*/
/*
	Sta_word_inf sta_word_1;
	if(0 != statistics_word(text_p,&sta_word_1))
	{
		fprintf(stderr,"statistics_word wrong!\n");
		return -1;
	}
//	tarverse_sta_word(&sta_word_1);
	//计算两句话相关信息
	cal_word_conf(&sta_word,&sta_word_1);
	//calculate dis
	Dis_word res;

	if(dis_w(&sta_word,&sta_word_1,&res,4)!=0)
		return -1;
	free_dis_word(&res);

	free_sta_word(&sta_word);
	free_sta_word(&sta_word_1);
	//tarverse_text(text_p);
*/
	if(destory_text(&text)!=0 || destory_text(&text_1)!=0)
	{
		fprintf(stderr,"destory wrong!\n");
		return -1;
	}
//#ifdef TONE
	free(f0_p1);
	free(f0_p2);
//#endif
//#ifdef RHYTHM_PHONE
	free(phone_L1);
	free(phone_L2);
//#endif
//#ifdef RHYTHM_WORD
	free(word_L1);
	free(word_L2);
//#endif
	free(har_p1);
	free(har_p2);
	return 0;
}
#endif
