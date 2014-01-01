

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mpi.h>
#include<math.h>
#define ARRAY_SIZE 10
double Log2(double n)
{
	return log(n)/log(2);
}
int main(int argc, char *argv[])
{
int rank;
int size;

if(MPI_Init(&argc, &argv) != MPI_SUCCESS)
{
    fprintf(stderr, "Unable to initialize MPI!\n");
    return -1;
}
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);
if(ARRAY_SIZE % size != 0 && rank == 0)
{
    fprintf(stderr,"Array size must be multiple of mpi job size.\n");
    return -1;
}

MPI_Status status;
int * x = (int *) malloc(sizeof(int) * ARRAY_SIZE);

int * array2 = (int *) malloc(sizeof(int) * ARRAY_SIZE);

int * max_array = (int *) malloc(sizeof(int) * ARRAY_SIZE);
int * chunk = (int *) malloc(sizeof(int) * ARRAY_SIZE/size);
int i = 0,j=0,i2=0,j2=0,y2=0;

for (i=0; i<ARRAY_SIZE; i++){
x[i]=0;
}
int total_sum = 0;
signed int array[ARRAY_SIZE]={-2,10,4,5,6,-20,40,-3,15,-1};

signed int rev_array[ARRAY_SIZE];
for(i = 0; i < ARRAY_SIZE; i++)
{
  
	x[i]=-1;
	array2[i]=-1;
	total_sum= total_sum+array[i];
}
MPI_Comm comm;
MPI_Barrier(MPI_COMM_WORLD);
MPI_Scatter(array,ARRAY_SIZE/size,MPI_INT,chunk,ARRAY_SIZE/size,MPI_INT,0,MPI_COMM_WORLD);
int sum = 0;
int temp = 0;
int key = 1;
int max=0,maxnum=-1;
int d=0;
int root=0;
int chunksize=ARRAY_SIZE/size;
	int n=0,y=0;

	//for (i=ARRAY_SIZE-1, d=0;i>=0;i--,d++){
	for(i=0; i<ARRAY_SIZE;i++){		
		//rev_array[i]=array[ARRAY_SIZE-i-1];
		//*(rev_array+d)= *(array+i);
	rev_array[i]=array[i];
	array2[i]=array[i];
	max_array[i]=-1;

}

i=rank;
MPI_Barrier(MPI_COMM_WORLD);
/////////////////// PREFIX SUM///////////////
for(j=0;j<log2(ARRAY_SIZE);j++){

	i=rank;
	if(i>=pow(2,j)){	
	y= (int)floor(pow(2,j));
	array[i]=array[i]+array[i-y];	
	}
	MPI_Barrier(MPI_COMM_WORLD);	
	if(i!=0){
		MPI_Send(&array[i], 1, MPI_INT, root,0,MPI_COMM_WORLD);}
	else{
		for (y=1; y<size; y++){
		MPI_Recv(&array[y], 1, MPI_INT,y,0,MPI_COMM_WORLD,&status);
		
		}
		}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&array,size,MPI_INT,root,MPI_COMM_WORLD);	

	}
/*
FIND MAX of ARRAY// WE NEED IT PARALLEL!
*/
for (i=1; i<ARRAY_SIZE; i++)
{if (array[i]>maxnum){
	max=i;
	maxnum=array[i];
for (j=0;j<i;j++)
	max_array[j]=array2[j];	
}
}

/// PREFIX SUM FOR THE REST//

for (d=0;d<ARRAY_SIZE-1;d++){
	for(i=0;i<=d;i++)				
	rev_array[i]=0;	
	
		for(j2=0;j2<log2(ARRAY_SIZE);j2++){

		i2=rank;
		if(i2>=pow(2,j2)){
	
		y2= (int)floor(pow(2,j2));
		rev_array[i2]=rev_array[i2]+rev_array[i2-y2];
	
		}
		MPI_Barrier(MPI_COMM_WORLD);
	
		if(i2!=0){
			MPI_Send(&rev_array[i2], 1, MPI_INT, root,0,MPI_COMM_WORLD);}
		else{
			for (y2=1; y2<size; y2++){
			MPI_Recv(&rev_array[y2], 1, MPI_INT,y2,0,MPI_COMM_WORLD,&status);
		
			}
			}
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Bcast(&rev_array,size,MPI_INT,root,MPI_COMM_WORLD);
//WE NEED PARALLEL MAX !!
			for (j=0; j<ARRAY_SIZE; j++)
	{
		if (rev_array[j]>maxnum){
		max=j;
		maxnum=rev_array[j];
		for(j2=0;j2<ARRAY_SIZE;j2++){
			max_array[j2]=0;}
		for (j2=d;j2<=j;j2++)
			{
			if(rev_array[j2]!=0){
			
			max_array[j2]=array2[j2];	
			//fprintf(stderr,"%d,",array2[j2]);
}
			}
			//fprintf(stderr,"\n");
}
		}
		}
//REVERSE ARRAY TO STARTING POSITION//
	if( d<ARRAY_SIZE-2)
	for(j=0;j<ARRAY_SIZE;j++){
	rev_array[j]=array2[j];}}

i=rank;
MPI_Barrier(MPI_COMM_WORLD);
///////////////////// SUFFIX SUM ( PREFIX SUM ON REVERSED ARRAY ) //////////////
/*

for(j=0;j<log2(ARRAY_SIZE);j++){

	i=rank;
	if(i>=pow(2,j)){	
	y= (int)floor(pow(2,j));
	rev_array[i]=rev_array[i]+rev_array[i-y];
	fprintf(stderr, "%d PIPIPI %d\n", j,rev_array[i]);
}
	MPI_Barrier(MPI_COMM_WORLD);
	
	if(i!=0){
		MPI_Send(&rev_array[i], 1, MPI_INT, root,0,MPI_COMM_WORLD);
}
	else{
		for (y=1; y<size; y++){
		MPI_Recv(&rev_array[y], 1, MPI_INT,y,0,MPI_COMM_WORLD,&status);
		//rev_array[y]=x[y];
		//	if (j==0){
		//	array2[0]=rev_array[0];
		//	array2[y]=x[y];}
		}
		}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&rev_array,size,MPI_INT,root,MPI_COMM_WORLD);
	}	*/
/*////////////////////////////

SUFFIX MAXIMA OF PREFIX SUM
//////////////////////////// */
	i=rank;	
MPI_Barrier(MPI_COMM_WORLD);
if(rank == size-1)
{
fprintf(stderr,"Prefix sum maximum subsequence : ");
for(i=0;i<ARRAY_SIZE;i++){
  fprintf(stderr," %d", max_array[i]);}
  fprintf(stderr,"\n");  
}
if(rank == 0)
{
fprintf(stderr,"Maximum subsequence prefix sum is : %d \n", maxnum);
}

free(x);
free(chunk);
free(array2);
free(max_array);
MPI_Finalize();
return 0;

}
