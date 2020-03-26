#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

class RainbowGraph {
  public:
    int countWays(vector <int> color, vector <int> a, vector <int> b);
    string Verbose;

    vector <int> Color;              // This is a copy of the input parameter "color".
    vector < vector <int> > Same;    // Adjacency lists of intracomponent edges.
    vector < vector <int> > Diff;    // Adjacency lists of intercomponent edges.
    vector < vector <int> > CNodes;  // Cnodes[i] contains all nodes whose color is i.
    vector < vector <int> > NP;      // NP[i][j] = number of paths from i to j that go
                                     // through all of the nodes in the component.

    vector <int> V;                  // The visited vector for the DFS.
    int NIP;                         // During the DFS, this is the number of nodes in the current path.
    int Source;                      // This is the initial node for each DFS call.
    int Target;                      // The size of Source's component: Cnodes[Source].size()

    vector < vector <long long > > Cache;  // The DP cache.

    void CountPaths(int n);          // This is the extended DFS.  Set Source, Target, V and NIP
                                     // before you call CountPaths(Source) to set NP[Source][j].

    long long NumWalks(int node, int setid);   // Number of walks starting at node node that 
                                               // still need to go through the nodes in setid.
};
