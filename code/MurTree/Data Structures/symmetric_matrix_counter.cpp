//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#include "symmetric_matrix_counter.h"

#include <memory>
#include <assert.h>
#include <iostream>

namespace MurTree
{
SymmetricMatrixCounter::SymmetricMatrixCounter(int num_labels, int num_features) :
	counts_(0),
	num_labels_(num_labels),
	num_features_(num_features)
{
	counts_ = new int[NumElements()];
	ResetToZeros();
}

SymmetricMatrixCounter::~SymmetricMatrixCounter()
{
	delete[] counts_;
	counts_ = 0;
}

int SymmetricMatrixCounter::operator()(int label, int feature1, int feature2) const
{
	assert(feature1 <= feature2);
	int index = num_labels_ * IndexSymmetricMatrix(feature1, feature2) + label;
	return counts_[index];
}

int& SymmetricMatrixCounter::operator()(int label, int feature1, int feature2)
{
	assert(feature1 <= feature2);
	int index = num_labels_ * IndexSymmetricMatrix(feature1, feature2) + label;
	return counts_[index];
}

void SymmetricMatrixCounter::ResetToZeros()
{
	for (int i = 0; i < NumElements(); i++) { counts_[i] = 0; }
	//memset(data2d_, 0, sizeof(int)*NumElements());
}

int SymmetricMatrixCounter::NumElements() const
{
	//the matrix is symmetric, and effectively each entry stores num_labels entries, corresponding the counts for each label
	return num_labels_ * ((num_features_ * (num_features_ + 1)) / 2);
}

int SymmetricMatrixCounter::IndexSymmetricMatrix(int index_row, int index_column) const
{
	assert(index_row <= index_column);
	return num_features_ * index_row + index_column - index_row * (index_row + 1) / 2;
}
}