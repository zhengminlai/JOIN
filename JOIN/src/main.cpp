#include <iostream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "util.h"
#include "pair_hash.h"
#include "StPathEnum.h"

using namespace std;

void write_queries_to_file(string& filename,
	unordered_set<pair<uint, uint>, pair_hash>& queries) {
	ofstream out;
	out.open(filename);
	for (auto query : queries) {
		out << query.first << " " << query.second << endl;
	}
	out.close();
}

unordered_set<pair<uint, uint>, pair_hash> generate_random_queries(
	string& dir, string& graph_name, GraphMap& g, int k, int cnt, int upper_bound) {
	unordered_set<pair<uint, uint>, pair_hash> queries;
	string file_name = dir + graph_name + "_" + to_string(k) + ".txt";
	fstream f(file_name, fstream::in);

	if (f.is_open()) {
		string s;
		vector<string> line;
		uint start, end;
		while (getline(f, s)) {
			line.clear();
			if (!isdigit(s[0])) {
				continue;
			}
			split(s, line, ' ');
			if (line.size() < 2)
				continue;
			start = stoi(line[0]);
			end = stoi(line[1]);
			auto q = make_pair(start, end);
			if (start != end && queries.find(q) == queries.end()) {
				queries.insert(q);
			}
		}
		f.close();
	}
	else {
		srand(time(NULL));
		while (queries.size() < cnt) {
			uint start = rand() % upper_bound;
			unordered_set<uint> visited;
			int target = -1;
			int hop = 1;
			dfs_hops_random_nbr(g, start, target, 0, hop, visited);
			if (target >= 0 && start != target) {
				auto q = make_pair(start, target);
				if (queries.find(q) == queries.end()) {
					queries.insert(q);
				}
			}
		}
		write_queries_to_file(file_name, queries);
	}
	return queries;
}


int main() {

	cout << "Please input the graph edge file: ";
	string file_name;
	cin >> file_name;

	cout << "Please input the separator:[c|b|t], `c` for `comma`, `b` for `blank`, `t` for tab: ";
	char sp;
	cin >> sp;
	if (sp == 'c') {
		sp = ',';
	}
	else if (sp == 'b') {
		sp = ' ';
	}
	else {
		sp = '\t';
	}
	cout << "How many queries do you want to evaluate: ";
	int query_num;
	cin >> query_num;

	cout << "Please input the length constraint: ";
	int k;
	cin >> k;

	cout << "Please input the graph name: ";
	string graph_name;
	cin >> graph_name;

	GraphMap graphMap;
	cout << "Reading Graph... " << endl;

	int ret = read_graph_map(file_name.c_str(), graphMap, sp);

	if (ret == 0) {
		cout << "File does not exist! " << endl;
		return 0;
	}

	cout << "Max Node Id: " << graphMap.get_max_node_id() << endl;

	string dir = "D:\\Work\\PathEnumeration\\queries\\";
	auto queries = generate_random_queries(dir, graph_name, graphMap, k, query_num, graphMap.get_max_node_id());
	double cpu_join_total_pre_and_query_time = 0;
	double cpu_join_total_query_time = 0;
	double cpu_join_total_pre_time = 0;

	long long rst_cnt = 0;
	int query_cnt = 0;
	for (auto q : queries) {
		int s = q.first;
		int t = q.second;
		cout << "Query " << query_cnt << ": (" << s << ", " << t << ")...."
			<< endl << endl;

		StaticGraphStPathCalculation st_path_enum(s, t, k, graphMap, true);
		cout << "st_path_enum initialization is finished..." << endl;

		cout << endl << "----------- JOIN in CPU ---------------" << endl;
		Paths paths;
		double cpu_time = 0, cpu_process_time = 0;

		uint cpu_rst = st_path_enum.calculate_st_k_paths_join(paths, cpu_time, cpu_process_time);
		cout << "CPU JOIN computed total path count: " << cpu_rst << endl;
		cout << endl << "#JOIN CPU Pre + Query Time: " << fixed
			<< setprecision(3) << cpu_time << "s" << endl << endl;
		cout << endl << "#JOIN CPU Query Time: " << fixed << setprecision(3)
			<< cpu_process_time << "s" << endl << endl;
		cpu_join_total_pre_and_query_time += cpu_time;
		cpu_join_total_query_time += cpu_process_time;
		cpu_join_total_pre_time += (cpu_time - cpu_process_time);

		rst_cnt += cpu_rst;
		if (cpu_rst > 0) {
			query_cnt++;
		}
		if (query_cnt != 0) {
			cout << "Valid Query Count: " << left << setw(3) << query_cnt << endl;
			cout << "Avg Path Count: " << left << setw(3) << rst_cnt / query_cnt
				<< endl;
			cout << "CPU JOIN Avg Preprocessing Time: " << left << setw(3)
				<< cpu_join_total_pre_time / (double)query_cnt << "s"
				<< endl;

			cout << "CPU JOIN Avg Total Time: " << left << setw(3)
				<< cpu_join_total_pre_and_query_time / (double)query_cnt
				<< "s" << endl;

			cout << "CPU JOIN Avg Query Time: " << left << setw(3)
				<< cpu_join_total_query_time / (double)query_cnt << "s"
				<< endl;
		}
	}

	cout << endl << endl;
	if (query_cnt == 0) {
		cout << "All random queries lead to 0 answer paths!" << endl;
	}
	return 0;
}
