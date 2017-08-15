#include "mathlib.h"
#include "utl.h"
#include "math.h"
#include <time.h>

extern int gl_preprocess_initialized;
extern int gl_total_weights;
extern int gl_total_nodes;

//sigmoid activation for regression
double sigmoid(double x,float y)
{

	return 1/(1+exp(-x/y));

}

//softmax activation for classification
double softmax(double *x, int num)
{

	double sum=0;

	for (int i=0; i<num; i++){
	
		x[i]=exp(x[i]);
		sum+=x[i];
	
	}

	for (int i=0; i<num;i++){
	
		x[i]/=sum;
	}

	//not necessarily use the sum
	return sum;
}

double activate_output (double *x, int num, bool regression)
{

  if (regression){
      for (int i=0;i<num;i++) x[i]=sigmoid(x[i],1.0);
  }else{
      softmax(x,num);
  }

}


int init_weight(int N_LAYER, int * LAYERS, double **WEIGHTS)
{
	
	int T=0;


	memset(&(WEIGHTS[0][0]), 0, sizeof(double)*gl_total_weights);

	for (int i=0; i<N_LAYER-1; i++){

		double *p=WEIGHTS[i];
		int t1=LAYERS[i];
		int t2=(i==N_LAYER-2)?LAYERS[i+1]:LAYERS[i+1]-1;

		for (int j=0; j<t1; j++){
			
			for (int k=0; k<t2; k++){
			
				p[j*t2+k]=(i==0)?rand_N01(LAYERS[0]):rand_N01(1);
			}
		}
	}

	return 0;


}


double forward_cal( int nlayer, int *layers, double **nodes, double *tnodes, double **weights, bool regression,float y)
{
	for (int i=1; i<nlayer; i++){//each layer

		for (int j=0; j<layers[i]; j++){
		
			//if this is not the output, give 1.0
			if ((i!=nlayer-1)&&(j==layers[i]-1)){
				nodes[i][j]=1.0;
				continue;
			}
			nodes[i][j]=0;

			//accumulate
			for (int k=0; k<layers[i-1]; k++){
			
				nodes[i][j]+=nodes[i-1][k]*WT(weights,(i-1),k,j,layers[i]);
			
			}


			//sigmoid for hidden layers
			if (i<nlayer-1) {
				nodes[i][j]=sigmoid(nodes[i][j],y);
                        }

		
		}



	
	}

	//activate the output layer
	activate_output(nodes[nlayer-1], layers[nlayer-1], regression);
#if 0
	printf("predict: ");
	for(int k=0; k<layers[nlayer-1]; k++) printf(" %lf ", nodes[nlayer-1][k]);
	printf("\n");

#endif

	//now calculate the error score
	return cal_error_function(layers[nlayer-1], nodes[nlayer-1], tnodes, regression);

	

}


void backward_cal(int nlayer, int *layers, double **nodes1, double **nodes2, double **weights)
{

	//first the output delta
	int olayer=nlayer-1;
	for (int i=0; i<layers[olayer];i++){
	
		nodes2[olayer][i]=nodes1[olayer][i]-nodes2[olayer][i];
	
	}


	for (int i=nlayer-2; i>=1; i--){
	
		for (int j=0; j<layers[i];j++){
		
		
			nodes2[i][j]=0;

			int dest_n=(i==nlayer-2)?layers[i+1]:layers[i+1]-1;
			for (int k=0; k<dest_n;k++){
			
				nodes2[i][j]+=nodes2[i+1][k]*WT(weights, i, j, k, layers[i+1]);

			
			}

			nodes2[i][j]*=nodes1[i][j]*(1-nodes1[i][j]);
		
		}

	
	}

}




//classification error
double CER(int out_num, double *cal, double *ref)
{

	int bcal=0; int bref=0;

	for (int i=0; i<out_num; i++){
	
		if (cal[i]>cal[bcal]) bcal=i;
		if (ref[i]>ref[bref]) bref=i;
	
	}
	
	return (bcal==bref)?0:1;


}


//square error, for regression
double square(int out_num, double *cal, double *ref)
{

	double sum=0;

	for (int i=0; i<out_num; i++){
	
		sum+=(cal[i]-ref[i])*(cal[i]-ref[i]);
	}

	sum/=2;

	return sum;

}

//cross entropy; for classification
double cross_entropy(int out_num, double *cal, double *ref)
{

	double sum=0;

	for (int i=0; i<out_num;i++){
		sum+=-log(cal[i])*ref[i];
                //printf("  in call_error_function3: cal=%lf ref=%lf sum=%lf\n",cal[i], ref[i],  sum);
	}

        //printf("call_error_function3: sum=%lf\n", sum);
	return sum;

}

double cal_error_function(int out_num, double *cal, double *ref, bool regression)
{

        return regression?square(out_num,cal,ref):cross_entropy(out_num,cal,ref);

}


int init_preprocessing(FILE *fp, int in_num, int out_num, double *mean, double *var)
{

	double *t1=new double[in_num];
	double *t2=new double[out_num];
	double *num=new double[in_num];
	
	memset(mean, 0, in_num*sizeof(double));
	memset(var, 0, in_num*sizeof(double));
	memset(num, 0, in_num*sizeof(double));
    


 	//first calculate the average
    

	int numx=0;
	while(read_pattern(fp, in_num, t1, out_num, t2)==1){//read each pattern for training

		
		for (int k=0; k<in_num; k++){
		
			if (t1[k] > -1e7){
				mean[k]+=t1[k];
				var[k]+=t1[k]*t1[k];
				num[k]++;
			}
		
		}

		numx++;
	}

	if (numx==0){ return 0;}

	double sum=0;
	for (int k=0; k<in_num-1; k++){ 
		mean[k]=mean[k]/num[k];
		//here we don't use 1/(n-1) as variance for simplicity, and inverse for efficient calculation
		var[k]=1/sqrt(var[k]/num[k]-mean[k]*mean[k]);
		
	}

	//the last is the bias
	mean[in_num-1]=0; var[in_num-1]=1;

	gl_preprocess_initialized=1;


	delete num;
	delete t1;
	delete t2;

	

	return 0;
}



void acc_variation(int nlayer, int *layers, double **weight, double **nodes1, double **nodes2)
{

	for (int i=0; i<nlayer-1; i++){
	
		for (int j=0; j<layers[i];j++){
		
			for (int k=0; k<layers[i+1]; k++){
			
				WT(weight, i, j, k, layers[i+1])+=nodes1[i][j]*nodes2[i+1][k];

			
			}
		
		}
	
	}
}

void adapt_weight(int nlayer, int *layers, double **sweight, double **weight, double theta, int num)
{

	for (int i=0; i<nlayer-1; i++){
	
		for (int j=0; j<layers[i];j++){

			int dest_n=(i==nlayer-1)? layers[i+1] : layers[i+1]-1;
		
			for (int k=0; k<dest_n; k++){
			
				WT(weight, i, j, k, layers[i+1])-=theta*(WT(sweight, i, j, k, layers[i+1])/(double)num);
			
			}
		
		}
	
	}

}



double rand_N01(int n)
{

	static int init=0;
	
	

	if (init==0) {
		//srand the seed with time
		srand( (unsigned)time( NULL ) );
		
		init=1;
	}

	//the n is used to tell the variance of the generated number. 
	//the generate number should be N(0, 1/n).

	//we actually assume the pseudo number from rand() is unique from 0-RAND_MAX

	//if random from 0-x, then the mean=x/2, variance is x^2/12, and use N to calculate mean 
	//will generate X^2/(12xN) as the variance, so x^2/(12XN)=1/n
	

	//I set N=20 
	const int N=20;
	double x=sqrt((double)(12*N)/(double)n);
	

	double sum=0;
	for (int i=0; i<N; i++){
	

		sum+=rand()*x/(double)RAND_MAX;
	
	}

	return sum/N-x/2;



}
