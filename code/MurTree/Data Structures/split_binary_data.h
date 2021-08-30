#pragma once

#include "../Data Structures/binary_data.h"

namespace MurTree
{
struct SplitBinaryData
{
	SplitBinaryData(int num_labels, int num_features):
		data_without_feature(num_labels, num_features),
		data_with_feature(num_labels, num_features)
	{
	}


	SplitBinaryData(int splitting_feature, BinaryDataInternal &data):
		data_without_feature(data.NumLabels(), data.NumFeatures()),
		data_with_feature(data.NumLabels(), data.NumFeatures())
	{
		SplitData(splitting_feature, data);
	}

	void SplitData(int splitting_feature, BinaryDataInternal& data)
	{
		data_without_feature.Clear();
		data_with_feature.Clear();
		
		for (int label = 0; label < data.NumLabels(); label++)
		{
			for (FeatureVectorBinary *fv : data.GetInstancesForLabel(label))
			{
				if (fv->IsFeaturePresent(splitting_feature))
				{
					data_with_feature.AddFeatureVector(fv, label);
				}
				else
				{
					data_without_feature.AddFeatureVector(fv, label);
				}
			}
		}
	}

	BinaryDataInternal data_without_feature, data_with_feature;
};
}