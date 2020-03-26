#include <iostream>
#include <cstddef>      // std::size_t
#include <cmath>
#include <stdlib.h>

using namespace std;

class Alice{
	public:
		long long findMinimum(long long,long long);
	private:
		long long aliceNum;
		long long findMinimumRec(long long,long long);
};

long long Alice::findMinimum(long long A, long long B){
	long double squareRoot = sqrt(A+B); //might break with very big numbers...
	if((squareRoot - floor(squareRoot)) == 0){
		//look for h last rounds going to Alice
		/*vector<long> flips;
		flips.push_back(r);
		for(int i=r-1; i>0; i--){
			flips.push_back(i^2+flips[i]);
		}
		cout << binary_search(flips.begin(), flips.end(), )*/
		long long r = squareRoot;
		cout << "r: " << r << endl;
		long long first = 1;
		long long last = r;
		long long middle;
		while(first <= last){
			//cout << "first+ last: " << first + last << endl;
			middle = (first+last)/2;
			cout << "middle 1st: " << middle << endl;
			cout << "middle eq: " <<((2*r*middle)-pow(middle,2)) << endl;
			cout << "middle eq-1: " << ((2*r*(middle+1))-pow(middle+1,2)) << endl;
			if((A == ((2*r*middle)-middle^2)) || (A > ((2*r*middle)-pow(middle,2)) && A < ((2*r*(middle+1))-pow(middle+1,2)))){
				cout << "middle!!: " << middle << endl;
				return middle + findMinimumRec(A - (2*r*middle-middle^2), r - middle);
			}
			else if(A > (2*r*middle-pow(middle,2))){
				first = middle + 1;
			}
			else{
				last = middle -1;;
			}
		}

		return findMinimumRec(A, squareRoot);
	}
	else{return -1;}
}

long long Alice::findMinimumRec(long long A, long long R){
	//long long r = (long long)sqrt(A+B);
	if(A<2*R){
		if(A%2==0){
			return 2;
		}
		else{
			return 1;
		}
	}
	else if(A == 2*R+1){return 3;}
	else{
		return 1 + findMinimumRec(A-(2*R-1), R-1);
	}
	//else{return -1; cout << "returning weird case\n";}
	cout << "I shouldnt get here!!" << endl;
}

int main(int argc, char *argv[]){

	if(argc!=3){
		cout << "usage: Alice number1 number2\n";
		return 1;
	}

	
	long long A = atoll(argv[1]);
	long long B = atoll(argv[2]);

	Alice alice;
	long long output = alice.findMinimum(A,B);
	cout << "output " << output <<endl;

}

