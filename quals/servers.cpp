#include <iostream>
#include <cstdio>
#include <vector>
#include <set>
#include <algorithm>
#include <map>
#include <queue>
#include <stack>

using namespace std;

const int MAX_C = 1000;
const int MAX_E = 1000;
const int MAX_V = 10000;

struct Edge {
  int c, l;

  Edge(int c, int l) : c(c), l(l) {}
};

struct Request {
  int v, e, n;

  Request(int v, int e, int n) : v(v), e(e), n(n) {}
};

int V, E, R, C, X;

int videos[MAX_V];
int latencies[MAX_E];
int nb_caches[MAX_E];
vector<Edge> edges[MAX_E];
int adjacency_e_c[MAX_E][MAX_C]; // adjacency matrix between endpoints and caches
int adjacency_c_v[MAX_C][MAX_V];
vector<Request> requests;

struct Solution {
  vector<int> caches[MAX_C];
};

void build_adjacency_matrix_c_v(Solution solution){
  for(int c = 0;c < C;c++){
    for(int v=0;v<V;v++){
      adjacency_c_v[c][v] = 0;
    }
  };

  vector<int> caches = solution.caches;
  for(int c=0;c<C;c++){
    for(int i=0;i<caches[c].size();i++){
      adjacency_c_v[c][v] = 1;
    }
  }
}

void build_adjacency_matrix_e_c(){
  for(int e = 0;e < E;e++){
    for(int c=0;c<C;c++){
      adjacency_e_c[e][c] = 0;
    }
  };

  for(int e = 0;e < E;e++){
    for(int i=0;i<edges.size();i++){
      Edge edge = edges[i]; // edge from e to c
      int c = edge.c;
      int l = edge.l;
      if(l == 0){
	cerr << "something went wrong, latency should not be 0!!!" << endl;
      }
      adjacency_e_c[e][c] = l;
    }
  }
}

double score(Solution solution){
  vector<int> caches = solution.caches;
  build_adjacency_matrix_e_c();
  build_adjacency_matrix_c_v(solution);
  double score = 0;
  for(int i=0;i< requests.size();i++){
    request r = requests[i];
    int video = r.v;
    int endpoint = r.e;
    int number_requests = r.n;
    double datacenter_latency = latencies[e];
    double min_latency = datacenter_latency;
    for(int cache=0;cache<C;cache++){
      if(adjacency_e_c[e][c] == 1){
	int latency = adjacency_c_v[c][v];
	if(latency != 0){
	  min_latency = min(min_latency,latency);
	}
      }
    }
    score += (datacenter_latency - min_latency) * number_requests;
  }
  return score;
}


void
read_input() {
  cin >> V >> E >> R >> C >> X;

  for (int i = 0; i < V; ++i) {
    cin >> videos[i];
  }

  for (int i = 0; i < E; ++i) {
    cin >> latencies[i];
    cin >> nb_caches[i];

    for (int j = 0; j < nb_caches[i]; ++j) {
      int c, l;
      cin >> c >> l;
      edges[i].push_back(make_pair(c, l));
    }
  }

  for (int i = 0; i < R; ++i) {
    int v, e, n;
    cin >> v >> e >> n;
    requests.push_back(Request(v, e, n));
  }
}

void
solve() {

}

int
main() {
  read_input();

  solve();

  return 0;
}
