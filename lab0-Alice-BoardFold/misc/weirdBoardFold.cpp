#include <iostream>
#include <cstddef>      // std::size_t
#include <cmath>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

using namespace std;

class Board{
	public:
		long long howMany(long long, long long, vector<int> const &);
};

long long Board::howMany(long long r, long long c, vector<int> const &Grid){
	vector<int> starting_places;
	starting_places.resize(Grid.size());
	starting_places[0] = 1;
	bool foldBool;
	for(int i=1; i<Grid.size(); i++){
		foldBool=true;
		cout << "i: " << i << endl;
		for(int j=0;j+i-1<Grid.size()&&i-j>0;j++){//figure out conditional
			cout << "j: " << j << endl;
			if(Grid[i-j]!=Grid[i+j-1]){//i believe calculating width on the fly is just as fast as calculating then storing
				foldBool=false;//as soon as we have a row that doesnt match, we out
				break;
			}
		}
		cout << "foldBool: " << foldBool << endl;
		starting_places[i]=foldBool;
	}
	vector<int> starting_placesR;
	starting_placesR.resize(Grid.size());
	cout << "----------------Starting Reverse-------------------" << endl;
	for(int i=Grid.size(); i>0; i--){
		foldBool=true;
		cout << "i: " << i << endl;
		for(int j=0;j+i-1<Grid.size()&&i-j>0;j++){//figure out conditional
			cout << "j: " << j << endl;
			if(Grid[i-j]!=Grid[i+j-1]){//i believe calculating width on the fly is just as fast as calculating then storing
				foldBool=false;//as soon as we have a row that doesnt match, we out
				break;
			}
		}
		cout << "foldBool: " << foldBool << endl;
		starting_placesR[i]=foldBool;//will have to handle the fact that its reversed correctly
	}
	vector<int> GridT;
	int GridWidth = 1 + (int)ceil(log10(Grid[0]));//width is the number of digits
	cout << "GridWidth: " << GridWidth << endl;
	GridT.resize(GridWidth);
	bool leadingZero=true;
	for(int i=0; i<Grid.size(); i++){
		int gridTint=0;
		for(int j=0; j<GridWidth; j++){
			if(Grid[i] == 0 && leadingZero){}
			else if(Grid[i]==1 && leadingZero){
				gridTint = 1;
				leadingZero=false;
			}
			else{
				gridTint *= 10;
				gridTint += Grid[i];
			}
			GridT[i]=gridTint;
		}
	}
	cout << "GridT:\n";
	for(int i=0; i<GridT.size(); i++){
		cout << GridT[i] <<endl;
	}
}

int main(int argc, char *argv[]){

	if(argc!=1){
		cout << "usage: Board < boardStuff\n";
		return 1;
	}
	vector<int> mainGrid;
	string tmp;
	while(cin>>tmp){
		mainGrid.push_back(atoi(tmp.c_str()));
	}
	Board b;
	b.howMany(1, 1, mainGrid);
}


