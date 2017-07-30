#include <stdlib.h>
#include <stdio.h>

#ifndef _MATHLIB_H

double rand_N01(int n);


double activate_hidden(double x);
double activate_output(double *x, int num);

int init_weight(int N_LAYER, int * LAYERS, double **WEIGHTS);
int init_preprocessing(FILE *fp, int in_num, int out_num, double *mean, double *var);


double forward_cal( int nlayer, int *layers, double **nodes, double *tnodes, double **weights,bool regression,float y);
void backward_cal(int nlayer, int *layers, double **nodes1, double **nodes2, double **weights);
void acc_variation(int nlayer, int *layers, double **weight, double **nodes1, double **nodes2);
void adapt_weight(int nlayer, int *layers, double **sweight, double **weight, double theta, int num);

double cal_error_function(int out_num, double *cal, double *ref,bool regression);
double CER(int out_num, double *cal, double *ref);


#endif
