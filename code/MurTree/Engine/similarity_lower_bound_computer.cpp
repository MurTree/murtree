#include "similarity_lower_bound_computer.h"
#include "binary_data_difference_computer.h"
#include "../Utilities/runtime_assert.h"

#include <cmath>

namespace MurTree
{
SimilarityLowerBoundComputer::SimilarityLowerBoundComputer(int max_depth, int size, int num_instances) :
disabled_(false)
{
	Initialise(max_depth, size);
}

PairLowerBoundOptimal SimilarityLowerBoundComputer::ComputeLowerBound(BinaryDataInternal& data, Branch& branch, int depth, int size, AbstractCache *cache)
{
	if (disabled_) { return PairLowerBoundOptimal(0, false); }

	PairLowerBoundOptimal result(0, false);
	for (ArchiveEntry& entry : archive_[depth])
	{
		int entry_lower_bound = cache->RetrieveLowerBound(entry.data, entry.branch, depth, size);
		if (entry.data.Size() > data.Size() && entry.data.Size() - data.Size() >= entry_lower_bound) { continue; }

		DifferenceMetrics metrics = BinaryDataDifferenceComputer::ComputeDifferenceMetrics(entry.data, data);
		result.lower_bound = std::max(result.lower_bound, entry_lower_bound - metrics.num_removals);

		if (metrics.total_difference == 0) 
		{ 
			cache->TransferAssignmentsForEquivalentBranches(entry.data, entry.branch, data, branch);
			if (cache->IsOptimalAssignmentCached(data, branch, depth, size))
			{
				result.optimal = true;
				break;
			}
		}
	}
	return result;
}

void SimilarityLowerBoundComputer::UpdateArchive(BinaryDataInternal& data, Branch& branch, int depth)
{
	if (disabled_) { return; }

	if (archive_[depth].size() < 2)
	{
		archive_[depth].push_back(ArchiveEntry(data, branch));
	}
	else
	{
		GetMostSimilarStoredData(data, depth) = ArchiveEntry(data, branch);
	}
}

void SimilarityLowerBoundComputer::Initialise(int max_depth, int size)
{
	if (disabled_) { return; }

	archive_.resize(max_depth + 1);
}

void SimilarityLowerBoundComputer::Disable()
{
	disabled_ = true;
}

SimilarityLowerBoundComputer::ArchiveEntry& SimilarityLowerBoundComputer::GetMostSimilarStoredData(BinaryDataInternal& data, int depth)
{
	runtime_assert(archive_[depth].size() > 0);

	SimilarityLowerBoundComputer::ArchiveEntry* best_entry = NULL;
	int best_similiarity_score = INT32_MAX;
	for (ArchiveEntry& archieve_entry : archive_[depth])
	{
		int similiarity_score = BinaryDataDifferenceComputer::ComputeDifferenceMetrics(archieve_entry.data, data).total_difference;
		if (similiarity_score < best_similiarity_score)
		{
			best_entry = &archieve_entry;
			best_similiarity_score = similiarity_score;
		}
	}
	assert(best_similiarity_score < INT32_MAX);
	return *best_entry;
}

}