//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "binary_data.h"
#include "decision_node.h"

namespace MurTree
{
class DecisionTree
{
public:
	DecisionNode* root_node;


/*	DecisionTree();
	static DecisionTree CreateStump(int label);

	void Reset(int depth);
	void SetNodeFeature(int node_id, int feature);
	void SetNodeLabel(int node_id, int label);

	int Evaluate(BinaryDataInternal& data);
	int Classify(FeatureVectorBinary* fv);

	DecisionNode GetFreshNode();
	DecisionNode GetRootNode();
	DecisionNode GetParent(DecisionNode node);
	DecisionNode GetLeftChild(DecisionNode node);
	DecisionNode GetRightChild(DecisionNode node);

	int NumFeatureNodes(); 
	int Depth();

private:	
	std::vector<DecisionNode> nodes_;
	std::vector<int> parent_, left_child_, right_child_;
	int next_index_;*/
};

}