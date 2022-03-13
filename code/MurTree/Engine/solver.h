//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "similarity_lower_bound_computer.h"
#include "abstract_cache.h"
#include "feature_selector_abstract.h"
#include "specialised_decision_tree_solver_abstract.h"
#include "../Data Structures/internal_node_description.h"
#include "../Data Structures/binary_data.h"
#include "../Data Structures/split_binary_data.h"
#include "../Data Structures/child_subtree_ordering.h"
#include "../Utilities/stopwatch.h"
#include "../Utilities/solver_result.h"
#include "../Utilities/statistics.h"
#include "../Utilities/parameter_handler.h"

namespace MurTree
{
class Solver
{
public:
	Solver(ParameterHandler &solver_parameters);
	~Solver();

	SolverResult Solve(ParameterHandler &runtime_parameters);

	void SetVerbosity(bool verbose);

//private:

	void ReplaceData(std::vector<std::vector<FeatureVectorBinary> >& new_instances); //a hack used in parameter tuning

	DecisionNode* ConstructOptimalTree(BinaryDataInternal& data, Branch& branch, int max_depth, int num_nodes); //reconstructs the optimal tree after the algorithm populated the cache
		
	InternalNodeDescription SolveSubtree(BinaryDataInternal &data, Branch &branch, int max_depth, int num_nodes, int upper_bound);
	InternalNodeDescription SolveSubtreeGeneralCase(BinaryDataInternal& data, Branch& branch, int max_depth, int num_nodes, int upper_bound);
	InternalNodeDescription SolveTerminalNode(BinaryDataInternal& data, Branch& branch, int max_depth, int num_nodes, int upper_bound);
	bool UpdateCacheUsingSimilarity(BinaryDataInternal& data, Branch& branch, int max_depth, int num_nodes);//returns true if the method updated the optimal solution of the branch; otherwise it only (tries) to increase the lower bound
	
	InternalNodeDescription CreateInfeasibleNodeDescription();
	InternalNodeDescription CreateLeafNodeDescription(BinaryDataInternal& data);
	InternalNodeDescription CombineLeftAndRightChildren(int feature, InternalNodeDescription& left_child, InternalNodeDescription& right_child);
	ChildSubtreeOrdering GetSortedChildren(ChildSubtreeInfo& left_child, ChildSubtreeInfo& right_child);

	int LeafLabel(BinaryDataInternal& data);
	int LeafMisclassification(BinaryDataInternal& data);
	int LabelMisclassification(int label, BinaryDataInternal& data);

	bool IsTerminalNode(int depth, int num_nodes);
	bool IsNodeBetter(InternalNodeDescription node1, InternalNodeDescription node2); //returns true if node1 is better than node2. Assumes both nodes are feasible.

	int num_labels_, num_features_; //for convenience, these are set upon instance creation and not changed
	bool verbose_;
	bool dynamic_child_selection_;
	AbstractCache *cache_;
	std::vector<std::vector<FeatureVectorBinary> > feature_vectors_;
	BinaryDataInternal *binary_data_;
	Statistics stats_;
	Stopwatch stopwatch_;
	std::vector<SplitBinaryData*> splits_data;
	std::vector<FeatureSelectorAbstract*> feature_selectors_;
	SimilarityLowerBoundComputer *similarity_lower_bound_computer_;
	SpecialisedDecisionTreeSolverAbstract* specialised_solver1_, *specialised_solver2_;
};
}//end namespace MurTree