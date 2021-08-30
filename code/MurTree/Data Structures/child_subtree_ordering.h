#pragma once

#include "child_subtree_info.h"

namespace MurTree
{
struct ChildSubtreeOrdering
{
	ChildSubtreeOrdering(ChildSubtreeInfo first_child, ChildSubtreeInfo second_child) :
		first_child(first_child),
		second_child(second_child)
	{}

	ChildSubtreeInfo first_child, second_child;
};
}