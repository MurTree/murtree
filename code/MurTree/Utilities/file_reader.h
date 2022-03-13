//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include <string>

#include "../Data Structures/feature_vector_binary.h"
#include "split_data.h"

namespace MurTree
{
class FileReader
{
public:
	//the DL file format has a instance in each row, where the label at the first position followed by the values of the binary features separated by whitespace
	//the method reads a file in the DL file format and returns the feature vectors, where v[i] is the vector of feature vectors of label i
	// duplicate_instances_factor states how many times each instance should be duplicated: this is only useful for testing/benchmarking purposes
	static std::vector<std::vector<FeatureVectorBinary> > ReadDataDL(std::string filename, int duplicate_instances_factor = 1); 

	static std::vector<SplitData> ReadSplits(std::string data_file, std::string splits_location_prefix);
	static std::vector<std::vector<FeatureVectorBinary> > ReadSplits_ReadPartitionFile(std::vector<std::vector<FeatureVectorBinary> >& data, std::string file);
};
}