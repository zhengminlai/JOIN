#pragma once
#include"Graph.h"
#include"Path.h"
#include"common.h"
#include"GraphMap.h"
#include"util.h"
#include<unordered_map>
#include<unordered_set>
#include<set>
#include<vector>
#include<queue>
#include<algorithm>
#include <ctime>
#include <iomanip>

using namespace std;

class StaticGraphStPathCalculation
{
public:
	NODE_TYPE s;
	NODE_TYPE t;
	int k;
	GraphMap g;
	bool count_only;
	int count = 0;
	int shortest_dis = 100;

	unordered_map<NODE_TYPE, uint> sd_s_u; // shortest distance from s to u.
	unordered_map<NODE_TYPE, uint> sd_u_t; // shortest distance from u to t.
	unordered_map<NODE_TYPE, uint> bar; // barrier of each vertex
	unordered_map<uint, set<NODE_TYPE>> middle_vertices_map; // middle vertices map

	StaticGraphStPathCalculation(NODE_TYPE s, NODE_TYPE t,
		int k, GraphMap& g, bool count_only);
	~StaticGraphStPathCalculation();
	void set_s(NODE_TYPE s);
	void set_t(NODE_TYPE t);
	void set_k(int k);
	int graph_node_count();
	uint calculate_st_k_paths_baseline(Paths& paths);
	uint calculate_st_k_paths_join(Paths& paths, double& run_time, double& cpu_process_time);
	uint bc_dfs(const NODE_TYPE& u, vector<NODE_TYPE>& cur_path, Paths& paths,
		unordered_set<NODE_TYPE>& visited);
	void update_barrier(const NODE_TYPE& u, uint u_t_dis, unordered_set<NODE_TYPE>& visited);
	NODE_TYPE add_virtual_vertex(bool left);
	void remove_node(const NODE_TYPE& node);
	void calculate_shortest_distance();
	void calculate_shortest_distance_map();
	void calculate_middle_vertices();
	void bfs_preprocessing_baseline(const NODE_TYPE& start_node, unordered_map<NODE_TYPE, uint>& sd_map, bool is_rev);
};
