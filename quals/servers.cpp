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
  int done = 0;
  
  Request(int v, int e, int n) : v(v), e(e), n(n) {}
};

int V, E, R, C, X;

int videos[MAX_V];
int latencies[MAX_E];
int nb_caches[MAX_E];
vector<Edge> edges[MAX_E];
vector<Request> requests;

// int N;

// void read_sol(){
//   Solution sol;
//   cin >> N;
//   for(int i=0;i<N;i++){
//     int c;
//     int v;
//     cin >> c;
//     string str;
//     getline (cin, str);
//     stringstream ss(str);
//     while (ss >> x)
//       sol.caches[c].push_back(v);
//   }
// }

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
precompute_min_latencies() { //const Solution& sol) {
  for (int e = 0; e < E; ++e) {
    min_latency[e] = latencies[e];
  }

  for (int e = 0; e < E; ++e) {
    min_latency[e] = latencies[e];
    for (const Edge& edge : edges[e]) {
      //      if (sol.load[edge.c] < X) {
        min_latency[e] = min(min_latency[e], edge.l);
        //      }
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
  //cerr << "building adjacency endpoint to cache matrix" << endl;
  for(int e=0;e<E;e++){
    for(int c=0;c<C;c++){
      adjacency_e_c[e][c] = 0;
    }
  };

  //cerr << "there are" << E << " endpoints" << endl;
  for(int e = 0;e<E;e++){
    //cerr << "size edges: " << edges[e].size() << endl;
    for(int i=0;i<edges[e].size();i++){
      // cerr << "considering edge" << i << " for endpoint " << e << endl;
      Edge edge = edges[e][i]; // edge from e to c
      int c = edge.c;
      int l = edge.l;
      if(l == 0){
	//cerr << "something went wrong, latency should not be 0!!!" << endl;
      }
      adjacency_e_c[e][c] = l;
    }
  }
  //cerr << "done building adjacency endpoint to cache matrix" << endl;
}

pair<double,double> compute_score(Solution solution){
  build_adjacency_matrix_e_c();
  
  build_adjacency_matrix_c_v(solution);

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
    score += ((double) ((double) datacenter_latency - (double) min_latency)) * (double) number_requests;
  }
  double normalized_score = ((double) score) * (double) 1000 / (double) cumulated_requests;
  return (make_pair(score,normalized_score));
}


int latency_matrix[MAX_V][MAX_E];

int cost_alternate(Request req,int idx_best_cache){
  return(max(0,req.n * (latency_matrix[req.v][req.e] - edges[req.e][idx_best_cache].l)));
}

void initialize_latency_matrix(){
  for(int v=0;v<V;v++){
    for(int e=0;e<E;e++){
      latency_matrix[v][e] = latencies[e];
    }
  };
}

void update_latency_matrix(int v,int c){
  for(int e =0;e<E;e++){
    if(adjacency_e_c[e][c] != 0){
      latency_matrix[v][e] = min(latency_matrix[v][e],adjacency_e_c[e][c]);
    }
  }
}

struct SortEdges {
  bool operator() (const Edge& e1, const Edge& e2) const {
    return (e1.l < e2.l);
  }
};


void sort_best_cache(int e){
  sort(edges[e].begin(),edges[e].end(),SortEdges());
  }

Solution alternate_solve(){
  //cerr << "Building ec adjacency matrix" << endl;
  build_adjacency_matrix_e_c();
  //cerr << "Initializing latency matrix" << endl;
  initialize_latency_matrix();
  
  int idx_best_cache[E]; // index of best cache for each endpoint
  int best_cache[E]; // said best cache for each endpoint (best_cache[e] = edges[e][idx_best_cache[e]].c)
  //since edges[e] contains all caches connected to endpoint e

  int count_endpoints[C];
  for(int c=0;c<C;c++){
    count_endpoints[c] = 0;
  }

  //cerr << "Sorting caches, for each endpoint, by latency" << endl;
  for(int e=0;e<E;e++)
    {
      sort_best_cache(e);
      idx_best_cache[e] = 0;
      if(edges[e].size()>0){
	int cache = edges[e][idx_best_cache[e]].c;
	best_cache[e] = cache;
	count_endpoints[cache]+=1;//TODO : instead of 1, a weight
      }
    }
  //for each endpoint, sort the caches by increasing latency
  //for each cache, count the number of endpoints for which "it is the best cache"

  Solution sol;

  //cerr << "Checking if algorithm should stop now" << endl;
  bool b = false;
  for(int e=0;e<E;e++){
    if(idx_best_cache[e] < edges[e].size())
      b = true;
  }
 
  while(b){
    //while some cache is left unused

    //cerr << "computing the best cache" << endl;
    int cur_best_cache = 0;
    for(int c=0;c<C;c++)
      {
	if(count_endpoints[c]>count_endpoints[cur_best_cache])
	  cur_best_cache = c;
      }
    //compute the cache such that it is the most "looked after" cache

    //cerr << "computing all endpoints connected to the best cache" << endl;
    vector<int> best_cache_endpoints;
    for(int e=0;e<E;e++){
      if(idx_best_cache[e] < edges[e].size() && edges[e][idx_best_cache[e]].c == cur_best_cache)
	best_cache_endpoints.push_back(e);
    }
    //take all the endpoints such that the current cache is the best one

    int cost_videos[V];
    vector<Request> request_per_vid[V];
  
    for(int v=0;v<V;v++){
      cost_videos[v] = 0;
    }

    //cerr << "computing the gain, for each video, of putting it is the server" << endl;
    for(int i=0;i< requests.size();i++){
      Request req = requests[i];
      if(!(req.done)){
	int v = req.v;
	int e = req.e;
      
	if(contains(best_cache_endpoints,e)){
	  cost_videos[v] += cost_alternate(req,idx_best_cache[e]);
	  request_per_vid[v].push_back(req);
	}
      }
    }
    //look at all requests that involve an endpoint considering the best_cache to be their best_cache
    //compute the gain of storing the video

    vector<Edge> t;
    for(int v=0;v<V;v++){
      t.push_back(Edge(v,cost_videos[v]));
    }
    //sort videos by the gain of storing them

    //cerr << "Sorting videos by highest gain" << endl;
    sort(t.begin(),t.end(),SortEdges());

    //cerr << "Filling the current best cache" << endl;
    int best_cache_fill = 0;
    for(int i=t.size()-1;i>=0;i--){
      int v = t[i].c;
      if(best_cache_fill + videos[v] <= X){
	sol.caches[cur_best_cache].push_back(v);
	for(int j=0;j<request_per_vid[v].size();j++){
	  request_per_vid[v][j].done = 1;
	}
	best_cache_fill += videos[v];
	update_latency_matrix(v,t[i].c);
      }
    }

    //cerr << "Updating pointers" << endl;
    //update the pointers from endpoints to cache servers
    count_endpoints[cur_best_cache]=-1;
    for(int i = 0; i < best_cache_endpoints.size(); i++){
      int e = best_cache_endpoints[i];
      int idx_e = idx_best_cache[e]+1;
      while((idx_e < edges[e].size()) && (count_endpoints[edges[e][idx_e].c]<0)){
	idx_e++;
      }
      idx_best_cache[e] = idx_e;
      if(idx_e < edges[e].size()){
	int new_best_cache = edges[e][idx_e].c; 
	if(count_endpoints[new_best_cache]!=-1)
	  count_endpoints[new_best_cache]++;
      }
    }

    //cerr << "Computing if loop should terminate" << endl;
    b = false;
    for(int e=0;e<E;e++){
      if(idx_best_cache[e] < edges[e].size())
	b = true;
    }
  }
  return sol;

}

Solution
solve() {
  Solution sol;
  precompute_min_latencies();
  precompute_cur_latencies(sol);
  sort(requests.begin(), requests.end(), SortByMagic());
  for (int e = 0; e < E; ++e) {
    sort(edges[e].begin(), edges[e].end());
  }

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

  //cerr << "Done reading input" << endl;
  
  Solution sol = alternate_solve();

  //cerr << "Done computing solution" << endl;
  
  sol.print();
  
  pair<double,double> score = compute_score(sol);
  
  cerr << "score:  unnormalized:" << score.first << " and normalized:  "  << score.second << endl;
  

  return 0;
}
