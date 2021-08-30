#pragma once

#include "feature_selector_abstract.h"

#include <vector>
#include <algorithm>

namespace MurTree
{
	class FeatureSelectorRandom : public FeatureSelectorAbstract
	{
	public:
		FeatureSelectorRandom(int num_features) :
			FeatureSelectorAbstract(num_features)
		{
			ResetUnexploredFeatures();
		}

	protected:
		int PopNextFeatureInternal()
		{
			int next_feature = unexplored_features_.back();
			unexplored_features_.pop_back();
			return next_feature; 
		}

		void ResetInternal(BinaryDataInternal& data) { ResetUnexploredFeatures(); }
		
		void ResetUnexploredFeatures()
		{
			unexplored_features_.resize(num_features_);
			for (int i = 0; i < num_features_; i++) { unexplored_features_[i] = i; }
			std::random_shuffle(unexplored_features_.begin(), unexplored_features_.end(), RandomInternal);
		}

		static int RandomInternal(int i) { return rand() % i; }

		std::vector<int> unexplored_features_;
	};
}