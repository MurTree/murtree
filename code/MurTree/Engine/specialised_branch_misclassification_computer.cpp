//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#include "binary_data_difference_computer.h"
#include "specialised_branch_misclassification_computer.h"
#include "../Utilities/runtime_assert.h"

namespace MurTree
{
SpecialisedBranchMisclassificationComputer::SpecialisedBranchMisclassificationComputer(int num_features, int num_total_data_points, bool using_incremental_updates) :
	counts_(num_features),
	using_incremental_updates_(using_incremental_updates),
	num_positives_(0),
	num_negatives_(0),
	data_old_(2, num_features), 
	data_to_remove_(2, num_features), 
	data_to_add_(2, num_features)
{
}

bool SpecialisedBranchMisclassificationComputer::Initialise(BinaryDataInternal& data_new)
{
	num_positives_ = data_new.NumInstancesForLabel(1);
	num_negatives_ = data_new.NumInstancesForLabel(0);

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

int SpecialisedBranchMisclassificationComputer::ProbeDifference(BinaryDataInternal& data)
{
	return BinaryDataDifferenceComputer::ComputeDifferenceMetrics(data_old_, data).total_difference;
}

int SpecialisedBranchMisclassificationComputer::BranchMisclassificationOne(int f1)
{
	return std::min(PositivesOneOne(f1, f1), NegativesOneOne(f1, f1));
}

int SpecialisedBranchMisclassificationComputer::BranchMisclassificationZero(int f1)
{
	return std::min(PositivesZeroZero(f1, f1), NegativesZeroZero(f1, f1));
}

int SpecialisedBranchMisclassificationComputer::PenaltyBranchOneOne(int feature1, int feature2)
{
	return std::min(PositivesOneOne(feature1, feature2), NegativesOneOne(feature1, feature2));
}

int SpecialisedBranchMisclassificationComputer::PenaltyBranchOneZero(int feature1, int feature2)
{
	return std::min(PositivesOneZero(feature1, feature2), NegativesOneZero(feature1, feature2));
}

int SpecialisedBranchMisclassificationComputer::PenaltyBranchZeroOne(int feature1, int feature2)
{
	return std::min(PositivesZeroOne(feature1, feature2), NegativesZeroOne(feature1, feature2));
}

int SpecialisedBranchMisclassificationComputer::PenaltyBranchZeroZero(int feature1, int feature2)
{
	return std::min(PositivesZeroZero(feature1, feature2), NegativesZeroZero(feature1, feature2));
}

int SpecialisedBranchMisclassificationComputer::PositivesOneOne(int feature1, int feature2)
{
	return counts_.Positives(feature1, feature2);
}

int SpecialisedBranchMisclassificationComputer::PositivesOneZero(int feature1, int feature2)
{
	return counts_.Positives(feature1, feature1) - counts_.Positives(feature1, feature2);
}

int SpecialisedBranchMisclassificationComputer::PositivesZeroOne(int feature1, int feature2)
{
	return counts_.Positives(feature2, feature2) - counts_.Positives(feature1, feature2);
}

int SpecialisedBranchMisclassificationComputer::PositivesZeroZero(int feature1, int feature2)
{
	return num_positives_ - (PositivesOneOne(feature1, feature1) + PositivesOneOne(feature2, feature2) - PositivesOneOne(feature1, feature2));
}

int SpecialisedBranchMisclassificationComputer::NegativesOneOne(int feature1, int feature2)
{
	return counts_.Negatives(feature1, feature2);
}

int SpecialisedBranchMisclassificationComputer::NegativesOneZero(int feature1, int feature2)
{
	return counts_.Negatives(feature1, feature1) - counts_.Negatives(feature1, feature2);
}

int SpecialisedBranchMisclassificationComputer::NegativesZeroOne(int feature1, int feature2)
{
	return counts_.Negatives(feature2, feature2) - counts_.Negatives(feature1, feature2);
}

int SpecialisedBranchMisclassificationComputer::NegativesZeroZero(int feature1, int feature2)
{
	return num_negatives_ - (counts_.Negatives(feature1, feature1) + counts_.Negatives(feature2, feature2) - counts_.Negatives(feature1, feature2));
	//return num_negatives_ - (NegativesOneOne(feature1, feature2) + NegativesOneZero(feature1, feature2) + NegativesZeroOne(feature1, feature2));
}

void SpecialisedBranchMisclassificationComputer::UpdateCounts(BinaryDataInternal& data, int value)
{
	for (FeatureVectorBinary *positive_data_point : data.GetInstancesForLabel(1))
	{
		int num_present_features = positive_data_point->NumPresentFeatures();
		for (int i = 0; i < num_present_features; i++)
		{
			int feature1 = positive_data_point->GetJthPresentFeature(i);
			for (int j = i; j < num_present_features; j++)
			{
				int feature2 = positive_data_point->GetJthPresentFeature(j);
				//runtime_assert(feature1 <= feature2);
				
				//assert(counts_.Positives(feature1.Index(), feature2.Index()) >= 0);
				//counts_.Positives(feature1.Index(), feature2.Index()) += value;
				counts_.CountLabel(1, feature1, feature2) += value;
			}
		}
	}

	for (FeatureVectorBinary *negative_data_point : data.GetInstancesForLabel(0))
	{
		int num_present_features = negative_data_point->NumPresentFeatures();
		for (int i = 0; i < num_present_features; i++)
		{
			int feature1 = negative_data_point->GetJthPresentFeature(i);
			for (int j = i; j < num_present_features; j++)
			{
				int feature2 = negative_data_point->GetJthPresentFeature(j);
				//assert(feature1 <= feature2);
				
				//assert(counts_.Negatives(feature1.Index(), feature2.Index()) >= 0);
				//counts_.Negatives(feature1.Index(), feature2.Index()) += value;
				counts_.CountLabel(0, feature1, feature2) += value;
			}
		}
	}
}

}
