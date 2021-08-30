#include "specialised_general_classification_decision_tree_solver.h"
#include "binary_data_difference_computer.h"
#include "../Utilities/runtime_assert.h"

#include <time.h>

namespace MurTree
{
	SpecialisedGeneralClassificationDecisionTreeSolver::SpecialisedGeneralClassificationDecisionTreeSolver(int num_labels, int num_features, int num_feature_vectors, bool use_incremental_frequencies) :
		num_labels_(num_labels),
		num_features_(num_features),
		penalty_computer_(num_labels, num_features, num_feature_vectors, use_incremental_frequencies),
		best_children_misclassification_info_(num_features),
		time_loop(0),
		time_initi(0)
	{
	}

	SpecialisedDecisionTreeSolverResult2 SpecialisedGeneralClassificationDecisionTreeSolver::Solve(BinaryDataInternal& data)
	{
		bool changes_made = Initialise(data);
		if (!changes_made) { return previous_output_; }
		ComputeOptimalTreesBasedOnFrequencyCounts();
		previous_output_ = ConstructOutput();
		return previous_output_;
	}

	int SpecialisedGeneralClassificationDecisionTreeSolver::ProbeDifference(BinaryDataInternal& data)
	{
		return penalty_computer_.ProbeDifference(data);
	}

	bool SpecialisedGeneralClassificationDecisionTreeSolver::Initialise(BinaryDataInternal& data)
	{
		clock_t clock_start = clock();

		bool changed_made = penalty_computer_.Initialise(data);
		if (!changed_made) { return false; }

		for (int i = 0; i < num_features_; i++)
		{
			best_children_misclassification_info_[i].left_child_feature = INT32_MAX;
			best_children_misclassification_info_[i].right_child_feature = INT32_MAX;
			best_children_misclassification_info_[i].left_child_penalty = INT32_MAX;
			best_children_misclassification_info_[i].right_child_penalty = INT32_MAX;
		}
		//todo change
		int leaf_misclassification = data.Size() - data.NumInstancesForLabel(0);
		for (int label = 1; label < num_labels_; label++)
		{
			leaf_misclassification = std::min(data.Size() - data.NumInstancesForLabel(label), 
											  leaf_misclassification);
		}
		results_ = SpecialisedDecisionTreeSolverResult(leaf_misclassification);
		time_initi += double(clock() - clock_start) / CLOCKS_PER_SEC;
		return true;
	}

	void SpecialisedGeneralClassificationDecisionTreeSolver::ComputeOptimalTreesBasedOnFrequencyCounts()
	{
		clock_t clock_start = clock();
		for (int f1 = 0; f1 < num_features_; f1++)
		{
			uint32_t penalty_left_classification = penalty_computer_.PenaltyBranchZeroZero(f1, f1);//penalty_computer_.BranchMisclassificationZero(f1);//std::min(penalty_computer_.PositivesZeroZero(f1, f1), penalty_computer_.NegativesZeroZero(f1, f1));
			uint32_t penalty_right_classification = penalty_computer_.PenaltyBranchOneOne(f1, f1);;//penalty_computer_.BranchMisclassificationOne(f1);//std::min(penalty_computer_.PositivesOneOne(f1, f1), penalty_computer_.NegativesOneOne(f1, f1));

			//update the misclassification for the tree with only one node
			int penalty_one_node = (penalty_left_classification + penalty_right_classification);

			if (penalty_one_node < results_.node_budget_one.Misclassifications())
			{
				results_.node_budget_one.feature = f1;
				results_.node_budget_one.misclassification_score = (penalty_left_classification + penalty_right_classification);
				results_.node_budget_one.num_nodes_left = 0;
				results_.node_budget_one.num_nodes_right = 0;
			}

			for (int f2 = f1 + 1; f2 < num_features_; f2++)
			{
				int penalty_branch_one_one = penalty_computer_.PenaltyBranchOneOne(f1, f2);
				int penalty_branch_one_zero = penalty_computer_.PenaltyBranchOneZero(f1, f2);
				int penalty_branch_zero_one = penalty_computer_.PenaltyBranchZeroOne(f1, f2);
				int penalty_branch_zero_zero = penalty_computer_.PenaltyBranchZeroZero(f1, f2);

				UpdateBestLeftChild(f1, f2, penalty_branch_zero_one + penalty_branch_zero_zero);
				UpdateBestRightChild(f1, f2, penalty_branch_one_one + penalty_branch_one_zero);

				UpdateBestLeftChild(f2, f1, penalty_branch_one_zero + penalty_branch_zero_zero);
				UpdateBestRightChild(f2, f1, penalty_branch_one_one + penalty_branch_zero_one);

				//update the best tree with two nodes in case a better tree has been found
				//use f1 as root
				UpdateBestTwoNodeAssignment(f1, penalty_left_classification, penalty_right_classification);
				//use f2 as root
				UpdateBestTwoNodeAssignment(f2, penalty_computer_.PenaltyBranchZeroZero(f2, f2), penalty_computer_.PenaltyBranchOneOne(f2, f2));
			}
			UpdateThreeNodeTreeInfo(f1); //it is important to call this after the previous loop, i.e., after calling UpdateTwoNodeTreeInfo(f1, f2) for all f2 > f1
		}
		time_loop += double(clock() - clock_start) / CLOCKS_PER_SEC;
	}

	SpecialisedDecisionTreeSolverResult2 SpecialisedGeneralClassificationDecisionTreeSolver::ConstructOutput()
	{
		SpecialisedDecisionTreeSolverResult2 result_final;
		result_final.node_budget_one.feature = results_.node_budget_one.feature;
		result_final.node_budget_one.misclassification_score = results_.node_budget_one.misclassification_score;
		result_final.node_budget_one.num_nodes_left = results_.node_budget_one.num_nodes_left;
		result_final.node_budget_one.num_nodes_right = results_.node_budget_one.num_nodes_right;
		result_final.node_budget_one.label = 0;

		result_final.node_budget_two.feature = results_.node_budget_two.feature;
		result_final.node_budget_two.misclassification_score = results_.node_budget_two.misclassification_score;
		result_final.node_budget_two.num_nodes_left = results_.node_budget_two.num_nodes_left;
		result_final.node_budget_two.num_nodes_right = results_.node_budget_two.num_nodes_right;
		result_final.node_budget_two.label = 0;

		result_final.node_budget_three.feature = results_.node_budget_three.feature;
		result_final.node_budget_three.misclassification_score = results_.node_budget_three.misclassification_score;
		result_final.node_budget_three.num_nodes_left = results_.node_budget_three.num_nodes_left;
		result_final.node_budget_three.num_nodes_right = results_.node_budget_three.num_nodes_right;
		result_final.node_budget_three.label = 0;

		return result_final;
	}

	void SpecialisedGeneralClassificationDecisionTreeSolver::UpdateBestTwoNodeAssignment(int root_feature, int misclassification_left, int misclassification_right)
	{
		int objective_two_nodes = best_children_misclassification_info_[root_feature].left_child_penalty + misclassification_right;
		if (results_.node_budget_two.Misclassifications() > objective_two_nodes)
		{
			results_.node_budget_two.misclassification_score = objective_two_nodes;
			results_.node_budget_two.feature = root_feature;
			results_.node_budget_two.num_nodes_left = 1;
			results_.node_budget_two.num_nodes_right = 0;
		}

		objective_two_nodes = best_children_misclassification_info_[root_feature].right_child_penalty + misclassification_left;
		if (results_.node_budget_two.Misclassifications() > objective_two_nodes)
		{
			results_.node_budget_two.misclassification_score = objective_two_nodes;
			results_.node_budget_two.feature = root_feature;
			results_.node_budget_two.num_nodes_left = 0;
			results_.node_budget_two.num_nodes_right = 1;
		}
	}

	void SpecialisedGeneralClassificationDecisionTreeSolver::UpdateThreeNodeTreeInfo(int root_feature)
	{
		runtime_assert(best_children_misclassification_info_[root_feature].left_child_penalty != UINT32_MAX);
		runtime_assert(best_children_misclassification_info_[root_feature].right_child_penalty != UINT32_MAX);

		uint32_t new_penalty = best_children_misclassification_info_[root_feature].left_child_penalty + best_children_misclassification_info_[root_feature].right_child_penalty;
		if (results_.node_budget_three.Misclassifications() > new_penalty)
		{
			results_.node_budget_three.misclassification_score = new_penalty;// best_children_misclassification_info_[root_feature].left_child_penalty + best_children_misclassification_info_[root_feature].right_child_penalty;
			//results_.node_budget_three.sparsity_score = 3 * weight_sparsity_;
			results_.node_budget_three.feature = root_feature;
			results_.node_budget_three.num_nodes_left = 1;
			results_.node_budget_three.num_nodes_right = 1;
		}
	}

	void SpecialisedGeneralClassificationDecisionTreeSolver::UpdateBestLeftChild(int feature, int left_child_feature, int penalty)
	{
		if (best_children_misclassification_info_[feature].left_child_penalty > penalty)
		{
			best_children_misclassification_info_[feature].left_child_feature = left_child_feature;
			best_children_misclassification_info_[feature].left_child_penalty = penalty;
		}
	}

	void SpecialisedGeneralClassificationDecisionTreeSolver::UpdateBestRightChild(int feature, int right_child_feature, int penalty)
	{
		if (best_children_misclassification_info_[feature].right_child_penalty > penalty)
		{
			best_children_misclassification_info_[feature].right_child_feature = right_child_feature;
			best_children_misclassification_info_[feature].right_child_penalty = penalty;
		}
	}

}//end namespace MurTree