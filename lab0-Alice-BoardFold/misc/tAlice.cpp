#include <iostream>
#include <iomanip>
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
	long double tmp = A+B;
	long double squareRoot = sqrtl(tmp); //might break with very big numbers...
	if((squareRoot - floor(squareRoot)) == 0 && A!=2 && B!=2 && A+B>0){
		//look for h last rounds going to Alice
		long long r = squareRoot;
		long long first = 1;
		long long last = r;
		long long middle;
		//perform binary search, being careful to use pow() and not ^
		while(first <= last){
			middle = (first+last)/2;
			//essentially asking if its between fn(middle) and fn(middle+1)
			if(A == ((2*r*middle)-pow(middle,2))
			|| A == ((2*r*(middle+1))-pow((middle+1),2))){
				return middle + findMinimumRec(A - (2*r*middle-pow(middle,2)), r - middle);
			}
			else if (A > ((2*r*middle)-pow(middle,2))					
			&&  A < ((2*r*(middle+1))-pow(middle+1,2))){
				//enter recursion accounting for number of rounds and amt difference in A
				cout << "middle: " << middle << endl;
				cout << setprecision(20) << "passing: " << 2*r*middle-pow(middle,2) << " " << r - middle << endl;
				//cout << "in first ret" << endl;
				return middle + findMinimumRec(A - (2*r*middle-pow(middle,2)), r - middle);
			}
			else if(A > (2*r*middle-pow(middle,2))){
				first = middle + 1;
			}
			else{
				last = middle -1;
			}
		}
		//cout << "in second ret" << endl;
		//otherwise just go straight into recursion
		return findMinimumRec(A, squareRoot);
	}
	else{return -1;}//if A=2, B=2, or an impossible combo is given
}

long long Alice::findMinimumRec(long long A, long long R){
	//base cases
	if (A==0){return 0;}
	else if(A == 2*R+1){
		//cout << "ret3" << endl;
		return 3;
	}
	else if(A<=2*R && A%2==1){
		//cout << "ret1" << endl;
		return 1;}
	else if(A<=2*R && A%2==0){
		//cout << "ret2" << endl;
		return 2;
	}
	else{//else recursify it! idk recursion is hard to explain
		return 1 + findMinimumRec(A-(2*R-1), R-1);
	}
	cout << "I shouldnt get here!!" << endl;//it hasnt happened since debugging but /shrug
}

