#include "cache_closure.h"
#include "../Utilities/runtime_assert.h"

//should include a lower bound on misclassification_score and a lower bound on sparsity_score!!!

namespace MurTree
{
	ClosureCache::ClosureCache(int num_features, int num_instances) :
		cache_(size_t(num_instances) + 1),
		use_lower_bound_caching_(true),
		use_optimal_caching_(true),
		num_features_(num_features),
		frequency_counts_(num_features, 0),
		stored_iterators_(num_instances+1)
	{
	}

	bool ClosureCache::IsOptimalAssignmentCached(BinaryDataInternal &data, const Branch& branch, int depth, int num_nodes)
	{
		runtime_assert(depth <= num_nodes);

		if (!use_optimal_caching_) { return false; }
		
		auto& hashmap = cache_[data.Size()];
		auto iter = FindIterator(data, branch);

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

	void ClosureCache::StoreOptimalBranchAssignment(BinaryDataInternal& data, const Branch& branch, const InternalNodeDescription& optimal_node, int depth, int num_nodes)
	{
		runtime_assert(depth <= num_nodes && num_nodes > 0);

		if (!use_optimal_caching_) { return; }
		
		auto& hashmap = cache_[data.Size()];
		auto iter_vector_entry = FindIterator(data, branch);
		
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
			if (!data.IsClosureSet()) { data.SetClosure(ComputeClosure(data, branch)); }
			cache_[data.Size()].insert(std::pair<Branch, std::vector<CacheEntry> >(data.GetClosure(), vector_entry));
			InvalidateStoredIterators(data);
		}
		else
		{
			//this sol is valid for size=[opt.NumNodes, num_nodes] and depths d=min(size, depth)

			//now we need to see if other node budgets have been seen before. 
			//For each budget that has been seen, update it;
			std::vector<std::vector<bool> > budget_seen(size_t(num_nodes) + 1, std::vector<bool>(depth + 1, false));
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

	void ClosureCache::TransferAssignmentsForEquivalentBranches(const BinaryDataInternal&, const Branch& branch_source, const BinaryDataInternal&, const Branch& branch_destination)
	{
		return;
	}

	InternalNodeDescription ClosureCache::RetrieveOptimalAssignment(BinaryDataInternal& data, const Branch& branch, int depth, int num_nodes)
	{
		auto& hashmap = cache_[data.Size()];
		auto iter = FindIterator(data, branch);

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

	void ClosureCache::UpdateLowerBound(BinaryDataInternal &data, const Branch& branch, int lower_bound, int depth, int num_nodes)
	{
		runtime_assert(depth <= num_nodes);

		if (!use_lower_bound_caching_) { return; }
		
		auto& hashmap = cache_[data.Size()];
		auto iter_vector_entry = FindIterator(data, branch);

		//if the branch has never been seen before, create a new entry for it
		if (iter_vector_entry == hashmap.end())
		{
			std::vector<CacheEntry> vector_entry(1, CacheEntry(depth, num_nodes));
			vector_entry[0].UpdateLowerBound(lower_bound);
			if (!data.IsClosureSet()) { data.SetClosure(ComputeClosure(data, branch)); }
			cache_[data.Size()].insert(std::pair<Branch, std::vector<CacheEntry> >(data.GetClosure(), vector_entry));
			InvalidateStoredIterators(data);
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

	int ClosureCache::RetrieveLowerBound(BinaryDataInternal &data, const Branch& branch, int depth, int num_nodes)
	{
		runtime_assert(depth <= num_nodes);

		if (!use_lower_bound_caching_) { return 0; }
		
		auto& hashmap = cache_[data.Size()];
		auto iter = FindIterator(data, branch);

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

	int ClosureCache::NumEntries() const
	{
		size_t count = 0;
		for (auto& c : cache_)
		{
			count += c.size();
		}
		return int(count);
	}

	void ClosureCache::DisableLowerBounding()
	{
		use_lower_bound_caching_ = false;
	}

	void ClosureCache::DisableOptimalCaching()
	{
		use_optimal_caching_ = false;
	}

	Branch ClosureCache::ComputeClosure(BinaryDataInternal &data, const Branch &branch)
	{
		runtime_assert(!data.IsClosureSet());
		for (int feature = 0; feature < num_features_; feature++) { frequency_counts_[feature] = 0; }
		for (int label = 0; label < data.NumLabels(); label++)
		{
			for (FeatureVectorBinary* fv : data.GetInstancesForLabel(label))
			{
				for (int j = 0; j < fv->NumPresentFeatures(); j++)
				{
					int feature = fv->GetJthPresentFeature(j);
					frequency_counts_[feature]++;
				}
			}
		}

		Branch closure;
		for (int feature = 0; feature < num_features_; feature++)
		{
			if (frequency_counts_[feature] == 0)
			{
				int code = closure.GetCode(feature, false);
				closure.branch_codes_.push_back(code);
			}
			else if (frequency_counts_[feature] == data.Size())
			{
				int code = closure.GetCode(feature, true);
				closure.branch_codes_.push_back(code);
			}
		}

		runtime_assert(branch.Depth() <= closure.Depth());
		closure.ConvertIntoCanonicalRepresentation();
		return closure;
	}

	void ClosureCache::InvalidateStoredIterators(BinaryDataInternal& data)
	{
		stored_iterators_[data.Size()].clear();
	}

	std::unordered_map<Branch, std::vector<CacheEntry>, BranchHashFunction, BranchEquality>::iterator ClosureCache::FindIterator(BinaryDataInternal& data, const Branch& branch)
	{
		//check to see if the entry is cached and return it if so
		for (ClosureCache::PairIteratorBranch& p : stored_iterators_[data.Size()])
		{
			if (p.branch == branch) { return p.iter; }
		}

		if (!data.IsClosureSet()) { data.SetClosure(ComputeClosure(data, branch)); }

		auto& hashmap = cache_[data.Size()];
		auto iter = hashmap.find(data.GetClosure());

		if (iter == hashmap.end()) { return iter; }

		ClosureCache::PairIteratorBranch hehe;
		hehe.branch = branch;
		hehe.iter = iter;

		if (stored_iterators_[data.Size()].size() == 2) { stored_iterators_[data.Size()].pop_back(); }
		stored_iterators_[data.Size()].push_front(hehe);
		return iter;
	}

}//end namespace MurTree