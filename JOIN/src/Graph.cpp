#include"Graph.h"

#include<iostream>
using namespace std;

// get the start and end pointer that are neighbors of v_idx[v] in e_l
pair<uint*, uint*> Graph::get_neighbors(uint v) {
	uint* start = &e_l[v_idx[v]];
	uint* end = &e_l[v_idx[v + 1]];
	return make_pair(start, end);
}

// get the start pointer that is the neighbor of v_idx[v] in e_l
uint* Graph::get_first_nbr(uint v) {
	uint start = v_idx[v];
	uint end = v_idx[v + 1];
	if (start == end) {
		return nullptr;
	}
	else {
		return &e_l[v_idx[v]];
	}
}

// given a current neighbor *cur_nei of v_idx[v], get the next neighbor of v_idx[v]
uint* Graph::get_next_nbr(uint v, uint* cur_nei) {
	cur_nei += 1;
	uint* end = &e_l[v_idx[v + 1]];
	if (cur_nei == end) {
		return nullptr;
	}
	else {
		return cur_nei;
	}
}

uint Graph::get_degree(uint v) {
	return v_idx[v + 1] - v_idx[v];
}

uint Graph::get_num_node() {
	return this->n_v;
}

uint Graph::get_num_edge() {
	return this->n_e;
}