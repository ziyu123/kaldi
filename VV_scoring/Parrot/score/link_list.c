#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"link_list.h"

void printf_phone(Phone_inf *phone)
{
	printf("%c %s %d %d %f ",
			phone->phone_mark,
			phone->phone_L,
			phone->start,
			phone->end,
			phone->conf);
	int i=0;
	for(i=0;i<FBANK_L;i++)
		printf("%f ",phone->fea_inf.fea[i]);
	printf("\n");
}
/*add word at last*/
int add_word(Word_inf *old_word_link,Word_inf *add_word,int *n)
{
	if(add_word == NULL || old_word_link == NULL || *n == 0)
	{
		fprintf(stderr,"no create can't add word\n");
		return -1;
	}
	/*
	if(*n == 0 && old_word_link == NULL)
	{
		old_word_link = add_word;
		(*n)+=1;
		return 0;
	}
	*/
	Word_inf *search_tail=NULL;
	search_tail = old_word_link;
	while(search_tail->next != NULL)
	{
		search_tail=search_tail->next;
	}
	search_tail->next=add_word;
	(*n)+=1;
//	search_tail->next->next=NULL;
	return 0;
}
/*add phone at last*/
int add_phone(Phone_inf *old_phone_link,Phone_inf *add_phone,int *n)
{
	if(add_phone == NULL || old_phone_link == NULL || *n == 0)
	{
		fprintf(stderr,"no create can't add phone\n");
		return -1;
	}
	/*
	if(*n==0 && old_phone_link==NULL)
	{
		old_phone_link = add_phone;
		(*n)+=1;
		return 0;
	}
	*/
	Phone_inf *search_tail=NULL;
	search_tail = old_phone_link;
	while(search_tail->next != NULL)
	{
		search_tail=search_tail->next;
	}
	search_tail->next=add_phone;
	(*n)+=1;
	return 0;
}

/*add word to text*/
int add_text_word(Text_inf *text,Word_inf *word)
{
	if(text->word_n == 0 && text->word_head == NULL)
	{
		text->word_head=word;
		text->word_n+=1;
		return 0;
	}
	if(add_word(text->word_head,word,&(text->word_n))!=0)
	{
		fprintf(stderr,"add_word wrong!\n");
		return -1;
	}
	return 0;
}
/* add phone to word*/
int add_word_phone(Word_inf *word,Phone_inf *phone)
{
	if(word->phone_head == NULL && word->phone_n == 0)
	{
		word->phone_head=phone;
		word->phone_n+=1;
		return 0;
	}
	if(add_phone(word->phone_head,phone,&(word->phone_n))!=0)
	{
		fprintf(stderr,"add_phone wrong!\n");
		return -1;
	}
	return 0;
}

/*malloc word and initialize*/
Word_inf *init_word(Word_inf *word)
{
	if(word==NULL)
	{
		fprintf(stderr,"word is NULL,is not initialize!\n");
		return NULL;
	}
	word->phone_n=0;
	word->phone_head=NULL;
	word->next=NULL;
	return word;
}
/*malloc word*/
Word_inf *malloc_word()
{
	Word_inf *word;
	word=(Word_inf *)malloc(sizeof(Word_inf));
	if(word == NULL)
	{
		fprintf(stderr,"malloc wrong!\n");
		return NULL;
	}
	return init_word(word);
}

/*malloc phone and initialize*/
Phone_inf *init_phone(Phone_inf *phone)
{
	if(phone == NULL)
	{
		fprintf(stderr,"word is NULL,is not initialize!\n");
		return NULL;
	}
	memset(phone,0x00,sizeof(Phone_inf));
	phone->next=NULL;
	return phone;
}
/* malloc phone
 * */
Phone_inf *malloc_phone()
{
	Phone_inf *phone;
	phone=(Phone_inf *)malloc(sizeof(Phone_inf));
	if(phone == NULL)
	{
		fprintf(stderr,"malloc wrong!\n");
		return NULL;
	}
	return init_phone(phone);
}
/*del the first*/
int destory_word(Word_inf *word)
{
	if(word==NULL)
	{
		fprintf(stderr,"del_word wrong!\n");
		return -1;
	}
	Phone_inf *del_phone=NULL;

	while(word->phone_head!=NULL && word->phone_n != 0)
	{
		del_phone=word->phone_head->next;
		free(word->phone_head);
		word->phone_n-=1;
		word->phone_head=del_phone;
	}
	return 0;
}
/* destory text link free all place
 * */
int destory_text(Text_inf *text_inf)
{
	if(text_inf==NULL)
	{
		return 0;
	}
	Word_inf *del_word=NULL;
	while(text_inf->word_head != NULL && text_inf->word_n !=0)
	{
		del_word=text_inf->word_head->next;
		destory_word(text_inf->word_head);
		free(text_inf->word_head);
		text_inf->word_n-=1;
		text_inf->word_head=del_word;
	}
	return 0;
}

/* tarverse text and print all information
 * */
void tarverse_text(Text_inf *text)
{
	Word_inf *word_cur=NULL;
	Phone_inf *phone_cur=NULL;
	word_cur=text->word_head;
	printf("word number:%d\n",text->word_n);
	while(word_cur!=NULL)
	{
		phone_cur=word_cur->phone_head;
		printf("phone number:%d\n",word_cur->phone_n);
		while(phone_cur!=NULL)
		{
			printf_phone(phone_cur);
			phone_cur=phone_cur->next;
		}
		word_cur=word_cur->next;
	}
}

/* initialize text 
 * if you want add text link,you must be initialize text
 * */
void init_text(Text_inf *text)
{
	text->word_n=0;
	text->word_head=NULL;
}


/* statistics word information
 * */

int statistics_word(Text_inf *text,Sta_word_inf *sta_word)
{
	if(NULL == text || text->word_n==0)
	{
		return -1;
	}
	
	Sta_word *tem_word=NULL;
	Word_inf *word_cur=NULL;
	Phone_inf *phone_cur=NULL;
	word_cur = text->word_head;
	tem_word = (Sta_word *)malloc(sizeof(Sta_word)*text->word_n);
	if(tem_word == NULL)
	{
		fprintf(stderr,"tem_word malloc wrong!\n");
		return -1;
	}
	memset(tem_word,0x00,sizeof(Sta_word)*text->word_n);
	sta_word->word_n=text->word_n;
	int i=0;//record how many word
	int total_phone_frame=0;
	float conf_cur=0;
	float fea_cur[FBANK_L];
	
	int j=0;
	//printf("word number:%d\n",text->word_n);
	while(word_cur!=NULL)
	{
		total_phone_frame=0;
		conf_cur=0;
		memset(fea_cur,0x00,FBANK_L*sizeof(float));

		tem_word[i].serial=i;
		tem_word[i].phone_n=word_cur->phone_n;
		phone_cur=word_cur->phone_head;
	//	printf("phone number:%d\n",word_cur->phone_n);
		while(phone_cur!=NULL)
		{
			//printf_phone(phone_cur);
			int phone_frame_cur=phone_cur->end - phone_cur->start;
			conf_cur+=phone_frame_cur * phone_cur->conf;
			for(j=0;j<FBANK_L;j++)
			{
				fea_cur[j]+=phone_frame_cur * phone_cur->fea_inf.fea[j];
			}
			total_phone_frame+=phone_frame_cur;
			phone_cur=phone_cur->next;
		}
		tem_word[i].frame_n=total_phone_frame;
		tem_word[i].conf=conf_cur/total_phone_frame;
		for(j=0;j<FBANK_L;j++)
		{
			tem_word[i].fea_mean[j]=fea_cur[j]/total_phone_frame;
		}

		word_cur=word_cur->next;
		i++;
	}
	sta_word->word=tem_word;
	return 0;
}

void free_sta_word(Sta_word_inf *word)
{
	free(word->word);
}

void tarverse_sta_word(Sta_word_inf *word)
{
	tarverse_sta_word_in(word->word,word->word_n);
}
void tarverse_sta_word_in(Sta_word *sta_word,int n)
{
	int i=0;
	int j=0;
	for(i=0;i<n;i++)
	{
		printf("%d %d %d %f ",sta_word[i].serial,sta_word[i].phone_n,sta_word[i].frame_n,sta_word[i].conf);
		for(j=0;j<FBANK_L;j++)
			printf("%f ",sta_word[i].fea_mean[j]);
		printf("\n");
	}
}
