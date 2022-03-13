//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "../Data Structures/binary_data.h"

#include <vector>

namespace MurTree
{

struct DifferenceMetrics
{
	DifferenceMetrics():num_removals(0), total_difference(0) {}
	int num_removals, total_difference;
};

class BinaryDataDifferenceComputer
{
public:
	static DifferenceMetrics ComputeDifferenceMetrics(BinaryDataInternal& data_old, BinaryDataInternal& data_new);
	static void ComputeDifference(BinaryDataInternal& data_old, BinaryDataInternal& data_new, BinaryDataInternal &data_to_add, BinaryDataInternal &data_to_remove);
};
}