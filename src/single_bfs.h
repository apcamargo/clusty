#pragma once 

// *******************************************************************************************
// This file is a part of Clusty software distributed under GNU GPL 3 license.
// The homepage of the Clusty project is https://github.com/refresh-bio/Clusty
//
// Copyright(C) 2024-2024, A.Gudys, K.Siminski, S.Deorowicz
//
// *******************************************************************************************

#include <vector>
#include <algorithm>
#include <deque>
#include <utility>

#include "distances.h"
#include "clustering.h"

#define debug(x) std::cerr << __FILE__ << " (" << __LINE__ << ") " << #x << " == " << (x) << std::endl

std::ostream & operator<< (std::ostream & s, const std::deque<int> & q)
{
	s << "[ ";
	for (const auto n : q)
	  s << n << " ";
	return s << "]";
}


/** MMseqs1 clustering algorithm
    based on connected component search in an undirected graph
*/
template <class DistanceMatrix>
class SingleLinkageBFS : public IClustering<DistanceMatrix>
{
public:
	
	/** @param[out] assignments The row is a complete clustering for a threshold. Each value in a row denotes a group an object has been assgned to.
	 @return number of elaborated clusters */
	int operator() (
		const DistanceMatrix& distances,
		const std::vector<int>& objects,
		const double threshold,
		std::vector<int>& assignments) override
	{
		const int NO_ASSIGNMENT { -1 };
		int nObjects = (int)objects.size();
		assignments.resize(nObjects, NO_ASSIGNMENT); // -1: no assignment
			
		/*
		std::vector<std::pair<int, int>> obj2connections(nObjects);
		for (int i = 0; i < nObjects; ++i) {
			int obj = objects[i];
			obj2connections[i].first = obj;
			obj2connections[i].second = distances.get_num_neighbours(obj);
		}

		std::stable_sort(obj2connections.begin(), obj2connections.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
		*/

		int cluster_number = 0;
		for (int i = 0; i < nObjects; ++i)
		{
			int obj = objects[i];
			
			if (assignments[obj] == NO_ASSIGNMENT)   // The current node has been not assigned with a group number.
			{
				// breadth first search
				std::deque<int> Q(1, obj); // queue of nodes
			
				while (not Q.empty())
				{
					int node = Q.front();
					Q.pop_front();
					
					if (assignments[node] == NO_ASSIGNMENT) {
						assignments[node] = cluster_number; // seed of a new cluster 

						for (const dist_t* edge = distances.begin(node); edge < distances.end(node); ++edge) {
							auto other = edge->u.s.hi;
							if (edge->d <= threshold && assignments[other] == NO_ASSIGNMENT) {
								Q.push_back(other);
							}
						}
					}
				}
				cluster_number++;
			}
		}
		return cluster_number;		
	}
	
};
