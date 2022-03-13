//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#include "hyper_parameter_tuner.h"
#include "solver.h"
#include "../Utilities/split_data.h"
#include "../Utilities/file_reader.h"
#include "../Utilities/parameter_handler.h"

namespace MurTree
{
HyperParameterTuningOutput HyperParameterTuner::Solve(HyperParameterTuningInput& tuning_parameters, ParameterHandler& solver_parameters)
{
	runtime_assert(tuning_parameters.max_depth <= tuning_parameters.max_num_nodes);

	std::vector<SplitData> splits = FileReader::ReadSplits(tuning_parameters.benchmark_location, tuning_parameters.partition_files_location_prefix);
	runtime_assert(splits.size() == 5);

	size_t num_splits = splits.size();
	std::vector<Solver*> solvers(num_splits);
	for (size_t i = 0; i < num_splits; i++)
	{
		solvers[i] = new Solver(solver_parameters);
		solvers[i]->ReplaceData(splits[i].training_data);
		solvers[i]->SetVerbosity(false);
	}

	ParameterHandler runtime_parameters = solver_parameters;
	runtime_parameters.SetIntegerParameter("max-depth", 0);
	runtime_parameters.SetIntegerParameter("max-num-nodes", 0);

	int num_labels = splits[0].training_data.size();
	int best_depth = -1;
	int best_num_nodes = -1;
	int best_testing_misclassification = INT32_MAX;
	int min_depth = tuning_parameters.single_parameter_set_tuning ? tuning_parameters.max_depth : 0;
	for (int depth = min_depth; depth <= tuning_parameters.max_depth; depth++)
	{
		int max_num_nodes = std::min(tuning_parameters.max_num_nodes, (int(1) << depth) - 1);
		int min_num_nodes = tuning_parameters.single_parameter_set_tuning ? max_num_nodes : depth;
		runtime_assert(min_num_nodes >= depth);
		for (int num_nodes = min_num_nodes; num_nodes <= max_num_nodes; num_nodes++)
		{
			std::cout << "\t(depth, size) = " << "(" << depth << ", " << num_nodes << ")\n";
			runtime_parameters.SetIntegerParameter("max-depth", depth);
			runtime_parameters.SetIntegerParameter("max-num-nodes", num_nodes);

			int current_testing_misclassifications = 0;
			for (int i = 0; i < splits.size(); i++)
			{
				SplitData& split = splits[i];

				SolverResult result = solvers[i]->Solve(runtime_parameters);
				int train_misclass = result.decision_tree_->ComputeMisclassificationScore(split.training_data);
				int test_misclass = result.decision_tree_->ComputeMisclassificationScore(split.testing_data);
				current_testing_misclassifications += test_misclass;

				int train_size = 0, test_size = 0;
				for (int label = 0; label < num_labels; label++)
				{
					train_size += split.training_data[label].size();
					test_size += split.testing_data[label].size();
				}
				
				double train_accuracy = double(train_size - train_misclass)/ train_size;
				double test_accuracy = double(test_size - test_misclass) / test_size;

				std::cout << "\t\t(" << train_accuracy << ", " << test_accuracy << ")\n";
			}

			if (best_testing_misclassification > current_testing_misclassifications)
			{
				best_testing_misclassification = current_testing_misclassifications;
				best_depth = depth;
				best_num_nodes = num_nodes;
			}
		}
	}
	runtime_assert(best_depth >= 0 && best_num_nodes >= 0);

	//now retrain a decision on the entire data using the hyper parameters determined
	runtime_parameters.SetIntegerParameter("max-depth", best_depth);
	runtime_parameters.SetIntegerParameter("max-num-nodes", best_num_nodes);

	std::cout << "Chosen (depth, size): " << "(" << best_depth << ", " << best_num_nodes << ")\n";
	Solver final_solver(solver_parameters);
		
	HyperParameterTuningOutput output;
	output.train_accuracy = 0;
	output.test_accuracy = 0;

	clock_t reconstruct_start = clock();
	for (int i = 0; i < splits.size(); i++)
	{
		runtime_assert(num_labels == splits[i].testing_data.size());

		SplitData& split = splits[i];
		SolverResult r = solvers[i]->Solve(runtime_parameters);
		int train_misclassifications = r.decision_tree_->ComputeMisclassificationScore(split.training_data);
		int test_misclassifications = r.decision_tree_->ComputeMisclassificationScore(split.testing_data);

		int train_size = 0, test_size = 0;
		for (int label = 0; label < num_labels; label++)
		{
			train_size += split.training_data[label].size();
			test_size += split.testing_data[label].size();
		}

		output.train_accuracy += double(train_size - train_misclassifications) / train_size;
		output.test_accuracy += double(test_size - test_misclassifications) / test_size;
	}
	output.train_accuracy /= splits.size();
	output.test_accuracy /= splits.size();

	std::cout << "\tReconstruct time: " << (clock() - reconstruct_start) / CLOCKS_PER_SEC << "\n";
	
	final_solver.SetVerbosity(false);
	output.result = final_solver.Solve(runtime_parameters);

	for (size_t i = 0; i < num_splits; i++) { delete solvers[i]; solvers[i] = 0; }

	return output;
}
}
