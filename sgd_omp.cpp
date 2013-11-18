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
        float neta = neta_default;
        int threads = thread_default;
        int iter = iter_default;
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
        d++; //Increment d for an empty x (noise)
	vector<int > Y;
	vector<node > Graph;
        vector<int> maxX;
	Y.resize(n);
	Graph.resize(d);
        maxX.assign(d,0);
        double initial_w = 0;
	int i=0; 
	while (i < n)
	{
                inFile >> Y[i];
		for (int j=0; j<d; j++) {
			if(i == 0) Graph[j].w = initial_w;
                        if(j == 0){Graph[j].samples[i] = 0; continue;}
			int k;
			inFile >> k; 
			Graph[j].samples[i] = k;
                        if(k > maxX[j]) maxX[j] = k;
		}
		i++;
	}
        //Normalize
        for(int i = 0; i< d; i++) {
            for(int j = 0; j < n; j++) {
                Graph[i].samples[j] /= maxX[i];
                if(i == 0) Graph[i].samples[j] = 0;
            }
        }
        
	if (i != n) {
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
			val = 0 - Y[j];
			for (i=0;i<Graph.size();i++) {
				if  (Graph[i].samples.find(j) != Graph[i].samples.end()) {
					val = val + (Graph[i].w * Graph[i].samples[j]);
				}
			}
//                        double sum_w = 0.0;
                        #pragma omp parallel for num_threads(threads) 
			for (i=0;i<Graph.size();i++) {
				if  (Graph[i].samples.find(j) != Graph[i].samples.end()) {
					Graph[i].w = Graph[i].w - (double)neta * 2.0 * Graph[i].samples[j] * val;
//                    sum_w += Graph[i].w;
				}
			}
                        //normalize w
//                      for (i=0;i<Graph.size();i++) Graph[i].w /= sum_w;
                        //error calculation
                        if(k== iter && (j > (n-6))) {
                        double error = 0.0;
                        for (int j1 = 0; j1 < n; j1++) {
                            double partError = 0.0 - Y[j1];
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
//	for (i=0;i<Graph.size();i++) {
//		cout << Graph[i].w << endl;
//        }
	
  	return 0;
}
