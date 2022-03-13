//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "abstract_cache.h"
#include "binary_data_difference_computer.h"
#include "../Data Structures/binary_data.h"
#include "../Data Structures/branch.h"

namespace MurTree
{
struct PairLowerBoundOptimal
{
	PairLowerBoundOptimal(int lb, bool opt) :lower_bound(lb), optimal(opt) {}
	int lower_bound;
	bool optimal;
};


class SimilarityLowerBoundComputer
{
public:
	SimilarityLowerBoundComputer(int max_depth, int size, int num_instances);

	//computes the lower bound with respect to the data currently present in the data structure
	//note that this does not add any information internally
	//use 'UpdateArchive' to add datasets to the data structure
	PairLowerBoundOptimal ComputeLowerBound(BinaryDataInternal& data, Branch &branch, int depth, int size, AbstractCache *cache);

	//adds the data, possibly replacing some previously stored dataset in case there the data structure is full. 
	//when replacing, it will find the most similar dataset and replace it with the input
	//TODO make it should replace the most disimilar dataset, and not the most similar?
	void UpdateArchive(BinaryDataInternal& data, Branch &branch, int depth);

	void Disable();

private:

	struct ArchiveEntry
	{
		ArchiveEntry(BinaryDataInternal& d, Branch& b) :
			data(d),
			branch(b)
		{}

		BinaryDataInternal data;
		Branch branch;
	};

	void Initialise(int max_depth, int size);
	ArchiveEntry& GetMostSimilarStoredData(BinaryDataInternal& data, int depth);
	
	std::vector<std::vector<ArchiveEntry> > archive_;//archive_[depth][i]
	bool disabled_;
};
}