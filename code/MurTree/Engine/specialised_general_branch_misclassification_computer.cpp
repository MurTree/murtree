//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#include "specialised_general_branch_misclassification_computer.h"
#include "binary_data_difference_computer.h"

namespace MurTree
{
SpecialisedGeneralBranchMisclassificationComputer::SpecialisedGeneralBranchMisclassificationComputer(int num_labels, int num_features, int num_feature_vectors, bool using_incremental_updates):
	counts_(num_labels, num_features),
	using_incremental_updates_(using_incremental_updates),
	num_labels_(num_labels),
	label_counts_(num_labels),
	data_old_(num_labels, num_features),
	data_to_remove_(num_labels, num_features),
	data_to_add_(num_labels, num_features)
{
}

bool SpecialisedGeneralBranchMisclassificationComputer::Initialise(BinaryDataInternal& data_new)
{
	assert(data_new.NumLabels() == num_labels_);

	for (int label = 0; label < num_labels_; label++) { label_counts_[label] = data_new.NumInstancesForLabel(label); }

	if (using_incremental_updates_)
	{
		//note that the function updates data_to_add and data_to_remove
		BinaryDataDifferenceComputer::ComputeDifference(data_old_, data_new, data_to_add_, data_to_remove_);

		if (data_to_add_.Size() + data_to_remove_.Size() == 0) { return false; }

		data_old_ = data_new;
	}

	if (using_incremental_updates_ && data_to_add_.Size() + data_to_remove_.Size() < data_new.Size())
	{
		//remove incrementally
		UpdateCounts(data_to_remove_, -1);
		UpdateCounts(data_to_add_, +1);
	}
	else
	{
		//compute from scratch
		counts_.ResetToZeros();
		UpdateCounts(data_new, +1);
	}
	return true;
}

int SpecialisedGeneralBranchMisclassificationComputer::ProbeDifference(BinaryDataInternal& data)
{
	return BinaryDataDifferenceComputer::ComputeDifferenceMetrics(data_old_, data).total_difference;
}

int SpecialisedGeneralBranchMisclassificationComputer::PenaltyBranchOneOne(int feature1, int feature2)
{
	return ComputeBranchPenalty(feature1, feature2, &SpecialisedGeneralBranchMisclassificationComputer::LabelOneOne);
}

int SpecialisedGeneralBranchMisclassificationComputer::PenaltyBranchOneZero(int feature1, int feature2)
{
	return ComputeBranchPenalty(feature1, feature2, &SpecialisedGeneralBranchMisclassificationComputer::LabelOneZero);
}

int SpecialisedGeneralBranchMisclassificationComputer::PenaltyBranchZeroOne(int feature1, int feature2)
{
	return ComputeBranchPenalty(feature1, feature2, &SpecialisedGeneralBranchMisclassificationComputer::LabelZeroOne);
}

int SpecialisedGeneralBranchMisclassificationComputer::PenaltyBranchZeroZero(int feature1, int feature2)
{
	return ComputeBranchPenalty(feature1, feature2, &SpecialisedGeneralBranchMisclassificationComputer::LabelZeroZero);
}

int SpecialisedGeneralBranchMisclassificationComputer::LabelOneOne(int label, int feature1, int feature2)
{
	return counts_(label, feature1, feature2);
}

int SpecialisedGeneralBranchMisclassificationComputer::LabelOneZero(int label, int feature1, int feature2)
{
	return counts_(label, feature1, feature1) - counts_(label, feature1, feature2);
}

int SpecialisedGeneralBranchMisclassificationComputer::LabelZeroOne(int label, int feature1, int feature2)
{
	return counts_(label, feature2, feature2) - counts_(label, feature1, feature2);
}

int SpecialisedGeneralBranchMisclassificationComputer::LabelZeroZero(int label, int feature1, int feature2)
{
	return label_counts_[label] - (LabelOneOne(label, feature1, feature1) + LabelOneOne(label, feature2, feature2) - LabelOneOne(label, feature1, feature2));
}

int SpecialisedGeneralBranchMisclassificationComputer::ComputeBranchPenalty(int feature1, int feature2, int(SpecialisedGeneralBranchMisclassificationComputer::* count_function)(int label, int feature1, int feature2))
{
	uint32_t sum_of_instances = 0;
	for (uint32_t label = 0; label < num_labels_; label++) { sum_of_instances += ((this->*count_function)(label, feature1, feature2)); }

	uint32_t min_penalty = sum_of_instances - ((this->*count_function)(0, feature1, feature2));
	for (uint32_t label = 1; label < num_labels_; label++)
	{
		min_penalty = std::min((sum_of_instances - (this->*count_function)(label, feature1, feature2)), min_penalty);
	}
	return min_penalty;
}

void SpecialisedGeneralBranchMisclassificationComputer::UpdateCounts(BinaryDataInternal& data, int value)
{
	for (int label = 0; label < num_labels_; label++)
	{
		for (FeatureVectorBinary* data_point : data.GetInstancesForLabel(label))
		{
			int num_present_features = data_point->NumPresentFeatures();
			for (int i = 0; i < num_present_features; i++)
			{
				int feature1 = data_point->GetJthPresentFeature(i);
				for (int j = i; j < num_present_features; j++)
				{
					int feature2 = data_point->GetJthPresentFeature(j);
					counts_(label, feature1, feature2) += value;
				}
			}
		}
	}
}



}