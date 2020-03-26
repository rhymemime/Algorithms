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
	cout << "out of function!" << endl;

	reverse(Grid.begin(), Grid.end());

	vector<int>starting_placesR;
	starting_placesR = starting_places(Grid);

	reverse(starting_placesR.begin(), starting_placesR.end());

	cout << "startingPlacesN: ";
	for(int i=0; i<starting_placesN.size(); i++){
		cout << starting_placesN[i] << " ";
	}
	cout << endl;

	cout << "startingPlacesR: ";
	for(int i=0; i<starting_placesR.size(); i++){
		cout << starting_placesR[i] << " ";
	}
	cout << endl;

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
	cout << endl;
	/*vector<int> starting_places;
	  starting_places.resize(Grid.size());
	  starting_places[0] = 1;
	  bool foldBool;
	  for(int i=0; i<Grid.size(); i++){
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
	  }*/
	//starting_places(GridT);
}

vector<int> Board::starting_places(vector<vector<int> > const &Grid){
	cout << Grid.size() << endl;
	cout << Grid[0].size() << endl;
	for(int i=0;i<Grid.size(); i++){
		for(int j=0;j<Grid[i].size();j++){
			cout << Grid[i][j];
		}
		cout << endl;
	}
	vector<int> starting_places;
	starting_places.resize(Grid.size()+1);
	starting_places[0] = 1;
	bool foldBool;
	//for(int i=0; i<Grid.size(); i++){
		//foldBool=true;
		//cout << "i: " << i << endl;
		for(int j=0;j<Grid.size();j++){
			cout << endl <<  "j: " << j << endl;
			for(int w=1; w<=j&&w+j<Grid.size()+1; w++){
				cout << "w: " << w << endl;
				if(starting_places[j-w] == 1){
					for(int r=0; r<w; r++){
						cout << "r: " << r << endl;
						if(Grid[j-r-1]!=Grid[j+r]){
							cout << "breaking to 0" << endl;
							starting_places[j]=0;
							break;
						}
						else{starting_places[j]=1;break;}
					}
				}
				//if(rect == true){starting_places[j]=1;};
			}
			/*if(Grid[i-j]!=Grid[i+j-1]){//i believe calculating width on the fly is just as fast as calculating then storing
				foldBool=false;//as soon as we have a row that doesnt match, we out
				break;
			}*/
		}
		//cout << "foldBool: " << foldBool << endl;
		//starting_places[i]=foldBool;
	//}
	return starting_places;
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
		cout << endl;
	}
	Board b;
	b.howMany(1, 1, mainGrid);
}


