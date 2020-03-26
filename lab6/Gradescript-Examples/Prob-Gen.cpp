#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "MOA.h"

using namespace std;
using namespace NeuroUtils;

int main(int argc, char **argv)
{
  int N, C, E, Conn;
  int i, c, te, j, n;
  vector < vector <int> > AM;
  vector < vector <int> > Comp;
  vector <int> Colors, A, B;
  vector <int> order;
  MOA rng;
  multimap <int, int> M;
  multimap <int, int>::iterator mit;
  string s;

  if (argc != 5) {
    fprintf(stderr, "usage: a.out Nodes Components Edges Connected(Y|N|C)\n");
    exit(1);
  }

  N = atoi(argv[1]);
  C = atoi(argv[2]);
  E = atoi(argv[3]);
  Conn = argv[4][0];

  rng.Seed(time(0));

  AM.resize(N);
  for (i = 0; i < AM.size(); i++) AM[i].resize(N, 0);

  Comp.resize(C);

  for (i = 0; i < N; i++) {
    do c = rng.Random_Integer()%C; while (Comp[c].size() >= 10);
    Colors.push_back(c);
    Comp[c].push_back(i);
  }

  te = 0;

  /* Make sure that there is one walk in the graph. */

  if (Conn == 'Y' || Conn == 'C') {
    M.clear();
    for (i = 0; i < C; i++) M.insert(make_pair(rng.Random_Integer(), i));
    for (mit = M.begin(); mit != M.end(); mit++) order.push_back(mit->second);
    n = -1;
    for (i = 0; i < order.size(); i++) {
      c = order[i];
      M.clear();
      for (j = 0; j < Comp[c].size(); j++) M.insert(make_pair(rng.Random_Integer(), Comp[c][j]));
      for (mit = M.begin(); mit != M.end(); mit++) {
        if (n != -1) {
          AM[n][mit->second] = 1;
          AM[mit->second][n] = 1;
          te++;
        }
        n = mit->second;
      }
    }
  } 

  if (Conn == 'C') {
    c = 0;
    for (i = 0; i < Comp.size(); i++) if (Comp[i].size() > Comp[c].size()) c = i;
    for (i = 0; i < Comp[c].size(); i++) {
      for (j = 0; j < i; j++) {
        if (AM[Comp[c][i]][Comp[c][j]] == 0) {
          AM[Comp[c][i]][Comp[c][j]] = 1;
          AM[Comp[c][j]][Comp[c][i]] = 1;
          te++;
        }
      }
    }
  }

  while (te < E) {
    do {
      i = rng.Random_Integer()%N;
      j = rng.Random_Integer()%N;
    } while (i == j || AM[i][j] == 1);
    AM[i][j] = 1;
    AM[j][i] = 1;
    te++;
  }

  printf("{ ");
  for (i = 0; i < Colors.size(); i++) {
    if (i % 25 == 24) printf("\n");
    printf("%d", Colors[i]);
    if (i != Colors.size()-1) printf(",");
    printf(" ");
  }
  printf("}\n");

  M.clear();
  for (i = 0; i < AM.size(); i++) {
    for (j = 0; j < i; j++) {
      if (AM[i][j]) {
        if (rng.Random_Integer()%2 == 1) {
          M.insert(make_pair(rng.Random_Integer(), i*N+j));
        } else {
          M.insert(make_pair(rng.Random_Integer(), j*N+i));
        }
      }
    }
  }

  for (mit = M.begin(); mit != M.end(); mit++) {
    A.push_back(mit->second/N);
    B.push_back(mit->second%N);
  }

  printf("{ ");
  for (i = 0; i < A.size(); i++) {
    if (i % 25 == 24) printf("\n");
    printf("%d", A[i]);
    if (i != A.size()-1) printf(",");
    printf(" ");
  }
  printf("}\n");

  printf("{ ");
  for (i = 0; i < B.size(); i++) {
    if (i % 25 == 24) printf("\n");
    printf("%d", B[i]);
    if (i != B.size()-1) printf(",");
    printf(" ");
  }
  printf("}\n");

}
