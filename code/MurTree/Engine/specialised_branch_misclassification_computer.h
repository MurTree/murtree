//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "../Data Structures/binary_data.h"
#include "../Data Structures/symmetric_matrix_positive_negative_counter_2d.h"

namespace MurTree
{
class SpecialisedBranchMisclassificationComputer
{
public:
	SpecialisedBranchMisclassificationComputer(int num_features, int num_feature_vectors, bool using_incremental_updates); //assumes the feature vectors ID are in the range of [0, ..., num_feature_vectors)

	bool Initialise(BinaryDataInternal& data); //returns true if any changed have been made; otherwise false. This is useful since if no changes were necessary we may use the results we previously computed.
	int ProbeDifference(BinaryDataInternal& data);

	int BranchMisclassificationOne(int f1);
	int BranchMisclassificationZero(int f1);

	int PenaltyBranchOneOne(int f1, int f2);
	int PenaltyBranchOneZero(int f1, int f2);
	int PenaltyBranchZeroOne(int f1, int f2);
	int PenaltyBranchZeroZero(int f1, int f2);

	//private:
	int PositivesOneOne(int f1, int f2);
	int PositivesOneZero(int f1, int f2);
	int PositivesZeroOne(int f1, int f2);
	int PositivesZeroZero(int f1, int f2);


	int NegativesOneOne(int f1, int f2);
	int NegativesOneZero(int f1, int f2);
	int NegativesZeroOne(int f1, int f2);
	int NegativesZeroZero(int f1, int f2);

	void UpdateCounts(BinaryDataInternal& data, int value);

	int num_positives_, num_negatives_;
	SymmetricMatrixPositiveNegativeCounter2D counts_;
	BinaryDataInternal data_old_, data_to_remove_, data_to_add_;
	bool using_incremental_updates_;
};
}