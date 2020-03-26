
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>

using namespace std;

int main ()
{
	int iSecret, iGuess;
	int numVecs = 1000;

	/* initialize random seed: */
	srand (time(NULL));

	vector<vector<string> > board;
	board.resize(2);

	/* generate secret number between 1 and 10: */
	for(int i=2; i<numVecs; i++){
		cout << i << endl;
		board[0].clear();
		board[1].clear();
		for(int k=0; k<2; k++){
			for(int j=0; j<i; j++){
				iSecret = rand() % 2;
				if(iSecret==0) board[k].push_back(".");
				else board[k].push_back("#");
			}
		}
		cout <<"TEST" << endl;
		cout << board[0].size() << endl;
		bool cont = false;
		for(int l=0; l<board[0].size()-1; l++){
			if(board[0][l] == "#" && board[1][l] == "#"){
				cout << "breaking!" << endl;
				cont = true;
			}
			else if(board[0][l] == "#" && board[1][l+1] == "#"){
				cont = true;
				
			}
			else if(board[1][l] == "#" && board[0][l+1] == "#"){
				cont = true;
			}
		}
		if(board[0][board.size()-1] == "#" && board[1][board.size()-1] == "#"){
			cout << "continuing!\n" << endl;
			i--;
			continue;
		}
		else if(cont == true){
			i--;
			continue;
		}
		ofstream output_file(to_string(i) + ".board");
		for(int k=0; k<2; k++){
			ostream_iterator<string> output_iterator(output_file, "");
			copy(board[k].begin(), board[k].end(), output_iterator);
			output_file << "\n";
		}
	}
}

