#include"util.h"

void print_data_for_debug(MyVec& barrier, MyVec& sd_s_u, MyVec& sd_u_t,
		uint* node_data, uint node_cnt, uint* edge_data, uint edge_cnt,
		uint* rev_node_data, uint* rev_edge_data) {
	cout << "barrier data:" << endl;
	for (uint b : barrier) {
		cout << b << " ";
	}
	cout << endl;
	cout << "sd_s_u data:" << endl;
	for (uint b : sd_s_u) {
		cout << b << " ";
	}
	cout << endl;
	cout << "sd_u_t data:" << endl;
	for (uint b : sd_u_t) {
		cout << b << " ";
	}
	cout << endl;
	cout << "node data:" << endl;
	for (int i = 0; i <= node_cnt; i++) {
		cout << node_data[i] << " ";
	}
	cout << endl;
	cout << "edge data:" << endl;
	for (int i = 0; i < edge_cnt; i++) {
		cout << edge_data[i] << " ";
	}
	cout << endl;
	cout << "rev node data:" << endl;
	for (int i = 0; i <= node_cnt; i++) {
		cout << rev_node_data[i] << " ";
	}
	cout << endl;
	cout << "rev edge data:" << endl;
	for (int i = 0; i < edge_cnt; i++) {
		cout << rev_edge_data[i] << " ";
	}
	cout << endl;
}

// split string s by char c
void split(const string& s, vector<string>& ans, char c) {
	int len = s.length();
	int last = 0;
	int fst = s.find(c);
	while (fst != -1) {
		if (fst != last) {
			ans.push_back(s.substr(last, fst));
		}
		last = fst + 1;
		fst = s.find(c, last);
	}
	ans.push_back(s.substr(last, len));
}

void put_graph(vector<vector<uint>>& g, uint* v_idx, uint* e_l) {
	v_idx[0] = 0;
	uint len = g.size();
	uint llen;
	for (uint i = 0; i < len; i += 1) {
		llen = g[i].size();
		v_idx[i + 1] = v_idx[i] + llen;

		sort(g[i].begin(), g[i].end());
		memcpy(e_l + v_idx[i], g[i].data(), llen * sizeof(uint));
	}
}

Graph into_static(vector<vector<uint>>& g, VecUInt& v_idx, VecUInt& e_l) {
	v_idx.clear();
	e_l.clear();

	v_idx.resize(g.size() + 1);

	int edge_cnt = 0;
	for (int i = 0; i < g.size(); i++) {
		edge_cnt += g[i].size();
	}
	e_l.resize(edge_cnt);

	put_graph(g, v_idx.data(), e_l.data());

	Graph rst_g(g.size(), edge_cnt, v_idx.data(), e_l.data());
	return rst_g;
}

int read_graph_map(const char filename[], GraphMap& graphMap, char sp) {
	fstream f(filename, fstream::in);

	if (f.is_open()) {
		std::string s;
		vector<std::string> line;
		uint start, end;
		while (getline(f, s)) {
			line.clear();
			if (!isdigit(s[0])) {
				continue;
			}
			split(s, line, sp);
			if (line.size() < 2)
				continue;
			start = stoi(line[0]);
			end = stoi(line[1]);
			//cout << "Start : " << start << ", End: " << end << endl;
			if(start != end){
				graphMap.insert(start, end);
			}
		}
		cout << "Graph Map Node Count: " << graphMap.node_cnt() << endl
				<< "Graph Map Edge Count: " << graphMap.edge_cnt() << endl;

		f.close();
		return 1;
	}
	return 0;
}

void compute_sd_map(const uint& start_node, const uint& k, GraphMap& g_map,
		unordered_map<uint, uint>& sd_map, bool is_rev) {

	queue<uint> nodes;
	nodes.push(start_node);

	sd_map[start_node] = 0;

	int layer = 0;
	while (!nodes.empty()) {
		int size = nodes.size();
		layer++;

		for (int i = 0; i < size; i++) {
			uint node = nodes.front();
			nodes.pop();
			vector<uint> nbrs;
			if (is_rev) {
				nbrs = g_map.get_in_nbrs(node);
			} else {
				nbrs = g_map.get_out_nbrs(node);
			}
			for (uint nbr : nbrs) {
				if (sd_map.find(nbr) == sd_map.end()) {
					sd_map[nbr] = layer;
					nodes.push(nbr);
				}
			}
		}

		if (layer >= k - 1)
			break;
	}
}

Graph pact_graph(GraphMap& g_map, GraphMap& tmpG, const uint& start_node,
		const uint& target_node, const uint& k,
		unordered_map<uint, uint>& sd_u_t_map, VecUInt& v_idx, VecUInt& e_l,
		unordered_map<uint, uint>& node_map,
		unordered_map<uint, uint>& rev_node_map) {

	compute_sd_map(target_node, k, g_map, sd_u_t_map, true);

	queue<uint> nodes;
	int layer = 0;
	int node_id = 0;

	nodes.push(start_node);
	node_map[start_node] = node_id;
	rev_node_map[node_id] = start_node;
	node_id++;
	while (!nodes.empty()) {
		int size = nodes.size();
		layer++;

		for (int i = 0; i < size; i++) {
			uint node = nodes.front();
			nodes.pop();
			vector<uint> nbrs = g_map.get_out_nbrs(node);
			unordered_set<uint> nbr_set;
			nbr_set.insert(nbrs.begin(), nbrs.end());
			for (uint nbr : nbr_set) {
				if (sd_u_t_map.find(nbr) != sd_u_t_map.end()) {
					if (node_map.find(nbr) == node_map.end()) {
						if (layer + sd_u_t_map[nbr] <= k) {
							node_map[nbr] = node_id;
							rev_node_map[node_id] = nbr;
							node_id++;
							nodes.push(nbr);
							tmpG.insert(node_map[node], node_map[nbr]);
						}
					} else {
						tmpG.insert(node_map[node], node_map[nbr]);
					}
				}
			}
		}

		if (layer >= k - 1)
			break;
	}

	// we have not touched t yet
	if (node_map.find(target_node) == node_map.end()) {
		node_map[target_node] = node_id;
		rev_node_map[node_id] = target_node;
		node_id++;

		vector<uint> nbrs = g_map.get_in_nbrs(target_node);
		for (uint nbr : nbrs) {
			if (node_map.find(nbr) != node_map.end()) {
				tmpG.insert(node_map[nbr], node_map[target_node]);
			}
		}
	}

	return into_static(tmpG.edges_vec, v_idx, e_l);
}

void dfs_hops_random_nbr(
	GraphMap& g, uint cur_node, int &ret, int cur_layer,
	int& total_layer, unordered_set<uint>& visited)
{
	visited.insert(cur_node);
	if(cur_layer == total_layer - 1)
	{
		vector<uint> nbrs = g.get_out_nbrs(cur_node);
		for(uint nbr: nbrs)
		{
			if(visited.find(nbr) == visited.end())
			{
				ret = nbr;
				break;
			}
		}
	}
	else
	{
		vector<uint> nbrs = g.get_out_nbrs(cur_node);
		uint nbr_next = cur_node;
		for(uint nbr: nbrs)
		{
			if(visited.find(nbr) == visited.end())
			{
				nbr_next = nbr;
				break;
			}
		}
		if(nbr_next == cur_node)
		{
			return;
		}
		dfs_hops_random_nbr(g, nbr_next, ret, cur_layer+1,
				total_layer, visited);
	}
	visited.erase(cur_node);
}
