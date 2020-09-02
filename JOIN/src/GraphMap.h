#pragma once

#include"common.h"
#include<iostream>
#include<unordered_map>
#include<unordered_set>

class GraphMap
{
public:
	vector<vector<uint>> edges_vec;
	vector<vector<uint>> rev_edges_vec;
	GraphMap() 
	{
		edge_count = 0;
		max_node_id = 0;
	}
	~GraphMap(){}
	void insert(const uint& src, const uint& target);
	uint node_cnt();
	uint edge_cnt();
	vector<uint> get_out_nbrs(const uint& node);
	vector<uint> get_in_nbrs(const uint& node);
	uint get_out_deg(const uint& node);
	uint get_in_deg(const uint& node);
	void remove_node(const uint& node, bool remove_edges_only);
	uint get_edge_count();
	uint get_max_node_id();

private:
	uint edge_count;
	uint max_node_id;
};
