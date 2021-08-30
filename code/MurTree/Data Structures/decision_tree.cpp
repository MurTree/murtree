#include "decision_tree.h"

namespace MurTree
{
/*int DecisionTree::Evaluate(BinaryDataInternal& data)
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

int DecisionTree::Classify(FeatureVectorBinary* fv)
{
	DecisionNode current_node = GetRootNode();
	while (current_node.IsLabelNode() == false)
	{
		if (fv->IsFeaturePresent(current_node.feature))
		{
			current_node = GetRightChild(current_node);
		}
		else
		{
			current_node = GetLeftChild(current_node);
		}
	}
	return current_node.label;
}*/

}