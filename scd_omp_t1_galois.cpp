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

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <sstream>
#include <ctime>
#include <string>

using namespace std;

int PRINT=20;

class node_pair {
public:
	node_pair(int i, double v) : first(i), second(v) { }
	int first; double second;
};

int num_examples, num_features;
//std::vector< std::vector<node_pair> > examples;
std::vector<double> labels; //Y

std::vector<double> w;
std::vector<double> innerprod_with_w;
double lambda;

struct node {
	std::vector<node_pair> examples;
	double w;
	int id;
};

vector<node> Graph;
typedef GaloisRuntime::WorkList::LIFO<> WL;

struct Process {
	int sn;
	double val;
	Process(int l, double v) { sn = l; val = v;}
	template<typename Context>
		void operator()(node& source, Context& ctx) {
			int curr_index;
			double curr_value;
			double sum=0, g, eta;
			for (int j=0; j < source.examples.size(); j++) {
				node_pair ivpair = source.examples[j];
				curr_index = ivpair.first;
				curr_value = ivpair.second;

				sum += (innerprod_with_w[curr_index] - labels[curr_index]) * curr_value;
			}

			g = sum / num_examples;
			if (w[source.id] - g > lambda) 
				eta = -g - lambda;
			else {
				if (w[source.id] - g < -lambda)
					eta = -g + lambda;
				else
					eta = -w[source.id];
			}

			// update weight vector w
			w[source.id] = w[source.id] + eta;

			// update inner products with w
			for (int j=0; j < source.examples.size(); j++) {
				node_pair ivpair = source.examples[j];
				curr_index = ivpair.first;
				curr_value = ivpair.second;

				innerprod_with_w[curr_index]  += eta * curr_value;

			}
		}
};


int main(int argc, char **argv) {

	string examples_filename, labels_filename;
	int loss_type, num_iters, threads;	

        examples_filename = "madelon_columnwise";
        labels_filename = "madelon_columnwise_Y";
        
        if(argc >= 3) {
            lambda = atof(argv[1]);
            loss_type = 2;
            num_iters = atoi(argv[2]);
            threads = atoi(argv[3]);
        }

	std::ifstream examples_file(examples_filename.c_str());


	// read the examples file
	std::string buffer;

	getline(examples_file,buffer);
	std::istringstream istr1(buffer);
	istr1 >> num_examples;
	istr1 >> num_features;

//	examples.resize(num_examples);
	Graph.resize(num_features);
	w.resize(num_features,0);
	innerprod_with_w.resize(num_examples,0);

	int line_count=0;
	int curr_index;
	double curr_value;
        double maxX = 0.0;
	while (getline(examples_file,buffer) && line_count < num_features) {
		line_count++;
		vector<node_pair> &curr_feature = Graph[line_count-1].examples;

		Graph[line_count - 1].id = line_count - 1;
		// read the index, value node_pairs from buffer
		std::istringstream istr2(buffer);
		while (istr2 >> curr_index) {
			istr2 >> curr_value;
                        if(maxX < curr_value) maxX = curr_value;		
			curr_feature.push_back(node_pair(curr_index,curr_value));
		}

	}
	examples_file.close();
        for(int i = 0; i < num_features; i++)
            for (int j=0; j < Graph[i].examples.size(); j++) {
                    node_pair *ivpair = &Graph[i].examples[j];
                    ivpair->second /= maxX;
            }

	// open labels file
	std::ifstream labels_file(labels_filename.c_str());
	double curr_label;

	// read labels file
	while (getline(labels_file,buffer)) {
		std::istringstream istr3(buffer);
		istr3 >> curr_label;
		labels.push_back(curr_label/maxX);
	}
	labels_file.close();

        time_t start1, end;
        time (&start1);

	int num_actual_iters = num_iters / num_features;
	for (int iter=0; iter < num_actual_iters; ++iter) {
		
		Galois::for_each<WL>(Graph.begin(), Graph.end(), Process(iter, iter));
		
                if (iter % (num_actual_iters/PRINT) == 0) {
                        double err = 0.0;
                      	for (int j=0; j < num_examples; j++) 
                        	err += (pow(innerprod_with_w[j] - labels[j], 2.0)/2);
                        err = err * maxX * maxX * 2 / num_examples;
                        cout<<err<<endl;
		}

        }

    time (&end);
            printf ("Elapsed time is %.2lf seconds.\n", difftime (end,start1)  );
    //    for (int i = 0; i < w.size(); i++) {
//        std::cout << i << " -> " << w[i] << std::endl;
//    }

}
