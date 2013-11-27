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

struct comp {
    bool operator() (const pair_int &a, const pair_int &b) {
        return a.second > b.second;
    }
};

struct node {
	map<int, double > samples;
	double w;
};

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
	vector<float > Y;
	vector<node > Graph;
    int maxX = 0;
	Y.resize(n);
	Graph.resize(d);
        // maxX.assign(d,0);
        double initial_w = 0;
	int j=0; 
	while (j < n)
	{
                inFile >> Y[j];
		for (int i=0; i<d; i++) {
			if(j == 0) Graph[i].w = initial_w;
                        int k = 1;
                        inFile >> k; 
                        Graph[i].samples[j] = k;
                        if(abs(k) > maxX) maxX = abs(k);
		}
		j++;
	}
        //Normalize
    if(maxX != 0) {
        for(int j = 0; j < n; j++) {
            for(int i = 0; i< d; i++) 
                Graph[i].samples[j] /= maxX;
            Y[j] /= maxX;
        }
        cout<< "Factor :" << maxX << endl;
    }
        
	if (j != n) {
		cout << "File input error" << endl; return 0;
	}	
	inFile.close();
	cout << "No .of samples=" << n << " No of features=" << d << endl;
        cout << "Neta : "<< neta << " Iterations : "<< iter << " Threads :"<< threads << endl;
	double w_next;
    int start_s = clock();
    double** weights = (double **)malloc(sizeof(double *)*threads);
    for(int y =0 ; y < threads; y++) weights[y] = (double *)malloc(sizeof(double)*d);
        int chunk_size = n/threads;
	for (int k = 0; k < iter; k++) {
        #pragma omp parallel for num_threads(threads)             
		for (int chunk = 0; chunk < threads; chunk++) {
        // j = k % n; {
            for(int y = 0; y< d; y++) weights[chunk][y] = Graph[y].w;
            double *val = (double *)malloc(sizeof(double) * chunk_size);
            for(int j = chunk*chunk_size; j < (chunk+1)*chunk_size; j++) {
                int j1 = j - chunk*chunk_size;
			    val[j1] = intercept - Y[j];
            // #pragma omp parallel for reduction(+ : val) num_threads(threads)
			for (int i=0; i<Graph.size(); i++) {
                // cout<<val[j1]<<" "<<weights[chunk][i]<<" "<<Graph[i].samples[j]<<endl;
				if  (Graph[i].samples.find(j) != Graph[i].samples.end()) 
					val[j1] = val[j1] + (weights[chunk][i] * Graph[i].samples[j]);
			}
            // cout<<j<<endl;
        } for(int j = chunk*chunk_size; j < (chunk+1)*chunk_size; j++) {
            int j1 = j - chunk*chunk_size;
            // if(is_intercept) intercept = intercept - (double)neta * val;
            
            // #pragma omp parallel for num_threads(threads) 
			for (int i=0; i < Graph.size(); i++) {
				if  (Graph[i].samples.find(j) != Graph[i].samples.end())
					weights[chunk][i] -= (double)neta * Graph[i].samples[j] * val[j1];
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
        Graph[y].w = mean/threads;
    }

    double error = 0.0;
    #pragma omp parallel for reduction(+ : error) num_threads(threads)
    for (int j1 = 0; j1 < n; j1++) {
        double partError = intercept - Y[j1];
        for(int i1 = 0; i1 < d; i1++) {
            partError = partError + Graph[i1].samples[j1]*Graph[i1].w;
        }
        error = error + partError * partError;
    }
    error = error * maxX * maxX / n;
    cout<<"Error : "<<error<<endl;    
}
	cout << "SGD Completed" << endl;
        cout << "Time taken: " << (clock()-start_s)/double(CLOCKS_PER_SEC)*1000 << " ms." << endl;
    // if(is_intercept) cout << intercept << endl;
	// for (int i=0;i<Graph.size();i++) {
	// 	cout << Graph[i].w << endl;
 //       }
	
  	return 0;
}
