#include <cmath>
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

int V, E, R, C, X;

int cur_latency[MAX_E][MAX_V];
int min_latency[MAX_E];

int videos[MAX_V];
int latencies[MAX_E];
int nb_caches[MAX_E];
vector<Edge> edges[MAX_E];


struct Request {
  int v, e, n;

  Request(int v, int e, int n) : v(v), e(e), n(n) {}

  double score() const {
    return (double)(n * (cur_latency[e][v] - min_latency[e])) * exp(-0.0005 * (double)(videos[v]));
  }

  bool operator < (const Request& other) const {
    return score() > other.score();
  }
};

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
precompute_min_latencies(const Solution& sol) {
  for (int e = 0; e < E; ++e) {
    min_latency[e] = latencies[e];
  }

  for (int e = 0; e < E; ++e) {
    min_latency[e] = latencies[e];
    for (const Edge& edge : edges[e]) {
      if (sol.load[edge.c] < X - 30) {
        min_latency[e] = min(min_latency[e], edge.l);
      }
    }
  }
}

struct SortByMagic {
  bool operator() (const Request& r1, const Request& r2) const {
    return (double)(r1.n * (cur_latency[r1.e][r1.v] - min_latency[r1.e])) / (double)(videos[r1.v]) >
      (double)(r2.n * (cur_latency[r2.e][r2.v] - min_latency[r2.e])) / (double)(videos[r2.v]);
  }
};


Solution
solve() {
  Solution sol;
  precompute_min_latencies(sol);
  precompute_cur_latencies(sol);
  sort(requests.begin(), requests.end());
  for (int e = 0; e < E; ++e) {
    sort(edges[e].begin(), edges[e].end());
  }

  for (int i = 0; i < R; ++i) {
    cerr << i << endl;
    const Request& req = requests[i];
    //  for (const Request& req : requests) {
    if (i % 500 == 0) {
      precompute_min_latencies(sol);
      precompute_cur_latencies(sol);
      sort(requests.begin() + i + 1, requests.end());
    }

    //    random_shuffle(edges[req.e].begin(), edges[req.e].end());


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
