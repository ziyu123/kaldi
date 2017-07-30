#include "utl.h"


extern int gl_total_weights;
extern int gl_total_nodes;
extern int gl_preprocess_initialized;

double* alloc_network_weight(double ** &weight, int nl, int *la)
{

	double *pool=new double[gl_total_weights];
	memset(pool, 0, gl_total_nodes *sizeof(double));

	weight=new double*[nl];

	int off=0;

	for (int i=0; i<nl-1; i++){
	
		weight[i]=pool+off;

		off+=la[i]*la[i+1];

	
	}

	return pool;
}

void print_network_weight(double ** weight, int nl, int *la)
{

	int off=0;
	for (int i=0; i<nl-1; i++){

		printf("WI: %d \n", i);

		for (int j=0; j<la[i];j++){
		
		
			for (int k=0; k<la[i+1]; k++){
			
				printf("  %d->%d : %5.2lf   ", j, k, WT(weight, i, j,k, la[i+1]));
			
			}

			printf ("\n");
		
		}

		printf("\n");


	
	}


}


double *alloc_network_node(double ** &node, int nl, int *la)
{
	double *pool=new double[gl_total_nodes];
	memset(pool, 0, gl_total_nodes *sizeof(double));

	node=new double *[nl];

	int off=0;
	
	for (int i=0; i<nl; i++){
	
		node[i]=pool+off;
		off+=la[i];
	
	}

	return pool;

}

void print_network_node(double **node, int nl , int *la, int simple)
{

	for (int i=0; i<nl; i++){
	
		if ((simple==TRUE)&&(i!=0)&&(i!=nl-1)) continue;

		printf("ND: %d \n", i);

		for (int j=0; j<la[i]; j++){
		
			printf("  %5.2lf ", node[i][j]);
		}

		printf("\n");
	
	}

}





int save_mlp(char *fn, int gl_total_weight, double **weights, int in_num, double *mean, double *var)
{

	FILE *fp=fopen(fn, "wb");

	if (fp==NULL){
	
		printf("open file %s failed\n", fn);
		return -1;
	
	}

	fwrite(mean, sizeof(double), in_num, fp);
	fwrite(var, sizeof(double), in_num, fp);
	fwrite(weights[0], sizeof(double), gl_total_weight, fp);
	fclose(fp);

	return 0;

}

int load_mlp(char *fn, int gl_total_weight, double **weights, int in_num, double *mean, double *var)
{

	FILE *fp=fopen(fn, "rb");

	if (fp==NULL){
	
		printf("open file %s failed\n", fn);
		return -1;
	
	}

	fread(mean, sizeof(double), in_num, fp);
	fread(var, sizeof(double), in_num, fp);
	fread(weights[0], sizeof(double), gl_total_weight, fp);
	

	fclose(fp);


	gl_preprocess_initialized=1;

	return 0;

}


int read_pattern(FILE *fp, int inode_n, double *inodes,  int onode_n, double *onodes)
{

	char buf[5000], *p;

	while ((p=fgets(buf, 5000, fp))!=NULL){

		if ((buf[0]=='/') && (buf[1]=='/')|| (buf[0]=='#')) continue;
		if ((buf[0]=='\r')||(buf[0]=='\n')) continue;

		break;

	}

	if (p==NULL) return 0;

	//using strtok go achieve serious of tokens


        while ((*p ==' ') || (*p=='\t')) p++;
	for (int j=0; j<inode_n-1; j++){
		if (j==0)
			p=strtok(buf, " \t");
		else
			p=strtok(NULL, " \t");

		if (p==NULL){
			//null is wrong
			printf("ERROR: read_pattern: not enough data\n");
			return -1;
		}

		sscanf(p, "%lf", inodes+j);


	}

	inodes[inode_n-1]=1.0; //the last one is biased
	
	for (int j=0; j<onode_n; j++){

		p=strtok(NULL, " \t");

		if (p==NULL){
				//null is wrong
				printf("ERROR: read_pattern: not enough data, buf=%s\n", buf);
				return -1;
			}

			sscanf(p, "%lf", onodes+j);

	}
	
	
#ifdef _DEBUG_2_
        printf("IN::");
		for (int j=0; j< inode_n; j++){
		  printf("%5.3lf ", inodes[j]);
		}

		printf("\n");
		printf("OUT::");
		for (int j=0;j< onode_n;j++){
			printf("%5.3lf ", onodes[j]);
			
		}

		printf("\n");


		
#endif

	
	
	return 1;
}

int preprocess_pattern(FILE *fp, int inode_n, double *inodes, double *mean, double *var)
{

	if (gl_preprocess_initialized ==0) {
		printf("gl_preprocess_initialized unenabled!");
		return -1;
	}
	for (int i=0; i<inode_n-1; i++){
	
		inodes[i]=(inodes[i]-mean[i])*var[i];
	
	}

	return 0;
}
