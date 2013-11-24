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
//#include <omp.h>

using namespace std;
#define pair_int pair< int, int >
#define lambda_default .0001
#define iter_default 1000
#define thread_default 10
typedef std::map<pair_int, float>::iterator it_type;

typedef GaloisRuntime::WorkList::dChunkedFIFO<256> WL;

struct comp {
	bool operator() (const pair_int &a, const pair_int &b) {
		return a.second > b.second;
	}
};

struct node {
	int id;
	double w;
	double yx;
};

typedef Galois::Graph::LC_CSR_Graph<node,float> Graph;
typedef Graph::GraphNode GNode;
Graph graph;
vector<int > Y;


struct process {
	template<typename ContextTy>
		void operator()(GNode& src, ContextTy& lwl) {
			double hii = 0;
			double val = 0.0;
			for (Graph::edge_iterator jj = graph.edge_begin(src), ej = graph.edge_end(src); jj != ej; ++jj) {
				if(graph.getEdgeDst(jj) != src){
					val = val + graph.getData(graph.getEdgeDst(jj)).w * graph.getEdgeData(jj);
				}
				else{
					hii = graph.getEdgeData(jj);
				}
			}	

			if(hii != 0){
				graph.getData(src).w = (graph.getData(src).yx - val)/hii;
			}

		}
};


/*Extremely slow for large graphs*/
void print_edges_in_graph(){
	for (Graph::iterator ii = graph.begin(), ei = graph.end(); ii != ei; ++ii) {
		Graph::GraphNode src = *ii;

		for (Graph::edge_iterator jj = graph.edge_begin(src), ej = graph.edge_end(src); jj != ej; ++jj) {
			Graph::GraphNode dst = graph.getEdgeDst(jj);
			double weight = graph.getEdgeData(jj);	
			//			std::cout << "Edge: " << src << " " << dst  << " data "<< weight << std::endl;
		}
	}
}


int main(int argc, char* argv[]) {
	ifstream inFile;
	std::string line;
	int n,d,src,dest,weight;
	float lambda = lambda_default;
	std::map<pair_int, float> X; //transpose of X
	std::map<pair_int, float> H; //transpose of X^TX
	std::vector< node > Graph;
	int threads = thread_default;
	int iter = iter_default;
	//inFile.open("madelon", ifstream::in);
	inFile.open("inputfile", ifstream::in);

	if(argc > 2) {
		lambda = atof(argv[1]);
		iter = atoi(argv[2]);
		threads = atoi(argv[3]);
	}
	if(!inFile.is_open())
	{
		cout << "Unable to open file graph.txt. \nProgram terminating...\n";
		return 0;
	}
	inFile>>n>>d;
	cout << "number of nodes " << n << " and featured " << d <<endl;

	vector<int> maxX;
	Y.resize(n);
	Graph.resize(d);


	maxX.assign(d,0);
	double initial_w = 0;
	int i=0;
	while (i < n)
	{
		inFile >> Y[i];
		int j=0;
		for (;j<d; j++) {
			int k;
			inFile >> k;
			if(k!=0) X[make_pair(i,j)] = k;
			if(k > maxX[j]) maxX[j] = k;
		}
		i++;
	}

	//calculate the \X^T * \X. This approach may not work for large matrices
	for(int i=0; i< n ; i++){
		for(int j=0; j< n; j++){
			for(int k=0; k<d; k++) {
				float tmp = X[make_pair(k,i)] * X[make_pair(k,j)];
				if(tmp!=0) H[make_pair(i,j)] += tmp;
			}
		}
	}

	ofstream myfile;
	myfile.open ("graph_input");
	for(it_type iterator = H.begin(); iterator != H.end(); iterator++) {
		myfile << iterator->first.first << " " << iterator->first.second << " " << iterator->second << endl;
	}
	myfile.close();

	system("../../tools/graph-convert/graph-convert -floatedgelist2gr graph_input graph_output.gr");
	graph.structureFromFile("graph_output.gr");
	print_edges_in_graph();


	for (Graph::iterator ii = graph.begin(), ei = graph.end(); ii != ei; ++ii) {
		GNode src = *ii;
		double temp = 0;
		for(int k=0; k<n; k++){
			temp += Y[k] *  X[make_pair(k,src)];
		}
		graph.getData(src, Galois::NONE).yx = temp;
	}

	int k=0;
	while(k<iter){
		k++;
		Galois::for_each<WL>(graph.begin(), graph.end(), process());
	}

	for (Graph::iterator ii = graph.begin(), ei = graph.end(); ii != ei; ++ii) {
		GNode src = *ii;
		cout <<  "i= " << src <<  " w= " << graph.getData(src).w << endl;
	}
	return 0;
}
