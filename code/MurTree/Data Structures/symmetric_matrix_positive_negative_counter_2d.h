//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include <stdint.h>
#include <stddef.h>

namespace MurTree
{
class SymmetricMatrixPositiveNegativeCounter2D
{
public:
	SymmetricMatrixPositiveNegativeCounter2D(size_t num_rows);
	~SymmetricMatrixPositiveNegativeCounter2D();

	uint32_t &Positives(uint32_t index_row, uint32_t index_column);
	uint32_t &Negatives(uint32_t index_row, uint32_t index_column);
	uint32_t &CountLabel(uint32_t label, uint32_t index_row, uint32_t index_column);

	void ResetToZeros();

	bool operator==(const SymmetricMatrixPositiveNegativeCounter2D &reference);

private:
	uint32_t NumElements() const;
	uint32_t IndexSymmetricMatrix(uint32_t index_row, uint32_t index_column);

	uint32_t * data2d_;
	uint32_t num_rows_;
};
}