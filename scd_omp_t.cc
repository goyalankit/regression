#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <sstream>
#include <ctime>
#include <string>

#include "omp.h"
//#include "cmd_line.h"
//#include "Losses.h"
typedef unsigned int uint;



double logistic_loss(double a, double b) {
  if (a <= 10.0 && a >= -10.0) 
    return(log(1+exp(-a*b)));
  else {
    if (a*b > 0.0) 
      return(0.0);
    else
      return(-a*b);
  }
}

double logistic_loss_grad(double a, double b) {
  if (a <= 10.0 && a >= -10.0) 
    return(-b/(1+exp(a*b)));
  else {
    if (a*b > 0.0)
      return(0.0);
    else
      return(-b);
  }
}


double quadratic_loss(double a, double b) {
  return(0.5*(a-b)*(a-b));
}

double quadratic_loss_grad(double a, double b) {
  return (a-b);
}

double hinge_loss(double a, double b) {
  double margin = 1-a*b;
  if (margin < 0.0) margin = 0.0;
  return(margin);
}

double hinge_loss_grad(double a, double b) {
  double g=0.0;
  if (a*b < 1) g=-b;
  return(g);
}

class Losses {

 public:
 Losses(int loss_type) : 
  type(loss_type), my_loss(&logistic_loss), my_grad_loss(&logistic_loss_grad), rho(0.25) {
    if (type == 1) {
      my_loss = &hinge_loss;
      my_grad_loss = &hinge_loss_grad;
    } else if (type == 2) {
      my_loss = &quadratic_loss;
      my_grad_loss = &quadratic_loss_grad;
      rho = 1;
    } 
  }

  double rho;

  double loss(double a,double b) {
    return((*my_loss)(a,b));
  }

  double loss_grad(double a,double b) {
    return((*my_grad_loss)(a,b));
  }

  ~Losses() { }

 protected:
  int type;
  double (*my_loss)(double,double);  
  double (*my_grad_loss)(double,double);  
};

class IndexValuePair {
public:
	IndexValuePair() { }
	IndexValuePair(uint i, double v) : first(i), second(v) { }

	uint first;
	double second;
};


// a sparse vector is just a vector of index, value pairs
typedef std::vector<IndexValuePair> sparse_vector;

void print_summary(std::vector<double>& innerprod_with_w,
		std::vector<double>& labels,
		std::vector<double>& w,
		int num_examples,
		double lambda,
		Losses& L) {

	double sum=0, mistakes=0;
	double w_norm = 0.0;
	uint w_density = 0;
	int j;

	for (j=-0; j<w.size(); j++) {
		double tmp = fabs(w[j]);
		w_norm += tmp;
		if (tmp > 0.0000001) w_density++;
	}

	for (j=0; j < num_examples; j++) {
		sum += L.loss(innerprod_with_w[j],labels[j]);
		if (labels[j] * (innerprod_with_w[j]) < 0) mistakes++;
	}
	
	sum /= num_examples;
	mistakes /= num_examples;

    double new_error = sum * 999 * 999 * 2;
	std::cout << w_norm << " " << w_density << " "
		<< sum << " " << sum + lambda*w_norm
		<< " " << mistakes << " "  << new_error << std::endl;

}


int main(int argc, char **argv) {

	std::string examples_filename, labels_filename;

	// loss that the user wants to use
	int loss_type;

	// number of iterations for which to run SCD
	int num_iters;

	// print summary every so many iterations;
	int print_me;
        int threads;
	
	// regularization parameter
	double lambda;

	// parse command line
//	learning::cmd_line cmdline;
//#ifdef _GREEDY_
//	cmdline.info("L1 regularized loss minimization using greedy coordinate descent");
//#else
//#ifdef _CYCLIC_
//	cmdline.info("L1 regularized loss minimization using cyclic coordinate descent");
//#else
//	cmdline.info("L1 regularized loss minimization using stochastic coordinate descent");
//#endif
//#endif
//	cmdline.add_master_option("<data-file>",&examples_filename);
//	cmdline.add_master_option("<label-file>",&labels_filename);
//	cmdline.add("-lambda", "regularization parameter (default = 0.001)",&lambda,0.001);
//	cmdline.add("-loss","0 for logistic, 2 for squared (default = 0)",&loss_type,0);
//	cmdline.add("-iters","number of iterations (default = 1000)", &num_iters, 1000);
//	cmdline.add("-printout","after how many iterations to print summary (default 1000)",&print_me,1000);
//
//	int rc = cmdline.parse(argc, argv);
//	if (rc < 2) {
//		cmdline.print_help();
//		return( EXIT_FAILURE );
//	}
        examples_filename = "madelon_columnwise";
        labels_filename = "madelon_columnwise_Y";
        
        if(argc >= 3) {
            lambda = atof(argv[1]);
            loss_type = 2;
            print_me = 20;
            num_iters = atoi(argv[2]);
            threads = atoi(argv[3]);
        }
	// choose seed from current time
	srand(time(NULL));

	// set the loss function
	if (loss_type != 0 && loss_type != 2) {
		std::cerr << "Only logistic and squared losses are supported" << std::endl;
	}

	Losses L(loss_type);

	// labels stored as a vector
	std::vector<double> labels; //Y

	// start reading the examples and labels
	// the examples file should look like :-
	// (m = # of examples, d = dimension)
	//
	// m d
	// i1 v1 i2 v2 i3 v3 ...  <-- first dimension or feature
	// i1 v1 i2 v2 i3 v3 ...  <-- second dimension or feature
	// .
	// .
	// i1 v1 i2 v2 i3 v3 ...  <-- last dimension or feature
	//
	// indices are in the range [0 .. m-1 ]

	// open examples file
	std::ifstream examples_file(examples_filename.c_str());
	if (!examples_file.good()) {
		std::cerr << "Error reading " << examples_filename << std::endl;
		return( EXIT_FAILURE );
	}

	// no. of examples, features in the dataset
	int num_examples, num_features;

	// read the examples file
	std::string buffer;

	getline(examples_file,buffer);
	std::istringstream istr1(buffer);
	istr1 >> num_examples;
	istr1 >> num_features;

	// examples stored feature-wise, each dimension being a sparse vector
	std::vector<sparse_vector> examples(num_features);

	int line_count=0;
	int curr_index;
	double curr_value;

	while (getline(examples_file,buffer) && line_count < num_features) {
		line_count++;

		sparse_vector& curr_feature = examples[line_count-1];

		// read the index, value pairs from buffer
		std::istringstream istr2(buffer);
		while (istr2 >> curr_index) {
			istr2 >> curr_value;

			if (curr_index < 0 || curr_index > num_examples-1) {
				std::cerr << "Illegal index value " << curr_index << " at line " << line_count+1 << " in file " << examples_filename << std::endl;
				return( EXIT_FAILURE );
			}

			// Examples should be normalized so that every feature lies in [-1,1]
			if (curr_value < -1 || curr_value > 1) {
				std::cerr << "Feature number " << curr_index << " exceeds 1 in absolute value at line " << line_count+1 << " in file " << examples_filename << std::endl;
				return( EXIT_FAILURE );
			}
		
			curr_feature.push_back(IndexValuePair(curr_index,curr_value));
		}

	}

	if (line_count < num_features) {
		std::cerr << "Examples file ended before all features were read" << std::endl;
		return( EXIT_FAILURE );
	}

	examples_file.close();


	// open labels file
	std::ifstream labels_file(labels_filename.c_str());
	if (!labels_file.good()) {
		std::cerr << "Error reading " << labels_filename << std::endl;
		return( EXIT_FAILURE );
	}

	double curr_label;
	int label_count=0;

	// read labels file

	while (getline(labels_file,buffer)) {
		std::istringstream istr3(buffer);
		istr3 >> curr_label;
		label_count++;
		labels.push_back(curr_label);
	}
	if (label_count != num_examples) {
		std::cerr << "File " << labels_filename << " has incorrect number of labels" << std::endl;
		return( EXIT_FAILURE );
	}

	labels_file.close();


	// Run the stochastic coordinate descent (SCD) algorithm

	// Initialize weight vector to zero
	std::vector<double> w(num_features,0);
	std::vector<double> innerprod_with_w(num_examples,0);

	// start a clock
	clock_t start = clock();
        time_t start1, end;
        time (&start1);
	clock_t elapsed = 0;

	// number of accesses to the data matrix
	unsigned long long num_accesses=0;

	int num_actual_iters = num_iters / num_features;
	for (int iter=0; iter < num_actual_iters; ++iter) {
		
		// update w
		
		// choose feature to update in a cyclic manner

                #pragma omp parallel for num_threads(16)
		for(int i = 0; i < num_features; i++){
		//i = iter % num_features;

			int curr_index;
			double curr_value;
		double sum=0, g, eta;
		for (int j=0; j < examples[i].size(); j++) {
			IndexValuePair ivpair = examples[i][j];
			curr_index = ivpair.first;
			curr_value = ivpair.second;

			sum += L.loss_grad(innerprod_with_w[curr_index], labels[curr_index]) * curr_value;
		}

		num_accesses += examples[i].size();

		g = sum / num_examples;

		if (w[i] - g / L.rho > lambda / L.rho) 
			eta = -g / L.rho - lambda / L.rho;
		else {
			if (w[i] - g / L.rho < -lambda / L.rho)
				eta = -g / L.rho + lambda / L.rho;
			else
				eta = -w[i];
		}

		// update weight vector w
		w[i] = w[i] + eta;

		// update inner products with w
		for (int j=0; j < examples[i].size(); j++) {
			IndexValuePair ivpair = examples[i][j];
			curr_index = ivpair.first;
			curr_value = ivpair.second;

//#pragma omp atomic
			innerprod_with_w[curr_index]  += eta * curr_value;
		}

		num_accesses += examples[i].size();
//		std::cout << "new iteration " << i << std::endl;
		

	}
               // once in a while print the time elapsed, no. of data accesses and the weight vector
			if (iter % (num_actual_iters/print_me) == 0) {

			elapsed += clock() - start;

			std::cout << ( ((double)elapsed/(double)CLOCKS_PER_SEC)*1000 ) << " " << num_accesses << " ";

			// print the summary (1-norm, density, loss, etc.) 
			print_summary(innerprod_with_w,labels,w,num_examples,lambda,L);

			start = clock();
		}

        }
    time (&end);
            printf ("Elasped time is %.2lf seconds.\n", difftime (end,start1)  );

  //  std::cout << "Actual results" << std::endl;
    
//    for (int i = 0; i < w.size(); i++) {
//        std::cout << i << " -> " << w[i] << std::endl;
//    }
    

	return( EXIT_SUCCESS );
}
