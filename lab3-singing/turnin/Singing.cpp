#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include<bits/stdc++.h>
using namespace std;

class Singing {
	public:
		int solve(int N, int low, int high, vector <int> pitch);
		void addEdge(int, int, int);

	private:
		vector<vector<int> > graph;
		void print_graph();
		bool bfs(int, vector<int> &parent);
};

void Singing::print_graph(){
	for(int i=0; i<graph.size(); i++){
		for(int j=0; j<graph[i].size(); j++){
			cout << graph[i][j] << " ";
		}
		cout << endl;
	}
}

bool Singing::bfs(int N, vector<int> &parent){//refactor without source/sink

	vector<bool> visit;
	visit.resize(N+2);
	visit.assign(N+2, false);
	visit[0] = true;

	queue<int> q;
	q.push(0);

	//standard bfs loop
	while(!q.empty()){
		int hold = q.front();
		q.pop();

		for(int i=0; i<N+2; i++){
			if(visit[i]==false && graph[hold][i] > 0){
				q.push(i);
				parent[i] = hold;
				visit[i] = true;
			}
		}
	}
	//cout << "N+1: " << N+1 << "" << visit[N+1] << endl;

	if (visit[N+1] == true){
		return true;
	}
	else return false;
}

int Singing::solve(int N, int low, int high, vector <int> pitch)
{
	int i;
	//using 0 and N+1 to store Bob and Alice Respectively
	graph.resize(N+2);

	//init graph to zero
	for(int i=0; i< graph.size(); i++){
		graph[i].resize(N+2);
		graph[i].assign(N+2, 0);
	}

	//init notes that only each singer can sing to inf
	for(int i=1; i<low; i++){
		graph[0][i] = INT_MAX;
		graph[i][0] = INT_MAX;
	}
	for(int i=high+1; i<N+1; i++){
		graph[i][N+1] = INT_MAX;
		graph[N+1][i] = INT_MAX;
	}

	//make edges between pitches
	for(int i=0; i<pitch.size()-1; i++){
		//if(pitch[i] > low && pitch[i] < high){
		graph[pitch[i]][pitch[i+1]]++;
		graph[pitch[i+1]][pitch[i]]++;
		//}
	}

	//parent vector stores parents of nodes in path found by bfs
	vector<int>parent;
	parent.resize(N+2);
	parent[0] = -1;
	//mc can technically be larger than INT_MAX if multiple INT MAX paths
	long mc=0;
	//print_graph();

	//solve for minimum cut
	while(bfs(N, parent)==true){
		//assume max path flow
		int pf = INT_MAX;
		for(int i=N+1; i!=0; i=parent[i]){
			int j = parent[i];
			pf = min(pf, graph[i][j]);
		}
		//reduce graph path
		for(int i=N+1; i!=0; i=parent[i]){
			int j = parent[i];
			graph[i][j] -= pf;
			graph[j][i] -= pf;
		}
		mc += (long)pf;
	}

	return mc;
}

int main(int argc, char **argv)
{
	int i;
	class Singing TheClass;
	int retval;
	int N;
	int low;
	int high;
	int tmp;
	vector <int> pitch;

	cin >> N >> low >> high;
	while(cin >> tmp){
		pitch.push_back(tmp);
	}

	retval = TheClass.solve(N, low, high, pitch);
	cout << retval << endl;

	exit(0);
}
