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
	cout << "in findMinimum\n";
	cout << A << std::endl;
	cout << B << std::endl;
	double squareRoot = sqrt(A+B); //might break with very big numbers...
	if((squareRoot - floor(squareRoot)) == 0){
		return findMinimumRec(A, B);
	}
	else{return -1;}
}

long long Alice::findMinimumRec(long long A, long long B){
	long long r = (long long)sqrt(A+B);
	cout << "Im in recursion!" << endl;
	if (A==2 || B==2){
		return -1;
	}
	else if(A<=2*r){
		if(A%2==0){
			return 2;
		}
		else{return 1;}
	}
	else if(A == 2*r+1){return 3;}
	else if(A!=2 && B!=2){
		return 1 + findMinimumRec(A-(2*r-1), r-1);
	}
	else{return -1;}
}

int main(int argc, char *argv[]){

	if(argc!=3){
		cout << "usage: Alice number1 number2\n";
		return 1;
	}

	
	long long A = atoll(argv[1]);
	long long B = atoll(argv[2]);

	cout << A << endl;

	Alice alice;
	long long output = alice.findMinimum(A,B);
	cout << "output " << output <<endl;

}

