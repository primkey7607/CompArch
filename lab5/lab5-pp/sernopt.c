/* -------------------------------------------------------*/
/* Non-optimized Serial  Matrix Multiplication Program */
/* The algorithm multiplies two N X N matrices.           */
/* -------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
//#define DIM 1024
#define ENTRY 1

//int A[DIM][DIM];
//int B[DIM][DIM];
//int C[DIM][DIM];  // C = A X B

// Intialize matrices A and B with value ENTRY
/*
void initial() {
  int row, col;
   for (row = 0; row < DIM; row++) {
       for (col = 0; col < DIM; col++) {
            A[row][col] = ENTRY;
            B[row][col] = ENTRY;
       }
   }
}
// Matrix Multiplication
void mult() {
   int i;
   int j;
   int k;
  for (i = 0; i < DIM; i++) {
     for (j = 0; j < DIM; j++){
        for(k = 0;k < DIM; k++){
          C[i][j] += A[i][k] * B[k][j];
        }
     }
  }
}
*/


int main(int argc, char *argv[]) {
    int DIM;
    int outfl;
    int BLK;
    int i, j, k;
    if (argc != 4) {
       printf("Usage <matrix dimension> <block size> <(1)produce output or (0)no output>\n");
    }
    else {
      DIM =   atoi(argv[1]);
      BLK   = atoi(argv[2]);
      outfl = atoi(argv[3]);
      if (BLK == 0) {
         printf("Error: block size has to be positive integer \n");
         exit(0);
      }
      if (BLK > DIM) {
         printf("Error: block size cannot be greater than matrix dimension\n");
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
      
    // Initialize
    int row, col;
    for (row = 0; row < DIM; row++) {
       for (col = 0; col < DIM; col++) {
            A[row][col] = ENTRY;
            B[row][col] = ENTRY;
       }
   }
   // Multiply
   for (i = 0; i < DIM; i++) {
      for (j = 0; j < DIM; j++){
         for(k = 0;k < DIM; k++){
            C[i][j] += A[i][k] * B[k][j];
         }
     }
  }
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
