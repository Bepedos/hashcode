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

  bool operator < (const Edge& other) const {
    return l < other.l;
  }
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
vector<Request> requests;

struct Solution {
  vector<int> caches[MAX_C];
  int load[MAX_C];

  Solution() {
    for (int i = 0; i < C; ++i) load[i] = 0;
  }

  void print() {
    cout << C << endl;
    for (int c = 0; c < C; ++c) {
      cout << c;
      for (int v : caches[c]) {
        cout << " " << v;
      }
      cout << endl;
    }
  }
};

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
      edges[i].push_back(Edge(c, l));
    }
  }

  for (int i = 0; i < R; ++i) {
    int v, e, n;
    cin >> v >> e >> n;
    requests.push_back(Request(v, e, n));
  }
}

int cur_latency[MAX_E][MAX_V];
int min_latency[MAX_E];

inline bool
contains(const vector<int>& v, int val) {
  for (int x : v) {
    if (x == val) return true;
  }

  return false;
}

void
precompute_cur_latencies(const Solution& sol) {
  for (int e = 0; e < E; ++e) {
    for (int v = 0; v < V; ++v) {
      cur_latency[e][v] = latencies[e];
      for (const Edge& edge : edges[e]) {
        if (contains(sol.caches[edge.c], v)) {
          cur_latency[e][v] = min(cur_latency[e][v], edge.l);
        }
      }
    }
  }
}

void
precompute_min_latencies() {
  for (int e = 0; e < E; ++e) {
    min_latency[e] = latencies[e];
  }

  for (int e = 0; e < E; ++e) {
    min_latency[e] = latencies[e];
    for (const Edge& edge : edges[e]) {
      min_latency[e] = min(min_latency[e], edge.l);
    }
  }
}

struct SortByMagic {
  bool operator() (const Request& r1, const Request& r2) const {
    return r1.n * (cur_latency[r1.e][r1.v] - min_latency[r1.e]) >
      r2.n * (cur_latency[r2.e][r2.v] - min_latency[r2.e]);
  }
};


int adjacency_e_c[MAX_E][MAX_C]; // adjacency matrix between endpoints and caches
int adjacency_c_v[MAX_C][MAX_V]; // adjacency matrix between caches and videos

void build_adjacency_matrix_c_v(Solution solution){
  for(int c = 0;c<C;c++){
    for(int v=0;v<V;v++){
      adjacency_c_v[c][v] = 0;
    }
  };

  for(int c=0;c<C;c++){
    for(int i=0;i<solution.caches[c].size();i++){
      adjacency_c_v[c][solution.caches[c][i]] = 1;
    }
  }
}

void build_adjacency_matrix_e_c(){
  cerr << "building adjacency endpoint to cache matrix" << endl;
  for(int e=0;e<E;e++){
    for(int c=0;c<C;c++){
      adjacency_e_c[e][c] = 0;
    }
  };

  cerr << "there are" << E << " endpoints" << endl;
  for(int e = 0;e<E;e++){
    cerr << "size edges: " << edges[e].size() << endl;
    for(int i=0;i<edges[e].size();i++){
      // cerr << "considering edge" << i << " for endpoint " << e << endl;
      Edge edge = edges[e][i]; // edge from e to c
      int c = edge.c;
      int l = edge.l;
      if(l == 0){
	cerr << "something went wrong, latency should not be 0!!!" << endl;
      }
      adjacency_e_c[e][c] = l;
    }
  }
  cerr << "done building adjacency endpoint to cache matrix" << endl;
}

pair<double,double> compute_score(Solution solution){
  build_adjacency_matrix_e_c();
  // cerr << "printing endpoint to cache matrix" << endl;
  // for(int e=0;e<E;e++){
  //   for(int c=0;c<C;c++){
  //     cerr << adjacency_e_c[e][c] << " ";
  //   }
  //   cerr << endl;
  // }
  
  build_adjacency_matrix_c_v(solution);

  // cerr << "printing cache to video matrix" << endl;
  // for(int c=0;c<C;c++){
  //   for(int v=0;v<V;v++){
  //     cerr << adjacency_c_v[c][v] << " ";
  //   }
  //   cerr << endl;
  // }

  double score = 0;
  int cumulated_requests = 0; // we will count the total number of requests
  for(int i=0;i< requests.size();i++){
    Request r = requests[i];
    int video = r.v;
    int endpoint = r.e;
    // cerr << "treating request number " << i << " coming from endpoint " << endpoint << endl;
    int number_requests = r.n;
    cumulated_requests += r.n;
    int datacenter_latency = latencies[endpoint];
    // cerr << "  datacenter latency: " << datacenter_latency << endl;
    int min_latency = datacenter_latency;
    for(int cache=0;cache<C;cache++){
      if(adjacency_e_c[endpoint][cache] != 0){
	// cerr << endpoint << " and " << cache << " are connected" << endl;
	if(adjacency_c_v[cache][video] ==1){
	  int latency = adjacency_e_c[endpoint][cache];
	  if(latency != 0){
	    min_latency = min(min_latency,latency);
	  }
	}
      }
    }
    // cerr << "  minimal latency: " << min_latency << endl;
    // cerr << "  time saved per request: " << (datacenter_latency - min_latency) << endl;
    // cerr << "  number of requests: " << number_requests << endl;
    score += ((double) ((double) datacenter_latency - (double) min_latency)) * (double) number_requests;
  }
  double normalized_score = ((double) score) * (double) 1000 / (double) cumulated_requests;
  return (make_pair(score,normalized_score));
}


Solution
solve() {
  precompute_min_latencies();
  sort(requests.begin(), requests.end(), SortByMagic());
  for (int e = 0; e < E; ++e) {
    sort(edges[e].begin(), edges[e].end());
  }

  Solution sol;
  int i = 0;
  for (const Request& req : requests) {
    // if (i % 100 == 0) {
    //   precompute_cur_latencies(sol);
    //   sort(requests.begin() + i + 1, requests.end(), SortByMagic());
    // }
    // i++;

    for (const Edge& edge : edges[req.e]) {
      if (contains(sol.caches[edge.c], req.v)) break;
      if (sol.load[edge.c] + videos[req.v] <= X) {
        sol.load[edge.c] += videos[req.v];
        sol.caches[edge.c].push_back(req.v);
        break;
      }
    }
  }

  return sol;
}

int
main() {
  read_input();

  Solution sol = solve();

  sol.print();
  
  pair<double,double> score = compute_score(sol);
  
  cerr << "score:  unnormalized:" << score.first << " and normalized:  "  << score.second << endl;
  

  return 0;
}
