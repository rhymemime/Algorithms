#include <iostream>
#include <cstddef>      // std::size_t
#include <cmath>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Board{
	public:
		long long howMany(long long, long long, vector<vector<int> > &);
	private:
		vector<int> starting_places(vector<vector<int> > const &);
};

long long Board::howMany(long long r, long long c, vector<vector<int> > &Grid){
	vector<int>starting_placesN;
	starting_placesN = starting_places(Grid);

	reverse(Grid.begin(), Grid.end());

	vector<int>starting_placesR;
	starting_placesR = starting_places(Grid);

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
	cout << "counter: " << counter << endl;

	cout << "startingPlacesN: ";
	for(int i=0; i<starting_placesN.size(); i++){
		cout << starting_placesN[i] << " ";
	}
	cout << endl;

	cout << "startingPlacesR: ";
	for(int i=0; i<starting_placesR.size(); i++){
		cout << starting_placesR[i] << " ";
	}
	cout << endl << endl;

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

	cout << "startingPlacesT: ";
	for(int i=0; i<starting_placesT.size(); i++){
		cout << starting_placesT[i] << " ";
	}
	cout << endl;

	cout << "startingPlacesRT: ";
	for(int i=0; i<starting_placesRT.size(); i++){
		cout << starting_placesRT[i] << " ";
	}
	cout << endl << endl;
	int Tcounter = 0;
	for(int i=0; i<starting_placesT.size(); i++){
		for(int j=i+1; j<starting_placesRT.size();j++){
			if(starting_placesT[i] == 1 && starting_placesRT[j] == 1){
				Tcounter++;
			}
		}
	}
	cout << "Tcounter: " << Tcounter << endl;
	return Tcounter * counter;
}

vector<int> Board::starting_places(vector<vector<int> > const &Grid){
	for(int i=0;i<Grid.size(); i++){
		for(int j=0;j<Grid[i].size();j++){
			cout << Grid[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl << endl;
	vector<int> starting_places;
	starting_places.resize(Grid.size()+1);
	starting_places[0] = 1;
	bool foldBool;
	for(int j=0;j<Grid.size();j++){
		//cout << endl <<  "j: " << j << endl;
		for(int w=1; w<=j&&w+j<Grid.size()+1; w++){
			bool isStart = true;
			//cout << "w: " << w << endl;
			if(starting_places[j-w] == 1){
				for(int r=0; r<w; r++){
					//cout << "r: " << r << endl;
					if(Grid[j-r-1]!=Grid[j+r]){
						isStart = false;
						//cout << "breaking to 0" << endl;
						//starting_places[j]=0;
						//break;
					}
					//else{starting_places[j]=1;break;}
				}
				if(isStart){starting_places[j]=1;break;}
				else{starting_places[j]=0;}
			}
		}
	}
	return starting_places;
}

int main(int argc, char *argv[]){

	if(argc!=1){
		cout << "usage: Board < boardStuff\n";
		return 1;
	}
	vector<vector<int> > mainGrid;
	string tmp;
	int cinCounter=0;
	while(cin>>tmp){
		mainGrid.resize(mainGrid.size()+1);
		for(int i=0;i<tmp.size();i++){
			mainGrid[cinCounter].push_back(tmp[i]-48);
		}
		cinCounter++;
	}
	for(int i=0;i<mainGrid.size(); i++){
		for(int j=0;j<mainGrid[i].size();j++){
			//cout << mainGrid[i][j];
		}
	}
	Board b;
	cout << b.howMany(1, 1, mainGrid);
}


