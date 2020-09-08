#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <string.h>
#include <mpi.h>
#include <stdbool.h>

// Function prototype
void WriteToFile(char *pFilename, int *pMatrix, int inRow);

int main()
{
	int n;
	int my_rank;
	int p;
	int *primeArray = NULL;
	struct timespec start, end, startComm, endComm; 
	double time_taken; 

	MPI_Status status;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

    	
    
    
    

	if(my_rank == 0)
	{
		// STEP 1: Only the root process gets the n value
		printf("Enter a n value : ");
        fflush(stdout);
        scanf("%d",&n); 
		
	}

    // Get current clock time.
	 // You can also MPI_Wtime()    
    
	// STEP 2: Broadcast the arrays to all other MPI processess in the group
	clock_gettime(CLOCK_MONOTONIC, &startComm);
	if(my_rank == 0){	
	    clock_gettime(CLOCK_MONOTONIC, &start);
	}
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if(my_rank!=0){
	    
	    clock_gettime(CLOCK_MONOTONIC, &start);
	}
	clock_gettime(CLOCK_MONOTONIC, &endComm);
	
	int i, j,sqrt_i;
	bool prime;
	
	int rpt = n / p; 
	int rptr = n % p; // rpt = rows per thread remainder
	
	int sp = my_rank * rpt; 
	int ep = sp + rpt; 
	if(my_rank == p-1)
		{ep += rptr;}
	int arraySize = ep-sp;
	primeArray = (int*)malloc( arraySize* sizeof(int));
	for (int z = 0; z< (ep-sp); z++){
	    primeArray[z] = 0;
	}
	
	int k =0;
	for( int i =sp ; i<=ep ; i=i+1)
    {
        if(i ==1){ 
        continue;}
        sqrt_i =sqrt(i);
        prime = true;
        for (int j = 2 ; j<= sqrt_i ; j++){
            if(i%j == 0){       //check if the remainder is zero 
                prime = false;
            }}
        if (prime == true){
            primeArray[k++] =  i;     //storing the prime number only in array
           }
    }
	
	
	char str[20];
    snprintf(str, 20, "process_%d.txt", my_rank); // puts string into buffer
    //printf("%s\n", str); // outputs so you can see it
    WriteToFile(str,primeArray,arraySize);
	
	free(primeArray);
	clock_gettime(CLOCK_MONOTONIC, &end); 
	time_taken = (end.tv_sec - start.tv_sec) * 1e9; 
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
	printf("Rank: %d. Overall time (s): %lf\n\n", my_rank, time_taken); // tp
	
	if(my_rank == 0){
	time_taken = (endComm.tv_sec - startComm.tv_sec) * 1e9; 
    time_taken = (time_taken + (endComm.tv_nsec - startComm.tv_nsec)) * 1e-9; 
	printf("Rank: %d. Broadcast time: %lf\n\n", my_rank, time_taken); 
	}
	
	MPI_Finalize();
	return 0;
}



void WriteToFile(char *pFilename, int *pMatrix, int inRow)
{
	int i, j;
	FILE *pFile = fopen(pFilename, "w");
	for(i = 0; i < inRow; i++){
	    if(pMatrix[i] != 0){     // writing only prime number in array into file
		fprintf(pFile, "%d\t", pMatrix[i]);
		fprintf(pFile, "\n");}
	}
	fclose(pFile);
}
