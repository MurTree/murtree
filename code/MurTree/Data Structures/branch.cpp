//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#include "branch.h"

#include <algorithm>

namespace MurTree
{
Branch::Branch()
{
}

void Branch::AddFeatureBranch(int feature, bool present)
{
	int code = GetCode(feature, present);
	branch_codes_.push_back(code);

	ConvertIntoCanonicalRepresentation();
}

int Branch::Depth() const
{
	return int(branch_codes_.size());
}

int Branch::operator[](int i) const
{
	return branch_codes_[i];
}

Branch Branch::LeftChildBranch(Branch& branch, int feature)
{
	Branch left_child_branch(branch);
	left_child_branch.AddFeatureBranch(feature, false); //the convention is that the left branch does not have the feature
	return left_child_branch;
}

Branch Branch::RightChildBranch(Branch& branch, int feature)
{
	Branch right_child_branch(branch);
	right_child_branch.AddFeatureBranch(feature, true); //the convention is that the right branch has the feature
	return right_child_branch;
}

bool Branch::operator==(const Branch& right_hand_side) const
{
	if (this->branch_codes_.size() != right_hand_side.branch_codes_.size()) { return false; }
	for (size_t i = 0; i < this->branch_codes_.size(); i++)
	{
		if (this->branch_codes_[i] != right_hand_side.branch_codes_[i]) { return false; }
	}
	return true;
}

void Branch::ConvertIntoCanonicalRepresentation()
{
	std::sort(branch_codes_.begin(), branch_codes_.end());
}

int Branch::GetCode(int feature, bool present)
{
	return 2 * feature + present;
}

}
