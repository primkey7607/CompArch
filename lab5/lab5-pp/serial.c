/*--------------------------------------------------------------------*/
/* Optimized sequential Matrix Multiplication. Uses blocked matrix    */
/* to optimize cache misses. Blocking factor set by hit and trial.    */
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
//#define DIM 1024
#define ENTRY 1
//#define BLK   256

int main (int argc, char* argv[]) {
    int DIM;
    int outfl;
    int BLK;
    int i, j, k, kk, jj;
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
    int **A = malloc(DIM * sizeof(int *));
    int **B = malloc(DIM * sizeof(int *));
    int **C = malloc(DIM * sizeof(int *));
    for(i = 0; i < DIM; i++) {
	A[i] = malloc(DIM * sizeof(int));
	B[i] = malloc(DIM * sizeof(int));
	C[i] = malloc(DIM * sizeof(int));
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
   // Multiply
     int sum;
     int en = BLK * (DIM/BLK); /* Amount that fits evenly into blocks */

     for (kk = 0; kk < en; kk += BLK) {
         for (jj = 0; jj < en; jj += BLK) {
             for (i = 0; i < DIM; i++) {
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
   // Multiply
    if(outfl == 1) {
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
