#pragma once

#include "feature_selector_abstract.h"

namespace MurTree
{
class FeatureSelectorInOrder : public FeatureSelectorAbstract
{
public:
	FeatureSelectorInOrder(int num_features) :
		FeatureSelectorAbstract(num_features),
		next_(0)
	{}

protected:
	int PopNextFeatureInternal() { return next_++; }
	void ResetInternal(BinaryDataInternal& data) { next_ = 0; }

	int next_;
};
}