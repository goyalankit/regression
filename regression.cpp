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
#define neta 0.1

typedef Galois::GAtomicPadded<double> AtomicInteger;
AtomicInteger errorf;
double val,w_next;
int n,d,src,dest,weight;


struct comp {
	bool operator() (const pair_int &a, const pair_int &b) {
		return a.second > b.second;
	}
};

struct node {
	map<int, int > samples;
	double w;
};




vector<int > Y;
vector<node > Graph;

struct Process {
	Process() { }
	template<typename Context>
		void operator()(node& source, Context& ctx) {
			int i = 0;
			double error, val, w_next;

			for (int j=0;j<n;j++) {
				error =0;
				val = 0 - Y[j];
				for (i=0;i<Graph.size();i++) {
					if  (Graph[i].samples.find(j) != Graph[i].samples.end()) {
						val = val + (Graph[i].w * Graph[i].samples[j]);
						cout << "found" << endl;
					}
					cout << "val=" << val << endl;
				}
				for (i=0;i<Graph.size();i++) {
					if  (Graph[i].samples.find(j) != Graph[i].samples.end()) {
						w_next = Graph[i].w - neta * 2 * Graph[i].samples[j] * val;
						error = error + w_next - Graph[i].w;
						Graph[i].w = w_next;
						cout << "w_next=" << w_next << endl;
					}
				}
				errorf = AtomicInteger(error);
//				if (errorf < AtomicInteger(1e-6)) break;
			}
		}
};

int main(int argc, char* argv[]) {
	Galois::StatManager statManager;
	ifstream inFile;
	std::string line;

	Galois::setActiveThreads(16);
	int file = atoi(argv[1]);
	if (file == 0) 	inFile.open("/scratch/01011/xinsui/graphdata/USA-road-d.USA.w_edgelist", ifstream::in); 
	else if (file == 1) inFile.open("/scratch/01011/xinsui/graphdata/rmat8-2e24.w_edgelist_clean", ifstream::in);
	else if (file == 2) inFile.open("/scratch/01011/xinsui/graphdata/random4-25.w_edgelist",ifstream::in);
	else if (file == 3) inFile.open("USA-road-d.NY.gr",ifstream::in);
	else inFile.open("inputfile", ifstream::in); 
	if(!inFile.is_open())
	{
		cout << "Unable to open file graph.txt. \nProgram terminating...\n";
		return 0;
	}
	inFile>>n>>d;

	Y.resize(n);
	Graph.resize(d);
	int i=0;

	while (inFile >> Y[i])
	{
		for (int j=0; j<d; j++) {
			//inFile >> Graph[i][j];
			int k;
			inFile >> k; 
			if (k!=0) Graph[j].samples[i] = k;
			Graph[i].w=0;
		}
		i++;
	}

	if (i != n) {
		cout << "File input error" << endl; return 0;
	}	
	inFile.close();

	cout << "No .of samples=" << n << " No of features=" << d << endl;

	for (i=0;i<Y.size();i++) cout << Y[i] << "|" ;
	cout << endl;

	for (i=0;i<Graph.size();i++) {
		cout << "Node =" << i << " ||||| ";
		for (std::map<int, int>::iterator it=Graph[i].samples.begin(); it!=Graph[i].samples.end(); ++it) {
			std::cout << it->first << " has " << it->second << " | ";
		}
		cout << endl;
	}


	errorf = 100;

	while (errorf > AtomicInteger(1e-6))) {
		Galois::for_each(Graph.begin(), Graph.end(), Process());
	}

	cout << "SGD Completed" << endl;

	for (i=0;i<Graph.size();i++) {
		cout << Graph[i].w << endl;
	}

	return 0;
}
