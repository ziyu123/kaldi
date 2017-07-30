#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "mathlib.h"
#include "utl.h"
#include "mlp.h"


#define MAX_ITERATION 100

int gl_total_weights=0;
int gl_total_nodes=0;
int gl_preprocess_initialized=0;

enum T_TASK{TRAIN=0, TEST=1, ERROR=2};
enum T_KIND{REGRESSION=0, CLASSIFICATION=1, KERROR=2};


char * stripfn(char * fn)
{
    int len=strlen(fn);
    if ((fn[len-1]=='\n')||(fn[len-1]=='\r')) fn[len-1]=0;
    if ((fn[len-2]=='\n')||(fn[len-2]=='\r')) fn[len-2]=0;
    return fn;
}

int gen_temporary_test_weights(double *WEIGHTS, double *s_weights, double *t_weights, double theta)
{

	for (int i=0; i<gl_total_weights;i++){
	
		t_weights[i]=WEIGHTS[i]-theta*s_weights[i];
	
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////

void print_array(double *array, int num)
{

	for (int i=0; i<num; i++){
		printf( " %5.2lf", array[i]);
	}

}

void print_1_of_c(double *array, int num)
{
	int maxi=0;
	double maxf=-1e10;

	for(int i=0; i<num; i++){
	
		if (array[i]>maxf){
			maxf=array[i];
			maxi=i;

		}
	}

	for (int i=0; i<num; i++){
		printf(" %5.2lf", (i==maxi)?1.0:0.0);
	}


}

static bool get_feature(const char *features,int inode_n, double *inodes)
{
	char *p,*q;
	p = q = new char[100];
	strncpy(p,features,100);

	while ((*p ==' ') || (*p=='\t')) p++;
	for (int j=0; j<inode_n-1; j++)
	{
		if (j==0)
			p=strtok((char*)p, " \t");
		else
			p=strtok(NULL, " \t");

		if (p==NULL){
			//null is wrong
			printf("ERROR: read_pattern: not enough data\n");
			goto error;
		}

		sscanf(p, "%lf", inodes+j);
	}
	inodes[inode_n-1]=1.0; //the last one is biased
	
	delete[] q;
	return true;
error:
	delete[] q;
	return false;
}

double test_mlp(char *feature, int N_LAYER, int * LAYERS, double **WEIGHTS, double *mean, double *var, int release, float y)
{
	int inode_n=LAYERS[0], onode_n=LAYERS[N_LAYER-1];
	int o_layer=N_LAYER-1, num=0;

	double **t_node1;

	double *t_node1_pool  =alloc_network_node(t_node1, N_LAYER, LAYERS);
	double *t_node2  =new double[onode_n];

	

	double sum=0;

	if(get_feature(feature, inode_n, t_node1[0]))
	{
		t_node2[0]=0;t_node2[1]=1;
		preprocess_pattern(NULL, inode_n, t_node1[0], mean, var);
		forward_cal(N_LAYER, LAYERS, t_node1, t_node2, WEIGHTS, false,y);

		sum = t_node1[o_layer][1];
	}


	delete[] t_node2;
	delete[] t_node1;
	delete[] t_node1_pool;
	

	return sum;
}
//////////////////////////////////////////////////


int mlp_score(char *feature,char *mlp_model,float y)
{

	char mlpfn[100];
	const int N_LAYER=3;
	//int LAYERS[N_LAYER]={13,120,2};
	int LAYERS[N_LAYER]={10,80,2};
	double  **WEIGHTS=NULL, *pool=NULL, *mean=NULL, *var=NULL;

	gl_total_nodes = 0;
	gl_preprocess_initialized = 0;
	 
	//now alloc memory
	int T_WEIGHT=0;
	for (int i=0; i< N_LAYER-1; i++)
	{
		T_WEIGHT+=LAYERS[i]*LAYERS[i+1];
		gl_total_nodes+=LAYERS[i];
	}
	gl_total_nodes+=LAYERS[N_LAYER-1];
	gl_total_weights=T_WEIGHT;

	 
	pool=alloc_network_weight(WEIGHTS, N_LAYER, LAYERS);

	 //alloc mean, var
	mean=new double[LAYERS[0]];
	var=new double[LAYERS[0]];
	 
	double sum =  0;
/*
#ifdef IOS
    char *class_mlp = (char*)"./class.mlp";
#elif __ANDROID_API__
    char *class_mlp = "/mnt/sdcard/MoFang/model/class.mlp";
#elif __unix__
    char *class_mlp = "model/class.mlp";
#elif WP8
	char *class_mlp = "pachira/model/class.mlp";
#endif
*/
	if(load_mlp(mlp_model, gl_total_weights, WEIGHTS, LAYERS[0], mean, var)!=0)
	{
		fprintf(stderr, "load mlp model from %s failed!\n", mlpfn);
	}else{
		sum = test_mlp(feature, N_LAYER, LAYERS, WEIGHTS, mean, var, 1,y);
	}


	if(NULL!=WEIGHTS) delete[] WEIGHTS;
	if(NULL!=mean) delete[] mean;
	if(NULL!=var) delete[] var;
	if(NULL!=pool) delete[] pool;

	return sum*100;

}

