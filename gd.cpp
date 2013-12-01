#include "Galois/Galois.h"
#include "Galois/Accumulator.h"
#include "Galois/Bag.h"
#include "Galois/Statistic.h"
#include "Galois/UnionFind.h"
#include "Galois/Graphs/LCGraph.h"
#include "Galois/ParallelSTL/ParallelSTL.h"
#include "llvm/Support/CommandLine.h"
#include "Galois/Runtime/WorkList.h"

#include "Lonestar/BoilerPlate.h"
#include<sys/time.h>

#include <utility>
#include <algorithm>
#include <iostream>
#include "Galois/Statistic.h"

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

using namespace std;
#define pair_int pair< int, int >
#define neta_default .0001
#define iter_default 10
#define thread_default 10
#define is_intercept false

int n,d,src,dest,weight;
float neta;
float intercept = 0.0;

struct comp {
    bool operator() (const pair_int &a, const pair_int &b) {
        return a.second > b.second;
    }
};

struct node {
	map<int, double > samples;
	vector<double> w;
	int id;
};

struct feature {
	double value;
	int id;
};

vector<double > Y;
vector<node > Graph;
vector<feature> w;

struct Process {
	Process(){}
	template<typename Context>
		void operator()(node& source, Context& ctx) {
			double val = 0.0 - Y[source.id];
			for (int j=0; j<d; j++) {
				if  (source.samples.find(j) != source.samples.end()) 
					val = val + w[j].value * source.samples[j];	
			}
			for (int j=0; j<d; j++) {
				if  (source.samples.find(j) != source.samples.end()) 
					Graph[source.id].w[j] -= (double)neta * source.samples[j] * val;
			}
	
		}
};

struct Process1 {
	Process1(){}
	template<typename Context>
		void operator()(feature& source, Context& ctx) {
                	double mean = 0.0;
                	for (int i = 0; i < Graph.size(); i++) {
                    		mean += Graph[i].w[source.id];
                	}
                	w[source.id].value = mean/Graph.size();
                }
};

int main(int argc, char* argv[]) {
	Galois::StatManager statManager;
	ifstream inFile;
	std::string line;
        neta = neta_default; 
	int threads = thread_default;
        int iter = iter_default;
	inFile.open("madelon", ifstream::in);
	//inFile.open("inputfile", ifstream::in);
        
        if(argc > 3) {
            neta = atof(argv[1]);
            iter = atoi(argv[2]);
	    threads = atoi(argv[3]);
        }
	Galois::setActiveThreads(threads);
        if(!inFile.is_open())
        {
		cout << "Unable to open file graph.txt. \nProgram terminating...\n";
                return 0;
        }
	inFile>>n>>d;
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
		Graph[j].id = j;
		for (int i=0; i<d; i++) {
			if (j==0) {
				w[i].value = initial_w;
				w[i].id = i;
			}
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

	inFile.close();
	cout << "No .of samples=" << n << " No of features=" << d << endl;
        cout << "Neta : "<< neta << " Iterations : "<< iter << endl;
	
	//typedef GaloisRuntime::WorkList::LIFO<> WL;
	typedef GaloisRuntime::WorkList::ChunkedFIFO<128> WL;
	time_t start, end;
        time (&start);

	for (int k = 0; k < iter; k++) {
 		Galois::for_each<WL>(Graph.begin(), Graph.end(), Process());
 		Galois::for_each<WL>(w.begin(), w.end(), Process1());

		//for (int j = 0; j< d; j++) {
                //	double mean = 0.0;
                //	for (int i = 0; i < Graph.size(); i++) {
                //    		mean += Graph[i].w[j];
                //	}
                //	w[j] = mean/Graph.size();
                //}
             	
		//double error = 0.0;
             	//for (int i = 0; i < n; i++) {
                // 	double partError = 0.0 - Y[i];
                // 	for(int j = 0; j < d; j++) {
		//		if  (Graph[i].samples.find(j) != Graph[i].samples.end()) 
                //     			partError = partError + Graph[i].samples[j]* w[j].value;
                // 	}
                // 	error = error + partError * partError;
             	//}
             	//error = error * maxX * maxX / n;
             	//cout<<"Error : "<<error<<endl;
    	}
    	time (&end);

	cout << "SGD Completed" << endl;
	printf ("Elasped time is %.2lf seconds.\n", difftime (end,start) );
	for (int i=0;i<d;i++) {
		cout << w[i].value << endl;
        }
	
  	return 0;
}
