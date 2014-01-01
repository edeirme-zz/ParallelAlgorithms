#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

char **floatalloc2d(int n, int m) {
    char *data = (char *)malloc(n*m*sizeof(char));
    char **array = (char **)calloc(1,n*sizeof(char *));
    int i;
    for (i=0; i<n; i++)
        array[i] = &(data[i*m]);

    return array;
}

char floatfree2d(char **array) {
    free(array[0]);
    free(array);
    return;
}

char chargenerator(){
	char randChar =' ';
	int randNum=0;
	randNum=26*(rand()/(RAND_MAX+1.0));
	randNum=randNum+65;
	randChar= (char) randNum;
	return randChar;
}
/* ... */

//a = floatalloc2d(nrows,ncols);


int main(int argc, char *argv[])
{
	int rank,size, nrows, ncols;
	int j,i,ii,jj,temp,flag=1;
	char **a, **b,tempchar;
	time_t t;
char * sendptr;
 if (argc!= 3)
 {
 	printf("usage: %s numofrows numofcollumns\n",argv[0] );
 	return -1;
 }
 


nrows = atoi(argv[1]);
ncols = atoi(argv[2]);
if(MPI_Init(&argc, &argv) != MPI_SUCCESS)
{
    fprintf(stderr, "Unable to initialize MPI!\n");
    return -1;
}

MPI_Comm_size(MPI_COMM_WORLD,&size);
MPI_Comm_rank(MPI_COMM_WORLD,&rank);
if (rank==0)
 {
 	 if (atoi(argv[1])<=0 || atoi(argv[2])<=0 )
 	{
 		printf("Please enter a positive number\n" );
 		return -1;
 	}
 }
 if(ncols%size != 0 && rank == 0)
	{
    fprintf(stderr,"Number of collumns must be multiple of mpi job size.\n");
    return -1;
	}
/*
while(flag){
	scanf("Set number of rows : %d", &nrows );
	scanf("Set number of collumns : %d",&ncols);
	
	else
	flag=0;
}*/

MPI_Status status;
srand((unsigned) time(&t));
if (rank == 0) {
    a = floatalloc2d(nrows,ncols);
    for ( i = 0; i < nrows; ++i)
    {
    	for (j = 0; j < ncols; ++j)
    	{
    		temp=rand()%2;
    		if (temp==1)
    		{
    			a[i][j]='1';
    		}
    		else
    			a[i][j]='0';
    		
    	}
    }
    
    sendptr = &(a[0][0]);
} else {
    sendptr = NULL;
}
int ncolsperproc = ncols/size;  /* we're assuming this divides evenly */
b = floatalloc2d(nrows, ncolsperproc);

MPI_Datatype acol, acoltype, bcol, bcoltype;

if (rank == 0) {
    MPI_Type_vector(nrows,    
               1,                  
               ncols,         
               MPI_CHAR,       
               &acol);       

     MPI_Type_commit(&acol);
     MPI_Type_create_resized(acol, 0, 1*sizeof(char), &acoltype);
     MPI_Type_commit(&acoltype);
}
MPI_Type_vector(nrows,    
               1,                  
               ncolsperproc,         
               MPI_CHAR,       
               &bcol);       

MPI_Type_commit(&bcol);
MPI_Type_create_resized(bcol, 0, 1*sizeof(char), &bcoltype);
MPI_Type_commit(&bcoltype);

MPI_Scatter (sendptr, ncolsperproc, acoltype, &(b[0][0]), ncolsperproc, bcoltype, 0, MPI_COMM_WORLD);
//////////// PIKIPIKI ///////////////////////


	for ( i = 0; i < nrows; ++i)
	{
		for (j = 0; j< ncolsperproc; ++j)
		{
			if (j==0 && rank>0)
				{
					MPI_Recv(&tempchar,1,MPI_CHAR,rank-1,0,MPI_COMM_WORLD,&status);
					
					if (b[i][j]=='1' && tempchar!='0')
					{
						b[i][j]=tempchar;
						//EDW ANADROMI OLO TON PINAKA GAMO TO FELEKI
						if (j<ncolsperproc-1)
						{
							if (b[i][j+1]!='0')
							{
								b[i][j+1]=tempchar;
								if (i+1<nrows )									
								{
									if (b[i+1][j]!='0')
									{
										b[i+1][j]=tempchar;
									}
									
								}

								if (i>0)
								{
									for ( ii = i; ii > 0; --ii) //ANADROMIKI LOOPA
									{
										for (jj = j; jj < ncolsperproc; ++jj)
										{
											if (b[ii][jj]==tempchar) //AN DEN EINAI KENO TOTE ALLAZEI FASH
											{
												if (jj>0) //koita aristera
												{
													if (b[ii][jj-1]!='0' && b[ii][jj-1]!=tempchar)
													{
														b[ii][jj-1]=tempchar;
													}
												}
												if (jj<ncolsperproc-1) // koitaei dexaa
												{
													if (b[ii][jj+1]!='0' && b[ii][jj+1]!=tempchar)
													{
														b[ii][jj+1]=tempchar;
													}
														
												}
												
												
												if (ii<i-1) //koita katoz
												{
													if (b[ii+1][jj]!='0' && b[ii+1][jj]!=tempchar)
													{
														b[ii+1][jj]=tempchar;

													}
												} 
												if (ii>0)//koitaei pano
												{
													if (b[ii-1][jj]!='0' && b[ii-1][jj]!=tempchar)
													 {
													 	b[ii-1][jj]=tempchar;
													 } 
												}
												
											}
										}
									}
								}
							}
						}
					}
				}
			if (b[i][j]=='1')
			{
				if (i>0)//PANOO//
				{
					if (b[i-1][j]!='0' && b[i-1][j]!='1')
					{
						b[i][j]=b[i-1][j];
						if (j>0)//ariseraa Checakrei ton aristero se periptosh pou exoume katasash KATASTASHS!
						{
							
							if (b[i][j-1]!='0' && b[i][j-1]!='1' && b[i][j-1]!=b[i-1][j])
							{
								b[i][j-1]=b[i-1][j];
							}

						}
					}	
					else if (b[i-1][j]=='1' && b[i][j]=='1')
					{
						b[i][j]=chargenerator();
					}				
				}
				if (j<ncolsperproc-1)//DEXA
				{
					if (b[i][j+1]!='0' && b[i][j+1]!='1')
					{
						b[i][j]=b[i][j+1];
					}
					else if (b[i][j+1]=='1' && b[i][j]=='1')
					{
						b[i][j]=chargenerator();
					}
				}
				if (i<nrows-1) //KATO
				{
					if (b[i+1][j]!='0' && b[i+1][j]!='1')
					{
						b[i][j]=b[i+1][j];
					}
					else if (b[i+1][j]=='1' && b[i][j]=='1')
					{
						b[i][j]=chargenerator();
					}
				}
				if (j>0)//ariseraa
				{
					if (b[i][j-1]!='0' && b[i][j-1]!='1')
					{
						b[i][j]=b[i][j-1];
					}
					else if (b[i][j-1]=='1' && b[i][j]=='1')
					{
						b[i][j]=chargenerator();
					}

				}
				////CHECK DEXA KAI KAT
				if (b[i][j]=='1')
				{
					b[i][j]=chargenerator();
				}
				
				//
			}
			if (j==ncolsperproc-1 && rank<size-1)
				{
					
					MPI_Send(&b[i][j],1,MPI_CHAR,rank+1,0,						
						MPI_COMM_WORLD);					
				}

			
		}
		
	}

	MPI_Barrier(MPI_COMM_WORLD);

if (rank==0)
	{
	
	for ( i = 0; i < nrows; ++i)
	{
		for ( j = 0; j < ncols; ++j)
		{
			fprintf(stderr, "%c", a[i][j],rank);
		}
		fprintf(stderr, "\n" );
	}
		
	fprintf(stderr, "\n" );
	}
MPI_Gather(&b[0][0], ncolsperproc, bcoltype, sendptr, ncolsperproc, acoltype, 0, MPI_COMM_WORLD);
	/////////////////////////TOUROUUOUU/////////////
/*
MPI_Datatype localarray;
int startcol;
startcol=rank*ncolsperproc;
int globalsizes[2]={nrows,ncols};
int localsizes[2]={nrows,ncolsperproc};
int starts[2]={0,startcol};
int order = MPI_ORDER_C;

MPI_Type_create_subarray(2,globalsizes,localsizes,starts,order,bcoltype, &localarray);
MPI_Type_commit(&localarray);
	////
MPI_File file;
MPI_File_open(MPI_COMM_WORLD, "output.txt", MPI_MODE_CREATE|MPI_MODE_WRONLY,MPI_INFO_NULL, &file);
//MPI_File_set_view(file,0,MPI_CHAR,localarray,"native",MPI_INFO_NULL);
MPI_File_write_all(file,b, nrows*ncolsperproc, bcoltype, &status);
MPI_File_close(&file);*/
	/////////SOUUROUOU////

	if (rank==0)
	{
	
	for ( i = 0; i < nrows; ++i)
	{
		for ( j = 0; j < ncols; ++j)
		{
			fprintf(stderr, "%c", a[i][j],rank);
		}
		fprintf(stderr, "\n" );
	}
		
	}
if (rank==0)
{
	MPI_Type_free(&acol);
	MPI_Type_free(&acoltype);
}
MPI_Type_free(&bcol);
MPI_Type_free(&bcoltype);
MPI_Finalize();
    return 0;
}