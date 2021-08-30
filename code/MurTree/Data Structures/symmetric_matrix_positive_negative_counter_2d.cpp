#include "symmetric_matrix_positive_negative_counter_2d.h"
#include "../Utilities/runtime_assert.h"

#include <memory>



namespace MurTree
{
	SymmetricMatrixPositiveNegativeCounter2D::SymmetricMatrixPositiveNegativeCounter2D(size_t num_rows) :
		data2d_(0),
		num_rows_(num_rows)
	{
		data2d_ = new uint32_t[NumElements()];
		ResetToZeros();
	}

	SymmetricMatrixPositiveNegativeCounter2D::~SymmetricMatrixPositiveNegativeCounter2D()
	{
		delete[] data2d_;
		data2d_ = 0;
	}

	uint32_t & SymmetricMatrixPositiveNegativeCounter2D::Positives(uint32_t index_row, uint32_t index_column)
	{
		//runtime_assert(index_row <= index_column);
		uint32_t index = 2 * IndexSymmetricMatrix(index_row, index_column);
		return data2d_[index];
	}

	uint32_t & SymmetricMatrixPositiveNegativeCounter2D::Negatives(uint32_t index_row, uint32_t index_column)
	{
		//runtime_assert(index_row <= index_column);		
		//if (!(index_row <= index_column)) { printf("rip\n"); printf(#x); assert(#x); abort(); }
		uint32_t index = 1 + 2 * IndexSymmetricMatrix(index_row, index_column);
		return data2d_[index];
	}

	uint32_t & SymmetricMatrixPositiveNegativeCounter2D::CountLabel(uint32_t label, uint32_t index_row, uint32_t index_column)
	{
		//runtime_assert(index_row <= index_column);
		uint32_t index = (1-label) + 2 * IndexSymmetricMatrix(index_row, index_column);
		return data2d_[index];
	}

	void SymmetricMatrixPositiveNegativeCounter2D::ResetToZeros()
	{
		for (uint32_t i = 0; i < NumElements(); i++) { data2d_[i] = 0; }
		//memset(data2d_, 0, sizeof(uint32_t)*NumElements());
	}

	bool SymmetricMatrixPositiveNegativeCounter2D::operator==(const SymmetricMatrixPositiveNegativeCounter2D & reference)
	{
		if (num_rows_ != reference.num_rows_) { return false; }

		for (uint32_t i = 0; i < NumElements(); i++)
		{
			if (data2d_[i] != reference.data2d_[i]) { return false; }
		}
		return true;
	}

	uint32_t SymmetricMatrixPositiveNegativeCounter2D::NumElements() const
	{
		return num_rows_ * (num_rows_ + 1); //recall that the matrix is symmetric, and effectively each entry stores two uint32_tegers (one for counting positives and one for counting negatives)
	}

	uint32_t SymmetricMatrixPositiveNegativeCounter2D::IndexSymmetricMatrix(uint32_t index_row, uint32_t index_column)
	{
		//runtime_assert(index_row <= index_column);
		return num_rows_ * index_row + index_column - index_row * (index_row + 1) / 2;
	}
}