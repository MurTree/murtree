#pragma once

#include "abstract_cache.h"
#include "../Utilities/runtime_assert.h"

#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <assert.h>
#include <deque>

namespace MurTree
{
	
//adapted from https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
//see also https://stackoverflow.com/questions/4948780/magic-number-in-boosthash-combine
//and https://www.boost.org/doc/libs/1_35_0/doc/html/boost/hash_combine_id241013.html
struct DatasetHashFunction
{
	//todo check about overflows
	int operator()(BinaryDataInternal const& data) const
	{
		return data.GetHash();
	}

	static int ComputeHashForData(BinaryDataInternal const& data)
	{
		int seed = int(data.Size());
		for (int label = 0; label < data.NumLabels(); label++)
		{
			for (int i = 0; i < data.NumInstancesForLabel(label); i++)
			{
				int code = data.GetInstance(label, i)->GetID();
				seed ^= code + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}
		}
		return seed;
	}
};

//assumes that both inputs are in canonical representation
struct DatasetEquality
{
	bool operator()(BinaryDataInternal const& data1, BinaryDataInternal const& data2) const
	{
		runtime_assert(data1.NumLabels() == data2.NumLabels() && data1.NumFeatures() == data2.NumFeatures());

		if (data1.GetHash() != data2.GetHash()) { return false; }

		//basic check on the size
		if (data1.Size() != data2.Size()) { return false; }
		//basic check on the size of each individual label
		for (int label = 0; label < data1.NumLabels(); label++)
		{
			if (data1.NumInstancesForLabel(label) != data2.NumInstancesForLabel(label)) { return false; }
		}

		//now compare individual feature vectors
		//note that the indicies are kept sorted in the data
		for (int label = 0; label < data1.NumLabels(); label++)
		{
			for (int i = 0; i < data1.NumInstancesForLabel(label); i++)
			{
				int code1 = data1.GetInstance(label, i)->GetID();
				int code2 = data2.GetInstance(label, i)->GetID();
				if (code1 != code2) { return false; }
			}
		}
		return true;
	}
};

//key: a dataset
//value: cached value contains the optimal value and the lower bound
class DatasetCache : public AbstractCache
{
public:
	DatasetCache(int max_branch_length);

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

//private:
	bool use_lower_bound_caching_;
	bool use_optimal_caching_;

	std::vector<
		std::unordered_map<BinaryDataInternal, std::vector<CacheEntry>, DatasetHashFunction, DatasetEquality >
	> cache_; //cache_[i] is a hash table with datasets of size i	

	//we store a few iterators that were previous used in case they will be used in the future
	//useful when we query the cache multiple times for the exact same query
	struct PairIteratorBranch
	{
		std::unordered_map<BinaryDataInternal, std::vector<CacheEntry>, DatasetHashFunction, DatasetEquality >::iterator iter;
		Branch branch;
	};

	void InvalidateStoredIterators(BinaryDataInternal& data);
	std::unordered_map<BinaryDataInternal, std::vector<CacheEntry>, DatasetHashFunction, DatasetEquality >::iterator FindIterator(BinaryDataInternal& data, const Branch& branch);
	std::vector<std::deque<PairIteratorBranch> > stored_iterators_;
};
}