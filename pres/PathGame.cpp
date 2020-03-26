#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <string>
using namespace std;

int grundy[10003][3][3];
int mex[100500];
class PathGame {
	public:
		string judge(vector<string> board) {
			int N = board[0].size();//lineaer
			for(int i=0;i<=10*N;++i){//initialize mex to 0's
				mex[i] = 0;
			}
			int pc = 0;
			for(int i=0;i<3;++i){//initialize first column
				for(int j=0;j<3;++j){
					grundy[0][i][j] = 0;
				}
			}
			for(int k=1;k<=N;++k){//for every element
				for(int left=0;left<3;++left){//go through 9 vals
					for(int right=0;right<3;++right){
						++pc;
						for(int i=1;i<=k;++i){//look back through?
							for(int p=1;p<=2;++p){//for p=1, 2
								if(i == 1 && left+p == 3) continue;
								if(i == k && right+p == 3) continue;
								int gnum = grundy[i-1][left][p] ^ grundy[k-i][p][right];
								mex[gnum] = pc;
							}
						}
						for(int i=0;i<=10*N;++i){//n-squared
							if(mex[i] < pc) {
								grundy[k][left][right] = i;
								break;
							}
						}
					}
				}
			}
			int next = 0;
			int prev = 0;
			int ans = 0;
			for(int i=0;i<N;++i){
				int p = 0;
				if(board[0][i] == '#') p = 1;
				if(board[1][i] == '#') p = 2;
				if (p > 0) {
					ans ^= grundy[i-next][prev][p];
					prev = p;
					next = i+1;
				}
			}
			ans ^= grundy[N-next][prev][0];
			if (ans == 0) return "Sothe"; else return "Snuke";
		}
};

int main(int argc, char**argv){

	ifstream inputFile(argv[1]);

	vector<string> board;

	string line;
	int count=0;
	while(getline(inputFile, line)){

		board.push_back(line);
		count ++;
	}


	PathGame pg;
	pg.judge(board);
	
}
