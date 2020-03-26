#include <Alice.cpp>

using namespace std;

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


