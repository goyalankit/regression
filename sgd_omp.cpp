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
	inFile.open("inputfile", ifstream::in);
        
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
	vector<int > Y;
	vector<node > Graph;
        vector<int> maxX;
	Y.resize(n);
	Graph.resize(d);
        maxX.assign(d,0);
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
                        if(k > maxX[i]) maxX[i] = k;
		}
		j++;
	}
        //Normalize
//         for(int i = 0; i< d; i++) {
//             for(int j = 0; j < n; j++) {
//                 Graph[i].samples[j] /= maxX[i];
// //                if(i == 0) Graph[i].samples[j] = 0;
//             }
//         }
        
	if (j != n) {
		cout << "File input error" << endl; return 0;
	}	
	inFile.close();
	cout << "No .of samples=" << n << " No of features=" << d << endl;
        cout << "Neta : "<< neta << " Iterations : "<< iter << " Threads :"<< threads << endl;
	double val,w_next;
    int k = 0;
    int start_s = clock();
	while (k < iter) {
                k++;
//                #pragma omp parallel for num_threads(threads)             
		for (int j = 0; j<n; j++) {
			val = intercept - Y[j];
			for (int i=0;i<Graph.size();i++) {
				if  (Graph[i].samples.find(j) != Graph[i].samples.end()) {
					val = val + (Graph[i].w * Graph[i].samples[j]);
				}
			}
            if(is_intercept) intercept = intercept - (double)neta * val / n;
            float max_w = 0.0;
            #pragma omp parallel for num_threads(threads) 
			for (int i=0;i<Graph.size();i++) {
				if  (Graph[i].samples.find(j) != Graph[i].samples.end()) {
					Graph[i].w = Graph[i].w - (double)neta * Graph[i].samples[j] * val / n;
                    if(Graph[i].w > max_w) max_w = Graph[i].w;
				}
			}
                        //normalize w
                    if(max_w > 10000) {
                     if(j == 0) cout << "Factor: "<< max_w;
                     for (int i=0;i<Graph.size();i++) Graph[i].w /= max_w;
                 }
                        //error calculation
                        if(k== iter && (j > (n-6))) {
                        double error = 0.0;
                        for (int j1 = 0; j1 < n; j1++) {
                            double partError = intercept - Y[j1];
                            for(int i1 = 0; i1 < d; i1++) {
                                partError = partError + Graph[i1].samples[j1]*Graph[i1].w;
                            }
                            error = error + partError * partError;
                        }
                        error = sqrt(error);
                        cout<<"Error : "<<error<<endl;
                        if (error < 1e-6) break; 
                        }
        	}
	}
	cout << "SGD Completed" << endl;
        cout << "Time taken: " << (clock()-start_s)/double(CLOCKS_PER_SEC)*1000 << " ms." << endl;
    // if(is_intercept) cout << intercept << endl;
	// for (int i=0;i<Graph.size();i++) {
	// 	cout << Graph[i].w << endl;
 //       }
	
  	return 0;
}
