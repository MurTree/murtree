#pragma once

#include "abstract_cache.h"
#include "../Data Structures/binary_data.h"
#include "../Data Structures/internal_node_description.h"
#include "../Data Structures/branch.h"
#include "../Utilities/runtime_assert.h"
#include "branch_cache.h"

#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <assert.h>
#include <deque>

namespace MurTree
{

	//key: a branch
	//value: cached value contains the optimal value and the lower bound
	class ClosureCache : public AbstractCache
	{
	public:
		ClosureCache(int num_features, int num_instances); 

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

		Branch ComputeClosure(BinaryDataInternal&, const Branch&);

		int num_features_;
		std::vector<int> frequency_counts_;

		//std::unordered_map<Branch, std::vector<CacheEntry>, BranchHashFunction, BranchEquality >::iterator FindIterator(BinaryDataInternal& data, const Branch& branch);

		std::vector<
			std::unordered_map<Branch, std::vector<CacheEntry>, BranchHashFunction, BranchEquality >
		> cache_; //cache_[i] is a hash table with branches of size i	


		//we store a few iterators that were previous used in case they will be used in the future
		//useful when we query the cache multiple times for the exact same query
		struct PairIteratorBranch
		{
			std::unordered_map<Branch, std::vector<CacheEntry>, BranchHashFunction, BranchEquality >::iterator iter;
			Branch branch;
		};

		void InvalidateStoredIterators(BinaryDataInternal& data);
		std::unordered_map<Branch, std::vector<CacheEntry>, BranchHashFunction, BranchEquality >::iterator FindIterator(BinaryDataInternal& data, const Branch& branch);
		std::vector<std::deque<ClosureCache::PairIteratorBranch> > stored_iterators_;

	};

}