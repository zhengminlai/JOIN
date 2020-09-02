#pragma once

#include<unordered_map>
#include"common.h"

// Static Graph Representation
class Graph {
	uint n_v;
	uint n_e;
	// the index of edge in edge list of this start point
	uint* v_idx;
	// the edge list of the graph
	uint* e_l;
public:
	Graph(){
		this->n_e = 0;
		this->n_v = 0;
		this->v_idx = nullptr;
		this->e_l = nullptr;
	}

	Graph(uint num_vertices, uint num_edges, uint* uint_idx,
		uint* edge_list) :
		n_v(num_vertices), n_e(num_edges), v_idx(uint_idx), e_l(edge_list) {
	}

	// from e_l[v_idx[v]] to el[v_idx[v + 1]]
	pair<uint*, uint*> get_neighbors(uint v);

	uint* get_first_nbr(uint v);
	uint* get_next_nbr(uint v, uint* cur_nei);
	uint get_num_node();
	uint get_num_edge();
	uint get_degree(uint v);

	uint* node_data() {
		return v_idx;
	}
	uint* edge_data() {
		return e_l;
	}

	~Graph() {}
};
