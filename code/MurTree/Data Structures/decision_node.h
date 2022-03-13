//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "../Utilities/runtime_assert.h"
#include "binary_data.h"

#include <stdint.h>

namespace MurTree
{
//TODO Memory leaks
struct DecisionNode
{
	static DecisionNode* CreateLabelNode(int label)
	{
		DecisionNode* node = new DecisionNode();
		node->feature_ = INT32_MAX;
		node->label_ = label;
		node->left_child_ = NULL;
		node->right_child_ = NULL;
		return node;
	}
	static DecisionNode* CreateFeatureNodeWithNullChildren(int feature)
	{
		DecisionNode* node = new DecisionNode();
		node->feature_ = feature;
		node->label_ = INT32_MAX;
		node->left_child_ = NULL;
		node->right_child_ = NULL;
		return node;
	}
	
	int Depth() const
	{
		if (IsLabelNode()) { return 0; }
		return 1 + std::max(left_child_->Depth(), right_child_->Depth());		
	}

	int NumNodes() const
	{
		if (IsLabelNode()) { return 0; }
		return 1 + left_child_->NumNodes() + right_child_->NumNodes();
	}

	bool IsLabelNode() const { return label_ != INT32_MAX; }
	bool IsFeatureNode() const { return feature_ != INT32_MAX; }

	int ComputeMisclassificationScore(BinaryDataInternal& data)
	{
		int misclassifications = 0;
		for (int label = 0; label < data.NumLabels(); label++)
		{
			for (FeatureVectorBinary* fv : data.GetInstancesForLabel(label))
			{
				misclassifications += (Classify(fv) != label);
			}
		}
		return misclassifications;
	}

	int ComputeMisclassificationScore(std::vector<std::vector<FeatureVectorBinary> >& instances)
	{
		int misclassifications = 0;
		for (int label = 0; label < instances.size(); label++)
		{
			for (FeatureVectorBinary& fv : instances[label])
			{
				misclassifications += (Classify(&fv) != label);
			}
		}
		return misclassifications;
	}
	
	int Classify(FeatureVectorBinary* feature_vector)
	{
		if (IsLabelNode())
		{
			return label_;
		}
		else if (feature_vector->IsFeaturePresent(feature_))
		{
			return right_child_->Classify(feature_vector);
		}
		else
		{
			return left_child_->Classify(feature_vector);
		}
	}

	int feature_, label_;
	DecisionNode* left_child_, * right_child_;
};
}//end namespace MurTree