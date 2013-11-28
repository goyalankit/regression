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
	vector<double> w;
};

int main(int argc, char* argv[]) {
	ifstream inFile;
	std::string line;
	int n,d,src,dest,weight;
	float intercept = 0.0;
        float neta = neta_default;
        int threads = thread_default;
        int iter = iter_default;
        //inFile.open("inputfile", ifstream::in);
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
	vector<double > w;
    	int maxX = 0;
	Y.resize(n);
	Graph.resize(n);
	w.resize(d);
        double initial_w = 0;
	int j=0; 
	while (j < n)
	{
                inFile >> Y[j];
		Graph[j].w.resize(d);
		for (int i=0; i<d; i++) {
			if (j==0) w[i] = initial_w;
			Graph[j].w[i] = initial_w;
			int k;
                        inFile >> k; 
                        if (k!=0) Graph[j].samples[i] = k;
                        if(abs(k) > maxX) maxX = abs(k);
		}
		j++;
	}
        //Normalize
        if(maxX != 0) {
            for(int j = 0; j < n; j++) {
                for(int i = 0; i< d; i++) 
                    if (Graph[j].samples.find(i) != Graph[j].samples.end()) Graph[j].samples[i] /= maxX;
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

        time_t start, end;
        time (&start);
	for (int k = 0; k < iter; k++) {
		for (int i=0; i<Graph.size();i++) {
			double val = 0.0 - Y[i];
			for (int j=0; j<d; j++) {
				if  (Graph[i].samples.find(j) != Graph[i].samples.end()) 
					val = val + w[j] * Graph[i].samples[j];	
			}
			for (int j=0; j<d; j++) {
				if  (Graph[i].samples.find(j) != Graph[i].samples.end()) 
					Graph[i].w[j] -= (double)neta * Graph[i].samples[j] * val;
			}
		}
       		for(int j = 0; j< d; j++) {
                	double mean = 0.0;
                	for (int i = 0; i < Graph.size(); i++) {
                    		mean += Graph[i].w[j];
                	}
                	w[j] = mean/Graph.size();
                }
             	
		double error = 0.0;
             	for (int i = 0; i < n; i++) {
                 	double partError = 0.0 - Y[i];
                 	for(int j = 0; j < d; j++) {
				if  (Graph[i].samples.find(j) != Graph[i].samples.end()) 
                     			partError = partError + Graph[i].samples[j]* w[j];
                 	}
                 	error = error + partError * partError;
             	}
             	error = error * maxX * maxX / n;
             	cout<<"Error : "<<error<<endl;    
	}
    	time (&end);
    	cout << "SGD Completed" << endl;
    	printf ("Elasped time is %.2lf seconds.\n", difftime (end,start) );
	//for (int i=0;i<d;i++) {
	// 	cout << w[i] << endl;
        //}
	
    	return 0;
}
