#include <string.h>

#include "RG.h"

int main(int argc, char **argv)
{
  RainbowGraph rg;
  vector <int> colors, a, b;
  string s;
  int state;
  int v, i;

  if (argc > 2 || (argc == 2 && strcmp(argv[1], "--help") == 0)) {
    fprintf(stderr, "usage: RG-Tester [Verbose-Chars] - colors, and b on standard input\n");
    fprintf(stderr, "       A = print the answer at the end.\n");
    fprintf(stderr, "       N = print non-zero values of NP[i][j].\n");
    fprintf(stderr, "       C = print non-empty values of the cache.\n");
    fprintf(stderr, "       You can add your own to these for debugging.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "colors, a and b should be { v0, v1, v2, ... } \n");
    fprintf(stderr, "   - braces should be their own words.\n");
    fprintf(stderr, "   - vals should be their own words which sscanf to numbers.\n");
    exit(1);
  }
  
  if (argc == 2) rg.Verbose = argv[1];
  
  state = 0;

  while (state != 6) {
    if (!(cin >> s)) {
      fprintf(stderr, "Input ended prematurerly.\n");
      exit(1);
    }
    if (s == "{") {
      if (state % 2 == 0) {
        state++;
      } else {
        fprintf(stderr, "Got '{' before '}'\n");
        exit(1);
      }
    } else if (state % 2 == 0) {
      fprintf(stderr, "Was looking for '{' and got '%s'.\n", s.c_str());
      exit(1);
    } else if (s == "}") {
      state++;
    } else if (sscanf(s.c_str(), "%d", &v) == 0) {
      fprintf(stderr, "Was looking for a number or '}' and got '%s'.\n", s.c_str());
      exit(1);
    } else if (state == 1) {
      colors.push_back(v);
    } else if (state == 3) {
      a.push_back(v);
    } else {
      b.push_back(v);
    }
  }
  if (a.size() != b.size()) {
    fprintf(stderr, "Error: a and b are not the same size.\n");
    exit(1);
  }
  for (i = 0; i < a.size(); i++) {
    if (a[i] < 0 || a[i] >= colors.size()) {
      fprintf(stderr, "Bad value for a[%d]=%d -- not between 0 and colors.size()-1.\n", i, a[i]);
      exit(1);
    }
    if (b[i] < 0 || b[i] >= colors.size()) {
      fprintf(stderr, "Bad value for b[%d]=%d -- not between 0 and colors.size()-1.\n", i, b[i]);
      exit(1);
    }
  }
  v = rg.countWays(colors, a, b);
  if (argc == 1 || rg.Verbose.find('A') != string::npos) cout << v << endl;
  exit(0);
  
}
