#pragma once

#include "abstract_cache.h"
#include "../Data Structures/binary_data.h"
#include "../Data Structures/internal_node_description.h"
#include "../Data Structures/branch.h"
#include "../Utilities/runtime_assert.h"

#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <assert.h>

namespace MurTree
{

//adapted from https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
//see also https://stackoverflow.com/questions/4948780/magic-number-in-boosthash-combine
//and https://www.boost.org/doc/libs/1_35_0/doc/html/boost/hash_combine_id241013.html
struct BranchHashFunction
{
	//todo check about overflows
	int operator()(Branch const& branch) const
	{
		int seed = int(branch.Depth());
		for (int i = 0; i < branch.Depth(); i++)
		{
			int code = branch[i];
			seed ^= code + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};

//assumes that both inputs are in canonical representation
struct BranchEquality
{
	bool operator()(Branch const& branch1, Branch const& branch2) const
	{
		if (branch1.Depth() != branch2.Depth()) { return false; }
		//runtime_assert(branch1.Depth() == branch2.Depth()); does not need to hold in new version
		for (int i = 0; i < branch1.Depth(); i++)
		{
			if (branch1[i] != branch2[i]) { return false; }
		}
		return true;
	}
};

//key: a branch
//value: cached value contains the optimal value and the lower bound
class BranchCache : public AbstractCache
{
public:
	BranchCache(int max_branch_length);

	//related to storing/retriving optimal branches
	bool IsOptimalAssignmentCached(BinaryDataInternal&, const Branch& branch, int depth, int num_nodes);
	void StoreOptimalBranchAssignment(BinaryDataInternal&, const Branch& branch, const InternalNodeDescription& optimal_node, int depth, int num_nodes);
	InternalNodeDescription RetrieveOptimalAssignment(BinaryDataInternal&, const Branch& branch, int depth, int num_nodes);
	void TransferAssignmentsForEquivalentBranches(const BinaryDataInternal&, const Branch& branch_source, const BinaryDataInternal&, const Branch& branch_destination);//this updates branch_destination with all solutions from branch_source. Should only be done if the branches are equivalent.

	//related to storing/retrieving lower bounds
	void UpdateLowerBound(BinaryDataInternal&, const Branch& branch, int lower_bound, int depth, int num_nodes);
	int RetrieveLowerBound(BinaryDataInternal&, const Branch& branch, int depth, int num_nodes);

	//misc
	int NumEntries() const;

	void DisableLowerBounding();
	void DisableOptimalCaching();

private:
	bool use_lower_bound_caching_;
	bool use_optimal_caching_;

	std::vector<
		std::unordered_map<Branch, std::vector<CacheEntry>, BranchHashFunction, BranchEquality >
	> cache_; //cache_[i] is a hash table with branches of size i		
};

}