#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <sstream>
#include <ctime>
#include <string>
#include <omp.h>
#include<sys/time.h>

#include "omp.h"
using namespace std;

int PRINT=20;

struct timeval start, end;

class node_pair {
public:
	node_pair(int i, double v) : first(i), second(v) { }
	int first; double second;
};

int main(int argc, char **argv) {
	
	string examples_filename, labels_filename;
	int loss_type, num_iters, threads;	
	double lambda;

        //examples_filename = "/scratch/02683/prat0318/mnist_columnwise";
        //labels_filename = "/scratch/02683/prat0318/mnist_columnwise_Y";
        
        examples_filename = "/scratch/02683/prat0318/Day1_columnwise";
        labels_filename = "/scratch/02683/prat0318/Day1_columnwise_Y";

        if(argc >= 3) {
            lambda = atof(argv[1]);
            loss_type = 2;
            num_iters = atoi(argv[2]);
            threads = atoi(argv[3]);
        }
	// labels stored as a vector
	std::vector<double> labels; //Y

	std::ifstream examples_file(examples_filename.c_str());

	// no. of examples, features in the dataset
	int num_examples, num_features;

	// read the examples file
	std::string buffer;

	getline(examples_file,buffer);
	std::istringstream istr1(buffer);
	istr1 >> num_examples;
	istr1 >> num_features;

	// examples stored feature-wise, each dimension being a sparse vector
	std::vector< std::vector<node_pair> > examples(num_features);

	int line_count=0;
	int curr_index;
	double curr_value;
        double maxX = 0.0;
	while (getline(examples_file,buffer) && line_count < num_features) {
		line_count++;
		vector<node_pair> &curr_feature = examples[line_count-1];

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
            for (int j=0; j < examples[i].size(); j++) {
                    node_pair *ivpair = &examples[i][j];
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

	std::vector<double> w(num_features,0);
	std::vector<double> innerprod_with_w(num_examples,0);

	//to lock a single element in array
	omp_lock_t lock[num_examples];
	//initialize locks
	for (int i=0; i<num_examples; i++)
		omp_init_lock(&(lock[i]));

	std::cout << "Running for " << num_features << " features and  " << num_examples << " examples" << std::endl;

        //time_t start1, end;
        //time (&start1);
	gettimeofday(&start, NULL); //start time of the actual page rank algorithm

	int num_actual_iters = num_iters / num_features;
	for (int iter=0; iter < num_actual_iters; ++iter) {
		
                #pragma omp parallel for num_threads(threads)
		for(int i = 0; i < num_features; i++){
		//i = iter % num_features;

			int curr_index;
			double curr_value;
		double sum=0, g, eta;
		for (int j=0; j < examples[i].size(); j++) {
			node_pair ivpair = examples[i][j];
			curr_index = ivpair.first;
			curr_value = ivpair.second;

			sum += (innerprod_with_w[curr_index] - labels[curr_index]) * curr_value;
		}

		g = sum / num_examples;
//		std::cout << "lambda " << lambda << endl;
		if (w[i] - g > lambda) 
			eta = -g - lambda;
		else {
			if (w[i] - g < -lambda)
				eta = -g + lambda;
			else
				eta = -w[i];
		}

		// update weight vector w
		w[i] = w[i] + eta;

		// update inner products with w
		for (int j=0; j < examples[i].size(); j++) {
			node_pair ivpair = examples[i][j];
			curr_index = ivpair.first;
			curr_value = ivpair.second;

			//omp_set_lock(&(lock[curr_index]));
			#pragma omp atomic
			innerprod_with_w[curr_index]  += eta * curr_value;
			//omp_unset_lock(&(lock[curr_index]));

		}
	}
               // once in a while print the time elapsed, no. of data accesses and the weight vector
                if (iter % (num_actual_iters/PRINT) == 0) {
                        double err = 0.0;
                      	for (int j=0; j < num_examples; j++) 
                        	err += (pow(innerprod_with_w[j] - labels[j], 2.0)/2);
                        err = err * maxX * maxX * 2 / num_examples;
                        cout<<err<<endl;
		}

        }

	//destroy the lock
	 for (int i=0; i<num_examples; i++)
		 omp_destroy_lock(&(lock[i]));
 gettimeofday(&end, NULL); //page rank ends here
    //time (&end);
	 cout << "Time taken by parallel execution with openmp is on " << threads << "threads is " <<  (((end.tv_sec  - start.tv_sec) * 1000000u +  end.tv_usec - start.tv_usec) / 1.e6) << endl;      
//      printf ("Elapsed time is %.2lf seconds.\n", difftime (end,start1)  );
    //    for (int i = 0; i < w.size(); i++) {
//        std::cout << i << " -> " << w[i] << std::endl;
//    }

}
