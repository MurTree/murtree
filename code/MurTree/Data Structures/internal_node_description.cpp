//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#include "internal_node_description.h"
#include "../Utilities/runtime_assert.h"

namespace MurTree
{
InternalNodeDescription::InternalNodeDescription(int feature, int label, int accuracy, int num_nodes_left, int num_nodes_right):
    feature(feature),
    label(label),
    misclassification_score(accuracy),
    num_nodes_left(num_nodes_left),
    num_nodes_right(num_nodes_right)
{
}

int InternalNodeDescription::Misclassifications() const
{
    runtime_assert(IsFeasible());
    return misclassification_score;
}

int InternalNodeDescription::SparseObjective(int sparse_coefficient) const
{
    return misclassification_score + sparse_coefficient * NumNodes();
}

int InternalNodeDescription::NumNodes() const
{
    if (feature == INT32_MAX) { return 0; }
    else { return 1 + num_nodes_left + num_nodes_right; }
}

bool InternalNodeDescription::IsInfeasible() const
{
    return !IsFeasible();
}

bool InternalNodeDescription::IsFeasible() const 
{
    return misclassification_score != INT32_MAX;
}

InternalNodeDescription InternalNodeDescription::CreateInfeasibleNodeDescription()
{
    return InternalNodeDescription(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);
}

}//end namespace MurTree