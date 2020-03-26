#include <iostream>
#include <fstream>
#include <cstddef>      // std::size_t
#include <cmath>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

class Board{
	public:
		long long howMany(long long, long long, vector<string> &);
	private:
		vector<int> starting_places(vector<vector<int> > const &);
		long long tonumber(char);
};

long long Board::tonumber(char c){
	//reverse engineer compress.cpp to get the right vals for input chars
	if('0' <= c && c <= '9')
		return c - '0';
	if('a' <= c && c <= 'z')
		return c - 'a' + 10;
	if('A' <= c && c <= 'Z')
		return c - 'A' + 35;
	if(c == '#')
		return 62;
	if(c == '@')
		return 63;
	else{
		cout << "i hath goofed royally!!!!!!!!!" << endl;
	}
}


long long Board::howMany(long long r, long long c, vector<string> &compressedGrid){

	vector<vector<int> >Grid;
	Grid.resize(r);
	for(int i=0; i<Grid.size(); i++){
		Grid[i].resize(c);
	}
	//decompress grid using given algorithm to adjust chars to ints
	for(int i=0; i<r; i++){
		for(int j=0; j<c; j++){
			Grid[i][j]=(((long long)(tonumber(compressedGrid[i][j / 6]))) >> (j % 6)) % 2;
		}
	}

	//call out to starting places with standard grid
	vector<int>starting_placesN;
	starting_placesN = starting_places(Grid);

	reverse(Grid.begin(), Grid.end());

	//reverse, the grid and recall starting places
	vector<int>starting_placesR;
	starting_placesR = starting_places(Grid);

	//then reverse grid
	reverse(starting_placesR.begin(), starting_placesR.end());

	//Find number of combinations
	int counter=0;
	for(int i=0; i<starting_placesN.size(); i++){
		for(int j=i+1; j<starting_placesR.size();j++){
			if(starting_placesN[i] == 1 && starting_placesR[j] == 1){
				counter++;
			}
		}
	}

	reverse(Grid.begin(), Grid.end());//back to original Grid

	//Transpose Grid
	vector<vector<int> >GridT;
	GridT.resize(Grid[0].size());
	for(int i=0; i<GridT.size(); i++){
		GridT[i].resize(Grid.size());
		for(int j=0; j<GridT[i].size(); j++){
			GridT[i][j] = Grid[j][i];
		}
	}

	//Get Starting Places for Transposed grid, i.e. columns
	vector<int>starting_placesT;
	starting_placesT = starting_places(GridT);

	//Reverse Transposed Grid
	reverse(GridT.begin(), GridT.end());

	//Get Starting Places for Reverse Transposed grid, i.e. reverse columns
	vector<int>starting_placesRT;
	starting_placesRT = starting_places(GridT);

	//reverse to make finding combinations easier
	reverse(starting_placesRT.begin(), starting_placesRT.end());

	int Tcounter = 0;
	for(int i=0; i<starting_placesT.size(); i++){
		for(int j=i+1; j<starting_placesRT.size();j++){
			if(starting_placesT[i] == 1 && starting_placesRT[j] == 1){
				Tcounter++;
			}
		}
	}

	return Tcounter * counter;

}

vector<int> Board::starting_places(vector<vector<int> > const &Grid){

	vector<int> starting_places;
	starting_places.resize(Grid.size()+1);
	starting_places[0] = 1;
	bool foldBool;
	for(int j=0;j<Grid.size();j++){//given by powerpoint
		for(int w=1; w<=j&&w+j<Grid.size()+1; w++){
			bool isStart = true;
			if(starting_places[j-w] == 1){
				/*if(equal(Grid.begin()+j-w+1,Grid.begin()+j+1,Grid.begin()+j+1)){
					starting_places[j]=1;
				}
				else{
					starting_places[j]=0;
				}*/
				for(int r=0; r<w; r++){//not sure how to make O(1) :(, see above and below for my attempts
					if(Grid[j-r-1]!=Grid[j+r]){
						isStart = false;
					}
				}
				if(isStart){starting_places[j]=1;break;}
				else{starting_places[j]=0;}
				/*if(Grid[j-1]==Grid[j]){
					starting_places[j]=1;
				}
				else{
					starting_places[j] = 0;
				}*/
			}
		}
	}
	return starting_places;
}

