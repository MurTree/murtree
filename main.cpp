//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

//Code for the paper: 
// "MurTree: optimal classification trees via dynamic programming and search", JMLR, 2022.
//  Note that some of the comments directly refer to the paper:
// 
//Authors: Emir Demirović, Anna Lukina, Emmanuel Hebrard, Jeffrey Chan, James Bailey, Christopher Leckie, Kotagiri Ramamohanarao, Peter J. Stuckey
//For any issues related to the code, please feel free to contact Dr Emir Demirović, e.demirovic@tudelft.nl

#include "code/MurTree/Utilities/parameters.h"
#include "code/MurTree/Engine/solver.h"
#include "code/MurTree/Engine/dataset_cache.h"
#include "code/MurTree/Engine/hyper_parameter_tuner.h"
#include "code/MurTree/Utilities/parameter_handler.h"
#include "code/MurTree/Utilities/stopwatch.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <time.h>
#include <algorithm>

int internalMain(int argc, char* argv[])
{
	MurTree::ParameterHandler parameters = MurTree::DefineParameters();

	bool manual_activation = false;
	bool single_parameter_set_tuning = false;
	std::string splits_location_prefix = "";
	std::string hyper_parameter_stats_file = "";	

	//standard expected case when run from the console
	//see DefineParameters for info about the parameters
	if (argc > 1)
	{
		parameters.ParseCommandLineArguments(argc, argv);
	}
	//this else statement was mostly used for debugging, now disabled
	else
	{
		std::cout << "Manual activation, no parameters supplied\n";
		exit(1);

		manual_activation = true;

		std::string file_location;

		file_location = "datasetsDL\\anneal.txt";
		//file_location = "datasetsDL\\german-credit.txt";
		//file_location = "datasetsDL\\vehicle.txt";
		//file_location = "datasetsNina\\converted\\colic-un_converted.txt";
		//file_location = "datasetsDL\\letter.txt";	
		//file_location = "datasetsDL\\lymph.txt";
		//file_location = "datasetsDL\\yeast.txt";
		//file_location = "datasetsDL\\pendigits.txt";		
		//file_location = "datasetsDL\\ionosphere.txt";
		//file_location = "datasetsDL\\diabetes.txt";
		//file_location = "datasetsDL\\audiology.txt";
		//file_location = "datasetsDL\\hypothyroid.txt";
		//file_location = "datasetsDL\\hepatitis.txt";
		//file_location = "datasetsDL\\soybean.txt";
		//file_location = "datasetsDL\\australian-credit.txt";
		//file_location = "datasetsDL\\anneal.txt";
		//file_location = "datasetsNL\\binarised\\wine_categorical_bin.txt";
		//file_location = "datasetsNL\\binarised\\winequality-red_categorical_bin.txt";	
		//file_location = "datasetsNL\\binarised\\magic04_categorical_bin.txt";
		//file_location = "datasetsNL\\binarised\\default_credit_categorical_bin.txt";
		//file_location = "datasetsNina\\australian-un.csv";
		//file_location = "datasetFair\\taiwan_binarised_caim.txt";
		//file_location = "datasetFair\\taiwan_binarised_ameva.txt";
		//file_location = "UCI\\wine_mdlp_binarised.txt";
		//file_location = "UCI\\bank_binarised.txt";
		//file_location = "DatasetsAnnachan\\rollouts_escape_room_10000_mdpl_binarised.txt";
		//file_location = "monk1_bin.txt";
		//file_location = "datasetsNina\\converted\\irish-un_converted.txt";
		//file_location = "temp_dataset.txt";
		//file_location = "datasetsNL\\binarised\\monk3_bin.txt";
		//file_location = "datasetsHu\\compas-binary.txt";
		//file_location = "datasetsHu\\fico-binary.txt";
		//file_location = "datasetsHu\\monk3-hu.txt";
		//file_location = "datasetsNina\\converted\\appendicitis-un_converted.txt";
		//file_location = "datasetsNina\\converted\\backache-un_converted.txt";
		//file_location = "datasetsNina\\converted\\australian-un_converted.txt";
		//file_location = "datasetsNina\\converted\\cleve-un_converted.txt";
		//file_location = "datasetsNina\\converted\\cleve-un_converted.txt";

		parameters.SetStringParameter("file", file_location);
		parameters.SetBooleanParameter("hyper-parameter-tuning", false);
		//splits_location_prefix = "datasetsTesting\\MurTreeSplits\\anneal";
		//hyper_parameter_stats_file = "datasetsTesting\\MurTreeHyperStats\\anneal_new.txt";
		single_parameter_set_tuning = false;
		parameters.SetIntegerParameter("duplicate-factor", 1);
		parameters.SetStringParameter("cache-type", "dataset");// "closure";// "branch";
		parameters.SetBooleanParameter("all-trees", false);
		parameters.SetIntegerParameter("max-depth", 4);
		parameters.SetIntegerParameter("max-num-nodes", 15);
		parameters.SetIntegerParameter("upper-bound", INT32_MAX);//INT32_MAX
		parameters.SetFloatParameter("sparse-coefficient", 0.00);
		parameters.SetBooleanParameter("similarity-lower-bound", true);
		parameters.SetStringParameter("node-selection", "dynamic");
		parameters.SetStringParameter("feature-ordering", "in-order");
	}

	MurTree::CheckParameters(parameters);

	if (parameters.GetBooleanParameter("verbose")) { parameters.PrintParameterValues(); }

	if (parameters.GetIntegerParameter("random-seed") == -1) { srand(time(0)); }
	else { srand(parameters.GetIntegerParameter("random-seed")); }

	MurTree::Stopwatch stopwatch;
	stopwatch.Initialise(0);

	if (parameters.GetBooleanParameter("hyper-parameter-tuning"))
	{
		std::cout << "NEW HYPER PARAMETER TUNING\n";
		MurTree::HyperParameterTuningInput hyper_p;
		hyper_p.benchmark_location = parameters.GetStringParameter("file");
		hyper_p.partition_files_location_prefix = splits_location_prefix;
		hyper_p.max_depth = parameters.GetIntegerParameter("max-depth");
		hyper_p.max_num_nodes = parameters.GetIntegerParameter("max-num-nodes");
		hyper_p.single_parameter_set_tuning = single_parameter_set_tuning;

		clock_t c_s = clock();
		MurTree::HyperParameterTuningOutput result = MurTree::HyperParameterTuner::Solve(hyper_p, parameters);
		std::cout << "BEST PARAMS: \n\tDepth = " << result.result.decision_tree_->Depth() << "\n\tNum nodes: " << result.result.decision_tree_->NumNodes() << "\n";
		std::cout << "Train/test: " << result.train_accuracy << ", " << result.test_accuracy << "\n";
		std::cout << "CLOCK: " << double(clock() - c_s) / CLOCKS_PER_SEC << "\n";

		std::ofstream f(hyper_parameter_stats_file.c_str());
		f << result.result.decision_tree_->Depth() << "\n";
		f << result.result.decision_tree_->NumNodes() << "\n";
		f << result.train_accuracy << "\n";
		f << result.test_accuracy << "\n";
		f << double(clock() - c_s) / CLOCKS_PER_SEC << "\n";
		f << "[blank]\n";
		f << single_parameter_set_tuning;
		return 0;
	}
	else
	{
		if (parameters.GetBooleanParameter("verbose")) { std::cout << "Optimal tree computation started!\n"; }
		MurTree::Solver mur_tree_solver(parameters);
		clock_t clock_before_solve = clock();
		MurTree::SolverResult mur_tree_solver_result = mur_tree_solver.Solve(parameters);
				
		if (parameters.GetBooleanParameter("verbose"))
		{
			if (mur_tree_solver_result.decision_tree_)
			{
				std::cout << "MISCLASSIFICATION SCORE: " << mur_tree_solver_result.misclassifications << "\n";
				std::cout << "DEPTH: " << mur_tree_solver_result.decision_tree_->Depth() << "\n";
				std::cout << "NUM NODES: " << mur_tree_solver_result.decision_tree_->NumNodes() << "\n";
				std::cout << "TIME: " << stopwatch.TimeElapsedInSeconds() << " seconds\n";
				std::cout << "CLOCKS FOR SOLVE: " << double(clock() - clock_before_solve) / CLOCKS_PER_SEC << "\n";
			}
			else
			{
				std::cout << "Timeout, no tree stats\n";
			}
		}

		if (parameters.GetStringParameter("result-file") != "")
		{
			std::ofstream out(parameters.GetStringParameter("result-file").c_str());
			//double clock_time_total = (double(clock() - clock_before_solve) / CLOCKS_PER_SEC);
			//out << clock_time_total << "\n";
			out << stopwatch.TimeElapsedInSeconds() << "\n";
			out << 0 << "\n";
			out << 0 << "\n";
			
			if (mur_tree_solver_result.decision_tree_) //print stats if finished within the allocated time
			{
				out << mur_tree_solver_result.misclassifications << "\n";
				out << mur_tree_solver.cache_->NumEntries() << "\n";
			}
			else //print failure
			{
				out << "-1\n-1\n";
			}
		}
	}
	return 0;
}

#if defined BUILD_LIBRARY || defined BUILD_PYTHON_MODULE
	#include "murtree.h"
	int murtree(std::string path_to_dataset, unsigned int maxdepth, unsigned int maxnumnodes, unsigned int time) {

		std::string smaxdepth = std::to_string(maxdepth);
		std::string smaxnumnodes = std::to_string(maxnumnodes);
		std::string stime = std::to_string(time);

		char* argv[9];
		argv[0] = "./murtree";
		argv[1] = "-file";
		argv[2] =  const_cast<char*>(path_to_dataset.c_str());
		argv[3] = "-max-depth";
		argv[4] = const_cast<char*>(smaxdepth.c_str());
		argv[5] = "-max-num-nodes";
		argv[6] = const_cast<char*>(smaxnumnodes.c_str());
		argv[7] = "-time";
		argv[8] = const_cast<char*>(stime.c_str());
		
		return internalMain(9, argv);
	}
	#ifdef BUILD_PYTHON_MODULE
		#include "include/pybind11/pybind11.h"
		PYBIND11_MODULE(murtree_python_module, m){
			m.doc() = "Murtree module Docs";
			m.def("murtree", &murtree);
		}
	#endif
#else
	int main(int argc, char* argv[]){
		return internalMain(argc, argv);
	}
#endif