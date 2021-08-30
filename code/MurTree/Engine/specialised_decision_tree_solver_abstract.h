#pragma once

#include "specialised_branch_misclassification_computer.h"
#include "../Data Structures/binary_data.h"
#include "../Data Structures/decision_tree.h"
#include "../Data Structures/children_information.h"
#include "../Data Structures/internal_node_description.h"

#include <vector>

namespace MurTree
{
struct SpecialisedSolverNode
{
	SpecialisedSolverNode(int inici) {
		misclassification_score = inici; feature = num_nodes_left = num_nodes_right = INT32_MAX;
	}

	SpecialisedSolverNode() { Initialise(); }
	void Initialise() { num_nodes_right = num_nodes_left = feature = misclassification_score = INT32_MAX; }

	int Misclassifications() const { return misclassification_score; }

	uint32_t feature;
	uint32_t misclassification_score;
	uint32_t num_nodes_left, num_nodes_right;
};

struct SpecialisedDecisionTreeSolverResult
{
	SpecialisedSolverNode node_budget_one, node_budget_two, node_budget_three;

	SpecialisedDecisionTreeSolverResult() {}

	SpecialisedDecisionTreeSolverResult(int misclassification) :
		node_budget_one(misclassification),
		node_budget_two(misclassification),
		node_budget_three(misclassification)
	{
	}
};

struct SpecialisedDecisionTreeSolverResult2
{
	InternalNodeDescription node_budget_one, node_budget_two, node_budget_three;

	SpecialisedDecisionTreeSolverResult2() :
		node_budget_one(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX),
		node_budget_two(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX),
		node_budget_three(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX)
	{}
};

class SpecialisedDecisionTreeSolverAbstract
{
public:
	virtual SpecialisedDecisionTreeSolverResult2 Solve(BinaryDataInternal& data) = 0;
	virtual int ProbeDifference(BinaryDataInternal& data) = 0;
};
}