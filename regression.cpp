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

struct SNode{
        unsigned int data;
};

typedef Galois::Graph::LC_CSR_Graph<SNode,void> Graph;
Graph graph;

int main(int argc, char **argv){
	graph.structureFromFile("/work/02681/ankitg/Galois-2.1.8/inputs/structured/torus5.gr");

	//Initialize the component ids of the nodes with the node number
	for (Graph::iterator ii = graph.begin(), ei = graph.end(); ii != ei; ++ii) {
		Graph::GraphNode src = *ii;
		graph.getData(src, Galois::NONE).data = src;
	}
	return 0;
}
