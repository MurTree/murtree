//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "feature_selector_abstract.h"
#include "../Data Structures/key_value_heap.h"

#include <cmath>

namespace MurTree
{
	class FeatureSelectorGini : public FeatureSelectorAbstract
	{
	public:
		//assumes that the indicies of the labels will go from [0, num_labels)
		FeatureSelectorGini(uint32_t num_labels, uint32_t num_features) :
			FeatureSelectorAbstract(num_features),
			gini_values_(num_features),
			num_label_with_feature_(num_labels, std::vector<uint32_t>(num_features)),
			num_label_without_this_feature_(num_labels),
			feature_heap_(num_features),
			max_gini_value_(-1)
		{
		}

	protected:

		int PopNextFeatureInternal();
		void ResetInternal(BinaryDataInternal& data);

		void InitialiseHelperCountingDataStructureSparseRepresentation(const BinaryDataInternal& data);
		void BuildHeap(BinaryDataInternal& data);

		double max_gini_value_;
		std::vector<double> gini_values_;

		std::vector<std::vector<uint32_t> > num_label_with_feature_;
		std::vector<uint32_t> num_label_without_this_feature_;
		KeyValueHeap feature_heap_;
	};

	inline int FeatureSelectorGini::PopNextFeatureInternal()
	{
		return feature_heap_.PopMax();
	}

	void FeatureSelectorGini::ResetInternal(BinaryDataInternal& data)
	{
		BuildHeap(data);
	}

	void FeatureSelectorGini::BuildHeap(BinaryDataInternal& data)
	{
		while (feature_heap_.Size() != 0) { feature_heap_.PopMax(); } //todo a beter way to do this...
		InitialiseHelperCountingDataStructureSparseRepresentation(data); //set the member gini_values
		for (uint32_t feature = 0; feature < data.NumFeatures(); feature++) //could construct the heap more efficiently but this is not the bottle neck
		{
			feature_heap_.Readd(feature);
			double heap_value = gini_values_[feature];
			//if (ascending_) { heap_value = max_gini_value_ - heap_value; } //converting a max heap into a min heap
			feature_heap_.Increment(feature, heap_value);
		}
		assert(feature_heap_.Size() == num_features_);
	}

	void FeatureSelectorGini::InitialiseHelperCountingDataStructureSparseRepresentation(const BinaryDataInternal& data)
	{
		uint32_t data_size = data.Size();

		//clear helper data structures
		max_gini_value_ = -1.0;

		for (uint32_t label = 0; label < data.NumLabels(); label++)
		{
			for (uint32_t feature = 0; feature < data.NumFeatures(); feature++)
			{
				num_label_with_feature_[label][feature] = 0;
			}
		}

		for (uint32_t feature = 0; feature < data.NumFeatures(); feature++) { gini_values_[feature] = 0.0; }
		//---

		for (uint32_t label = 0; label < data.NumLabels(); label++)
		{
			for (FeatureVectorBinary *fv : data.GetInstancesForLabel(label))
			{
				for(int i = 0; i < fv->NumPresentFeatures(); i++)
				{
					int feature = fv->GetJthPresentFeature(i);
					//assert(fv[feature] == true);
					num_label_with_feature_[label][feature]++;
				}
			}
		}

		//compute the gini values for each feature - see gini_formula.jpg in the folder
		double I_D = 1.0;
		for (uint32_t label = 0; label < data.NumLabels(); label++)
		{
			I_D -= pow(double(data.NumInstancesForLabel(label)) / data.Size(), 2);
		}
		//the split divides the data into two groups D0 and D1 (without and with the feature)
		for (uint32_t feature = 0; feature < data.NumFeatures(); feature++)
		{
			uint32_t num_with_feature = 0;
			for (uint32_t label = 0; label < data.NumLabels(); label++)
			{
				num_with_feature += num_label_with_feature_[label][feature];
				num_label_without_this_feature_[label] = data.NumInstancesForLabel(label) - num_label_with_feature_[label][feature];
			}
			uint32_t num_without_feature = data_size - num_with_feature;

			double I_D_without_feature = 1.0;
			if (num_without_feature > 0)
			{
				for (uint32_t label = 0; label < data.NumLabels(); label++)
				{
					I_D_without_feature -= pow(double(num_label_without_this_feature_[label]) / num_without_feature, 2);
				}
			}

			double I_D_with_feature = 1.0;
			if (num_with_feature > 0)
			{
				for (uint32_t label = 0; label < data.NumLabels(); label++)
				{
					I_D_with_feature -= pow(double(num_label_with_feature_[label][feature]) / num_with_feature, 2);
				}
			}

			gini_values_[feature] = I_D - (double(num_without_feature) / data_size) * I_D_without_feature
				- (double(num_with_feature) / data_size) * I_D_with_feature;
			max_gini_value_ = std::max(gini_values_[feature], max_gini_value_);
		}
	}
}