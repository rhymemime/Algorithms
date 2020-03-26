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
			if((A == ((2*r*middle)-pow(middle,2))) || (A > ((2*r*middle)-pow(middle,2)) && A < ((2*r*(middle+1))-pow(middle+1,2)))){
				//enter recursion accounting for number of rounds and amt difference in A
				return middle + findMinimumRec(A - (2*r*middle-pow(middle,2)), r - middle);
			}
			else if(A > (2*r*middle-pow(middle,2))){
				first = middle + 1;
			}
			else{
				last = middle -1;;
			}
		}
		//otherwise just go straight into recursion
		return findMinimumRec(A, squareRoot);
	}
	else{return -1;}//if A=2, B=2, or an impossible combo is given
}

long long Alice::findMinimumRec(long long A, long long R){
	//base cases
	if(A<2*R){
		if(A%2==0){
			return 2;
		}
		else{
			return 1;
		}
	}
	else if(A == 2*R+1){return 3;}
	else{//else recursify it! idk recursion is hard to explain
		return 1 + findMinimumRec(A-(2*R-1), R-1);
	}
	cout << "I shouldnt get here!!" << endl;//it hasnt happened since debugging but /shrug
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
	cout << output <<endl;

}

