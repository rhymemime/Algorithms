#include "Singing.cpp"

int main(int argc, char **argv)
{
  int i;
  class Singing TheClass;
  int retval;
  int N;
  int low;
  int high;
  int tmp;
  vector <int> pitch;

  cin >> N >> low >> high;
  while(cin >> tmp){
	  pitch.push_back(tmp);
  }

  retval = TheClass.solve(N, low, high, pitch);
  cout << retval << endl;

  exit(0);
}
