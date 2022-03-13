//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "../Data Structures/binary_data.h"
#include <stdint.h>

namespace MurTree
{
class FeatureSelectorAbstract
{
public:
	FeatureSelectorAbstract(int num_features);
	virtual ~FeatureSelectorAbstract();

	void Reset(BinaryDataInternal& data);
	int PopNextFeature();
	bool AreThereAnyFeaturesLeft();

protected:
	virtual int PopNextFeatureInternal() = 0;
	virtual void ResetInternal(BinaryDataInternal& data) = 0;

	int num_features_;
	int num_features_popped_;
};
}