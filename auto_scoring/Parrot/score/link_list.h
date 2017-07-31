#include "fea_struct.h"
#ifdef   __cplusplus
extern   "C"
{
#endif
void printf_phone(Phone_inf *phone);

int add_word(Word_inf *old_word_link,Word_inf *add_word,int *n);

int add_phone(Phone_inf *old_phone_link,Phone_inf *add_phone,int *n);

int add_text_word(Text_inf *text,Word_inf *word);

int add_word_phone(Word_inf *word,Phone_inf *phone);

Word_inf *init_word(Word_inf *word);

Word_inf *malloc_word();

Phone_inf *init_phone(Phone_inf *phone);

Phone_inf *malloc_phone();

int destory_word(Word_inf *word);

int destory_text(Text_inf *text_inf);

void tarverse_text(Text_inf *text);

void init_text(Text_inf *text);

int statistic_word(Text_inf *text,Sta_word_inf *sta_word);

void free_sta_word(Sta_word_inf *word);

void tarverse_sta_word(Sta_word_inf *word);

void tarverse_sta_word_in(Sta_word *sta_word,int n);

#ifdef   __cplusplus
}
#endif
