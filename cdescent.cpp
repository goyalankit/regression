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
#define neta_default .0001
#define iter_default 10
#define thread_default 10
typedef std::map<pair_int, double>::iterator it_type;

struct comp {
    bool operator() (const pair_int &a, const pair_int &b) {
        return a.second > b.second;
    }
};

struct node {
    map<int, double > samples;
    double w;
};

struct sparse_array {
    std::vector<unsigned int> idxs;
    std::vector<double> values;

    int length() {
        return idxs.size();
    }
    void add(unsigned int _idx, double value) {
        idxs.push_back(_idx);
        values.push_back(value);
    }
};

int main(int argc, char* argv[]) {
    ifstream inFile;
    std::string line;
    int n,d,src,dest,weight;
    float neta = neta_default;
    //std::map<pair_int, double> XT; //transpose of X
    std::vector<sparse_array>  X_cols,  X_rows;
    std::map<pair_int, double> XT; //transpose of X
    std::map<pair_int, double> H; //transpose of XXt
    int threads = thread_default;
    int iter = iter_default;
    inFile.open("inputfile", ifstream::in);

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
    cout << "number of nodes " << n << " and featured " << d <<endl;

    vector<int > Y;
    vector<int> maxX;
    Y.resize(n);
    X_cols.resize(d);
    X_rows.resize(n);


    maxX.assign(d,0);
    double initial_w = 0;
    int i=0;
    while (i < n)
    {
        inFile >> Y[i];
        for (int j=0; j<d; j++) {
            int k;
            inFile >> k;

            X_cols[j].add(i,k);
            X_rows[i].add(j,k);

            //			X[make_pair(i,j)] = k;
            //			XT[make_pair(j,i)] = k;
        }
        i++;
    }


    for(int i=0; i< n ; i++){
        for(int j=0; j< n; j++){
            for (int k = 0; k < d; k++) {
                H[make_pair(i,j)] = H[make_pair(i,j)] + X_rows[i].values[X_rows[i].idxs[k]] * X_rows[j].values[X_rows[j].idxs[k]];
            }
        }
    }


    	for(it_type iterator = H.begin(); iterator != H.end(); iterator++) {
            cout << iterator->first.first << ", " << iterator->first.second << " " << iterator->second << endl;
        }

    //	cout << "The vertex 2,3 "<< X[make_pair(4,3)] << " in transpose " << XT[make_pair(3,4)] << endl;
    //	for(it_type iterator = X.begin(); iterator != X.end(); iterator++) {
    //		H[make_pair(iterator->first.first,iterator->first.second)] = X[make_pair(iterator->first.first, iterator->first.second)] * XT[make_pair(iterator->first.first, iterator->first.second)];
    //	}



    return 0;
}
