#include"GraphMap.h"

void GraphMap::insert(const uint& src, const uint& target) {
	if (src > max_node_id) {
		max_node_id = src;
	}

	if (target > max_node_id) {
		max_node_id = target;
	}

	if (edges_vec.size() < max_node_id + 1) {
		edges_vec.resize(max_node_id + 1);
	}
	edges_vec[src].push_back(target);

	if (rev_edges_vec.size() < max_node_id + 1) {
		rev_edges_vec.resize(max_node_id + 1);
	}
	rev_edges_vec[target].push_back(src);
	edge_count++;
}

uint GraphMap::node_cnt() {
	return edges_vec.size();
}

uint GraphMap::edge_cnt() {
	return this->edge_count;
}

vector<uint> GraphMap::get_out_nbrs(const uint& node) {
	vector<uint> tmp;
	if (node < edges_vec.size())
		return edges_vec[node];
	else
		return tmp;
}

vector<uint> GraphMap::get_in_nbrs(const uint& node) {
	vector<uint> tmp;
	if (node < rev_edges_vec.size())
		return rev_edges_vec[node];
	else
		return tmp;
}

uint GraphMap::get_out_deg(const uint& node) {
	if (node < edges_vec.size())
		return edges_vec[node].size();
	else
		return 0;
}

uint GraphMap::get_in_deg(const uint& node) {
	if (node < rev_edges_vec.size())
		return rev_edges_vec[node].size();
	else
		return 0;
}

uint GraphMap::get_edge_count() {
	return this->edge_count;
}

uint GraphMap::get_max_node_id() {
	return this->max_node_id;
}

void GraphMap::remove_node(const uint& node, bool remove_edges_only) {
	if (node < edges_vec.size()) {
		edge_count -= edges_vec[node].size();
		vector<uint> tmp;
		edges_vec[node] = tmp;

		vector<uint> tmp1;
		rev_edges_vec[node] = tmp1;

		for (int i = 0; i < edges_vec.size(); i++) {
			auto outNbrs = edges_vec[i];
			bool flag = false;
			for (auto it1 = outNbrs.begin(); it1 < outNbrs.end(); it1++) {
				if (*it1 == node) {
					outNbrs.erase(it1);
					flag = true;
					break;
				}
			}
			if (flag)
				edges_vec[i] = outNbrs;
		}

		for (int i = 0; i < rev_edges_vec.size(); i++) {
			auto inNbrs = rev_edges_vec[i];
			bool flag = false;
			for (auto it1 = inNbrs.begin(); it1 < inNbrs.end(); it1++) {
				if (*it1 == node) {
					inNbrs.erase(it1);
					flag = true;
					break;
				}
			}
			if (flag)
				rev_edges_vec[i] = inNbrs;
		}
	}
}
