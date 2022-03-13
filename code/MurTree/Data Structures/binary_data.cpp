//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#include "binary_data.h"
#include "../Utilities/runtime_assert.h"

#include <numeric>

namespace MurTree
{
BinaryDataInternal::BinaryDataInternal(int num_labels, int num_features):
	num_features_(num_features),
	instances(num_labels),
	hash_value_(-1),
	is_closure_set_(false)
{
}

int BinaryDataInternal::NumLabels() const
{
	return int(instances.size()); //recall that [i] are the instances of label 'i', and size would be the number of labels
}

int BinaryDataInternal::NumInstancesForLabel(int label) const
{
	return int(instances[label].size());
}

const FeatureVectorBinary* BinaryDataInternal::GetInstance(int label, int index) const
{
	return instances[label][index];
}

const std::vector<FeatureVectorBinary*>& BinaryDataInternal::GetInstancesForLabel(int label) const
{
	return instances[label];
}

FeatureVectorBinary* BinaryDataInternal::GetInstance(int label, int index)
{
	return instances[label][index];
}

int BinaryDataInternal::NumFeatures() const
{
	return num_features_;
}

int BinaryDataInternal::MaxFeatureVectorID() const
{
	int max_id = 0;
	for (const std::vector<FeatureVectorBinary*>& i : instances)
	{
		for (FeatureVectorBinary *fv : i)
		{
			max_id = std::max(max_id, int(fv->GetID()));
		}
	}
	return max_id + 1;
}

void BinaryDataInternal::AddFeatureVector(FeatureVectorBinary* fv, int label)
{
	assert(num_features_ == fv->NumTotalFeatures() && label < instances.size());
	instances[label].push_back(fv);
}

int BinaryDataInternal::Size() const
{
	return std::accumulate(instances.begin(), instances.end(), 0, [](int sum, const std::vector<FeatureVectorBinary*>& v)->int { return sum + int(v.size()); });
}

bool BinaryDataInternal::IsEmpty() const
{
	return Size() == 0;
}

void BinaryDataInternal::Clear()
{
	for (auto& feature_vectors : instances) { feature_vectors.clear(); }
	hash_value_ = -1;
	is_closure_set_ = false;
}

double BinaryDataInternal::ComputeSparsity() const
{
	if (Size() == 0) { return 0.0; }
	double sum_sparsity = 0.0;
	for (const std::vector<FeatureVectorBinary*>& i : instances)
	{
		for (const FeatureVectorBinary* fv : i)
		{
			sum_sparsity += fv->Sparsity();
		}
	}
	return sum_sparsity / (Size());
}

void BinaryDataInternal::PrintStats()
{
	std::cout << "Num features: " << NumFeatures() << "\n";
	std::cout << "Size: " << Size() << "\n";
	for (int label = 0; label < int(instances.size()); label++)
	{
		std::cout << "\tLabel " << label << ": " << instances[label].size() << "\n";
	}
	std::cout << "\tSparsity: " << ComputeSparsity() << "\n";
}

bool BinaryDataInternal::IsHashSet() const
{
	return hash_value_ != -1;
}

int BinaryDataInternal::GetHash() const
{
	runtime_assert(hash_value_ != -1); 
	return hash_value_;
}

void BinaryDataInternal::SetHash(int new_hash)
{
	runtime_assert(new_hash != -1);
	hash_value_ = new_hash;
}

bool BinaryDataInternal::IsClosureSet() const
{
	return is_closure_set_;
}

Branch &BinaryDataInternal::GetClosure()
{
	runtime_assert(IsClosureSet());
	return closure_;
}

void BinaryDataInternal::SetClosure(const Branch& closure)
{
	closure_ = closure;
	is_closure_set_ = true;
}

std::ostream& operator<<(std::ostream& os, BinaryDataInternal& data)
{
	os << "Number of instances: " << data.Size();

	for (int label = 0; label < data.NumLabels(); label++)
	{
		int counter_instances = 0;
		os << "\nNumber of instances with label " << label << ": " << data.NumInstancesForLabel(label);
		for (FeatureVectorBinary* fv : data.GetInstancesForLabel(label))
		{
			os << "\n\t" << counter_instances++ << ":\t" << (*fv);
		}
	}
	return os;
}
}
