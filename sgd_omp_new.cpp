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

// struct node {
//  map<int, double > features;
//  double w;
// };

int main(int argc, char* argv[]) {
    ifstream inFile;
    std::string line;
    int n,d,src,dest,weight;
    float intercept = 0.0;
    float neta = neta_default;
    int threads = thread_default;
    int iter = iter_default;
    // inFile.open("inputfile", ifstream::in);
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
        inFile >> Y[j];
        for (int i=0; i<d; i++) {
            if(j == 0) w[i] = initial_w;
            int k = 1;
            inFile >> k; 
            X[j][i] = k;
            if(abs(k) > maxX) maxX = abs(k);
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
    //converging values: MADELON "***"
    cout << "No .of samples=" << n << " No of features=" << d << endl;
    cout << "Neta : "<< neta << " Iterations : "<< iter << " Threads :"<< threads << endl;

    double w_next;
    time_t start, end;
    time (&start);
    for (int k = 0; k < iter; k++) {

        #pragma omp parallel for num_threads(threads)             
        for (int chunk = 0; chunk < threads; chunk++) {
                int j = rand() % n;
                double val = intercept - Y[j];
            // #pragma omp parallel for reduction(+ : val) num_threads(threads)
            for (map<int, double>::iterator it=X[j].begin(); it!=X[j].end(); ++it) {
                val += (w[it->first] * it->second);
            }
            for (map<int, double>::iterator it=X[j].begin(); it!=X[j].end(); ++it) {
                w[it->first] -= (double)neta * it->second * val;
            }
    }

    double error = 0.0;
    #pragma omp parallel for reduction(+ : error) num_threads(threads)
    for (int j1 = 0; j1 < n; j1++) {
        double partError = intercept - Y[j1];
        for (std::map<int, double>::iterator it=X[j1].begin(); it!=X[j1].end(); ++it)
            partError += w[it->first] * it->second;
        error += partError * partError;
    }
    error = error * maxX * maxX / n;
    cout<<"Error : "<<error<<endl;    
}
    time (&end);
    cout << "SGD Completed" << endl;
    printf ("Elasped time is %.2lf seconds.\n", difftime (end,start) );
    // if(is_intercept) cout << intercept << endl;
    // for (int i=0;i< w.size();i++) {
    //  cout << w[i] << endl;
    //    }
    
    return 0;
}
