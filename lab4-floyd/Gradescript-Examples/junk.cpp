#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
using namespace std;

main()
{
  int nn, ne, mw, mc, n, e, w, c, i;
  vector <int> f;
  vector <int> t;

  srand48(time(0));

  n = 50;
  e = 500;
   
  if (e > n*n) e = n*n;
  c = 1000000000;
  mw = 100000;
  
  printf("%d\n", n);
  for (i = 0; i < e; i++) f.push_back(lrand48()%(n-2)+1);
  for (i = 0; i < e; i++) printf("%d ", f[i]);
  printf("\n");

  t.resize(e);
  for (i = 0; i < e; i++) {
    do {
    t[i] = lrand48()%n+1;
  } while (n != 1 && t[i] <= f[i]);
  }
  for (i = 0; i < e; i++) printf("%d ", t[i]);
  printf("\n");

  for (i = 0; i < e; i++) printf("%ld ", lrand48()%(mw+1));
  printf("\n");
  printf("%d\n", c);
  exit(0);
} 
