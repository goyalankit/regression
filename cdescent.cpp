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
#define iter_default 10
#define thread_default 10
typedef std::map<pair_int, double>::iterator it_type;

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

int main(int argc, char* argv[]) {
    ifstream inFile;
    std::string line;
    int n,d,src,dest,weight;
    float lambda = lambda_default;
    std::map<pair_int, double> X; //transpose of X
    std::map<pair_int, double> H; //transpose of X^TX
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

    vector<int > Y;
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
            Graph[j].w = initial_w; //could be removed outside the loop
            Graph[j].id = j;       //""
            if(k > maxX[j]) maxX[j] = k;
        }
        i++;
    }

    //for (int i = 0; i < d; i++) {
    //    for (int j = 0; j < n; j++) {
    //    	if(maxX[j]!=0) X[make_pair(i,j)] /= maxX[j];
    //    }
    //}


    //calculate the \X^T * \X. This approach may not work for large matrices
    for(int i=0; i< n ; i++){
        for(int j=0; j< n; j++){
            for(int k=0; k<d; k++) {
                double tmp = X[make_pair(k,i)] * X[make_pair(k,j)];
                if(tmp!=0) H[make_pair(i,j)] += tmp;
            }
        }
    }

    //    std::cout << "size of H "<< H.size() << std::endl;
    //calculating the yx for each node
    for (int ii = 0; ii < d; ii++) {
        double temp = 0;
        for(int k=0; k<n; k++){
            temp += Y[k] *  X[make_pair(k,ii)];
        }
        Graph[ii].yx = temp;
	cout << temp << endl;
    }

    int k=0;
    while(k<iter){
        k++;
        for (int i = 0; i < d; i++) {
            double val = 0.0;
            for (int m = 0; m < d; m++) {
		if (m != i) val = val + Graph[m].w * H[make_pair(m,i)];
            }
            if(H[make_pair(i,i)]!=0) Graph[i].w = (Graph[i].yx - val)/(H[make_pair(i,i)]);
            //    cout << "weight for "<< i <<" node" << Graph[i].w << endl;
            if(k== iter && i>d-2){
                double error = 0.0;
                for(int j1=0; j1<n ; j1++){
                    double part_error = 0.0 - Y[j1];
                    for (int i1 = 0; i1 < d; i1++) {
                        part_error = part_error + (Graph[i1].w * X[make_pair(i1,j1)]);
                    }
                    error = error + part_error * part_error;
                }
                error = sqrt(error);
                std::cout << "Error: "<< error << std::endl;
                if(error<1e-9) break;
            }
        }
    }

    for (i=0;i<Graph.size();i++) {
        cout << Graph[i].w << endl;
    }

 //     for(it_type iterator = H.begin(); iterator != H.end(); iterator++) {
 //       cout << iterator->first.first << ", " << iterator->first.second << " " << iterator->second << endl;
 //       }

  return 0;
}
