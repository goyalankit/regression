#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cmath>
#include <vector>
#include <map>
#include <queue>
#include <sys/time.h>
#include <omp.h>

using namespace std;
#define pair_int pair< int, int >
#define neta_default .0001
#define iter_default 10
#define thread_default 10
#define is_intercept false

// struct node {
// 	map<int, double > features;
// 	double w;
// };

int main(int argc, char* argv[]) {
	ifstream inFile;
	std::string line;
	int n,d,src,dest,weight;
    float intercept = 0.0;
    float neta = neta_default;
    int threads = thread_default;
    int iter = iter_default;
    // inFile.open("inputfile", ifstream::in);
    inFile.open("madelon", ifstream::in);
        
        if(argc > 3) {
            neta = atof(argv[1]);
            iter = atoi(argv[2]);
            threads = atoi(argv[3]);
        }
        if(!inFile.is_open())
        {
		cout << "Unable to open file graph.txt. \nProgram terminating...\n";
                return 0;
        }
	inFile>>n>>d;
	vector<double> Y;
	vector<map<int, double> > X;
    vector<double> w;

    int maxX = 0;
	Y.resize(n);
	X.resize(n);
    w.resize(d);
        // maxX.assign(d,0);
    double initial_w = 0;
	int j=0; 
    //j -> sample, i -> feature
	while (j < n)
	{
        inFile >> Y[j];
		for (int i=0; i<d; i++) {
    		if(j == 0) w[i] = initial_w;
            int k = 1;
            inFile >> k; 
            X[j][i] = k;
            if(abs(k) > maxX) maxX = abs(k);
		}
		j++;
	}

    if (j != n) {
        cout << "File input error" << endl; return 0;
    }   

    //Normalize
    if(maxX != 0) {
        for(int j = 0; j < n; j++) {
            for(int i = 0; i< d; i++) 
                X[j][i] /= maxX;
            Y[j] /= maxX;
        }
        cout<< "Factor :" << maxX << endl;
    }
        
	inFile.close();
    //converging values: MADELON "0.000001 50 1", "0.0001 50 32"
	cout << "No .of samples=" << n << " No of features=" << d << endl;
    cout << "Neta : "<< neta << " Iterations : "<< iter << " Threads :"<< threads << endl;

	double w_next;
    time_t start, end;
    time (&start);
    double** weights = (double **)malloc(sizeof(double *)*threads);

    for(int y =0 ; y < threads; y++) weights[y] = (double *)malloc(sizeof(double)*d);
    int chunk_size = n/threads;
	for (int k = 0; k < iter; k++) {

        #pragma omp parallel for num_threads(threads)             
		for (int chunk = 0; chunk < threads; chunk++) {
            for(int y = 0; y < d; y++) weights[chunk][y] = w[y];
            double *val = (double *)malloc(sizeof(double) * chunk_size);
            
            for(int j = chunk*chunk_size; j < (chunk+1)*chunk_size; j++) {
                int j1 = j - chunk*chunk_size;
			    val[j1] = intercept - Y[j];
            // #pragma omp parallel for reduction(+ : val) num_threads(threads)
            for (map<int, double>::iterator it=X[j].begin(); it!=X[j].end(); ++it) {
			// for (int i=0; i<Graph.size(); i++) {
				val[j1] += (w[it->first] * it->second);
			}
        /*BELOW TWO LINES ARE COMMENTED - else Error varies with no. of threads */
        // } for(int j = chunk*chunk_size; j < (chunk+1)*chunk_size; j++) {
        //     int j1 = j - chunk*chunk_size;
            for (std::map<int, double>::iterator it=X[j].begin(); it!=X[j].end(); ++it) {
			// for (int i=0; i < Graph.size(); i++) {
    			weights[chunk][it->first] -= (double)neta * it->second * val[j1];
			}
        }
        free(val);
    }
    #pragma omp parallel for num_threads(threads) 
    for(int y = 0; y< d; y++) {
        double mean = 0.0;
        for (int chunk = 0; chunk < threads; chunk++) {
            mean += weights[chunk][y];
        }
        w[y] = mean/threads;
    }

    double error = 0.0;
    #pragma omp parallel for reduction(+ : error) num_threads(threads)
    for (int j1 = 0; j1 < n; j1++) {
        double partError = intercept - Y[j1];
        for (std::map<int, double>::iterator it=X[j1].begin(); it!=X[j1].end(); ++it)
        // for(int i1 = 0; i1 < d; i1++) {
            partError += w[it->first] * it->second;
        // }
        error += partError * partError;
    }
    error = error * maxX * maxX / n;
    cout<<"Error : "<<error<<endl;    
}
    time (&end);
	cout << "SGD Completed" << endl;
    printf ("Elasped time is %.2lf seconds.\n", difftime (end,start) );
    // if(is_intercept) cout << intercept << endl;
	// for (int i=0;i<Graph.size();i++) {
	// 	cout << Graph[i].w << endl;
 //       }
	
  	return 0;
}
