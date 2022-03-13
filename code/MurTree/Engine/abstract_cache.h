//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "../Data Structures/binary_data.h"
#include "../Data Structures/internal_node_description.h"
#include "../Data Structures/branch.h"
#include "../Utilities/runtime_assert.h"

namespace MurTree
{

struct CacheEntry
{
	CacheEntry(int depth, int num_nodes) :
		optimal_assignment(InternalNodeDescription::CreateInfeasibleNodeDescription()),
		lower_bound(0),
		depth(depth),
		num_nodes(num_nodes)
	{
		runtime_assert(depth <= num_nodes);
	}

	CacheEntry(int depth, int num_nodes, InternalNodeDescription optimal_node) :
		optimal_assignment(optimal_node),
		lower_bound(optimal_node.Misclassifications()),
		depth(depth),
		num_nodes(num_nodes)
	{
		runtime_assert(depth <= num_nodes);
	}

	InternalNodeDescription GetOptimalSolution() const { runtime_assert(IsOptimal()); return optimal_assignment; }

	int GetOptimalValue() const { runtime_assert(IsOptimal()); return optimal_assignment.Misclassifications(); }

	int GetLowerBound() const { runtime_assert(lower_bound >= 0 && lower_bound < INT32_MAX); return lower_bound; }
	
	void SetOptimalAssignment(const InternalNodeDescription& optimal_node)
	{
		runtime_assert(optimal_assignment.IsInfeasible());
		optimal_assignment = optimal_node;
		lower_bound = optimal_node.Misclassifications();
	}

	void UpdateLowerBound(int lb)
	{
		runtime_assert(lb < INT32_MAX&& lb >= 0 && (lb <= lower_bound && IsOptimal() || !IsOptimal()));
		lower_bound = std::max(lower_bound, lb);
	}

	bool IsOptimal() const { return optimal_assignment.IsFeasible(); }

	int GetNodeBudget() const { return num_nodes; }

	int GetDepthBudget() const { return depth; }

private:
	InternalNodeDescription optimal_assignment;
	int lower_bound;
	int depth;
	int num_nodes;
};

//key: a branch
//value: cached value contains the optimal value and the lower bound
class AbstractCache
{
public:
	//related to storing/retriving optimal branches
	virtual bool IsOptimalAssignmentCached(BinaryDataInternal&, const Branch& branch, int depth, int num_nodes) = 0;
	virtual void StoreOptimalBranchAssignment(BinaryDataInternal&, const Branch& branch, const InternalNodeDescription& optimal_node, int depth, int num_nodes) = 0;
	virtual InternalNodeDescription RetrieveOptimalAssignment(BinaryDataInternal&, const Branch& branch, int depth, int num_nodes) = 0;
	virtual void TransferAssignmentsForEquivalentBranches(const BinaryDataInternal&, const Branch& branch_source, const BinaryDataInternal&, const Branch& branch_destination) = 0;//this updates branch_destination with all solutions from branch_source. Should only be done if the branches are equivalent.

	/*virtual std::vector<CacheEntry> GetCacheEntries(const BinaryDataInternal&, const Branch& branch) = 0;
	virtual InternalNodeDescription RetrieveOptimalAssignment(std::vector<CacheEntry>& entries, int depth, int num_nodes) = 0;
	virtual int RetrieveLowerBound(std::vector<CacheEntry>& entries, int depth, int num_nodes) = 0;*/

	//related to storing/retrieving lower bounds
	virtual void UpdateLowerBound(BinaryDataInternal&, const Branch& branch, int lower_bound, int depth, int num_nodes) = 0;
	virtual int RetrieveLowerBound(BinaryDataInternal&, const Branch& branch, int depth, int num_nodes) = 0;

	//misc
	virtual int NumEntries() const = 0;

	virtual void DisableLowerBounding() = 0;
	virtual void DisableOptimalCaching() = 0;
};
}