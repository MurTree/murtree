#pragma once

#include "binary_data.h"
#include "branch.h"

namespace MurTree
{
struct ChildSubtreeInfo
{
	ChildSubtreeInfo(BinaryDataInternal* binary_data, Branch& branch, int depth, int size) :
		binary_data(binary_data),
		branch(branch),
		depth(depth),
		size(size)
	{
		runtime_assert(depth <= size);
	}

	BinaryDataInternal* binary_data;
	Branch branch;
	int depth;
	int size;
};
}