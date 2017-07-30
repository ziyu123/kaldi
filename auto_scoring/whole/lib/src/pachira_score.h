#ifndef __PACHIRA_SCORE_H__
#define __PACHIRA_SCORE_H__

#ifdef   __cplusplus   
extern   "C"   
{   
#endif

int sys_init();
int score_init(char *text);
int send_wav(char* wav_data, int wav_len, int flag);
int score_get(int **w_score,int *n);


#ifdef   __cplusplus   
} 
#endif
#endif
