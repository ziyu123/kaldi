#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#define _DEBUG_
//#define _DEBUG_2_

#ifndef _FUNC_H_
#define TRUE 1
#define FALSE 0

#define WT(weight, i,j,k, nk) weight[i][(j)*(nk)+(k)]


double* alloc_network_weight(double ** &weight, int nl, int *la);
void print_network_weight(double ** weight, int nl, int *la);
double *alloc_network_node(double ** &node, int nl, int *la);
void print_network_node(double **node, int nl , int *la, int simple=FALSE);


int read_pattern(FILE *fp, int inode_n, double *inodes,  int onode_n, double *onodes);
int preprocess_pattern(FILE *fp, int inode_n, double *inodes, double *mean, double *var);

int save_mlp(char *fn, int gl_total_weight, double **weights, int in_num, double *mean, double *var);
int load_mlp(char *fn, int gl_total_weight, double **weights, int in_num, double *mean, double *var);



#endif
