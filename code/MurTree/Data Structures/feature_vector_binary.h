//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include <vector>
#include <fstream>

namespace MurTree
{
	class FeatureVectorBinary
	{
	public:
		FeatureVectorBinary(const std::vector<bool>& feature_values, int id);

		bool IsFeaturePresent(int feature) const;
		int GetJthPresentFeature(int j) const;
		int NumPresentFeatures() const;
		int NumTotalFeatures() const;
		int GetID() const;
		double Sparsity() const;

		std::vector<int>::const_iterator begin() const;
		std::vector<int>::const_iterator end() const;

		friend std::ostream& operator<<(std::ostream& os, const FeatureVectorBinary& fv);

	private:
		int id_;
		std::vector<bool> is_feature_present_; //[i] indicates if the feature is true or false, i.e., if it is present in present_Features.
		std::vector<int> present_features_;

	};
}