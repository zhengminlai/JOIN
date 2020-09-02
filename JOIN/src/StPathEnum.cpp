#include"StPathEnum.h"

StaticGraphStPathCalculation::StaticGraphStPathCalculation(NODE_TYPE s,
		NODE_TYPE t, int k, GraphMap& g, bool count_only) {
	this->s = s;
	this->t = t;
	this->k = k;
	this->g = g;
	this->count_only = count_only;
	count = 0;
}

StaticGraphStPathCalculation::~StaticGraphStPathCalculation() {
}

void StaticGraphStPathCalculation::set_s(NODE_TYPE s) {
	this->s = s;
}

void StaticGraphStPathCalculation::set_t(NODE_TYPE t) {
	this->t = t;
}

void StaticGraphStPathCalculation::set_k(int k) {
	this->k = k;
}

int StaticGraphStPathCalculation::graph_node_count() {
	return g.node_cnt();
}

/// BC-DFS
uint StaticGraphStPathCalculation::calculate_st_k_paths_baseline(Paths& paths) {
	this->count = 0;
	calculate_shortest_distance_map(); // pre-processing
	vector<NODE_TYPE> cur_stack;
	unordered_set<NODE_TYPE> visited;
	bc_dfs(s, cur_stack, paths, visited);
	return this->count;
}

/// JOIN
uint StaticGraphStPathCalculation::calculate_st_k_paths_join(Paths& paths,
		double& run_time, double& cpu_process_time) {
	this->count = 0;
	run_time = 0;
	cpu_process_time = 0;
	clock_t cpu_start = clock();
	calculate_shortest_distance_map();
	calculate_middle_vertices(); // calculate Pm

	unordered_set<uint> middleNodes;
	for (auto m : middle_vertices_map) {
		for (auto v : m.second) {
			middleNodes.insert(v);
		}
	}
	if (middleNodes.size() > 0) {
		NODE_TYPE virtual_node1 = add_virtual_vertex(true); // for each vertex v in Pm, add an edge (v, virtual_node1)
		NODE_TYPE virtual_node2 = add_virtual_vertex(false); // for each vertex v in Pm, add an edge (virtual_node2, v)

		StaticGraphStPathCalculation st_paths_left(s, virtual_node1,
				(k + 1) / 2 + 1, g, false);
		Paths paths_s_t1;

		st_paths_left.calculate_shortest_distance_map(); // pre-processing
		vector<NODE_TYPE> cur_stack;
		unordered_set<NODE_TYPE> visited;

		clock_t start_process_left = clock();
		st_paths_left.bc_dfs(s, cur_stack, paths_s_t1, visited);

		cout << "PL computed: " << paths_s_t1.size() << endl;
		// paths_s_t1.output();
		clock_t end_process_left = clock();

		StaticGraphStPathCalculation st_paths_right(virtual_node2, t, k / 2 + 1,
				g, false);
		Paths paths_t_t2;
		st_paths_right.calculate_shortest_distance_map(); // pre-processing
		vector<NODE_TYPE> cur_stack_1;
		unordered_set<NODE_TYPE> visited_1;

		clock_t start_process_right = clock();
		st_paths_right.bc_dfs(virtual_node2, cur_stack_1, paths_t_t2, visited_1);

		cout << "PR computed: " << paths_t_t2.size() << endl;
		// paths_t_t2.output();

		paths_s_t1.join_not_containing_left_end_and_right_start_with_len_constraint_and_no_repeat(
				k, paths_t_t2, paths);
		this->count = paths.size();

		clock_t end_process_right = clock();

		clock_t cpu_end = clock();

		run_time = (double) (cpu_end - cpu_start) / CLOCKS_PER_SEC;
		cpu_process_time = (double)(end_process_right - start_process_right
							+ end_process_left - start_process_left) / CLOCKS_PER_SEC;
		remove_node(virtual_node1);
		remove_node(virtual_node2);
		return this->count;
	} else {
		cout << "Middle Vertices Cut is empty! No virtual nodes added!" << endl;
		clock_t cpu_end = clock();
		run_time = (double) (cpu_end - cpu_start) / CLOCKS_PER_SEC;
		this->count = 0;
		return this->count;
	}
}

uint StaticGraphStPathCalculation::bc_dfs(const NODE_TYPE& u,
		vector<NODE_TYPE>& cur_path, Paths& paths,
		unordered_set<NODE_TYPE>& visited) {
	uint F = k + 1;
	cur_path.push_back(u);
	visited.insert(u);
	if (u == t) {
		count++;
		if (!count_only) {
			// add cur path to rst paths
			paths.push_back(cur_path);
		}
		cur_path.pop_back();
		visited.erase(u);
		return 0;
	} else if (cur_path.size() - 1 < k) {
		auto nei = g.get_out_nbrs(u);
		unordered_set<uint> nbr_set;
		nbr_set.insert(nei.begin(), nei.end());
		//cout << "Node " << u << "'s nbr size: " << nei.size() << endl;
		for (uint nbr : nbr_set) {
			bool found = visited.find(nbr) != visited.end();
			// initialize the bar[out_nbr]
			if (bar.find(nbr) == bar.end()) {
				if (sd_u_t.find(nbr) == sd_u_t.end()) {
					bar[nbr] = k + 1;
				} else {
					bar[nbr] = sd_u_t[nbr]; // because sd_u_t[u] <= bar[u] <= (k - sd_s_u[u]) holds
				}
			}
			if (!found && cur_path.size() + bar[nbr] <= k) {
				uint f = bc_dfs(nbr, cur_path, paths, visited);
				if (f != k + 1) {
					F = F < (f + 1) ? F : (f + 1);
				}
			}
		}
	}
	if (F == k + 1) {
		bar[u] = k + 1 - cur_path.size();
		//if (bar[u] < k - sd_s_u[u])
		//bar[u] = k - sd_s_u[u]; // because sd_u_t[u] <= bar[u] <= (k - sd_s_u[u]) holds
	} else {
		update_barrier(u, F, visited);
	}

	cur_path.pop_back();
	visited.erase(u);
	return F;
}

void StaticGraphStPathCalculation::update_barrier(const NODE_TYPE& u,
		uint u_t_dis, unordered_set<NODE_TYPE>& visited) {
	//if (bar[u] > u_t_dis && visited.size() <= k + 1)
	if (bar[u] > u_t_dis) {
		bar[u] = u_t_dis;
		auto nei = g.get_in_nbrs(u);
		for (uint v : nei) {
			if (visited.find(v) == visited.end()) {
				visited.insert(v);
				update_barrier(v, u_t_dis + 1, visited);
				visited.erase(v);
			}
		}
	}
}

NODE_TYPE StaticGraphStPathCalculation::add_virtual_vertex(bool left) {
	NODE_TYPE virtual_node = g.get_max_node_id() + 1;

	cout << "Added virtual node: " << virtual_node << endl;
	unordered_set<uint> middleNodes;
	for (auto m : middle_vertices_map) {
		for (auto v : m.second) {
			middleNodes.insert(v);
		}
	}

	for (uint v : middleNodes) {
		if (left) {
			g.insert(v, virtual_node);
		} else {
			g.insert(virtual_node, v);
		}
	}
	return virtual_node;
}

void StaticGraphStPathCalculation::remove_node(const NODE_TYPE& node) {
	cout << "Remove virtual node " << node << endl;
	g.remove_node(node, false);
}

void StaticGraphStPathCalculation::calculate_shortest_distance() {
	queue<NODE_TYPE> nodes;
	nodes.push(s);

	unordered_set<NODE_TYPE> visited;
	visited.insert(s);

	int layer = 0;
	while (!nodes.empty()) {
		int size = nodes.size();
		layer++;

		for (int i = 0; i < size; i++) {
			NODE_TYPE node = nodes.front();
			nodes.pop();
			vector<uint> nbrs;
			nbrs = g.get_out_nbrs(node);

			for (uint nbr : nbrs) {
				if(nbr == t)
				{
					shortest_dis = layer;
					return;
				}
				if (visited.find(nbr) == visited.end()) {
					nodes.push(nbr);
					visited.insert(nbr);
				}
			}
		}

		if (layer >= k)
			break;
	}
}

void StaticGraphStPathCalculation::calculate_shortest_distance_map() {
	bfs_preprocessing_baseline(s, sd_s_u, false);
	bfs_preprocessing_baseline(t, sd_u_t, true);
}

void StaticGraphStPathCalculation::calculate_middle_vertices() {
	set<NODE_TYPE> Lm, Rm;
	Lm.insert(s);
	Rm.insert(t);

	for (int i = 1; i <= (k + 1) / 2; i++) {
		set<NODE_TYPE> tmp_Lm;
		for (NODE_TYPE node : Lm) {
			auto nei = g.get_out_nbrs(node);
			for (auto nbr : nei) {
				tmp_Lm.insert(nbr);
			}
		}

		Lm = tmp_Lm; // update to next layer nodes

		set<NODE_TYPE> middle_vertices_set_1;
		std::set_intersection(Lm.begin(), Lm.end(), Rm.begin(), Rm.end(),
				std::inserter(middle_vertices_set_1,
						middle_vertices_set_1.begin()));
		middle_vertices_map[2 * i - 1] = middle_vertices_set_1;

		if (i == k / 2 + 1) {
			break;
		}

		set<NODE_TYPE> tmp_Rm;
		for (NODE_TYPE node : Rm) {
			auto nei = g.get_in_nbrs(node);
			for (auto nbr : nei) {
				tmp_Rm.insert(nbr);
			}
		}

		Rm = tmp_Rm; // update to next layer nodes

		set<NODE_TYPE> middle_vertices_set_2;
		std::set_intersection(Lm.begin(), Lm.end(), Rm.begin(), Rm.end(),
				std::inserter(middle_vertices_set_2,
						middle_vertices_set_2.begin()));
		middle_vertices_map[2 * i] = middle_vertices_set_2;
	}
}

void StaticGraphStPathCalculation::bfs_preprocessing_baseline(
		const NODE_TYPE& start_node, unordered_map<NODE_TYPE, uint>& sd_map,
		bool is_rev) {
	queue<NODE_TYPE> nodes;
	nodes.push(start_node);

	if (is_rev) {
		sd_u_t[start_node] = 0;
	} else {
		sd_s_u[start_node] = 0;
	}

	int layer = 0;
	while (!nodes.empty()) {
		int size = nodes.size();
		layer++;

		for (int i = 0; i < size; i++) {
			NODE_TYPE node = nodes.front();
			nodes.pop();
			vector<uint> nbrs;
			if (is_rev) {
				nbrs = g.get_in_nbrs(node);
			} else {
				nbrs = g.get_out_nbrs(node);
			}
			for (uint nbr : nbrs) {
				if (sd_map.find(nbr) == sd_map.end()) {
					sd_map[nbr] = layer;
					nodes.push(nbr);
				}
			}
		}

		if (layer >= k)
			break;
	}
}
