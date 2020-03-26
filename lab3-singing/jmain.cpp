#include "Singing.cpp"

int main(int argc, char **argv)
{
  int i;
  class Singing TheClass;
  int retval;
  int N;
  int low;
  int high;
  vector <int> pitch;

  if (argc != 2) { fprintf(stderr, "usage: a.out num\n"); exit(1); }

/*
  N = ;
  low = ;
  high = ;
  pitch = ;
*/


 if (atoi(argv[1]) == 0) {
    N = 3;
    low = 2;
    high = 2;
    pitch.push_back(1);
    pitch.push_back(2);
    pitch.push_back(3);
    pitch.push_back(2);
    pitch.push_back(1);
    pitch.push_back(2);
  }

 if (atoi(argv[1]) == 1) {
    N = 10;
    low = 3;
    high = 7;
    pitch.push_back(4);
    pitch.push_back(4);
    pitch.push_back(5);
    pitch.push_back(5);
    pitch.push_back(6);
    pitch.push_back(5);
    pitch.push_back(3);
    pitch.push_back(6);
  }

 if (atoi(argv[1]) == 2) {
    N = 6;
    low = 2;
    high = 5;
    pitch.push_back(5);
    pitch.push_back(3);
    pitch.push_back(1);
    pitch.push_back(6);
    pitch.push_back(4);
    pitch.push_back(2);
  }

 if (atoi(argv[1]) == 3) {
    N = 10;
    low = 4;
    high = 5;
    pitch.push_back(1);
    pitch.push_back(4);
    pitch.push_back(3);
    pitch.push_back(5);
    pitch.push_back(2);
    pitch.push_back(5);
    pitch.push_back(7);
    pitch.push_back(5);
    pitch.push_back(9);
  }

 if (atoi(argv[1]) == 4) {
    N = 100;
    low = 20;
    high = 80;
    pitch.push_back(2);
    pitch.push_back(27);
    pitch.push_back(3);
    pitch.push_back(53);
    pitch.push_back(53);
    pitch.push_back(52);
    pitch.push_back(52);
    pitch.push_back(60);
    pitch.push_back(85);
    pitch.push_back(89);
    pitch.push_back(100);
    pitch.push_back(53);
    pitch.push_back(60);
    pitch.push_back(2);
    pitch.push_back(3);
    pitch.push_back(53);
    pitch.push_back(100);
    pitch.push_back(89);
    pitch.push_back(40);
    pitch.push_back(42);
    pitch.push_back(2);
    pitch.push_back(53);
    pitch.push_back(2);
    pitch.push_back(85);
  }

  retval = TheClass.solve(N, low, high, pitch);
  cout << retval << endl;

  exit(0);
}
