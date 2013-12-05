#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <cmath>
#include <vector>
#include <map>
#include <queue>
#include <sys/time.h>
#include <omp.h>

using namespace std;

int main()
{
    double *A, *B, *C;
    int m, n, k, i, j;
    double alpha = 1.0, beta = 0.0;
    ifstream inFile;
        string line;

        inFile.open("mnist", ifstream::in);
    getline(inFile, line);
    istringstream iss(line);
    iss>>n>>m;
	
      k = m;
    alpha = 1.0; beta = 0.0;

    	vector<double> Y;
	vector<map<int, double> > X;
	Y.resize(n);
	X.resize(m);

//    A = (double *)mkl_malloc( n*m*sizeof( double ), 64 );
//    // B = (double *)mkl_malloc( k*n*sizeof( double ), 64 );
//    C = (double *)mkl_malloc( m*m*sizeof( double ), 64 );
//    if (A == NULL || C == NULL) {
//      printf( "\n ERROR: Can't allocate memory for matrices. Aborting... \n\n");
//      mkl_free(A);
//      mkl_free(C);
//      return 1;
//    }


//	printf("The dimesnstions are M = %d and N = %d", m, n );

	int s=0;
	int cnt = 0;
	//m = 4, n =5
	while (s < n)
	{
//            for (int j=0;j<m; j++) A[s*m + j] = 0;
            getline(inFile, line); string k;
            istringstream iss(line);
            iss >> Y[s];
            while(iss >> k) {
                size_t pos = k.find(":");
                X[atoi((k.substr(0,pos)).c_str())][s] = atof((k.substr(pos+1)).c_str());
            }
            s++;
	}
//        for(int i = 0; i < n; i++) cout<<Y[i]<<endl;
        for(int j = 0; j < m; j++) {
            for (map<int, double>::iterator it=X[j].begin(); it!=X[j].end(); ++it) {
               cout<< it->first << " " << it->second;   
            }
            cout<<endl;
        }

//    printf (" Intializing matrix data \n\n");
//    for (i = 0; i < (m*m); i++) {
//        C[i] = 0.0;
//    }

//    printf (" Computing matrix product using Intel® MKL dgemm function via CBLAS interface \n\n");
	//A = n * m B -> n * m
//    cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans, m, m, n, alpha, A, m, A, m, beta, C, m);
//    printf ("\n Computations completed.\n\n");

// cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A, k, B, n, beta, C, n);

//    printf (" Top left corner of matrix A: \n");
//    for (i=0; i<min(n,6); i++) {
//      for (j=0; j<min(m,6); j++) {
//        printf ("%12.0f", A[j+i*m]);
//      }
//      printf ("\n");
//    }

//    printf ("\n Top left corner of matrix B: \n");
/*    for (i=0; i<min(k,6); i++) {
      for (j=0; j<min(n,6); j++) {
        printf ("%12.0f", B[j+i*n]);
      }
      printf ("\n");
    }
  */
  
//    printf ("\n Top left corner of matrix C: \n");
//    for (i=0; i<min(m,6); i++) {
//      for (j=0; j<min(m,6); j++) {
//        printf ("%12.5G", C[j+i*m]);
//      }
//      printf ("\n");
//    }
//
////    printf ("\n Deallocating memory \n\n");
//    mkl_free(A);
//    // mkl_free(B);
//    mkl_free(C);

//    printf (" Example completed. \n\n");
    return 0;
}

