#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <iostream>
#include <functional>
#include <iostream>
#include <set>
#include <vector>
#include <fstream>
#include <map>
#include <stack>
#include <queue>
#include <time.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iterator>
#include <random>

using namespace std;

#define DISTANCE_TYPE unsigned int
#define NODE_TYPE unsigned int

struct VectorHash {
	size_t operator()(const std::vector<unsigned int>& v) const {
		std::hash<unsigned int> hasher;
		size_t seed = 0;
		for (unsigned int i : v) {
			seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};

class Paths {
public:
	vector<vector<NODE_TYPE>> paths;
	Paths() {};
	Paths(vector<vector<NODE_TYPE>> a) : paths(a) {}

	inline void drop_repeat_paths_with_sort()
	{
		sort(paths.begin(), paths.end());
		auto it = unique(paths.begin(), paths.end());
		paths.erase(it, paths.end());
	}

	int size()
	{
		return paths.size();
	}

	void sort_by_string_order()
	{
		stable_sort(paths.begin(), paths.end(), less<vector<NODE_TYPE>>());
	}

	void clear()
	{
		paths.clear();
	}

	void push_back(vector<NODE_TYPE>& a)
	{
		paths.push_back(a);
	}

	void output()
	{
		// cout << "paths output : " << endl;
		for (auto iter = paths.begin(); iter != paths.end(); iter++)
		{
			for (auto iter1 = iter->begin(); iter1 != iter->end(); iter1++)
			{
				cout << *iter1 << " ";
			}
			cout << " end " << endl;
		}
	}

	void output(unordered_map<NODE_TYPE, NODE_TYPE> &node_map)
	{
		// cout << "paths output : " << endl;
		for (auto iter = paths.begin(); iter != paths.end(); iter++)
		{
			for (auto iter1 = iter->begin(); iter1 != iter->end(); iter1++)
			{
				cout << node_map[*iter1] << " ";
			}
			cout << " end " << endl;
		}
	}
	vector<vector<NODE_TYPE> >& get_path()
	{
		return paths;
	}

	void add_paths(Paths a)
	{
		for (auto iter = a.get_path().begin(); iter != a.get_path().end(); iter++)
		{
			paths.push_back(*iter);
		}
	}

	void write_to_file_append(string filename)
	{
		ofstream out;
		out.open(filename, ios::app);
		for (auto iter = paths.begin(); iter != paths.end(); iter++)
		{
			for (auto iter1 = iter->begin(); iter1 != iter->end(); iter1++)
			{
				out << *iter1 << " ";
			}
			out << endl;
		}
		out.close();
	}

	void write_to_file(string filename)
	{
		ofstream out;
		out.open(filename);
		for (auto iter = paths.begin(); iter != paths.end(); iter++)
		{
			for (auto iter1 = iter->begin(); iter1 != iter->end(); iter1++)
			{
				out << *iter1 << " ";
			}
			out << endl;
		}
		out.close();
	}

	void clear_file(string filename)
	{
		ofstream out;
		out.open(filename, fstream::out | ios_base::trunc);
		out.close();
	}

	void write_to_file_append_edgeID_and_result_size(string filename, NODE_TYPE edgeID, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		ofstream out;
		out.open(filename, std::ofstream::out | std::ofstream::app);
		out << edgeID << " : " << query_node1 << " : " << query_node2 << " : " << paths.size() << endl;
		out.close();
	}

	void write_to_file_append_edgeID(string filename, NODE_TYPE edgeID, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		ofstream out;
		out.open(filename, std::ofstream::out | std::ofstream::app);
		out << edgeID << " : " << query_node1 << " : " << query_node2 << " : " << paths.size() << endl;
		for (auto iter = paths.begin(); iter != paths.end(); iter++)
		{
			for (auto iter1 = iter->begin(); iter1 != iter->end(); iter1++)
			{
				out << *iter1 << " ";
			}
			out << endl;
		}
		out.close();
	}

	void drop_path_length_more_than_k(int k)
	{
		for (auto iter = paths.begin(); iter != paths.end();)
		{
			if (iter->size() > k + 1)
			{
				iter = paths.erase(iter);
			}
			else {
				iter++;
			}
		}
	}

	void reverse()
	{
		for (auto iter = paths.begin(); iter != paths.end(); iter++)
		{
			std::reverse(iter->begin(), iter->end());
		}
	}

	void join_not_containing_left_end_and_right_start_with_len_constraint_and_no_repeat(int k, Paths& right, Paths& rst)
	{
		unordered_map<uint, vector<vector<uint>>> right_path_map;

		for (auto rp : right.paths)
		{
			right_path_map[rp[1]].push_back(rp);
		}

		unordered_set<vector<NODE_TYPE>, VectorHash> rst_set;
		for (auto lp : this->paths)
		{
			int len1 = lp.size() - 2; // path1 len not containing last node
			auto r_paths = right_path_map[lp[len1]];
			for (auto rp : r_paths)
			{
				int len2 = rp.size() - 2; // path2 len not containing first node
				if (lp.size() >= 2 && rp.size() > 1
					&& lp.at(lp.size() - 2) == rp.at(1)
					&& len1 + len2 <= k && (len1 == len2 || len1 == len2 + 1))
				{
					bool flag = true;
					vector<NODE_TYPE> rst_p;
					set<NODE_TYPE> visited;
					for (int i = 0; i < lp.size() - 1; i++)
					{
						visited.insert(lp[i]);
						rst_p.push_back(lp[i]);
					}
					for (int i = 2; i < rp.size(); i++)
					{
						if (visited.find(rp[i]) == visited.end())
						{
							rst_p.push_back(rp[i]);
						}
						else
						{
							flag = false;
							break;
						}
					}
					if (flag)
					{
						if (rst_set.find(rst_p) == rst_set.end()) {
							rst.paths.push_back(rst_p);
							rst_set.insert(rst_p);
						}
					}
				}
			}
		}
	}

	int get_min_distance()
	{
		bool first_updated = true;
		int min_distance = 0;
		for (auto iter = paths.begin(); iter != paths.end(); iter++)
		{
			if (iter->size() == 0)
			{
				continue;
			}
			if (first_updated) {
				min_distance = iter->size() - 1;
				first_updated = false;
			}
			else {
				if (iter->size() - 1 < min_distance)
				{
					min_distance = iter->size() - 1;
				}
			}
		}
		return min_distance;
	}
};
