#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <map>
#include <cmath>
#include <set>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <bits/stdc++.h>

using namespace std;

class NegativeGraphDiv1{
	public:
		long findMin(int, vector<int>, vector<int>, vector<int>, int);
	private:
		void printMat(vector<vector<long> > M);
};


long NegativeGraphDiv1::findMin(int numEl, vector<int> from, vector<int> to, vector<int> weight, int charges){
	int i,j,x,y,f,t,z;
	long long k;
	vector<vector<long> > M;
	vector<vector<long> > S;
	vector<vector<long> > L;
	vector<vector<long> > A;
	vector<vector<long> > Ai;
	vector<vector<long> > Aj;
	vector<vector<long> > Ax;
	int INF = 6000000;
	/* run initial setup */
	M.resize(numEl);
	L.resize(numEl);
	A.resize(numEl);
	for(i=0; i<numEl; i++){
		M[i].resize(numEl);
		L[i].resize(numEl);
		A[i].resize(numEl);
		for(j=0; j<numEl; j++){
			if(i==j)
				M[i][j] = 0;
			else
				M[i][j] = INF;
			L[i][j] = 0;
			A[i][j] = INF;
		}
	}
	/* Setup M from input */
	for(i=0; i<from.size(); i++){
		if(M[from[i]-1][to[i]-1] > weight[i])
			M[from[i]-1][to[i]-1] = weight[i];
		if(L[from[i]-1][to[i]-1] < weight[i])
			L[from[i]-1][to[i]-1] = weight[i];
	}
	S=M;/* start with S as M */
	for(i=0; i<numEl; i++){//for each element
		for(x=0; x<numEl; x++){
			for(y=0; y<numEl; y++){//for each pair
				if(S[x][i] + S[i][y] < S[x][y]){
					S[x][y] = S[x][i] + S[i][y];
				}
				if(x==y)S[x][y]=0;//distance to self starts as 0
			}
		}
	}

	A=S;
	if(charges==0) return S[0][numEl-1];
	for(f=0; f<numEl; f++){//start nodes
		for(t=0; t<numEl; t++){//end nodes
			for(i=0; i<numEl; i++){
				for(j=0; j<numEl; j++){
					if(L[i][j]!=0){
						if(S[f][i] + -1 * L[i][j] + S[j][t] < A[f][t] && A[f][t] != INF){
							A[f][t] = S[f][i] + -1 * L[i][j] + S[j][t];
						}
					}
				}
			}
		}
	}
	f=1;
	j=1;
	k=1;
	Ai = A;
	Aj = A;
	Ax = A;
	vector<vector<vector<long> > > cache;
	while(k<charges){
		if(k*2 <= charges){//check if we can do another power of 2
			//cout << k << endl;
			cache.push_back(Ax);//store it
			Aj = Ax;//setting Ai/Aj to output double Ax each time
			Ai = Ax;
			k = k*2;
		}
		else{
			for(i=cache.size()-1; i>=0; i--){//look through cache
				/*find bigest already calc'd value that can be used*/
				if(k + pow(2,i) <= charges){
					Ai = cache[i];
					Aj = Ax;
					k = k+pow(2,i);
					break;
				}
			}
		}
		for(i=0; i<numEl; i++){
			for(j=0; j<numEl; j++){
				for(z=0;z<numEl; z++){
					if(Ax[i][j] > Ai[i][z] + Aj[z][j]){
						/* distance from i z then z j */
						Ax[i][j] = Ai[i][z] + Aj[z][j];
					}
				}
			}
		}
	}
	return Ax[0][numEl-1];
}

/* way to output Matrix for debugging, unused in solution */
void NegativeGraphDiv1::printMat(vector<vector<long> > M){
	int width = 11;
	int INF = 1000000;
	for(int i=0; i<M.size(); i++){
		for(int j=0; j<M.size(); j++){
			if(M[i][j] == INF)
				cout << setw(width) << left << "INF";
			else
				cout << left << setw(width) << M[i][j];
		}
		cout << endl;
	}
	cout << endl;
}

int main(int argc, char** argv){

	int numEl;
	int i;
	int tmp;
	string stmp;
	int charges;

	vector<int> from;
	vector<int> to;
	vector<int> weight;

	cin >> numEl;
	getline(cin, stmp);
	for(i=0; i<3; i++){
		string line;
		getline(cin, line);
		stringstream ss;
		ss << line;
		while(!ss.eof()){
			if(i==0){
				ss >> tmp;
				from.push_back(tmp);
			}
			if(i==1){
				ss >> tmp;
				to.push_back(tmp);
			}
			if(i==2){
				ss >> tmp;
				weight.push_back(tmp);
			}
		}
	}
	cin >> charges;

	NegativeGraphDiv1 N;
	cout << N.findMin(numEl, from, to, weight, charges) << endl;
}
