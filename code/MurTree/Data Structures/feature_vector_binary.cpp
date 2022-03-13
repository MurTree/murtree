//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#include "feature_vector_binary.h"
#include "../Utilities/runtime_assert.h"

#include <iostream>
#include <assert.h>

namespace MurTree
{
FeatureVectorBinary::FeatureVectorBinary(const std::vector<bool>& feature_values, int id):
	id_(id),
	is_feature_present_(feature_values)
{
	for (size_t feature_index = 0; feature_index < feature_values.size(); feature_index++)
	{
		if (feature_values[feature_index] == true) { present_features_.push_back(feature_index); }
	}
}

bool FeatureVectorBinary::IsFeaturePresent(int feature) const
{
	return is_feature_present_[feature];
}

int FeatureVectorBinary::GetJthPresentFeature(int j) const
{
	return present_features_[j];
}

double FeatureVectorBinary::Sparsity() const
{
	return double(NumPresentFeatures()) / is_feature_present_.size();
}

int FeatureVectorBinary::NumPresentFeatures() const
{
	return int(present_features_.size());
}

int FeatureVectorBinary::NumTotalFeatures() const
{
	return is_feature_present_.size();
}

int FeatureVectorBinary::GetID() const
{
	return id_;
}

typename std::vector<int>::const_iterator FeatureVectorBinary::begin() const
{
	return present_features_.begin();
}

typename std::vector<int>::const_iterator FeatureVectorBinary::end() const
{
	return present_features_.end();
}

std::ostream& operator<<(std::ostream& os, const FeatureVectorBinary& fv)
{
	if (fv.NumPresentFeatures() == 0) { std::cout << "[empty]"; }
	else
	{
		auto iter = fv.begin();
		std::cout << *iter;
		++iter;
		while (iter != fv.end())
		{
			std::cout << " " << *iter;
			++iter;
		}
	}
	return os;
}

}//end namespace MurTree