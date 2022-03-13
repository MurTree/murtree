//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "../Data Structures/decision_node.h"

namespace MurTree
{

class SolverResult
{
public:

	//SolverResult();

	bool IsFeasible() const;
	bool IsProvenOptimal() const;

	//DecisionTreeNode decision_tree;

//private:
	int misclassifications;
	bool is_proven_optimal;
	DecisionNode* decision_tree_;
};

}