#define min(x,y) (((x) < (y)) ? (x) : (y))

#include <stdio.h>
#include <stdlib.h>
#include "mkl.h"
#include<fstream>
#include<iostream>

using namespace std;

int main()
{
    double *A, *B, *C;
    int m, n, k, i, j;
    double alpha, beta;
    ifstream inFile;

    printf ("\n This example computes real matrix C=alpha*A*B+beta*C using \n"
            " Intel® MKL function dgemm, where A, B, and  C are matrices and \n"
            " alpha and beta are double precision scalars\n\n");

	inFile.open("inputfile", ifstream::in);
	inFile >> n >> m;
	
//    m = 2000, k = 200, n = 1000;
	// A -> n * m AT -> m * n n = 5, m = 4
      k = m;
    printf (" Initializing data for matrix multiplication C=A*B for matrix \n"
            " A(%ix%i) and matrix B(%ix%i)\n\n", m, k, k, n);
    alpha = 1.0; beta = 0.0;

    printf (" Allocating memory for matrices aligned on 64-byte boundary for better \n"
            " performance \n\n");

    double *Y;	
    A = (double *)mkl_malloc( n*m*sizeof( double ), 64 );
    B = (double *)mkl_malloc( k*n*sizeof( double ), 64 );
    C = (double *)mkl_malloc( m*m*sizeof( double ), 64 );
    if (A == NULL || B == NULL || C == NULL) {
      printf( "\n ERROR: Can't allocate memory for matrices. Aborting... \n\n");
      mkl_free(A);
      mkl_free(B);
      mkl_free(C);
      return 1;
    }


	printf("The dimesnstions are M = %d and N = %d", m, n );

	int s=0;
	int cnt = 0;

	//m = 4, n =5
	while (s < n)
	{
		int l;
		inFile >> l;
		int j=0;
		for (;j<m; j++) {
			double k;
			inFile >> k;
			printf(" %d = %f ",cnt, k);
			A[cnt] = k;
			cnt++;
		}
		s++;
	}


    printf (" Intializing matrix data \n\n");
    for (i = 0; i < (m*m); i++) {
        C[i] = 0.0;
    }

    printf (" Computing matrix product using Intel® MKL dgemm function via CBLAS interface \n\n");
	//A = n * m B -> n * m
    cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans, m, m, n, alpha, A, n, A, n, beta, C, m);
    printf ("\n Computations completed.\n\n");

// cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A, k, B, n, beta, C, n);

    printf (" Top left corner of matrix A: \n");
    for (i=0; i<min(n,6); i++) {
      for (j=0; j<min(m,6); j++) {
        printf ("%12.0f", A[j+i*m]);
      }
      printf ("\n");
    }

    printf ("\n Top left corner of matrix B: \n");
/*    for (i=0; i<min(k,6); i++) {
      for (j=0; j<min(n,6); j++) {
        printf ("%12.0f", B[j+i*n]);
      }
      printf ("\n");
    }
  */
  
    printf ("\n Top left corner of matrix C: \n");
    for (i=0; i<min(m,6); i++) {
      for (j=0; j<min(m,6); j++) {
        printf ("%12.5G", C[j+i*m]);
      }
      printf ("\n");
    }

    printf ("\n Deallocating memory \n\n");
    mkl_free(A);
    mkl_free(B);
    mkl_free(C);

    printf (" Example completed. \n\n");
    return 0;
}
