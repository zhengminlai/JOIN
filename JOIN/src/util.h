#pragma once
#include<iostream>
#include<vector>
#include<unordered_set>
#include<unordered_map>
#include<algorithm>
#include<string.h>
#include<string>
#include<fstream>
#include<queue>
#include"common.h"
#include"Graph.h"
#include"GraphMap.h"
using namespace std;

void print_data_for_debug(MyVec& barrier, MyVec& sd_s_u, MyVec& sd_u_t,
	uint* node_data, uint node_cnt, uint* edge_data, uint edge_cnt, uint* rev_node_data,
	uint* rev_edge_data);
// split string s by char c
void split(const string& s, vector<string>& ans, char c);
uint get_node_hash(const string& node, int& node_collision_cnt);
void put_graph(vector<vector<uint>>& g, uint* v_idx, uint* e_l);
Graph into_static(vector<vector<uint>>& g, VecUInt& v_idx, VecUInt& e_l);
int read_graph_map(const char filename[], GraphMap& graphMap, char sp = ' ');
void dfs_hops_random_nbr(
	GraphMap& g, uint cur_node, int &ret, int cur_layer,
	int& total_layer, unordered_set<uint>& visited);
void compute_sd_map(const uint& start_node, const uint& k,
		GraphMap& g_map, unordered_map<uint, uint>& sd_map, bool is_rev);
Graph pact_graph(GraphMap& g_map, GraphMap& tmpG,
		const uint& start_node, const uint& target_node,
		const uint& k, unordered_map<uint, uint>& sd_u_t_map,
		VecUInt& v_idx, VecUInt& e_l,
		unordered_map<uint, uint>& node_map,
		unordered_map<uint, uint>& rev_node_map);
