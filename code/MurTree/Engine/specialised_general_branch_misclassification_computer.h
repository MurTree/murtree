//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "../Data Structures/symmetric_matrix_counter.h"
#include "../Data Structures/binary_data.h"

namespace MurTree
{
class SpecialisedGeneralBranchMisclassificationComputer
{
public:
	SpecialisedGeneralBranchMisclassificationComputer(int num_labels, int num_features, int num_feature_vectors, bool using_incremental_updates); //assumes the feature vectors ID are in the range of [0, ..., num_feature_vectors)

	//Returns true if any changed have been made; otherwise false. 
	//This is useful since if no changes were necessary we may use the results we previously computed.
	bool Initialise(BinaryDataInternal& data);
	int ProbeDifference(BinaryDataInternal& data);

	int PenaltyBranchOneOne(int f1, int f2);
	int PenaltyBranchOneZero(int f1, int f2);
	int PenaltyBranchZeroOne(int f1, int f2);
	int PenaltyBranchZeroZero(int f1, int f2);

	//private:
	int LabelOneOne(int label, int f1, int f2);
	int LabelOneZero(int label, int f1, int f2);
	int LabelZeroOne(int label, int f1, int f2);
	int LabelZeroZero(int label, int f1, int f2);

	int ComputeBranchPenalty(int feature1, int feature2, int(SpecialisedGeneralBranchMisclassificationComputer::* count_function)(int label, int feature1, int feature2));

	void UpdateCounts(BinaryDataInternal& data, int value);

	SymmetricMatrixCounter counts_;
	std::vector<uint32_t> label_counts_;
	int num_labels_;
	BinaryDataInternal data_old_, data_to_remove_, data_to_add_;
	bool using_incremental_updates_;
};
}