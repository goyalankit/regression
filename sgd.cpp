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
#define neta 0.1

struct comp {
    bool operator() (const pair_int &a, const pair_int &b) {
        return a.second > b.second;
    }
};

struct node {
	map<int, int > samples;
	int w;
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
	else inFile.open("inputfile", ifstream::in); 
        if(!inFile.is_open())
        {
		cout << "Unable to open file graph.txt. \nProgram terminating...\n";
                return 0;
        }
	inFile>>n>>d;
	
	vector<int > Y;
	vector<node > Graph;
	Y.resize(n);
	Graph.resize(d);
	int i=0;
	while (inFile >> Y[i])
	{
		for (int j=0; j<d; j++) {
			//inFile >> Graph[i][j];
			int k;
			inFile >> k; 
			if (k!=0) Graph[i].samples[j] = k;
			Graph[i].w=0;
		}
		i++;
	}
	if (i != n) {
		cout << "File input error" << endl; return 0;
	}	
	inFile.close();
	cout << "No .of samples=" << n << " No of features=" << d << endl;
	for (i=0;i<Y.size();i++) cout << Y[i] << "|" ;
	cout << endl;
	for (i=0;i<Graph.size();i++) {
		cout << "Node =" << i << " ||||| ";
		for (std::map<int, int>::iterator it=Graph[i].samples.begin(); it!=Graph[i].samples.end(); ++it) {
                	std::cout << it->first << " has " << it->second << " | ";
		}
		cout << endl;
        }
	int error;
	while (error > 1e-6) {
		for (int j=0;j<n;j++) {
			
			for (i=0;i<Graph.size();i++) {
				Graph[i].w = Graph[i].w - neta * 
			//calculate
			}
			
        	}
	}
  	return 0;
}
