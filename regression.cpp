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

typedef Galois::GAtomicPadded<double> AtomicInteger;
AtomicInteger errorf;
double outerror;
double w_next;
int n,d,src,dest,weight;
float neta;


struct comp {
	bool operator() (const pair_int &a, const pair_int &b) {
		return a.second > b.second;
	}
};

struct node {
	map<int, int > samples;
	double w;
	int id;
};




vector<int > Y;
vector<node > Graph;
struct Process {
	int sn;
	double val;
	Process(int l, double v) { sn = l; val = v;}
	template<typename Context>
		void operator()(node& source, Context& ctx) {
			if  (source.samples.find(sn) != source.samples.end()) {
				Graph[source.id].w = source.w - (double)neta * 2.0 * source.samples[sn] * val;
			}
		}
};

int main(int argc, char* argv[]) {
	Galois::StatManager statManager;
	ifstream inFile;
	std::string line;
	Galois::setActiveThreads(1);
        neta = neta_default; 
        int iter = iter_default;
	//inFile.open("madelon", ifstream::in);
	inFile.open("inputfile", ifstream::in);
        
        if(argc > 2) {
            neta = atof(argv[1]);
            iter = atoi(argv[2]);
        }
	if(!inFile.is_open())
	{
		cout << "Unable to open file graph.txt. \nProgram terminating...\n";
		return 0;
	}
	inFile>>n>>d;
	d++;
        vector<double> maxX;
        maxX.assign(d,0);
	Y.resize(n);
	Graph.resize(d);
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
			Graph[j].id = j;
                        if(k > maxX[j]) maxX[j] = k;
		}
		i++;
	}
	inFile.close();
        //Normalize
        for(int i = 0; i< d; i++) {
            for(int j = 0; j < n; j++) {
                Graph[i].samples[j] /= maxX[i];
                if(i == 0) Graph[i].samples[j] = 0;
            }
        }
	cout << "check" << endl;
	cout << "No .of samples=" << n << " No of features=" << d << endl;
	for (i=0;i<Y.size();i++) cout << Y[i] << "|" ;
	cout << endl;
        cout << "Neta : "<< neta << " Iterations : "<< iter << endl;

	typedef GaloisRuntime::WorkList::LIFO<> WL;
	double val,w_next;
        int k = 0;
        int start_s = clock();
	while (k < iter) {
            k++;
            int j = 0;
		for (;j<n;j++) {
			val = 0 - Y[j];
			for (i=0;i<Graph.size();i++) {
				if  (Graph[i].samples.find(j) != Graph[i].samples.end()) {
					val = val + (Graph[i].w * Graph[i].samples[j]);
				}
			}
			cout << "val=" << val << endl;
			Galois::for_each<WL>(Graph.begin(), Graph.end(), Process(j, val));
			for (i=0;i<Graph.size();i++) cout << "i=" << i << " w=" << Graph[i].w << endl;
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

	for (i=0;i<Graph.size();i++) {
		cout << Graph[i].w << endl;
	}

	return 0;
}
