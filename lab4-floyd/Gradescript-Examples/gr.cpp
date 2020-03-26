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

  cin >> nn >> ne >> mw >> mc;

  srand48(time(0));

  n = lrand48()%nn + 1;
  e = lrand48()%(ne-n) + n+1;
  if (e > n*n) e = n*n;
  c = lrand48()%(mc+1);
  
  printf("%d\n", n);
  for (i = 0; i < e; i++) f.push_back(lrand48()%n+1);
  for (i = 0; i < e; i++) printf("%d ", f[i]);
  printf("\n");

  t.resize(e);
  for (i = 0; i < e; i++) do {
    t[i] = lrand48()%n+1;
  } while (n != 1 && t[i] == f[i]);
  for (i = 0; i < e; i++) printf("%d ", t[i]);
  printf("\n");

  for (i = 0; i < e; i++) printf("%ld ", lrand48()%(mw+1));
  printf("\n");
  printf("%d\n", c);
  exit(0);
} 
