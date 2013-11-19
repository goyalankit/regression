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
#define iter_default 100
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
    float lambda = lambda_default;
    std::vector<sparse_array>  X_cols,  X_rows;
    std::map<pair_int, double> XT; //transpose of X
    std::map<pair_int, double> H; //transpose of X^TX
    std::vector< node > Graph;
    int threads = thread_default;
    int iter = iter_default;
    inFile.open("inputfile", ifstream::in);

    if(argc > 3) {
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
    X_cols.resize(d);
    X_rows.resize(n);
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

            X_cols[j].add(i,k);
//            X_rows[i].add(j,k);
            Graph[j].w = initial_w; //could be removed outside the loop
            Graph[j].id = j;       //""
            if(k > maxX[j]) maxX[j] = k;
        }
        i++;
    }

    //calculate the \X^T * \X
    for(int i=0; i< d ; i++){
        for(int j=0; j< d; j++){
            for (int k = 0; k < n; k++) {
                H[make_pair(i,j)] = H[make_pair(i,j)] + X_cols[i].values[X_cols[i].idxs[k]] * X_cols[j].values[X_cols[j].idxs[k]];
            }
        }
    }

    //calculating the yx for each node
    for (int ii = 0; ii < d; ii++) {
        double temp = 0;
        for(int k=0; k<n; k++){
            temp += Y[k] * X_cols[ii].values[X_cols[ii].idxs[k]];
        }
        Graph[ii].yx = temp;
    }


    int k=0;
    while(k<iter){
        k++;
        for (int i = 0; i < d; i++) {
            double val = 0;
            for (int m = 0; m < d; m++) {
                val = val + Graph[m].w * H[make_pair(m,i)];
            }
            Graph[i].w = (Graph[i].yx - val)/(H[make_pair(i,i)]);
            cout << "weight for "<< i <<" node" << Graph[i].w << endl;
        }
    }

    for (i=0;i<Graph.size();i++) {
//        cout << Graph[i].w << endl;
    }


    /*
       for(it_type iterator = H.begin(); iterator != H.end(); iterator++) {
       cout << iterator->first.first << ", " << iterator->first.second << " " << iterator->second << endl;
       }
    */
    return 0;
}
