//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include "../Utilities/solver_result.h"
#include "../Utilities/parameter_handler.h"

#include <string>

namespace MurTree
{

struct HyperParameterTuningInput
{
	std::string benchmark_location;
	std::string partition_files_location_prefix;
	int max_depth;
	int max_num_nodes;
	bool single_parameter_set_tuning;
};

struct HyperParameterTuningOutput
{
	SolverResult result;
	double train_accuracy, test_accuracy;
};

class HyperParameterTuner
{
public:
	static HyperParameterTuningOutput Solve(HyperParameterTuningInput &tuning_parameters, ParameterHandler &solver_parameters);
};
}