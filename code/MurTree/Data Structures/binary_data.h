//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "feature_vector_binary.h"
#include "branch.h"
#include "..//Utilities/runtime_assert.h"

#include <vector>
#include <iostream>

namespace MurTree
{
class BinaryDataInternal
{
public:
	BinaryDataInternal(int num_labels, int num_features);
	
	int NumLabels() const;
	int NumFeatures() const;
	int NumInstancesForLabel(int label) const;
	int Size() const;
	bool IsEmpty() const;

	const FeatureVectorBinary* GetInstance(int label, int index) const;
	const std::vector<FeatureVectorBinary*>& GetInstancesForLabel(int label) const;
	FeatureVectorBinary* GetInstance(int label, int index);

	void Clear();
	void AddFeatureVector(FeatureVectorBinary* fv, int label);

	int MaxFeatureVectorID() const;	
	double ComputeSparsity() const;
	void PrintStats();
	friend std::ostream& operator<<(std::ostream& os, BinaryDataInternal& data);

	bool IsHashSet() const;
	int GetHash() const;
	void SetHash(int new_hash);

	bool IsClosureSet() const;
	Branch &GetClosure();
	void SetClosure(const Branch& closure);

private:
	long long hash_value_;

	bool is_closure_set_;
	Branch closure_;

	int num_features_;
	std::vector<std::vector<FeatureVectorBinary*> > instances;//the learning data is partitioned based on the labels, i.e., instances[label] is the vector of feature vectors with label 'label'
};
}