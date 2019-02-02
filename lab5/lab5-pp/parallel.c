/*------------------------------------------------------------------------------------------*/
/* Non-optimized parallel program to multiply two square matrices using 4 Posix threads.    */
/*  parallel program to multiply two nxn matrices- matrix_mul_pthreads.c                    */
/*------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define NUMTHREAD 4
#define ENTRY 1
//int a[n][n],b[n][n],c[n][n];
static int **A;
static int **B;
static int **C;
int DIM = 0;
int BLK  = 0;
 
void *thread_mult(void *slice) {
    int *mysl =(int *) slice;
    int lb = *mysl;
    int ub  = lb + (DIM/NUMTHREAD);
    int bdim = ub - lb+1;
    int en = BLK * (DIM/BLK);   //Amount that fits evenly into blocks
    int sum = 0;
    int i, j, k, jj, kk;

     for (kk = 0; kk < en; kk += BLK) {
         for (jj = 0; jj < en; jj += BLK) {
             for (i = lb; i < ub; i++) {
                 for (j = jj; j < jj + BLK; j++) {
                     sum = C[i][j];
                     for (k = kk; k < kk + BLK; k++) {
                         sum += A[i][k]*B[k][j];
                     }
                     C[i][j] = sum;
                 }
            }
        }
     }
}
// Main
int main(int argc, char* argv[]) {
    
    int outfl;
    pthread_t thread_id[NUMTHREAD];
    int matslice[NUMTHREAD];
    int i = 0;
    int j = 0; 
    int k = 0;

    // Process inputs
    if (argc != 4) {
       printf("Usage <matrix dimension> <block size> <(1)produce output or (0)no output>\n");
    }
    else {
      DIM =   atoi(argv[1]);
      BLK =   atoi(argv[2]);
      outfl = atoi(argv[3]);
      if (BLK == 0) {
         printf("Error: block size has to be positive integer \n");
         exit(0);
      }
      if (BLK > DIM) {
         printf ("Error: block size cannot be larger than matrix dimension\n");
         exit(0);
      }
    }
    A = (int **)malloc(DIM * sizeof(int *));
    B = (int **)malloc(DIM * sizeof(int *));
    C = (int **)malloc(DIM * sizeof(int *));
    for(i = 0; i < DIM; i++) {
	A[i] = (int*) malloc(DIM * sizeof(int));
	B[i] = (int*) malloc(DIM * sizeof(int));
	C[i] = (int*) malloc(DIM * sizeof(int));
    }
    i = 0;
    // Initialize
    int row, col;
    for (row = 0; row < DIM; row++) {
       for (col = 0; col < DIM; col++) {
            A[row][col] = ENTRY;
            B[row][col] = ENTRY;
       }
   }
   // Create 4 threads and pass the matrix slice.
   for (i = 0; i < NUMTHREAD; i++) {
      matslice[i] = i * (DIM / NUMTHREAD);
      pthread_create(&thread_id[i], NULL, thread_mult, &matslice[i]);
   }
   // join the 4 thread to terminate multiplication  
   for(i = 0; i < NUMTHREAD; i++)
      pthread_join(thread_id[i],NULL);

    if(outfl == 1) {     // Print output
       printf("Product matrix \n;");
       int i, j;
       for(i = 0; i < DIM; i++) {
            for(j = 0;j < DIM; j++) {
               printf(" %d",C[i][j]);
            }
            printf("\n");  // End of row
       }
    }
 
}
