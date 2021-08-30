#include "feature_selector_abstract.h"
#include "../Utilities/runtime_assert.h"

namespace MurTree
{
FeatureSelectorAbstract::FeatureSelectorAbstract(int num_features):
	num_features_(num_features),
	num_features_popped_(0)
{
	runtime_assert(num_features_ > 0);
}

FeatureSelectorAbstract::~FeatureSelectorAbstract()
{
}

void FeatureSelectorAbstract::Reset(BinaryDataInternal& data)
{
	num_features_popped_ = 0;
	ResetInternal(data);
}

int FeatureSelectorAbstract::PopNextFeature()
{
	runtime_assert(AreThereAnyFeaturesLeft());

	int next_feature = PopNextFeatureInternal();
	num_features_popped_++;
	return next_feature;
}

bool FeatureSelectorAbstract::AreThereAnyFeaturesLeft()
{
	return num_features_popped_ != num_features_;
}

}