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
      for (int v : caches[c]) {
        cout << v << " ";
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

int min_latency[MAX_E];

void
precompute_min_latencies() {
  for (int e = 0; e < E; ++e) {
    min_latency[e] = latencies[e];
    for (const Edge& edge : edges[e]) {
      min_latency[e] = min(min_latency[e], edge.l);
    }
  }
}

struct SortByMagic {
  bool operator() (const Request& r1, const Request& r2) const {
    return r1.n * (latencies[r1.e] - min_latency[r1.e]) >
      r2.n * (latencies[r2.e] - min_latency[r2.e]);
  }
};

inline bool
contains(const vector<int>& v, int val) {
  for (int x : v) {
    if (x == val) return true;
  }

  return false;
}

Solution
solve() {
  precompute_min_latencies();
  sort(requests.begin(), requests.end(), SortByMagic());
  for (int e = 0; e < E; ++e) {
    sort(edges[e].begin(), edges[e].end());
  }

  Solution sol;
  for (const Request& req : requests) {
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

  return 0;
}
