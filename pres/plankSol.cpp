#include <string>
#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

/*
 *
 * #.#
 * ... = 1   1S
 *
 * #..
 * ..# = 0   1O
 *
 * #..#
 * .... = 2%2 = 0  2S
 *
 * #...
 * ...# = 1  2O
 *
 * #...# = 1   3S -- you can split it into 2S, 2x1S or 2x1O.  In either case, it's 1 + something even.
 * .....      
 *
 * #.... = 0   3O -- you can split it into 2O (1+1) or 1S+10 (1+1+0), 
 * ....#      
 *
 * #....# = 0   4S -- you can split it into 3S, (1+1), 1S&2S (1+1+0), 1O+2O (1+0+1)
 * ......      
 *
 * #..... = 1   4O -- you can split it into 30, (1+0), 1S&2O (1+1+1), 2S+1O(1+0).
 * .....#          
 *
 * So, I'm good.  If one end is open, then Snuke wins.  If zero ends are open, then
 * it's deterministic.  If two ends are open, then you want to avoid being the one to break
 * either end.  
 *
 * Here's the approach.  Three parameters: l, r, m: cells left, cells right, cells middle.
 *       If l <= 1 && r <= 1, then it's determinstic.
 *             Otherwise, if l <= 1 || r <= 1, then snuke wins.
 *                   Otherwise, if l == 2 && r == 2, it's up to whoever has to break it.  
 *                         Otherwise, it's a waiting game, but you ought to be able to make it recursive.
 *                         */

class PathGame {
	public:
		string judge(vector <string> board);
		int DP(int L, int M, int R);
		map <string, int> Cache;
};

int PathGame::DP(int L, int M, int R)
{
	cout << "L: " << L << "M: " << M << "R: " << R << endl;
	int i;
	char buf[50];
	string key;

	if (R > L) return DP(R, M, L);

	/* The easy cases */

	if (L == 0) return M;
	if (L == 1) return (M+R+L)%2;
	if (L == 2 && R == 2) return (M)%2;
	if (L > 1 && R <= 1) return 1;
	if (L - R >= 2) return 1;

	sprintf(buf, "%d:%d:%d", L, R, M);
	key = buf;
	if (Cache.find(key) != Cache.end()) return Cache[key];

	if (M == 1 && DP(L,0,R) == 0) { Cache[key] = 1;; return 1;; }

	for (i = 0; i < L-1; i++) {
		if (DP(i, 0, R) == 0) { Cache[key] = 1; return 1; }
		if (DP(i, 1, R) == 0) { Cache[key] = 1; return 1; }
	}
	if (DP(L-1,M,R) == 0) { Cache[key] = 1; return 1; }

	for (i = 0; i < R-1; i++) {
		if (DP(L, 0, i) == 0) { Cache[key] = 1; return 1; }
		if (DP(L, 1, i) == 0) { Cache[key] = 1; return 1; }
	}
	if (DP(L,M,R-1) == 0) { Cache[key] = 1; return 1; }
	{ Cache[key] = 0; return 0; }

}

string PathGame::judge(vector <string> B)
{
	int i, j;
	int N, L, R, M;
	string C;
	map <string, int>::iterator mit;

	for (i = 0; i < B[0].size(); i++) {
		if (B[0][i] == '#') {
			C.push_back('D');
		} else if (B[1][i] == '#') {
			C.push_back('U');
		} else {
			C.push_back('E');
		}
	}

	N = C.size();
	cout << C.size() << endl;

	for (i = 0; i < N && C[i] == 'E'; i++) ;
	L = i;

	if (L == N) {
		for (i = 0; i < N; i++) {
			if (DP(i,0,N-1-i) == 0) {
				cout << Cache.size() << endl;
				return "Snuke";
			}
		}

		/* for (mit = Cache.begin(); mit != Cache.end(); mit++) {
		 *        cout << mit->first << " " << mit->second;
		 *            } */

		cout << Cache.size() << endl;
		return "Sothe";
	}

	for (i = N-1; i >= 0 && C[i] == 'E'; i--) ;
	R = N - 1 - i;

	M = 0;
	i = L;

	while (i < N-1-R) {
		for (j = i+1; C[j] == 'E'; j++) ;
		if ((j - i) % 2 == 1) {
			if (C[i] != C[j]) M = 1 - M;
		} else {
			if (C[i] == C[j]) M = 1 - M;
		}
		i = j;
	}

	i = DP(L, M, R);
	cout << Cache.size() << endl;
	return (i) ? "Snuke" : "Sothe";
}

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
	cout << pg.judge(board) << endl;

}
