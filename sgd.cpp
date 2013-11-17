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

using namespace std;
#define pair_int pair< int, int >
#define neta .001
#define iter 1000

struct comp {
    bool operator() (const pair_int &a, const pair_int &b) {
        return a.second > b.second;
    }
};

struct node {
	map<int, double > samples;
	double w;
};

int main(int argc, char* argv[]) {
	ifstream inFile;
	std::string line;
	int n,d,src,dest,weight;
	int file = atoi(argv[1]);
	if (file == 0) 	inFile.open("/scratch/01011/xinsui/graphdata/USA-road-d.USA.w_edgelist", ifstream::in); 
	else if (file == 1) inFile.open("/scratch/01011/xinsui/graphdata/rmat8-2e24.w_edgelist_clean", ifstream::in);
	else if (file == 2) inFile.open("/scratch/01011/xinsui/graphdata/random4-25.w_edgelist",ifstream::in);
	else if (file == 3) inFile.open("USA-road-d.NY.gr",ifstream::in);
	else inFile.open("madelon", ifstream::in); 
        if(!inFile.is_open())
        {
		cout << "Unable to open file graph.txt. \nProgram terminating...\n";
                return 0;
        }
	inFile>>n>>d;
        d++; //Increment d for an empty x (noise)
	vector<int > Y;
	vector<node > Graph;
        vector<int> maxX;
	Y.resize(n);
	Graph.resize(d);
        maxX.assign(d,0);
        double initial_w = 0;
	int i=0; 
	while (inFile >> Y[i])
	{
		for (int j=0; j<d; j++) {
                    if(j == 0){Graph[j].samples[i] = 0; Graph[j].w=initial_w;continue;}
			int k;
			inFile >> k; 
			Graph[j].samples[i] = k;
//                        cout<<k<<"--"<< maxX[j]<<":";
                        if(k > maxX[j]) maxX[j] = k;
			Graph[i].w=initial_w;
		}
		i++;
	}
        //Normalize
        for(int i = 0; i< d; i++) {
//            cout<<maxX[i]<<":";
            for(int j = 0; j < n; j++) {
                Graph[i].samples[j] /= maxX[i];
//                cout << Graph[i].samples[j]<<" ";
                if(i == 0) Graph[i].samples[j] = 0;
            }
        }
        
	if (i != n) {
		cout << "File input error" << endl; return 0;
	}	
	inFile.close();
	cout << "No .of samples=" << n << " No of features=" << d << endl;
	double val,w_next;
        int k = 0;
	while (k < iter) {
            k++;
            int j = 0;
		for (;j<n;j++) {
			val = 0 - Y[j];
			for (i=0;i<Graph.size();i++) {
//				if  (Graph[i].samples.find(j) != Graph[i].samples.end()) {
					val = val + (Graph[i].w * Graph[i].samples[j]);
//				}
			}
//                        double sum_w = 0.0;
			for (i=0;i<Graph.size();i++) {
				if  (Graph[i].samples.find(j) != Graph[i].samples.end()) {
					Graph[i].w = Graph[i].w - (double)neta * 2.0 * Graph[i].samples[j] * val;
//                                        cout<<Graph[i].w<<" ";
//                    sum_w += Graph[i].w;
				}
			}
                        //normalize w
//                        for (i=0;i<Graph.size();i++) Graph[i].w /= sum_w;
                        //error calculation
                        cout<<k<<endl;
                        if(k== iter && (j > (n-5))) {
                        double error = 0.0;
                        for (int j1 = 0; j1 < n; j1++) {
                            double partError = 0.0 - Y[j1];
                            for(int i1 = 0; i1 < d; i1++) {
                                partError = partError + Graph[i1].samples[j1]*Graph[i1].w;
                            }
                            error = error + partError * partError;
                        }
                        error = sqrt(error);
                        cout<<"\nError : "<<error<<endl;
                        if (error < 1e-6) break; 
                        }
        	}
	}
	cout << "SGD Completed" << endl;
	for (i=0;i<Graph.size();i++) {
		cout << Graph[i].w << endl;
        }
	
  	return 0;
}
