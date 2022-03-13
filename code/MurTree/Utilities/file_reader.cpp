//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#include "file_reader.h"
#include "runtime_assert.h"

#include <iostream>
#include <sstream>
#include <string>
#include <set>

namespace MurTree
{
std::vector<std::vector<FeatureVectorBinary> > FileReader::ReadDataDL(std::string filename, int duplicate_instances_factor)
{
	std::vector<std::vector<FeatureVectorBinary> > feature_vectors;
	
	std::ifstream file(filename.c_str());

	if (!file) { std::cout << "Error: File " << filename << " does not exist!\n"; runtime_assert(file); }

	std::string line;
	int id = 0;
	int num_features = INT32_MAX;
	while (std::getline(file, line))
	{
		runtime_assert(num_features == INT32_MAX || num_features == (line.length() - 1) / 2);
		if (num_features == INT32_MAX) { num_features = (line.length() - 1) / 2; }

		std::istringstream iss(line);
		//the first value in the line is the label, followed by 0-1 features
		int label;
		iss >> label;
		std::vector<bool> v;
		for (uint32_t i = 0; i < num_features; i++)
		{
			uint32_t temp;
			iss >> temp;
			runtime_assert(temp == 0 || temp == 1);
			v.push_back(temp);
		}

		if (feature_vectors.size() <= label) { feature_vectors.resize(label+1); } //adjust the vector to take into account the new label (recall that labels are expected to be from 0..num_labels-1
		for (int i = 0; i < duplicate_instances_factor; i++)
		{
			feature_vectors[label].push_back(FeatureVectorBinary(v, id));
			id++;			
		}		
	}
	return feature_vectors;
}

std::vector<SplitData> FileReader::ReadSplits(std::string data_file, std::string splits_location_prefix)
{//splits_location_prefix' + '_train[i].txt'
	std::vector<std::vector<FeatureVectorBinary> > raw_data = ReadDataDL(data_file);
	std::vector<SplitData> splits;
	for (int k = 0; k < 5; k++)
	{
		std::string file_test = splits_location_prefix + "_test" + std::to_string(k) + ".txt";
		std::string file_train = splits_location_prefix + "_train" + std::to_string(k) + ".txt";
		SplitData split;
		split.testing_data = ReadSplits_ReadPartitionFile(raw_data, file_test);
		split.training_data = ReadSplits_ReadPartitionFile(raw_data, file_train);
		splits.push_back(split);
	}
	return splits;
}

std::vector<std::vector<FeatureVectorBinary> > FileReader::ReadSplits_ReadPartitionFile(std::vector<std::vector<FeatureVectorBinary> >& instances, std::string file)
{
	std::ifstream input(file.c_str());
	std::vector<std::vector<FeatureVectorBinary> > partition(instances.size());

	int num_instances = 0;
	for (auto& v : instances) { num_instances += v.size(); }
	std::vector<int> labels(num_instances, -1);
	std::vector<int> indicies(num_instances, -1);
	for (int i = 0; i < instances.size(); i++)
	{
		auto& v = instances[i];
		for (int j = 0; j < v.size(); j++)
		{
			auto& fv = v[j];
			runtime_assert(labels[fv.GetID()] == -1);
			labels[fv.GetID()] = i;
			indicies[fv.GetID()] = j;
		}
	}

	runtime_assert(input);
	int index;
	while (input >> index)
	{
		int label = labels[index];
		int i = indicies[index];
		partition[label].push_back(instances[label][i]);
	}
	return partition;
}
}