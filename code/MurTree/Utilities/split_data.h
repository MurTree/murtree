//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "../Data Structures/feature_vector_binary.h"

#include <vector>

namespace MurTree
{
struct SplitData
{
	std::vector<std::vector<FeatureVectorBinary> > training_data, testing_data;
};
}