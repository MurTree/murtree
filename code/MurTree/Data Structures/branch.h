//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include <stdint.h>
#include <vector>
#include <iostream>

namespace MurTree
{
class Branch
{
public:
	Branch();

	int Depth() const;
	int operator[](int i) const;

	static Branch LeftChildBranch(Branch &branch,int feature);
	static Branch RightChildBranch(Branch& branch, int feature);

	bool operator==(const Branch& right_hand_side) const;
	friend std::ostream& operator<<(std::ostream& out, const Branch& branch)
	{
		for (int code : branch.branch_codes_)
		{
			out << code << " ";
		}
		return out;
	}

//private:
	int GetCode(int feature, bool present);
	void AddFeatureBranch(int feature, bool present);
	void ConvertIntoCanonicalRepresentation();

	std::vector<int> branch_codes_;
};
}