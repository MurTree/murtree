//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#include "branch_cache.h"
#include "../Utilities/runtime_assert.h"

//should include a lower bound on misclassification_score and a lower bound on sparsity_score!!!

namespace MurTree
{
BranchCache::BranchCache(int max_branch_length) :
	cache_(size_t(max_branch_length) + 1),
    use_lower_bound_caching_(true),
	use_optimal_caching_(true)
{
}

bool BranchCache::IsOptimalAssignmentCached(BinaryDataInternal&, const Branch& branch, int depth, int num_nodes)
{
	runtime_assert(depth <= num_nodes);

	if (!use_optimal_caching_) { return false; }

	auto& hashmap = cache_[branch.Depth()];
	auto iter = hashmap.find(branch);

	if (iter == hashmap.end()) { return false; }

	for (CacheEntry& entry : iter->second)
	{
		if (entry.GetNodeBudget() == num_nodes && entry.GetDepthBudget() == depth)
		{
			return entry.IsOptimal();
		}
	}
	return false;
}

void BranchCache::StoreOptimalBranchAssignment(BinaryDataInternal &data, const Branch& branch, const InternalNodeDescription& optimal_node, int depth, int num_nodes)
{
	runtime_assert(depth <= num_nodes && num_nodes > 0);

	if (!use_optimal_caching_) { return; }

	auto& hashmap = cache_[branch.Depth()];
	auto iter_vector_entry = hashmap.find(branch);
	int optimal_node_depth = std::min(depth, optimal_node.NumNodes()); //this is an estimate of the depth, it could be lower actually. We do not consider lower for simplicity, but it would be good to consider it as well.

	//if the branch has never been seen before, create a new entry for it
	if (iter_vector_entry == hashmap.end()) 
	{
		std::vector<CacheEntry> vector_entry;
		for (int node_budget = optimal_node.NumNodes(); node_budget <= num_nodes; node_budget++)
		{
			for (int depth_budget = optimal_node_depth; depth_budget <= std::min(depth, node_budget); depth_budget++)
			{
				CacheEntry entry(depth_budget, node_budget, optimal_node);
				vector_entry.push_back(entry);
			}
		}
		cache_[branch.Depth()].insert(std::pair<Branch, std::vector<CacheEntry> >(branch, vector_entry));
	}
	else
	{
		//this sol is valid for size=[opt.NumNodes, num_nodes] and depths d=min(size, depth)

		//now we need to see if other node budgets have been seen before. 
		//For each budget that has been seen, update it;
		std::vector<std::vector<bool> > budget_seen(size_t(num_nodes) + 1, std::vector<bool>(depth+1, false));
		for (CacheEntry& entry : iter_vector_entry->second)
		{
			//todo enable this here! //runtime_assert(optimal_node.Misclassifications() >= entry.GetLowerBound() || optimal_node.NumNodes() > entry.GetNodeBudget());

			//I believe it rarely happens that we receive a solution with less nodes than 'num_nodes', but it is possible
			if (optimal_node.NumNodes() <= entry.GetNodeBudget() && entry.GetNodeBudget() <= num_nodes
				&& optimal_node_depth <= entry.GetDepthBudget() && entry.GetDepthBudget() <= depth)
			{
				if (!(!entry.IsOptimal() || entry.GetOptimalValue() == optimal_node.Misclassifications()))
				{
					std::cout << "opt node: " << optimal_node.NumNodes() << ", " << optimal_node.misclassification_score << "\n";
					std::cout << "\tnum nodes: " << num_nodes << "\n";
					std::cout << entry.GetNodeBudget() << ", " << entry.GetOptimalValue() << "\n";
				}
				runtime_assert(!entry.IsOptimal() || entry.GetOptimalValue() == optimal_node.Misclassifications());

				budget_seen[entry.GetNodeBudget()][entry.GetDepthBudget()] = true;
				if (!entry.IsOptimal()) { entry.SetOptimalAssignment(optimal_node); }

				runtime_assert(entry.GetDepthBudget() <= entry.GetNodeBudget()); //fix the case when it turns out that more nodes do not give a better result...e.g., depth 4 and num nodes 4, but a solution with three nodes found...that solution is then optimal for depth 3 as well...need to update but lazy now
				int g = 0;
				g++;
			}
		}
		//create entries for those which were not seen
		//note that most of the time this loop only does one iteration since usually using the full node budget gives better results
		for (int node_budget = optimal_node.NumNodes(); node_budget <= num_nodes; node_budget++)
		{
			for (int depth_budget = optimal_node_depth; depth_budget <= std::min(depth, node_budget); depth_budget++)
			{				
				if (!budget_seen[node_budget][depth_budget])
				{
					CacheEntry entry(depth_budget, node_budget, optimal_node);
					iter_vector_entry->second.push_back(entry);
					runtime_assert(entry.GetDepthBudget() <= entry.GetNodeBudget()); //todo no need for this assert
				}			
			}			
		}
	}
	//TODO: the cache needs to invalidate out solutions that are dominated, i.e., with the same objective value but less nodes
	//or I need to rethink this caching to include exactly num_nodes -> it might be strange that we ask for five nodes and get UNSAT, while with four nodes it gives a solution
	//I am guessing that the cache must store exactly num_nodes, and then outside the loop when we find that the best sol has less node, we need to insert that in the cache?
	//and mark all solutions with more nodes as infeasible, i.e., some high cost
	runtime_assert(RetrieveOptimalAssignment(data, branch, depth, num_nodes).Misclassifications() == optimal_node.Misclassifications());
}

void BranchCache::TransferAssignmentsForEquivalentBranches(const BinaryDataInternal&, const Branch& branch_source, const BinaryDataInternal&, const Branch& branch_destination)
{
	if (!use_lower_bound_caching_) { return; }

	if (branch_source == branch_destination) { return; }

	auto& hashmap = cache_[branch_source.Depth()];
	auto iter_source = hashmap.find(branch_source);
	auto iter_destination = hashmap.find(branch_destination);
	
	runtime_assert(iter_source != hashmap.end());//I believe the method will not be called if branch_source is empty, todo check
	//if (iter_vector_entry == hashmap.end()) { return; }

	if (iter_destination == hashmap.end()) //if the branch has never been seen before, create a new entry for it and copy everything into it
	{
		std::vector<CacheEntry> vector_entry = iter_source->second;
		cache_[branch_destination.Depth()].insert(std::pair<Branch, std::vector<CacheEntry> >(branch_destination, vector_entry));
	}
	else
	{
		for (CacheEntry& entry_source : iter_source->second)
		{
			//todo could be done more efficiently
			bool should_add = true;
			for (CacheEntry& entry_destination : iter_destination->second)
			{
				if (entry_source.GetDepthBudget() == entry_destination.GetDepthBudget() &&
					entry_source.GetNodeBudget() == entry_destination.GetNodeBudget())
				{
					should_add = false;
					//if the source entry is strictly better than the destination entry, replace it
					if (entry_source.IsOptimal() && !entry_destination.IsOptimal() || entry_source.GetLowerBound() > entry_destination.GetLowerBound())
					{
						entry_destination = entry_source;
						break;
					}
				}
			}
			if (should_add) { iter_destination->second.push_back(entry_source); }			
		}
	}	
}

InternalNodeDescription BranchCache::RetrieveOptimalAssignment(BinaryDataInternal &data, const Branch& branch, int depth, int num_nodes)
{
	auto& hashmap = cache_[branch.Depth()];
	auto iter = hashmap.find(branch);
	if (iter == hashmap.end()) { return InternalNodeDescription::CreateInfeasibleNodeDescription(); }

	for (CacheEntry& entry : iter->second)
	{
		if (entry.GetDepthBudget() == depth && entry.GetNodeBudget() == num_nodes && entry.IsOptimal())
		{
			return entry.GetOptimalSolution();
		}
	}
    return InternalNodeDescription::CreateInfeasibleNodeDescription();
}

void BranchCache::UpdateLowerBound(BinaryDataInternal&, const Branch& branch, int lower_bound, int depth, int num_nodes)
{
	runtime_assert(depth <= num_nodes);
	
	if (!use_lower_bound_caching_) { return; }

	auto& hashmap = cache_[branch.Depth()];
	auto iter_vector_entry = hashmap.find(branch);

	//if the branch has never been seen before, create a new entry for it
	if (iter_vector_entry == hashmap.end()) 
	{
		std::vector<CacheEntry> vector_entry(1, CacheEntry(depth, num_nodes));
		vector_entry[0].UpdateLowerBound(lower_bound);
		cache_[branch.Depth()].insert(std::pair<Branch, std::vector<CacheEntry> >(branch, vector_entry));
	}
	else
	{
		//now we need to see if this node node_budget has been seen before. 
		//If it was seen, update it; otherwise create a new entry
		bool found_corresponding_entry = false;
		for (CacheEntry& entry : iter_vector_entry->second)
		{
			if (entry.GetDepthBudget() == depth && entry.GetNodeBudget() == num_nodes)
			{
				entry.UpdateLowerBound(lower_bound);
				found_corresponding_entry = true;
				break;
			}
		}

		if (!found_corresponding_entry)
		{
			CacheEntry entry(depth, num_nodes);
			entry.UpdateLowerBound(lower_bound);
			iter_vector_entry->second.push_back(entry);
		}
	}
}

int BranchCache::RetrieveLowerBound(BinaryDataInternal&, const Branch& branch, int depth, int num_nodes)
{
	runtime_assert(depth <= num_nodes);

	if (!use_lower_bound_caching_) { return 0; }

	auto& hashmap = cache_[branch.Depth()];
	auto iter = hashmap.find(branch);

	if (iter == hashmap.end()) { return 0; }

	//compute the misclassification lower bound by considering that branches with more node/depth budgets 
	//  can only have less or equal misclassification than when using the prescribed number of nodes and depth
	int best_lower_bound = 0;
	for (CacheEntry& entry : iter->second)
	{
		if (num_nodes <= entry.GetNodeBudget() && depth <= entry.GetDepthBudget())
		{
			int local_lower_bound = entry.GetLowerBound();
			best_lower_bound = std::max(best_lower_bound, local_lower_bound);
		}
	}
	return best_lower_bound;
}

int BranchCache::NumEntries() const
{
	size_t count = 0;
	for (auto& c : cache_)
	{
		count += c.size();
	}
	return int(count);
}

void BranchCache::DisableLowerBounding()
{
    use_lower_bound_caching_ = false;
}

void BranchCache::DisableOptimalCaching()
{
	use_optimal_caching_ = false;
}

}//end namespace MurTree