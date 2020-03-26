#include "RG.h"
#include <iomanip>

using namespace std;

void printNP(vector<vector<int> > p){
	for(int i=0; i<p.size(); i++){
		for(int j=0; j<p[i].size(); j++){
			if(p[i][j]!=0)
				cout << "NP[" << i << "][" << j << "] = " << p[i][j]  << endl;
		}
	}
}

void printCache(vector<vector<long long> > Cache){
	for(int i=0; i<Cache.size(); i++){
		for(int j=0; j<Cache[i].size(); j++){
			if(Cache[i][j]!=-1)
				cout << "Cache[" << i << "][0x" << hex << j << "] = " << dec << Cache[i][j]  << endl;
		}
	}
}


int RainbowGraph::countWays(vector <int> color, vector <int> a, vector <int> b){
	int i,j,csize;
	Color = color;
	int setid=0;	
	csize = color.size();

	for(i=0; i<Color.size(); i++){
		setid |= 1 << Color[i];
	}

	/*Init NP */
	NP.resize(csize, vector<int>(csize));
	for(i=0;i<csize;i++){
		for(j=0;j<csize;j++){
			NP[i][j]=0;
		}
	}

	/*Populate Cnodes */
	CNodes.resize(10);
	for(i=0;i<color.size(); i++){
		CNodes[color[i]].push_back(i);
	}

	/* Setup Same and Diff */
	Same.resize(csize);
	Diff.resize(csize);
	for(i=0;i<a.size();i++){
		if(color[a[i]] == color[b[i]]){
			Same[a[i]].push_back(b[i]);
			Same[b[i]].push_back(a[i]);
		}
		else if(color[a[i]] != color[b[i]]){
			Diff[a[i]].push_back(b[i]);
			Diff[b[i]].push_back(a[i]);
		}
	}

	V.resize(csize, 0);

	/*for each node, countPaths, populate NP */
	for(i=0;i<csize;i++){
		NIP=0;
		Source = i;
		CountPaths(Source);
		V.assign(V.size(),0);
	}
	
	/*init cache */
	Cache.resize(Color.size());
	for(i=0;i<Cache.size(); i++){
		Cache[i].resize(1024,-1);
	}

	/*count num walks at each node, disregaring its own compartment */
	long long count=0;
	for(i=0; i<csize; i++){
		count += NumWalks(i,setid - (1 << Color[i]));
	}

	if(Verbose=="N")printNP(NP);
	if(Verbose=="C")printCache(Cache);
	return count;
}

/*DFS, augmented to allow reuse of nodes once an end is found */
void RainbowGraph::CountPaths(int n){
	int i;
	NIP++;
	V[n]=1;
	/*if find Target */
	if(NIP == CNodes[Color[n]].size()){
		NP[Source][n] += 1;
	}
	for(i=0; i<Same[n].size(); i++){
		if(V[Same[n][i]] == 0){
			CountPaths(Same[n][i]);
		}
	}
	V[n]=0;
	NIP--;
}

/* count number of walks from node, only using sets found in setid */
long long RainbowGraph::NumWalks(int node, int setid){
	int i,j,m,l;
	int n = node;
	int s = setid;
	/*basic cache, if present, dont do recursion */
	if(Cache[n][s] != -1){
		return Cache[n][s];
	}

	/*Base case, count all NP and return */
	if(s == 0){
		long long count = 0;
		for(m=0; m<NP[n].size(); m++){
			count += NP[n][m];
		}
		return count;
	}

	long long ret=0;
	for(i=0; i<CNodes[Color[n]].size(); i++){
		/*save current node in same compartment */
		m = CNodes[Color[n]][i];
		if(NP[n][m] > 0){
			for(j=0; j<Diff[m].size(); j++){
				/* save node in different compartment */
				l = Diff[m][j];
				/*if in remaining set */
				if(((1 << Color[l]) & setid) != 0){
					/* mult returned val, with NP at current */
					ret +=  NP[n][m] * NumWalks(l, s - (1 << Color[l]));
					/* save in cache */
					Cache[n][s] =  ret;
				}
			}
		}
	}

	/*cant seem to make my Cache match solution */
	if (Cache[n][s]==-1){
		Cache[n][s] = 0;
	}
	return ret;
}