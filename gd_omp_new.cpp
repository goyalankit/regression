#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <sstream>
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

// struct node {
// 	map<int, double > features;
// 	double w;
// };

int main(int argc, char* argv[]) {
	ifstream inFile;
	std::string line;
	int n,d,src,dest,weight;
    float intercept = 0.0;
    float neta = neta_default;
    int threads = thread_default;
    int iter = iter_default;
    int show_errors = 1;
    // inFile.open("inputfile", ifstream::in);
    char* filename = "madelon"; // "inputfile"; //"madelon";
        
        if(argc > 3) {
            neta = atof(argv[1]);
            iter = atoi(argv[2]);
            threads = atoi(argv[3]);
            if(argc > 4) show_errors = atoi(argv[4]);
            if(argc > 5) filename = argv[5];
        }
    inFile.open(filename, ifstream::in);

    if(!inFile.is_open())
        {
		cout << "Unable to open file graph.txt. \nProgram terminating...\n";
                return 0;
        }

    getline(inFile, line);
    istringstream iss(line);
    iss>>n>>d;
	// inFile>>n>>d;
	vector<double> Y;
	vector<map<int, double> > X;
    vector<double> w;

    int maxX = 0;
	Y.resize(n);
	X.resize(n);
    w.resize(d);
        // maxX.assign(d,0);
    double initial_w = 0;
	int j=0; 
    //j -> sample, i -> feature
	while (j < n)
	{
        getline(inFile, line);
        istringstream iss(line);
        iss >> Y[j]; string k; int i = 0;
        // for (int i=0; i<d; i++) {
        while(iss >> k) {
            // if(j == 0) w[i] = initial_w;
            // int k = 1;
            // inFile >> k; 
            if(strcmp(filename, "mnist") == 0) {
                size_t pos = k.find(":");
                X[j][atoi((k.substr(0,pos)).c_str())] = atof((k.substr(pos+1)).c_str());
                maxX = 255;
            }
            else {   
                if(atoi(k.c_str()) != 0) X[j][i] = atoi(k.c_str());
                if(abs(atoi(k.c_str())) > maxX) maxX = abs(atoi(k.c_str()));
            }
            i++;
		}
		j++;
	}

    if (j != n) {
        cout << "File input error" << endl; return 0;
    }   

    //Normalize
    if(maxX != 0) {
        for(int j = 0; j < n; j++) {
            for(int i = 0; i< d; i++) 
                X[j][i] /= maxX;
            Y[j] /= maxX;
        }
        cout<< "Factor :" << maxX << endl;
    }
        
	inFile.close();
    //converging values: MADELON "0.000001 50 1", "0.0001 50 32"
	cout << "No .of samples=" << n << " No of features=" << d << endl;
    cout << "Neta : "<< neta << " Iterations : "<< iter << " Threads :"<< threads << endl;

	double w_next;
    struct timeval start, end;
    gettimeofday(&start, NULL); //start time of the actual algorithm
    double** weights = (double **)malloc(sizeof(double *)*threads);

    for(int y =0 ; y < threads; y++) weights[y] = (double *)malloc(sizeof(double)*d);
    int chunk_size = n/threads;
	for (int k = 0; k < iter; k++) {

        #pragma omp parallel for num_threads(threads)             
		for (int chunk = 0; chunk < threads; chunk++) {
            for(int y = 0; y < d; y++) weights[chunk][y] = w[y];
            double *val = (double *)malloc(sizeof(double) * chunk_size);
            
            for(int j = chunk*chunk_size; j < (chunk+1)*chunk_size; j++) {
                int j1 = j - chunk*chunk_size;
			    val[j1] = intercept - Y[j];
            // #pragma omp parallel for reduction(+ : val) num_threads(threads)
            for (map<int, double>::iterator it=X[j].begin(); it!=X[j].end(); ++it) {
			// for (int i=0; i<Graph.size(); i++) {
				val[j1] += (w[it->first] * it->second);
			}
        /*BELOW TWO LINES ARE COMMENTED - else Error varies with no. of threads */
        // } for(int j = chunk*chunk_size; j < (chunk+1)*chunk_size; j++) {
        //     int j1 = j - chunk*chunk_size;
            for (std::map<int, double>::iterator it=X[j].begin(); it!=X[j].end(); ++it) {
			// for (int i=0; i < Graph.size(); i++) {
    			weights[chunk][it->first] -= (double)neta * it->second * val[j1];
			}
        }
        free(val);
    }
    #pragma omp parallel for num_threads(threads) 
    for(int y = 0; y< d; y++) {
        double mean = 0.0;
        for (int chunk = 0; chunk < threads; chunk++) {
            mean += weights[chunk][y];
        }
        w[y] = mean/threads;
    }
    if(show_errors > 0) {
        double error = 0.0;
        #pragma omp parallel for reduction(+ : error) num_threads(threads)
        for (int j1 = 0; j1 < n; j1++) {
            double partError = intercept - Y[j1];
            for (std::map<int, double>::iterator it=X[j1].begin(); it!=X[j1].end(); ++it)
            // for(int i1 = 0; i1 < d; i1++) {
                partError += w[it->first] * it->second;
            // }
            error += partError * partError;
        }
        error = error * maxX * maxX / n;
        cout<<"Error : "<<error<<endl;    
    }
}
    
    
            double error = 0.0;
        #pragma omp parallel for reduction(+ : error) num_threads(threads)
        for (int j1 = 0; j1 < n; j1++) {
            double partError = intercept - Y[j1];
            for (std::map<int, double>::iterator it=X[j1].begin(); it!=X[j1].end(); ++it)
            // for(int i1 = 0; i1 < d; i1++) {
                partError += w[it->first] * it->second;
            // }
            error += partError * partError;
        }
        error = error * maxX * maxX / n;
        cout<<"Error : "<<error<<endl;   
        
        
    gettimeofday(&end, NULL); 
	cout << "SGD Completed" << endl;
    printf ("Elasped time is %.4lf seconds.\n", (((end.tv_sec  - start.tv_sec) * 1000000u +  end.tv_usec - start.tv_usec) / 1.e6) );
    // if(is_intercept) cout << intercept << endl;
	// for (int i=0;i<Graph.size();i++) {
	// 	cout << Graph[i].w << endl;
 //       }
	
  	return 0;
}
